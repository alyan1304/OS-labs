
#include "VectorLibrary.h"
#include <cmath>

class Vector {
private:
    NumberLibrary::Number x;
    NumberLibrary::Number y;

public:
    Vector(double x_val, double y_val) : x(x_val), y(y_val) {}

    double getX() const { return x.getValue(); }
    double getY() const { return y.getValue(); }

    double getRadius() const {
        return std::sqrt(x.getValue() * x.getValue() +
            y.getValue() * y.getValue());
    }

    double getTheta() const {
        return std::atan2(y.getValue(), x.getValue());
    }

    Vector add(const Vector& other) const {
        return Vector(x.getValue() + other.x.getValue(),
            y.getValue() + other.y.getValue());
    }
};

static Vector* zero_vector = nullptr;
static Vector* one_one_vector = nullptr;

extern "C" void* vector_create(double x, double y) {
    return new Vector(x, y);
}

extern "C" void vector_delete(void* vector) {
    delete static_cast<Vector*>(vector);
}

extern "C" double vector_get_x(void* vector) {
    return static_cast<Vector*>(vector)->getX();
}

extern "C" double vector_get_y(void* vector) {
    return static_cast<Vector*>(vector)->getY();
}

extern "C" double vector_get_radius(void* vector) {
    return static_cast<Vector*>(vector)->getRadius();
}

extern "C" double vector_get_theta(void* vector) {
    return static_cast<Vector*>(vector)->getTheta();
}

extern "C" void* vector_add(void* vector1, void* vector2) {
    Vector* vec1 = static_cast<Vector*>(vector1);
    Vector* vec2 = static_cast<Vector*>(vector2);
    return new Vector(vec1->add(*vec2));
}

extern "C" void* vector_zero() {
    if (!zero_vector) {
        zero_vector = new Vector(0.0, 0.0);
    }
    return zero_vector;
}

extern "C" void* vector_one_one() {
    if (!one_one_vector) {
        one_one_vector = new Vector(1.0, 1.0);
    }
    return one_one_vector;
}
