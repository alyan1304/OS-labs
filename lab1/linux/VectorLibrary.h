#pragma once


#include "NumberLibrary.h"

extern "C" void* vector_create(double x, double y);
extern "C" void vector_delete(void* vector);
extern "C" double vector_get_x(void* vector);
extern "C" double vector_get_y(void* vector);
extern "C" double vector_get_radius(void* vector);
extern "C" double vector_get_theta(void* vector);
extern "C" void* vector_add(void* vector1, void* vector2);
extern "C" void* vector_zero();
extern "C" void* vector_one_one();
