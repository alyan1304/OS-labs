#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <chrono>
#include <cstdlib>

class Matrix {
private:
    std::vector<std::vector<int>> data;
    int rows;
    int cols;

public:
    Matrix(int n){
        rows = n;
        cols = n;
        data = std::vector<std::vector<int>>(n, std::vector<int>(n, 0));
      }
    Matrix(int _rows, int _cols){
        rows = _rows;
        cols = _cols;
        data = std::vector<std::vector<int>>(rows, std::vector<int>(cols, 0));
    }
    int getRows() const{
        return rows;
    }
    int getCols() const{
        return cols;
    }
    int& operator()(int i, int j) {
        return data[i][j];
    }
    const int& operator()(int i, int j) const{
    return data[i][j]; 
    } 
    void fillRandomly(){
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                data[i][j] = rand() % 10;
            }
        }
    }
    void print() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                std::cout << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

class Block {
private:
    std::vector<std::vector<int>> data;
    int startRow;
    int startCol;
    int numberOfRows;
    int numberOfCols;

public:
    Block(int _startRow, int _startCol, int blockSize, int matrixSize){
        startRow = _startRow;
        startCol = _startCol;
        numberOfRows = (startRow + blockSize <= matrixSize) ? blockSize : matrixSize - startRow;
        numberOfCols = (startCol + blockSize <= matrixSize) ? blockSize : matrixSize - startCol;
        data.resize(numberOfRows, std::vector<int>(numberOfCols, 0));
    }
    int getStartRow() const { return startRow; }
    int getStartCol() const { return startCol; }
    int getNumberOFRows() const { return numberOfRows; }
    int getNumberOFCols() const { return numberOfCols; }
    int& operator()(int i, int j) { return data[i][j]; }
    void fillFromMatrix(Matrix& matrix) {
        for (int i = 0; i < numberOfRows; ++i) {
            for (int j = 0; j < numberOfCols; ++j) {
                int row = startRow + i;
                int col = startCol + j;
                data[i][j] = matrix(row, col);
            }
        }
    }
    void addToMatrix(Matrix& matrix) const {
        for (int i = 0; i < numberOfRows; ++i) {
            for (int j = 0; j < numberOfCols; ++j) {
                int row = startRow + i;
                int col = startCol + j;
                matrix(row, col) += data[i][j];
            }
        }
    }
    void clear() {
        for (int i = 0; i < numberOfRows; ++i) {
            for (int j = 0; j < numberOfCols; ++j) {
                data[i][j] = 0;
            } 
        }
    }
};

class MatrixMultiplier {
private:
    Matrix A, B, res;
    int blockSize;
    int numberOfThreads;
    std::mutex myMutex;
    int matrixSize;

    void multiplyBlocks(Block& A, Block& B, Block& result){
        int rowsA = A.getNumberOFRows();
        int rowsB = B.getNumberOFRows();
        int colsA = A.getNumberOFCols();
        int colsB = B.getNumberOFCols();
        if (colsA != rowsB) {
            throw std::invalid_argument("Block dimensions incompatible for multiplication");
        }

        
        int common = colsA;
        for (int i = 0; i < rowsA; i++) {
            for (int j = 0; j < colsB; j++) {
                result(i, j) = 0;
            }
        }
        for(int i = 0; i < rowsA; i++) {
            for (int j = 0; j < colsB; j++) {
                for(int k = 0; k < common; k++) {
                    result(i, j) += A(i, k) * B(k, j);
                }
            }
        }
    }

