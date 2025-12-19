#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace std;

/**
 * Estructura para representar el resultado de la validación
 */
struct ResultadoValidacion {
    bool esValido;
    string mensaje;
};

/**
 * Clase para validar n-sudokus
 */
class ValidadorSudoku {
private:
    vector<vector<int>> sudoku;  // Matriz del sudoku (0 = celda vacía)
    int n;                       // Parámetro n del sudoku
    int tamano;                  // n² (tamaño del tablero)
    
    /**
     * Calcula el ancho de cada símbolo según n
     * Retorna: ⌊log₁₀(n²) + 1⌋
     */
    int calcularAnchoSimbolo() {
        return floor(log10(tamano) + 1);
    }
    
    /**
     * Valida que el sudoku tenga dimensiones n² x n²
     */
    bool validarDimensiones() {
        if (sudoku.size() != tamano) {
            return false;
        }
        
        for (const auto& fila : sudoku) {
            if (fila.size() != tamano) {
                return false;
            }
        }
        
        return true;
    }
    
    /**
     * Valida que no haya símbolos repetidos en ninguna fila
     */
    bool validarFilas() {
        for (int i = 0; i < tamano; i++) {
            set<int> simbolos;
            
            for (int j = 0; j < tamano; j++) {
                int valor = sudoku[i][j];
                
                // Ignorar celdas vacías
                if (valor == 0) continue;
                
                // Verificar rango válido [1, n²]
                if (valor < 1 || valor > tamano) {
                    return false;
                }
                
                // Verificar duplicados
                if (simbolos.count(valor) > 0) {
                    return false;
                }
                
                simbolos.insert(valor);
            }
        }
        
        return true;
    }
    
    /**
     * Valida que no haya símbolos repetidos en ninguna columna
     */
    bool validarColumnas() {
        for (int j = 0; j < tamano; j++) {
            set<int> simbolos;
            
            for (int i = 0; i < tamano; i++) {
                int valor = sudoku[i][j];
                
                // Ignorar celdas vacías
                if (valor == 0) continue;
                
                // Verificar rango válido [1, n²]
                if (valor < 1 || valor > tamano) {
                    return false;
                }
                
                // Verificar duplicados
                if (simbolos.count(valor) > 0) {
                    return false;
                }
                
                simbolos.insert(valor);
            }
        }
        
        return true;
    }
    
    /**
     * Valida que no haya símbolos repetidos en ningún subcuadrado n x n
     */
    bool validarSubcuadrados() {
        // Iterar sobre cada subcuadrado
        for (int filaBloque = 0; filaBloque < n; filaBloque++) {
            for (int colBloque = 0; colBloque < n; colBloque++) {
                set<int> simbolos;
                
                // Extraer símbolos del subcuadrado actual
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < n; j++) {
                        int fila = filaBloque * n + i;
                        int col = colBloque * n + j;
                        int valor = sudoku[fila][col];
                        
                        // Ignorar celdas vacías
                        if (valor == 0) continue;
                        
                        // Verificar rango válido [1, n²]
                        if (valor < 1 || valor > tamano) {
                            return false;
                        }
                        
                        // Verificar duplicados
                        if (simbolos.count(valor) > 0) {
                            return false;
                        }
                        
                        simbolos.insert(valor);
                    }
                }
            }
        }
        
        return true;
    }
    
