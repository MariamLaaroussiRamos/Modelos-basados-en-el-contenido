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

/**
 * @brief Reads the input file and populates the utility matrix.
 * 
 * @param filename The name of the input file.
 * @param utilityMatrix The matrix to store user-item ratings.
 * @param minRating The minimum possible rating.
 * @param maxRating The maximum possible rating.
 */
void readInputFile(const std::string &filename, std::vector<std::vector<double>> &utilityMatrix, double &minRating, double &maxRating) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open the file!" << std::endl;
        exit(1);
    }

    std::string line;
    std::getline(file, line);
    minRating = std::stod(line);

    std::getline(file, line);
    maxRating = std::stod(line);

    while (std::getline(file, line)) {
        std::vector<double> row;
        std::stringstream ss(line);
        std::string value;
        while (ss >> value) {
            if (value == "-") {
                row.push_back(-1);
            } else {
                row.push_back(std::stod(value));
            }
        }
        utilityMatrix.push_back(row);
    }
    file.close();
}

/**
 * @brief Computes the Pearson correlation coefficient between two vectors.
 * 
 * @param a The first vector.
 * @param b The second vector.
 * @return The Pearson correlation coefficient.
 */
double pearsonCorrelation(const std::vector<double> &a, const std::vector<double> &b) {
    std::vector<double> commonA, commonB;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != -1 && b[i] != -1) {
            commonA.push_back(a[i]);
            commonB.push_back(b[i]);
        }
    }
    if (commonA.empty()) return 0.0;

    double meanA = std::accumulate(commonA.begin(), commonA.end(), 0.0) / commonA.size();
    double meanB = std::accumulate(commonB.begin(), commonB.end(), 0.0) / commonB.size();
    double numerator = 0.0, denomA = 0.0, denomB = 0.0;
    for (size_t i = 0; i < commonA.size(); ++i) {
        numerator += (commonA[i] - meanA) * (commonB[i] - meanB);
        denomA += std::pow(commonA[i] - meanA, 2);
        denomB += std::pow(commonB[i] - meanB, 2);
    }
    return numerator / std::sqrt(denomA * denomB);
}

/**
 * @brief Computes the cosine similarity between two vectors.
 * 
 * @param a The first vector.
 * @param b The second vector.
 * @return The cosine similarity.
 */
double cosineSimilarity(const std::vector<double> &a, const std::vector<double> &b) {
    double dotProduct = 0.0, normA = 0.0, normB = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != -1 && b[i] != -1) {
            dotProduct += a[i] * b[i];
            normA += std::pow(a[i], 2);
            normB += std::pow(b[i], 2);
        }
    }
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

/**
 * @brief Computes the Euclidean distance between two vectors.
 * 
 * @param a The first vector.
 * @param b The second vector.
 * @return The Euclidean distance.
 */
double euclideanDistance(const std::vector<double> &a, const std::vector<double> &b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != -1 && b[i] != -1) {
            sum += std::pow(a[i] - b[i], 2);
        }
    }
    return std::sqrt(sum);
}

/**
 * @brief Finds the k-nearest neighbors for a given user and item.
 * 
 * @param utilityMatrix The user-item ratings matrix.
 * @param userIndex The index of the user.
 * @param itemIndex The index of the item.
 * @param k The number of neighbors to find.
 * @param metric The similarity metric to use (pearson/cosine/euclidean).
 * @return A vector of pairs, each containing the index of a neighbor and its similarity score.
 */
std::vector<std::pair<int, double>> findNeighbors(const std::vector<std::vector<double>> &utilityMatrix, int userIndex, int itemIndex, int k, const std::string &metric) {
    std::vector<std::pair<int, double>> similarities;
    for (size_t i = 0; i < utilityMatrix.size(); ++i) {
        if (i != userIndex && utilityMatrix[i][itemIndex] != -1) {
            double similarity = 0.0;
            if (metric == "pearson") {
                similarity = pearsonCorrelation(utilityMatrix[userIndex], utilityMatrix[i]);
            } else if (metric == "cosine") {
                similarity = cosineSimilarity(utilityMatrix[userIndex], utilityMatrix[i]);
            } else if (metric == "euclidean") {
                similarity = -euclideanDistance(utilityMatrix[userIndex], utilityMatrix[i]);
            } else {
                std::cerr << "Invalid metric!" << std::endl;
                exit(1);
            }
            similarities.push_back(std::make_pair(i, similarity));
        }
    }
    std::sort(similarities.begin(), similarities.end(), [](const std::pair<int, double> &a, const std::pair<int, double> &b) {
        return a.second > b.second;
    });
    if (similarities.size() > k) {
        similarities.resize(k);
    }
    return similarities;
}

