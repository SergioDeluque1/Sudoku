#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cstring>

using namespace std;
using namespace chrono;

struct SudokuConEtiqueta {
    string etiqueta;
    vector<vector<int>> tablero;
    int n;
    int tamano;
};

/**
 * Resolvedor híbrido: propagación avanzada + backtracking optimizado
 */
class ResolvedorSudokuHibrido {
private:
    int sudoku[25][25];
    bitset<26> filaCandidatos[25];
    bitset<26> colCandidatos[25];
    bitset<26> bloqueCandidatos[25];
    int n, tamano;
    long long nodosExplorados;
    int celdasVacias;
    
    int obtenerBloque(int fila, int col) {
        return (fila / n) * n + (col / n);
    }
    
    bitset<26> obtenerCandidatos(int fila, int col) {
        int bloque = obtenerBloque(fila, col);
        return filaCandidatos[fila] & colCandidatos[col] & bloqueCandidatos[bloque];
    }
    
    void colocarValor(int fila, int col, int valor) {
        sudoku[fila][col] = valor;
        int bloque = obtenerBloque(fila, col);
        filaCandidatos[fila].reset(valor);
        colCandidatos[col].reset(valor);
        bloqueCandidatos[bloque].reset(valor);
    }
    
    void quitarValor(int fila, int col, int valor) {
        sudoku[fila][col] = 0;
        int bloque = obtenerBloque(fila, col);
        filaCandidatos[fila].set(valor);
        colCandidatos[col].set(valor);
        bloqueCandidatos[bloque].set(valor);
    }
    
