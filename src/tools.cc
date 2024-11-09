#include "tools.h"
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

void Usage(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <filename> <metric> <k> <predictionType>" << std::endl;
        std::cout << "<metric> - pearson/cosine/euclidean" << std::endl;
        std::cout << "<k> - number of neighbors" << std::endl;
        std::cout << "<predictionType> - simple/mean" << std::endl;
        exit(1);
    }
}

bool readStopWords(const std::string &filename, std::set<std::string> &stopWords) {
    std::ifstream file(filename);
    std::string word;
    while (file >> word) {
        stopWords.insert(word);
    }
    file.close();
    return true;
}

bool readLemmatization(const std::string &filename, std::map<std::string, std::string> &lemmatizationMap) {
    std::ifstream file(filename);
    std::string term, lemma;
    while (file >> term >> lemma) {
        lemmatizationMap[term] = lemma;
    }
    file.close();
    return true;
}

// Lee documentos de un archivo y almacena cada línea como un documento
bool readDocuments(const std::string &filename, std::vector<std::string> &documents) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        documents.push_back(line);
    }
    return true;
}

// Calcula la frecuencia de términos (TF) para cada término en cada documento
void calculateTF(const std::vector<std::string> &documents, std::vector<std::unordered_map<std::string, double>> &tf) {
    for (const auto &doc : documents) {
        std::unordered_map<std::string, double> termFrequency;
        std::istringstream stream(doc);
        std::string term;
        int totalTerms = 0;
        while (stream >> term) {
            termFrequency[term]++;
            totalTerms++;
        }
        for (auto &entry : termFrequency) {
            entry.second /= totalTerms; // Normaliza la frecuencia dividiendo por el total de términos
        }
        tf.push_back(termFrequency);
    }
}

void calculateIDF(const std::vector<std::string> &documents, std::unordered_map<std::string, double> &idf) {
    int numDocuments = documents.size();
    for (const auto &doc : documents) {
        std::unordered_map<std::string, bool> termsInDoc;
        std::istringstream stream(doc);
        std::string term;
        while (stream >> term) {
            termsInDoc[term] = true;
        }
        for (const auto &entry : termsInDoc) {
            idf[entry.first] += 1;
        }
    }
    for (auto &entry : idf) {
        entry.second = std::log(numDocuments / entry.second);
    }
}

// Calcula el TF-IDF multiplicando TF y IDF para cada término en cada documento
void calculateTFIDF(const std::vector<std::unordered_map<std::string, double>> &tf, const std::unordered_map<std::string, double> &idf,
                    std::vector<std::unordered_map<std::string, double>> &tfidf) {
    for (size_t i = 0; i < tf.size(); ++i) {
        std::unordered_map<std::string, double> tfidfValues;
        for (const auto &entry : tf[i]) {
            tfidfValues[entry.first] = entry.second * idf.at(entry.first);
        }
        tfidf.push_back(tfidfValues);
    }
}

// Calcula la similaridad coseno entre dos vectores TF-IDF representados como unordered_map
double cosineSimilarity(const std::unordered_map<std::string, double> &tfidf1,
                        const std::unordered_map<std::string, double> &tfidf2) {
    double dotProduct = 0.0;
    double norm1 = 0.0, norm2 = 0.0;

    // Calcular producto punto y norma del primer vector
    for (const auto &[term, value] : tfidf1) {
        if (tfidf2.count(term)) {
            dotProduct += value * tfidf2.at(term);
        }
        norm1 += value * value;
    }

    // Calcular norma del segundo vector
    for (const auto &[term, value] : tfidf2) {
        norm2 += value * value;
    }

    return dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
}

// Calcula la matriz de similitudes coseno entre los vectores TF-IDF de los documentos
std::vector<std::vector<double>> calculateCosineSimilarities(const std::vector<std::unordered_map<std::string, double>> &tfidfVectors) {
    size_t numDocuments = tfidfVectors.size();
    std::vector<std::vector<double>> similarities(numDocuments, std::vector<double>(numDocuments, 0.0));

    for (size_t i = 0; i < numDocuments; ++i) {
        for (size_t j = i + 1; j < numDocuments; ++j) {
            double similarity = cosineSimilarity(tfidfVectors[i], tfidfVectors[j]);
            similarities[i][j] = similarities[j][i] = similarity;
        }
    }
    return similarities;
}

void printDocument(const std::vector<std::string> &documents,
                   const std::vector<std::unordered_map<std::string, double>> &tf,
                   const std::unordered_map<std::string, double> &idf,
                   const std::vector<std::unordered_map<std::string, double>> &tfidf) {
    for (size_t docIndex = 0; docIndex < documents.size(); ++docIndex) {
        std::cout << "Documento " << docIndex + 1 << ":\n";
        std::cout << "Índice\tTérmino\t\tTF\t\tIDF\t\tTF-IDF\n";
        int termIndex = 0;
        for (const auto &term : tf[docIndex]) {
            const std::string &word = term.first;
            double tfValue = term.second;
            double idfValue = idf.at(word);
            double tfidfValue = tfidf[docIndex].at(word);
            
            // Ajuste de formato para una salida más clara
            std::cout << std::fixed << std::setprecision(4);
            std::cout << termIndex++ << "\t" << word << "\t\t" << tfValue
                      << "\t\t" << idfValue << "\t\t" << tfidfValue << "\n";
        }
        std::cout << std::endl;
    }
}
