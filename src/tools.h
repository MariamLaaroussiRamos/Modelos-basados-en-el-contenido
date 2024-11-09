#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <limits>
#include <algorithm>
#include <numeric>
#include <set>
#include <map>
#include <string>
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
void readInputFile(const std::string &filename, std::vector<std::vector<double>> &utilityMatrix, double &minRating, double &maxRating);

/**
 * @brief Computes the Pearson correlation coefficient between two vectors.
 * 
 * @param a The first vector.
 * @param b The second vector.
 * @return The Pearson correlation coefficient.
 */
double pearsonCorrelation(const std::vector<double> &a, const std::vector<double> &b);

/**
 * @brief Computes the cosine similarity between two vectors.
 * 
 * @param a The first vector.
 * @param b The second vector.
 * @return The cosine similarity.
 */
double cosineSimilarity(const std::vector<double> &a, const std::vector<double> &b);


/**
 * @brief Computes the Euclidean distance between two vectors.
 * 
 * @param a The first vector.
 * @param b The second vector.
 * @return The Euclidean distance.
 */
double euclideanDistance(const std::vector<double> &a, const std::vector<double> &b);

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
std::vector<std::pair<int, double>> findNeighbors(const std::vector<std::vector<double>> &utilityMatrix, int userIndex, int itemIndex, int k, const std::string &metric);

/**
 * @brief Predicts the rating for a given user and item using a simple weighted average.
 * 
 * @param utilityMatrix The user-item ratings matrix.
 * @param userIndex The index of the user.
 * @param itemIndex The index of the item.
 * @param neighbors The list of neighbors and their similarity scores.
 * @return The predicted rating.
 */
double predictSimple(const std::vector<std::vector<double>> &utilityMatrix, int userIndex, int itemIndex, const std::vector<std::pair<int, double>> &neighbors);

/**
 * @brief Predicts the rating for a given user and item using the difference with the mean approach.
 * 
 * @param utilityMatrix The user-item ratings matrix.
 * @param userIndex The index of the user.
 * @param itemIndex The index of the item.
 * @param neighbors The list of neighbors and their similarity scores.
 * @return The predicted rating.
 */
double predictDiffWithMean(const std::vector<std::vector<double>> &utilityMatrix, int userIndex, int itemIndex, const std::vector<std::pair<int, double>> &neighbors);

/**
 * @brief Prints the matrix to the standard output.
 * 
 * @param matrix The matrix to print.
 */
void printMatrix(const std::vector<std::vector<double>> &matrix);

void printMatrixinFile(const std::vector<std::vector<double>> &matrix, std::string filename);
void Usage(int argc, char *argv[]);

void readStopWords(const std::string &filename, std::set<std::string> &stopWords);
void readLemmatization(const std::string &filename, std::map<std::string, std::string> &lemmatizationMap);
bool readDocuments(const std::string &filename, std::vector<std::string> &documents);
void calculateTF(const std::vector<std::string> &documents, std::vector<std::unordered_map<std::string, double>> &tf);
void calculateIDF(const std::vector<std::string> &documents, std::unordered_map<std::string, double> &idf);
void calculateTFIDF(const std::vector<std::unordered_map<std::string, double>> &tf, const std::unordered_map<std::string, double> &idf,
                    std::vector<std::unordered_map<std::string, double>> &tfidf);
double cosineSimilarity(const std::unordered_map<std::string, double> &tfidf1,
                        const std::unordered_map<std::string, double> &tfidf2);
std::vector<std::vector<double>> calculateCosineSimilarities(const std::vector<std::unordered_map<std::string, double>> &tfidf);
void printDocument(const std::vector<std::string> &documents,
                   const std::vector<std::unordered_map<std::string, double>> &tf,
                   const std::unordered_map<std::string, double> &idf,
                   const std::vector<std::unordered_map<std::string, double>> &tfidf);