    /**
     * Propagación de restricciones (solo para sudokus grandes)
     */
    bool propagarRestricciones() {
        if (tamano <= 9) return true; // Skip para sudokus pequeños
        
        bool cambios = true;
        int iteraciones = 0;
        
        while (cambios && iteraciones < 10) {
            cambios = false;
            iteraciones++;
            
            // Naked Singles
            for (int i = 0; i < tamano; i++) {
                for (int j = 0; j < tamano; j++) {
                    if (sudoku[i][j] == 0) {
                        bitset<26> candidatos = obtenerCandidatos(i, j);
                        int numCandidatos = candidatos.count();
                        
                        if (numCandidatos == 0) {
                            return false;
                        }
                        
                        if (numCandidatos == 1) {
                            for (int v = 1; v <= tamano; v++) {
                                if (candidatos[v]) {
                                    colocarValor(i, j, v);
                                    celdasVacias--;
                                    cambios = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        return true;
    }
    
    /**
     * Backtracking optimizado con MRV
     */
    bool resolverBacktracking() {
        nodosExplorados++;
        
        // Propagar restricciones periódicamente para sudokus grandes
        if (tamano > 16 && nodosExplorados % 100 == 0) {
            if (!propagarRestricciones()) {
                return false;
            }
        }
        
        if (celdasVacias == 0) {
            return true;
        }
        
        // MRV: Encontrar celda con menos candidatos
        int mejorFila = -1, mejorCol = -1;
        int minCandidatos = tamano + 1;
        
        for (int i = 0; i < tamano; i++) {
            for (int j = 0; j < tamano; j++) {
                if (sudoku[i][j] == 0) {
                    int numCandidatos = obtenerCandidatos(i, j).count();
                    
                    if (numCandidatos == 0) {
                        return false;
                    }
                    
                    if (numCandidatos < minCandidatos) {
                        minCandidatos = numCandidatos;
                        mejorFila = i;
                        mejorCol = j;
                        
                        if (minCandidatos == 1) {
                            goto encontrado;
                        }
                    }
                }
            }
        }
        
        encontrado:
        
        if (mejorFila == -1) {
            return celdasVacias == 0;
        }
        
        bitset<26> candidatos = obtenerCandidatos(mejorFila, mejorCol);
        
        // Probar cada candidato
        for (int v = 1; v <= tamano; v++) {
            if (candidatos[v]) {
                // Guardar estado
                bitset<26> filaBackup = filaCandidatos[mejorFila];
                bitset<26> colBackup = colCandidatos[mejorCol];
                int bloque = obtenerBloque(mejorFila, mejorCol);
                bitset<26> bloqueBackup = bloqueCandidatos[bloque];
                
                colocarValor(mejorFila, mejorCol, v);
                celdasVacias--;
                
                if (resolverBacktracking()) {
                    return true;
                }
                
                // Restaurar estado
                quitarValor(mejorFila, mejorCol, v);
                filaCandidatos[mejorFila] = filaBackup;
                colCandidatos[mejorCol] = colBackup;
                bloqueCandidatos[bloque] = bloqueBackup;
                celdasVacias++;
            }
        }
        
        return false;
    }
    
public:
    ResolvedorSudokuHibrido() : nodosExplorados(0), celdasVacias(0) {
        memset(sudoku, 0, sizeof(sudoku));
    }
    
    void cargarSudoku(const vector<vector<int>>& tablero, int nParam) {
        n = nParam;
        tamano = n * n;
        nodosExplorados = 0;
        celdasVacias = 0;
        
        // Inicializar bitsets
        for (int i = 0; i < 25; i++) {
            filaCandidatos[i].reset();
            colCandidatos[i].reset();
            bloqueCandidatos[i].reset();
        }
        
        for (int i = 0; i < tamano; i++) {
            for (int v = 1; v <= tamano; v++) {
                filaCandidatos[i].set(v);
                colCandidatos[i].set(v);
                bloqueCandidatos[i].set(v);
            }
        }
        
        memset(sudoku, 0, sizeof(sudoku));
        
        // Cargar tablero
        for (int i = 0; i < tamano; i++) {
            for (int j = 0; j < tamano; j++) {
                sudoku[i][j] = tablero[i][j];
                
                if (tablero[i][j] == 0) {
                    celdasVacias++;
                } else {
                    int valor = tablero[i][j];
                    int bloque = obtenerBloque(i, j);
                    filaCandidatos[i].reset(valor);
                    colCandidatos[j].reset(valor);
                    bloqueCandidatos[bloque].reset(valor);
                }
            }
        }
    }
    
    bool validarEstadoInicial() {
        // Verificar filas
        for (int i = 0; i < tamano; i++) {
            bitset<26> vistos;
            for (int j = 0; j < tamano; j++) {
                int valor = sudoku[i][j];
                if (valor != 0) {
                    if (valor < 1 || valor > tamano) return false;
                    if (vistos[valor]) return false;
                    vistos[valor] = 1;
                }
            }
        }
        
        // Verificar columnas
        for (int j = 0; j < tamano; j++) {
            bitset<26> vistos;
            for (int i = 0; i < tamano; i++) {
                int valor = sudoku[i][j];
                if (valor != 0) {
                    if (vistos[valor]) return false;
                    vistos[valor] = 1;
                }
            }
        }
        
        // Verificar bloques
        for (int b = 0; b < tamano; b++) {
            bitset<26> vistos;
            int filaInicio = (b / n) * n;
            int colInicio = (b % n) * n;
            
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    int valor = sudoku[filaInicio + i][colInicio + j];
                    if (valor != 0) {
                        if (vistos[valor]) return false;
                        vistos[valor] = 1;
                    }
                }
            }
        }
        
        return true;
    }
    
    bool resolverSudoku() {
        if (!validarEstadoInicial()) {
            return false;
        }
        
        // Aplicar propagación inicial para sudokus grandes
        if (tamano > 16) {
            if (!propagarRestricciones()) {
                return false;
            }
        }
        
        return resolverBacktracking();
    }
    
    vector<vector<int>> obtenerSolucion() const {
        vector<vector<int>> resultado(tamano, vector<int>(tamano));
        for (int i = 0; i < tamano; i++) {
            for (int j = 0; j < tamano; j++) {
                resultado[i][j] = sudoku[i][j];
            }
        }
        return resultado;
    }
    
    long long obtenerNodosExplorados() const {
        return nodosExplorados;
    }
};

class ProcesadorMultipleSudoku {
private:
    vector<SudokuConEtiqueta> sudokus;
    
    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
    
    vector<int> parsearLinea(const string& linea, int anchoSimbolo) {
        vector<int> fila;
        
        for (size_t i = 0; i < linea.length(); i += anchoSimbolo) {
            string simbolo = linea.substr(i, anchoSimbolo);
            
            bool esVacio = false;
            for (char c : simbolo) {
                if (c == '-') {
                    esVacio = true;
                    break;
                }
            }
            
            if (esVacio) {
                fila.push_back(0);
            } else {
                try {
                    int valor = stoi(simbolo);
                    fila.push_back(valor);
                } catch (...) {
                    fila.push_back(0);
                }
            }
        }
        
        return fila;
    }
    
public:
    void leerArchivo(const string& archivo) {
        ifstream file(archivo);
        if (!file.is_open()) {
            throw runtime_error("No se pudo abrir el archivo: " + archivo);
        }
        
        string linea;
        string etiquetaActual = "";
        vector<string> lineasSudoku;
        int lineasEsperadas = -1;
        
        while (getline(file, linea)) {
            linea = trim(linea);
            
            // Verificar si es una etiqueta
            if (linea.find("Sudoku") != string::npos || 
                linea.find("sudoku") != string::npos ||
                (linea.length() > 0 && linea.back() == '#')) {
                
                // Procesar sudoku anterior si existe
                if (!lineasSudoku.empty() && !etiquetaActual.empty()) {
                    procesarSudoku(etiquetaActual, lineasSudoku);
                    lineasSudoku.clear();
                }
                
                etiquetaActual = linea;
                lineasEsperadas = -1;
            }
            else if (!linea.empty() && !etiquetaActual.empty()) {
                lineasSudoku.push_back(linea);
                
                // Determinar tamaño esperado
                if (lineasEsperadas == -1) {
                    for (int n = 2; n <= 10; n++) {
                        int t = n * n;
                        int ancho = floor(log10(t) + 1);
                        if (linea.length() == t * ancho) {
                            lineasEsperadas = t;
                            break;
                        }
                    }
                }
                
                // Procesar cuando tengamos todas las líneas
                if (lineasEsperadas > 0 && lineasSudoku.size() == lineasEsperadas) {
                    procesarSudoku(etiquetaActual, lineasSudoku);
                    lineasSudoku.clear();
                    etiquetaActual = "";
                    lineasEsperadas = -1;
                }
            }
        }
        
        // Procesar último sudoku
        if (!lineasSudoku.empty() && !etiquetaActual.empty()) {
            procesarSudoku(etiquetaActual, lineasSudoku);
        }
        
        file.close();
    }
    
    void procesarSudoku(const string& etiqueta, const vector<string>& lineas) {
        if (lineas.empty()) return;
        
        SudokuConEtiqueta sudoku;
        sudoku.etiqueta = etiqueta;
        sudoku.tamano = lineas.size();
        sudoku.n = sqrt(sudoku.tamano);
        
        if (sudoku.n * sudoku.n != sudoku.tamano) {
            cerr << "Advertencia: " << etiqueta << " dimensiones invalidas" << endl;
            return;
        }
        
        if (sudoku.tamano > 25) {
            cerr << "Advertencia: " << etiqueta << " excede tamaño maximo" << endl;
            return;
        }
        
        int anchoSimbolo = floor(log10(sudoku.tamano) + 1);
        
        sudoku.tablero.resize(sudoku.tamano);
        for (int i = 0; i < sudoku.tamano; i++) {
            sudoku.tablero[i] = parsearLinea(lineas[i], anchoSimbolo);
        }
        
        sudokus.push_back(sudoku);
    }
    
    void resolverTodos(const string& archivoSalida) {
        ofstream file(archivoSalida);
        if (!file.is_open()) {
            throw runtime_error("No se pudo crear archivo: " + archivoSalida);
        }
        
        ResolvedorSudokuHibrido resolvedor;
        
        for (size_t idx = 0; idx < sudokus.size(); idx++) {
            auto& sudoku = sudokus[idx];
            
            cout << "Resolviendo: " << sudoku.etiqueta << " (" 
                 << sudoku.tamano << "x" << sudoku.tamano << ") ... " << flush;
            
            file << sudoku.etiqueta << endl;
            
            resolvedor.cargarSudoku(sudoku.tablero, sudoku.n);
            
            auto inicio = high_resolution_clock::now();
            bool resuelto = resolvedor.resolverSudoku();
            auto fin = high_resolution_clock::now();
            auto duracion = duration_cast<milliseconds>(fin - inicio);
            
            if (resuelto) {
                cout << "Resuelto (" << duracion.count() / 1000.0 << "s, " 
                     << resolvedor.obtenerNodosExplorados() << " nodos)" << endl;
                
                escribirSudoku(file, resolvedor.obtenerSolucion(), sudoku.n);
            } else {
                cout << "Sin solucion" << endl;
                file << "Sin solucion" << endl;
            }
            
            if (idx < sudokus.size() - 1) {
                file << endl;
            }
        }
        
        file.close();
        cout << "\nSoluciones guardadas en: " << archivoSalida << endl;
    }
    
    void escribirSudoku(ofstream& file, const vector<vector<int>>& tablero, int n) {
        int tamano = n * n;
        int anchoSimbolo = floor(log10(tamano) + 1);
        
        for (int i = 0; i < tamano; i++) {
            for (int j = 0; j < tamano; j++) {
                if (tablero[i][j] == 0) {
                    file << string(anchoSimbolo, '-');
                } else {
                    file << setfill('0') << setw(anchoSimbolo) << tablero[i][j];
                }
            }
            file << endl;
        }
    }
    
    int obtenerCantidadSudokus() const {
        return sudokus.size();
    }
};

int main(int argc, char* argv[]) {
    try {
        string archivoEntrada = "sudokus_entrada.txt";
        string archivoSalida = "sudokus_solucion.txt";
        
        if (argc > 1) archivoEntrada = argv[1];
        if (argc > 2) archivoSalida = argv[2];
        
        cout << "=== RESOLVEDOR HIBRIDO N-SUDOKU ===" << endl << endl;
        
        ProcesadorMultipleSudoku procesador;
        
        cout << "Leyendo: " << archivoEntrada << endl;
        procesador.leerArchivo(archivoEntrada);
        
        cout << "Sudokus encontrados: " << procesador.obtenerCantidadSudokus() << endl << endl;
        
        if (procesador.obtenerCantidadSudokus() == 0) {
            cout << "No se encontraron sudokus." << endl;
            return 1;
        }
        
        procesador.resolverTodos(archivoSalida);
        
        cout << "\nCompletado" << endl;
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}