#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string line;
    if (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        int x;
        while (iss >> x) {
            std::cout << x * 7 << " ";
        }
    }
    std::cout << std::endl;
    return 0;
}