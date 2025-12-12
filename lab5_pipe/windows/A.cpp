#include <iostream>
#include <sstream>
#include <string>


int main() {
    int N = 18;
    std::string line;
    if (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        int x;
        while (iss >> x) {
            std::cout << x + N << " ";
        }
    }
    std::cout << std::endl;
    return 0;
}