#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "tools.h"

/**
 * @file main.cc
 * @brief Programa principal para el sistema de recomendación basado en el contenido.
 *
 * Este programa lee documentos, palabras vacías y lematización desde archivos,
 * calcula las frecuencias de término (TF), la frecuencia inversa de documentos (IDF),
 * y las similitudes de coseno entre los documentos.
 *
 * @param argc Número de argumentos de línea de comandos.
 * @param argv Argumentos de línea de comandos.
 * @return int Código de retorno del programa.
 */
int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0]
              << " <document_file> <stop_words_file> <lemmatization_file>"
              << std::endl;
    return 1;
  }

  const std::string documentFile = argv[1];
  const std::string stopWordsFile = argv[2];
  const std::string lemmatizationFile = argv[3];

  // Variables para almacenar datos procesados
  std::vector<std::string> documents;
  std::set<std::string> stopWords;
  std::map<std::string, std::string> lemmatizationMap;

  // Leer documentos desde el archivo
  if (!readDocuments(documentFile, documents)) {
    std::cerr << "Error: no se pudo leer el archivo de documentos: "
              << documentFile << std::endl;
    return 1;
  }

  // Leer palabras vacías desde el archivo
  if (!readStopWords(stopWordsFile, stopWords)) {
    std::cerr << "Error: no se pudo leer el archivo de palabras vacías: "
              << stopWordsFile << std::endl;
    return 1;
  }

  // Leer lematización desde el archivo
  if (!readLemmatization(lemmatizationFile, lemmatizationMap)) {
    std::cerr << "Error: no se pudo leer el archivo de lematización: "
              << lemmatizationFile << std::endl;
    return 1;
  }

  // Calcular TF
  std::vector<std::unordered_map<std::string, double>> tf;
  calculateTF(documents, tf);

  // Calcular IDF
  std::unordered_map<std::string, double> idf;
  calculateIDF(documents, idf);

  // Calcular TF-IDF
  std::vector<std::unordered_map<std::string, double>> tfidf;
  calculateTFIDF(tf, idf, tfidf);

  // Calcular similitudes coseno
  std::vector<std::vector<double>> cosineSimilarities =
      calculateCosineSimilarities(tfidf);

  // Imprimir similitudes coseno
  printDocument(documents, tf, idf, tfidf);

  return 0;
}