/**
 * @brief Predicts the rating for a given user and item using a simple weighted average.
 * 
 * @param utilityMatrix The user-item ratings matrix.
 * @param userIndex The index of the user.
 * @param itemIndex The index of the item.
 * @param neighbors The list of neighbors and their similarity scores.
 * @return The predicted rating.
 */
double predictSimple(const std::vector<std::vector<double>> &utilityMatrix, int userIndex, int itemIndex, const std::vector<std::pair<int, double>> &neighbors) {
    double numerator = 0.0, denominator = 0.0;
    for (const auto &neighbor : neighbors) {
        int neighborIndex = neighbor.first;
        double similarity = neighbor.second;
        if (utilityMatrix[neighborIndex][itemIndex] != -1) {
            numerator += similarity * utilityMatrix[neighborIndex][itemIndex];
            denominator += std::abs(similarity);
        }
    }
    return numerator / denominator;
}

/**
 * @brief Predicts the rating for a given user and item using the difference with the mean approach.
 * 
 * @param utilityMatrix The user-item ratings matrix.
 * @param userIndex The index of the user.
 * @param itemIndex The index of the item.
 * @param neighbors The list of neighbors and their similarity scores.
 * @return The predicted rating.
 */
double predictDiffWithMean(const std::vector<std::vector<double>> &utilityMatrix, int userIndex, int itemIndex, const std::vector<std::pair<int, double>> &neighbors) {
    double meanUser = std::accumulate(utilityMatrix[userIndex].begin(), utilityMatrix[userIndex].end(), 0.0,
                                      [](double sum, double rating) { return rating != -1 ? sum + rating : sum; }) /
                      std::count_if(utilityMatrix[userIndex].begin(), utilityMatrix[userIndex].end(),
                                    [](double rating) { return rating != -1; });
    double numerator = 0.0, denominator = 0.0;
    for (const auto &neighbor : neighbors) {
        int neighborIndex = neighbor.first;
        double similarity = neighbor.second;
        double meanNeighbor = std::accumulate(utilityMatrix[neighborIndex].begin(), utilityMatrix[neighborIndex].end(), 0.0,
                                              [](double sum, double rating) { return rating != -1 ? sum + rating : sum; }) /
                             std::count_if(utilityMatrix[neighborIndex].begin(), utilityMatrix[neighborIndex].end(),
                                           [](double rating) { return rating != -1; });
        if (utilityMatrix[neighborIndex][itemIndex] != -1) {
            numerator += similarity * (utilityMatrix[neighborIndex][itemIndex] - meanNeighbor);
            denominator += std::abs(similarity);
        }
    }
    return meanUser + (numerator / denominator);
}

/**
 * @brief Prints the given matrix to the standard output.
 * 
 * @param matrix The matrix to print.
 */
void printMatrix(const std::vector<std::vector<double>> &matrix) {
    for (const auto &row : matrix) {
        for (const auto &elem : row) {
            if (elem == -1) {
                std::cout << "- ";
            } else {
                std::cout << elem << " ";
            }
        }
        std::cout << std::endl;
    }
}

void printMatrixinFile(const std::vector<std::vector<double>> &matrix, std::string filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Could not open the file!" << std::endl;
        exit(1);
    }
    for (const auto &row : matrix) {
        for (const auto &elem : row) {
            if (elem == -1) {
                file << "- ";
            } else {
                file << elem << " ";
            }
        }
        file << std::endl;
    }
    file.close();
}

void Usage(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <filename> <metric> <k> <predictionType>" << std::endl;
        std::cout << "<metric> - pearson/cosine/euclidean" << std::endl;
        std::cout << "<k> - number of neighbors" << std::endl;
        std::cout << "<predictionType> - simple/mean" << std::endl;
        exit(1);
    }
}

void readStopWords(const std::string &filename, std::set<std::string> &stopWords) {
    std::ifstream file(filename);
    std::string word;
    while (file >> word) {
        stopWords.insert(word);
    }
    file.close();
}

void readLemmatization(const std::string &filename, std::map<std::string, std::string> &lemmatizationMap) {
    std::ifstream file(filename);
    std::string term, lemma;
    while (file >> term >> lemma) {
        lemmatizationMap[term] = lemma;
    }
    file.close();
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