    void getAndMultiplyBlocks(int rowIndexA, int colIndexA, int rowIndexB, int colIndexB) {
        int startRowA = rowIndexA * blockSize;
        int startRowB = rowIndexB * blockSize;
        int startColB = colIndexB * blockSize;
        int startColA = colIndexA * blockSize;

        Block blockA(startRowA, startColA, blockSize, matrixSize);
        Block blockB(startRowB, startColB, blockSize, matrixSize);
        Block temp(startRowA, startColB, blockSize, matrixSize);

        blockA.fillFromMatrix(A);
        blockB.fillFromMatrix(B);
        
        multiplyBlocks(blockA, blockB, temp);
        
        std::lock_guard<std::mutex> lock(myMutex);
        temp.addToMatrix(res);
    }

public:
    MatrixMultiplier(int _matrixSize, int _blockSize, const Matrix& _A, const Matrix& _B) 
        : A(_A), B(_B), res(_matrixSize), 
          blockSize(_blockSize), numberOfThreads(0), matrixSize(_matrixSize) {
    }
    Matrix multiply(){
        for(int i = 0; i < matrixSize; i++) {
            for(int j = 0; j < matrixSize; j++) {
                res(i, j) = 0;
            }
        }
        int numberOFBlocks = (matrixSize + blockSize - 1)/blockSize;
        std::vector<std::thread> threads;
        for(int rowIndexA = 0; rowIndexA < numberOFBlocks; rowIndexA++) {
            for(int colIndexA = 0; colIndexA < numberOFBlocks; colIndexA++) {
                for(int colIndexB = 0; colIndexB < numberOFBlocks; colIndexB++) {
                    int rowIndexB = colIndexA;
                    threads.emplace_back([this, rowIndexA, colIndexA, rowIndexB, colIndexB] {
                        this->getAndMultiplyBlocks(rowIndexA, colIndexA, rowIndexB, colIndexB);
                    });
                }
            }
        }
        numberOfThreads = threads.size();
        for(std::thread& thread : threads) {
            thread.join();
        }
        return res;
    }

    int getNumberOfThreads() const {
        return numberOfThreads;
    }

    Matrix multiplyBasically() const { 
        int n = A.getRows(); 
        Matrix res(n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                res(i, j) = 0; 
                for (int k = 0; k < n; ++k) {
                    res(i, j) += A(i, k) * B(k, j);
                }
            }
        }
        return res;
    }
};

int main() {
    int size;

    do {
        std::cout << "Input matrix size (>=5)";
        std::cin >> size;
        if (size < 5) {
            std::cout << "The size can't be less than 5" << std::endl;

        }
    } while (size < 5);

    Matrix A(size);
    Matrix B(size);
    srand(time(0));
    A.fillRandomly();
    B.fillRandomly();

if (size <= 15) {
    std::cout << "Matrix A: " << std::endl;
    A.print();
    std::cout << std::endl << "Matrix B:" << std::endl;
    B.print();
} 

std::cout << "Basic multiplication:" << std::endl;
auto startSimple = std::chrono::high_resolution_clock::now();
MatrixMultiplier basic(size, size, A, B);
Matrix resultSimple = basic.multiplyBasically();
auto endSimple = std::chrono::high_resolution_clock::now();
long long simpleTimeMicro = std::chrono::duration_cast<std::chrono::microseconds>(endSimple - startSimple).count();
double simpleTimeMs = simpleTimeMicro / 1000.0;
std::cout << "Time: " << simpleTimeMs << " ms" << std::endl;
    
std::cout << "Block multiplication:" << std::endl;
for (int blockSize = 1; blockSize <= size; blockSize++) {
    MatrixMultiplier multiplier(size, blockSize, A, B);
    auto start = std::chrono::high_resolution_clock::now();
    Matrix result = multiplier.multiply();
    auto end = std::chrono::high_resolution_clock::now();
    
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    double timeMs = time / 1000.0;
    int numberOfthreads = multiplier.getNumberOfThreads();
    std::cout << "Block size: " << blockSize << std::endl;
    std::cout << "Number of threads: " << numberOfthreads << std::endl;
    std::cout << "Time: " << timeMs << " ms" << std::endl;
    bool correct = true;
        for (int i = 0; i < size && correct; ++i) {
            for (int j = 0; j < size && correct; ++j) {
                if (result(i, j) != resultSimple(i, j)) {
                    correct = false;
                }
            }
        }
        if (correct == false) {
            std::cout << "Wrong result!" << std::endl;
        }
    }
    if (size <= 15) {
        std::cout << "\nMultiplication result:" << std::endl;
        resultSimple.print();
    } else {
        std::cout << "Calculated sucessfully. Result too big to print.";
    }

    return 0;

}


