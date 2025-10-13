
#include "VectorLibrary.h"
#include <cmath>

static Vector* zero_vector = nullptr;
static Vector* one_one_vector = nullptr;

Vector::Vector(NumberLibrary::Number x_val, NumberLibrary::Number y_val)
    : x(x_val), y(y_val) {
}

double Vector::getX() const {
    return x.getValue();
}

double Vector::getY() const {
    return y.getValue();
}

double Vector::getRadius() const {
    return std::sqrt(x.getValue() * x.getValue() +
        y.getValue() * y.getValue());
}

double Vector::getTheta() const {
    return std::atan2(y.getValue(), x.getValue());
}

Vector Vector::add(const Vector& other) const {
    return Vector(x.getValue() + other.x.getValue(),
        y.getValue() + other.y.getValue());
}


void* vector_create(double x, double y) {
    return new Vector(x, y);
}

void vector_delete(void* vector) {
    delete static_cast<Vector*>(vector);
}

double vector_get_x(void* vector) {
    return static_cast<Vector*>(vector)->getX();
}

double vector_get_y(void* vector) {
    return static_cast<Vector*>(vector)->getY();
}

double vector_get_radius(void* vector) {
    return static_cast<Vector*>(vector)->getRadius();
}

double vector_get_theta(void* vector) {
    return static_cast<Vector*>(vector)->getTheta();
}

void* vector_add(void* vector1, void* vector2) {
    Vector* vec1 = static_cast<Vector*>(vector1);
    Vector* vec2 = static_cast<Vector*>(vector2);
    Vector result = vec1->add(*vec2);
    return new Vector(result.getX(), result.getY());
}

void* vector_zero() {
    if (!zero_vector) {
        zero_vector = new Vector(0.0, 0.0);
    }
    return zero_vector;
}

void* vector_one_one() {
    if (!one_one_vector) {
        one_one_vector = new Vector(1.0, 1.0);
    }
    return one_one_vector;
}