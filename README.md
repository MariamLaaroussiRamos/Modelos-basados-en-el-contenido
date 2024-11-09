# Modelos-basados-en-el-contenido

## 1. Introducción

Este software está diseñado para procesar documentos, eliminar palabras vacías, lematizar las palabras, y calcular la similitud entre documentos utilizando la técnica de **TF-IDF** (Frecuencia de Término-Inversa de Frecuencia de Documento) y la **Similitud Coseno**. El sistema permite realizar análisis de similitudes entre documentos de texto, proporcionando un valor numérico que indica cuán similares son entre sí.

## 2. Requisitos

### 2.1. Dependencias

El proyecto requiere las siguientes dependencias:

- **C++11** o superior para compilar el código, debido al uso de características como `unordered_map`, `unordered_set`, entre otros.
- **Librerías estándar de C++**, como `iostream`, `fstream`, `unordered_map`, `unordered_set`, `vector`, `string`, y `iomanip`.

No es necesario instalar librerías externas adicionales, ya que el código depende únicamente de la biblioteca estándar de C++.

### 2.2. Archivos de Entrada

El programa requiere tres archivos de entrada:

1. **Documento de texto (`document_file`)**: Contiene los documentos en los que se calcularán las similitudes.
2. **Palabras vacías (`stop_words_file`)**: Un archivo que lista las palabras que deben ser ignoradas durante el análisis (e.g., "y", "el", "de").
3. **Lematización (`lemmatization_file`)**: Un archivo que mapea las palabras a su forma base o raíz (por ejemplo, "corrí" → "correr").

### 2.3. Herramientas necesarias

Para compilar el código, puedes usar cualquier compilador compatible con C++11 o superior. Ejemplos:

- **g++ (GNU Compiler)**: 
  ```bash
  g++ -std=c++11 main.cc tools.cc -o document_similarity

El archivo main.cc contiene la lógica principal de ejecución, mientras que tools.cc define las funciones auxiliares para leer los documentos, calcular TF, IDF, y similitudes.

## 3.1. `main.cc`

Este archivo contiene la función principal del programa y coordina las tareas del sistema. Realiza lo siguiente:

### Lectura de Archivos:
- **Documentos**: Llama a la función `readDocuments()` para cargar los documentos desde un archivo.
- **Palabras Vacías**: Llama a la función `readStopWords()` para cargar las palabras vacías desde un archivo.
- **Lematización**: Llama a `readLemmatization()` para cargar un mapa de lematización.

### Cálculo de TF, IDF y TF-IDF:
- Llama a `calculateTF()` para calcular la frecuencia de términos en cada documento.
- Llama a `calculateIDF()` para calcular la frecuencia inversa de documentos de cada término.
- Llama a `calculateTFIDF()` para calcular el valor de TF-IDF para cada término en cada documento.

### Similitudes Coseno:
- Llama a `calculateCosineSimilarities()` para calcular la similitud coseno entre los documentos.

### Impresión de Resultados:
- Llama a `printDocument()` para imprimir los resultados de los cálculos de TF, IDF y TF-IDF por documento.

---

## 3.2. `tools.cc`

Este archivo contiene las funciones auxiliares utilizadas en el proceso:

- **`readStopWords()`**: Lee el archivo de palabras vacías y las almacena en un conjunto.
- **`readLemmatization()`**: Lee el archivo de lematización y lo almacena en un mapa que asocia cada palabra con su lema.
- **`readDocuments()`**: Lee el archivo de documentos y los almacena en un vector.
- **`calculateTF()`**: Calcula la frecuencia de término (TF) de cada documento.
- **`calculateIDF()`**: Calcula la frecuencia inversa de documento (IDF) para cada término en el corpus.
- **`calculateTFIDF()`**: Calcula el valor de TF-IDF multiplicando el TF por el IDF.
- **`cosineSimilarity()`**: Calcula la similitud coseno entre dos vectores TF-IDF.
- **`calculateCosineSimilarities()`**: Calcula la matriz de similitudes coseno entre todos los documentos.
- **`printDocument()`**: Imprime los resultados de TF, IDF y TF-IDF para cada documento en un formato tabular.

---

## 4. Ejemplo de Uso

El uso básico del software se realiza ejecutando el programa desde la línea de comandos con los archivos correspondientes como parámetros. La sintaxis es la siguiente:

```./document_similarity <document_file> <stop_words_file> <lemmatization_file>```


### Ejemplo:

Si tienes los siguientes archivos:

#### `document.txt` (contiene los documentos que deseas analizar):

#### `stop_words.txt` (contiene las palabras vacías a excluir):

#### `lemmatization.txt` (contiene las lematizaciones):


Entonces puedes ejecutar el programa de la siguiente manera:

```./document_similarity document.txt stop_words.txt lemmatization.txt```


Este comando procesará los documentos, calculará TF, IDF, TF-IDF, y las similitudes coseno, y luego imprimirá los resultados.

---

## 5. Salida Esperada

La salida será una tabla por cada documento, con las siguientes columnas:

- **Index**: El índice del término.
- **Word**: La palabra en el documento.
- **TF**: La frecuencia de término normalizada para ese documento.
- **IDF**: La frecuencia inversa de documento para la palabra.
- **TF-IDF**: El valor de TF-IDF para la palabra en el documento.
