#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <set>

using namespace std;

class SudokuGenerator {
private:
    int n;  // Size of the Sudoku (n x n grid)
    int subgridSize; // Size of the subgrids (e.g., for 9x9 grid, subgridSize = 3)
    vector<vector<int>> grid; // Sudoku grid
    random_device rd;
    mt19937 gen;

public:
    SudokuGenerator(int size) : n(size), subgridSize(sqrt(size)), gen(rd()) {
        grid.resize(n, vector<int>(n, 0)); // Initialize the grid with zeros
    }

	// Helper function to print the grid
	void printGrid() {
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				if (grid[i][j] == 0) {
					cout << "--";  // Empty cell, represented by "--"
				} else {
					cout << grid[i][j];  // Print the number directly without spacing
				}
			}
			cout << endl;
		}
	}

    // Function to check if a number can be placed at position (row, col)
    bool isValidPlacement(int row, int col, int num) {
        for (int i = 0; i < n; ++i) {
            if (grid[row][i] == num || grid[i][col] == num) {
                return false;
            }
        }

        int startRow = (row / subgridSize) * subgridSize;
        int startCol = (col / subgridSize) * subgridSize;
        for (int i = startRow; i < startRow + subgridSize; ++i) {
            for (int j = startCol; j < startCol + subgridSize; ++j) {
                if (grid[i][j] == num) {
                    return false;
                }
            }
        }
        return true;
    }

    // Function to fill the diagonal subgrids with random values
    void fillDiagonalSubgrids() {
        for (int subgridIndex = 0; subgridIndex < n; subgridIndex += subgridSize) {
            set<int> usedNumbers;
            for (int i = subgridIndex; i < subgridIndex + subgridSize; ++i) {
                for (int j = subgridIndex; j < subgridIndex + subgridSize; ++j) {
                    vector<int> availableNums;
                    for (int num = 1; num <= n; ++num) {
                        if (usedNumbers.find(num) == usedNumbers.end()) {
                            availableNums.push_back(num);
                        }
                    }
                    uniform_int_distribution<> dis(0, availableNums.size() - 1);
                    int selectedNum = availableNums[dis(gen)];
                    grid[i][j] = selectedNum;
                    usedNumbers.insert(selectedNum);
                }
            }
        }
    }

    // Function to swap cells randomly
    void swapCells(int swapPercentage) {
        vector<pair<int, int>> filledCells;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (grid[i][j] != 0) {
                    filledCells.push_back({i, j});
                }
            }
        }
        shuffle(filledCells.begin(), filledCells.end(), gen);

        int numCellsToSwap = filledCells.size() * swapPercentage / 100;
        uniform_int_distribution<> dis(0, filledCells.size() - 1);

        for (int swapCount = 0; swapCount < numCellsToSwap; ++swapCount) {
            int idx1 = swapCount;
            int row1 = filledCells[idx1].first;
            int col1 = filledCells[idx1].second;

            vector<pair<int, int>> emptyCells;
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (grid[i][j] == 0) {
                        emptyCells.push_back({i, j});
                    }
                }
            }

            if (emptyCells.empty()) return;

            int idx2 = dis(gen) % emptyCells.size();
            int row2 = emptyCells[idx2].first;
            int col2 = emptyCells[idx2].second;

            swap(grid[row1][col1], grid[row2][col2]);
        }
    }

    // Generate the Sudoku puzzle
    void generate(int swapPercentage = 50) {
        fillDiagonalSubgrids();
        swapCells(swapPercentage);

        // Only one final output
        cout << "Generated Sudoku Puzzle:\n";
        printGrid();
    }
};

int main() {
    int k;
    cout << "Enter a number (e.g., 3 for a 9x9 grid, 4 for a 16x16 grid): ";
    cin >> k;

    int n = k * k;

    try {
        SudokuGenerator generator(n);
        generator.generate(50);
    } catch (const invalid_argument& e) {
        cout << e.what() << endl;
    }

    return 0;
}
