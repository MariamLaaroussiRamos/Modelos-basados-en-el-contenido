/**
 * @file tools.cc
 * @brief Implementation of various utility functions for text processing and similarity calculations.
 * 
 * This file contains functions for reading stop words, lemmatization, documents, 
 * and calculating term frequency (TF), inverse document frequency (IDF), and TF-IDF values.
 * It also includes functions for calculating cosine similarity between TF-IDF vectors 
 * and printing document details.
 */

#include "tools.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

 /**
 * @brief Prints the usage message for the program.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 */
void Usage(int argc, char *argv[]) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " <filename> <metric> <k> <predictionType>" << std::endl;
    std::cout << "<metric> - pearson/cosine/euclidean" << std::endl;
    std::cout << "<k> - number of neighbors" << std::endl;
    std::cout << "<predictionType> - simple/mean" << std::endl;
    exit(1);
  }
}

/**
 * @brief Reads stop words from a file and stores them in a set.
 * 
 * @param filename The name of the file containing stop words.
 * @param stopWords A set to store the stop words.
 * @return true if the file was read successfully.
 */
bool readStopWords(const std::string &filename,
                   std::set<std::string> &stopWords) {
  std::ifstream file(filename);
  std::string word;
  while (file >> word) {
    stopWords.insert(word);
  }
  file.close();
  return true;
}

/**
 * @brief Reads lemmatization pairs from a file and stores them in a map.
 * 
 * @param filename The name of the file containing lemmatization pairs.
 * @param lemmatizationMap A map to store the lemmatization pairs.
 * @return true if the file was read successfully.
 */
bool readLemmatization(const std::string &filename,
                       std::map<std::string, std::string> &lemmatizationMap) {
  std::ifstream file(filename);
  std::string term, lemma;
  while (file >> term >> lemma) {
    lemmatizationMap[term] = lemma;
  }
  file.close();
  return true;
}

/**
 * @brief Reads documents from a file and stores each line as a document.
 * 
 * @param filename The name of the file containing documents.
 * @param documents A vector to store the documents.
 * @return true if the file was read successfully.
 */
bool readDocuments(const std::string &filename,
                   std::vector<std::string> &documents) {
  std::ifstream file(filename);
  std::string line;
  while (std::getline(file, line)) {
    documents.push_back(line);
  }
  return true;
}

/**
 * @brief Calculates the term frequency (TF) for each term in each document.
 * 
 * @param documents A vector of documents.
 * @param tf A vector to store the term frequency for each document.
 */
void calculateTF(const std::vector<std::string> &documents,
                 std::vector<std::unordered_map<std::string, double>> &tf) {
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
      entry.second /= totalTerms;  // Normaliza la frecuencia dividiendo por el
                                   // total de términos
    }
    tf.push_back(termFrequency);
  }
}

/**
 * @brief Calculates the inverse document frequency (IDF) for each term in the documents.
 * 
 * @param documents A vector of documents.
 * @param idf A map to store the inverse document frequency for each term.
 */
void calculateIDF(const std::vector<std::string> &documents,
                  std::unordered_map<std::string, double> &idf) {
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

/**
 * @brief Calculates the TF-IDF by multiplying TF and IDF for each term in each document.
 * 
 * @param tf A vector of term frequency maps for each document.
 * @param idf A map of inverse document frequency for each term.
 * @param tfidf A vector to store the TF-IDF values for each document.
 */
void calculateTFIDF(
    const std::vector<std::unordered_map<std::string, double>> &tf,
    const std::unordered_map<std::string, double> &idf,
    std::vector<std::unordered_map<std::string, double>> &tfidf) {
  for (size_t i = 0; i < tf.size(); ++i) {
    std::unordered_map<std::string, double> tfidfValues;
    for (const auto &entry : tf[i]) {
      tfidfValues[entry.first] = entry.second * idf.at(entry.first);
    }
    tfidf.push_back(tfidfValues);
  }
}

/**
 * @brief Calculates the cosine similarity between two TF-IDF vectors.
 * 
 * @param tfidf1 The first TF-IDF vector.
 * @param tfidf2 The second TF-IDF vector.
 * @return The cosine similarity between the two vectors.
 */
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

/**
 * @brief Calculates the cosine similarity matrix between the TF-IDF vectors of the documents.
 * 
 * @param tfidfVectors A vector of TF-IDF vectors for the documents.
 * @return A matrix of cosine similarities between the documents.
 */
std::vector<std::vector<double>> calculateCosineSimilarities(
    const std::vector<std::unordered_map<std::string, double>> &tfidfVectors) {
  size_t numDocuments = tfidfVectors.size();
  std::vector<std::vector<double>> similarities(
      numDocuments, std::vector<double>(numDocuments, 0.0));

  for (size_t i = 0; i < numDocuments; ++i) {
    for (size_t j = i + 1; j < numDocuments; ++j) {
      double similarity = cosineSimilarity(tfidfVectors[i], tfidfVectors[j]);
      similarities[i][j] = similarities[j][i] = similarity;
    }
  }
  return similarities;
}

/**
 * @brief Prints the details of each document including TF, IDF, and TF-IDF values.
 * 
 * @param documents A vector of documents.
 * @param tf A vector of term frequency maps for each document.
 * @param idf A map of inverse document frequency for each term.
 * @param tfidf A vector of TF-IDF values for each document.
 */
void printDocument(
    const std::vector<std::string> &documents,
    const std::vector<std::unordered_map<std::string, double>> &tf,
    const std::unordered_map<std::string, double> &idf,
    const std::vector<std::unordered_map<std::string, double>> &tfidf) {
  const char separator = ' ';
  const int width = 15;
  for (size_t docIndex = 0; docIndex < documents.size(); ++docIndex) {
    std::cout << "Documento " << docIndex + 1 << ":\n";
    std::cout << std::left << std::setw(width) << "Index" << separator
              << std::left << std::setw(width) << "Word" << separator
              << std::left << std::setw(width) << "TF" << separator << std::left
              << std::setw(width) << "IDF" << separator << std::left
              << std::setw(width) << "TF-IDF" << std::endl;
    int termIndex = 0;
    for (const auto &term : tf[docIndex]) {
      const std::string &word = term.first;
      double tfValue = term.second;
      double idfValue = idf.at(word);
      double tfidfValue = tfidf[docIndex].at(word);

      // Ajuste de formato para una salida más clara
      std::cout << termIndex++ << std::setw(width) << separator << std::left
                << std::setw(width) << word << separator << std::left
                << std::setw(width) << std::setprecision(2) << tfValue
                << separator << std::left << std::setw(width)
                << std::setprecision(2) << idfValue << separator << std::left
                << std::setw(width) << std::setprecision(2) << tfidfValue
                << std::endl;
    }
    std::cout << std::endl;
  }
}
