# Компилятор и флаги
CXX = g++
CXXFLAGS = -I./include -Wall -Wextra -pedantic -std=c++17 -g -fPIC
LDFLAGS = -L./lib
LDLIBS = -lVector

# Директории
INCLUDE_DIR = ./include
SRC_DIR = ./src
LIB_DIR = ./lib
BIN_DIR = ./bin
NUMBER_DIR = $(SRC_DIR)/NumberLibrary
VECTOR_DIR = $(SRC_DIR)/VectorLibrary

# Основные цели
.PHONY: all clean run help

# Цель по умолчанию
all: bin/main_program

# Компиляция Number объектов (всегда с -fPIC)
$(NUMBER_DIR)/NumberLibrary.o: $(NUMBER_DIR)/NumberLibrary.cpp include/NumberLibrary.h
	@echo "Компиляция NumberLibrary..."
	@mkdir -p $(NUMBER_DIR)
	$(CXX) $(CXXFLAGS) -c -o $(NUMBER_DIR)/NumberLibrary.o $(NUMBER_DIR)/NumberLibrary.cpp

# Динамическая библиотека Vector (включает Number код)
lib/libVector.so: $(VECTOR_DIR)/VectorLibrary.cpp include/VectorLibrary.h $(NUMBER_DIR)/NumberLibrary.o
	@echo "Сборка динамической библиотеки Vector..."
	@mkdir -p lib
	$(CXX) $(CXXFLAGS) -c -o $(VECTOR_DIR)/VectorLibrary.o $(VECTOR_DIR)/VectorLibrary.cpp
	$(CXX) -shared -o lib/libVector.so $(VECTOR_DIR)/VectorLibrary.o $(NUMBER_DIR)/NumberLibrary.o

# Исполняемый файл
bin/main_program: src/MainApp/MainApp.cpp lib/libVector.so
	@echo "Сборка исполняемого файла..."
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o bin/main_program src/MainApp/MainApp.cpp $(LDFLAGS) $(LDLIBS)

# Очистка
clean:
	@echo "Очистка проекта..."
	rm -f $(NUMBER_DIR)/*.o $(VECTOR_DIR)/*.o
	rm -f lib/libVector.so bin/main_program

# Запуск программы
run: bin/main_program
	@echo "Запуск программы..."
	LD_LIBRARY_PATH=./lib ./bin/main_program

help:
	@echo "Доступные команды:"
	@echo "  make all    - Полная сборка проекта"
	@echo "  make run    - Сборка и запуск программы"
	@echo "  make clean  - Очистка скомпилированных файлов"
	@echo "  make help   - Показать эту справку"
