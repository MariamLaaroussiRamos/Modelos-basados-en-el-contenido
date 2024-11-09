#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

void Usage(int argc, char *argv[]);

bool readStopWords(const std::string &filename,
                   std::set<std::string> &stopWords);
bool readLemmatization(const std::string &filename,
                       std::map<std::string, std::string> &lemmatizationMap);
bool readDocuments(const std::string &filename,
                   std::vector<std::string> &documents);
void calculateTF(const std::vector<std::string> &documents,
                 std::vector<std::unordered_map<std::string, double>> &tf);
void calculateIDF(const std::vector<std::string> &documents,
                  std::unordered_map<std::string, double> &idf);
void calculateTFIDF(
    const std::vector<std::unordered_map<std::string, double>> &tf,
    const std::unordered_map<std::string, double> &idf,
    std::vector<std::unordered_map<std::string, double>> &tfidf);
double cosineSimilarity(const std::unordered_map<std::string, double> &tfidf1,
                        const std::unordered_map<std::string, double> &tfidf2);
std::vector<std::vector<double>> calculateCosineSimilarities(
    const std::vector<std::unordered_map<std::string, double>> &tfidf);
void printDocument(
    const std::vector<std::string> &documents,
    const std::vector<std::unordered_map<std::string, double>> &tf,
    const std::unordered_map<std::string, double> &idf,
    const std::vector<std::unordered_map<std::string, double>> &tfidf);