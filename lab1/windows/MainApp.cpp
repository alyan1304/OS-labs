#include <iostream>
#include "VectorLibrary.h"

int main() {

    void* vec1 = vector_create(3.0, 4.0);
    void* vec2 = vector_create(1.0, 2.0);

    void* result = vector_add(vec1, vec2);

    std::cout << "Vector 1: (" << vector_get_x(vec1) << ", " << vector_get_y(vec1) << ")\n";
    std::cout << "Vector 2: (" << vector_get_x(vec2) << ", " << vector_get_y(vec2) << ")\n";
    std::cout << "Result: (" << vector_get_x(result) << ", " << vector_get_y(result) << ")\n";
    std::cout << "Polar: r=" << vector_get_radius(result) << ", θ=" << vector_get_theta(result) << "\n";

    vector_delete(vec1);
    vector_delete(vec2);
    vector_delete(result);

    return 0;
}