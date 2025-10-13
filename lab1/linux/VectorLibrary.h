#pragma once

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


void* vector_create(double x, double y);
void vector_delete(void* vector);
double vector_get_x(void* vector);
double vector_get_y(void* vector);
double vector_get_radius(void* vector);
double vector_get_theta(void* vector);
void* vector_add(void* vector1, void* vector2);
void* vector_zero();
void* vector_one_one();
