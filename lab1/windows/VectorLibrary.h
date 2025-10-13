#pragma once

#ifdef VECTORLIBRARY_EXPORTS
#define VECTORLIBRARY_API __declspec(dllexport)
#else
#define VECTORLIBRARY_API __declspec(dllimport)
#endif

#include "NumberLibrary.h"

class Vector {
private:
    NumberLibrary::Number x;
    NumberLibrary::Number y;

public:
    Vector(NumberLibrary::Number x_val, NumberLibrary::Number y_val);

    double getX() const;
    double getY() const;
    double getRadius() const;
    double getTheta() const;
    Vector add(const Vector& other) const;
};


extern "C" {
    VECTORLIBRARY_API void* vector_create(double x, double y);
    VECTORLIBRARY_API void vector_delete(void* vector);
    VECTORLIBRARY_API double vector_get_x(void* vector);
    VECTORLIBRARY_API double vector_get_y(void* vector);
    VECTORLIBRARY_API double vector_get_radius(void* vector);
    VECTORLIBRARY_API double vector_get_theta(void* vector);
    VECTORLIBRARY_API void* vector_add(void* vector1, void* vector2);
    VECTORLIBRARY_API void* vector_zero();
    VECTORLIBRARY_API void* vector_one_one();
}