public:
    /**
     * Constructor que lee el sudoku desde un archivo
     */
    ValidadorSudoku(const string& archivoEntrada) {
        leerSudoku(archivoEntrada);
    }
    
    /**
     * Lee un n-sudoku desde un archivo de texto
     */
    void leerSudoku(const string& archivo) {
        ifstream file(archivo);
        if (!file.is_open()) {
            throw runtime_error("No se pudo abrir el archivo: " + archivo);
        }
        
        vector<string> lineas;
        string linea;
        
        // Leer todas las líneas no vacías
        while (getline(file, linea)) {
            if (!linea.empty()) {
                lineas.push_back(linea);
            }
        }
        file.close();
        
        // Determinar n a partir del número de líneas
        tamano = lineas.size();
        n = sqrt(tamano);
        
        // Verificar que sea un cuadrado perfecto
        if (n * n != tamano) {
            throw runtime_error("El número de líneas debe ser un cuadrado perfecto");
        }
        
        // Calcular el ancho de cada símbolo
        int anchoSimbolo = calcularAnchoSimbolo();
        
        // Parsear el sudoku
        sudoku.resize(tamano, vector<int>(tamano));
        
        for (int i = 0; i < tamano; i++) {
            int col = 0;
            for (size_t pos = 0; pos < lineas[i].length() && col < tamano; pos += anchoSimbolo) {
                string simbolo = lineas[i].substr(pos, anchoSimbolo);
                
                // Eliminar espacios
                simbolo.erase(0, simbolo.find_first_not_of(" \t"));
                simbolo.erase(simbolo.find_last_not_of(" \t") + 1);
                
                // Verificar si es una celda vacía
                if (simbolo.empty() || simbolo.find('-') != string::npos) {
                    sudoku[i][col] = 0;
                } else {
                    try {
                        sudoku[i][col] = stoi(simbolo);
                    } catch (...) {
                        sudoku[i][col] = 0;
                    }
                }
                col++;
            }
        }
    }
    
    /**
     * Valida el sudoku completo
     */
    ResultadoValidacion validar() {
        ResultadoValidacion resultado;
        
        try {
            // Validar dimensiones
            if (!validarDimensiones()) {
                resultado.esValido = false;
                resultado.mensaje = "Error: Las dimensiones del sudoku no son validas";
                return resultado;
            }
            
            // Validar filas
            if (!validarFilas()) {
                resultado.esValido = false;
                resultado.mensaje = "Error: Hay símbolos repetidos en alguna fila";
                return resultado;
            }
            
            // Validar columnas
            if (!validarColumnas()) {
                resultado.esValido = false;
                resultado.mensaje = "Error: Hay simbolos repetidos en alguna columna";
                return resultado;
            }
            
            // Validar subcuadrados
            if (!validarSubcuadrados()) {
                resultado.esValido = false;
                resultado.mensaje = "Error: Hay simbolos repetidos en algún subcuadrado";
                return resultado;
            }
            
            resultado.esValido = true;
            resultado.mensaje = "El sudoku es valido";
            
        } catch (const exception& e) {
            resultado.esValido = false;
            resultado.mensaje = string("Error al procesar: ") + e.what();
        }
        
        return resultado;
    }
    
    /**
     * Escribe el sudoku en un archivo con el formato especificado
     */
    void escribirSudoku(const string& archivoSalida) {
        ofstream file(archivoSalida);
        if (!file.is_open()) {
            throw runtime_error("No se pudo crear el archivo: " + archivoSalida);
        }
        
        int anchoSimbolo = calcularAnchoSimbolo();
        
        for (int i = 0; i < tamano; i++) {
            for (int j = 0; j < tamano; j++) {
                if (sudoku[i][j] == 0) {
                    // Celda vacía
                    file << string(anchoSimbolo, '-');
                } else {
                    // Símbolo con ceros a la izquierda
                    file << setfill('0') << setw(anchoSimbolo) << sudoku[i][j];
                }
            }
            file << endl;
        }
        
        file.close();
    }
    
    /**
     * Imprime el sudoku en consola
     */
    void imprimir() {
        int anchoSimbolo = calcularAnchoSimbolo();
        
        for (int i = 0; i < tamano; i++) {
            for (int j = 0; j < tamano; j++) {
                if (sudoku[i][j] == 0) {
                    cout << string(anchoSimbolo, '-');
                } else {
                    cout << setfill('0') << setw(anchoSimbolo) << sudoku[i][j];
                }
                cout << " ";
            }
            cout << endl;
        }
    }
};

/**
 * Función principal - Ejemplo de uso
 */
int main(int argc, char* argv[]) {
    try {
        string archivoEntrada = "ejemplo_sudoku.txt";
        
        // Si se proporciona un archivo como argumento
        if (argc > 1) {
            archivoEntrada = argv[1];
        }
        
        // Crear validador y leer sudoku
        ValidadorSudoku validador(archivoEntrada);
        
        cout << "Sudoku leido:" << endl;
        validador.imprimir();
        cout << endl;
        
        // Validar
        ResultadoValidacion resultado = validador.validar();
        
        cout << "Resultado: " << resultado.mensaje << endl;
        cout << "¿Es valido? " << (resultado.esValido ? "Si" : "No") << endl;
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}