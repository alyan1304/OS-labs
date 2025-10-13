#pragma once

#ifdef VECTORLIBRARY_EXPORTS
#define VECTORLIBRARY_API __declspec(dllexport)
#else
#define VECTORLIBRARY_API __declspec(dllimport)
#endif

#include "NumberLibrary.h"

extern "C" VECTORLIBRARY_API void* vector_create(double x, double y);
extern "C" VECTORLIBRARY_API void vector_delete(void* vector);
extern "C" VECTORLIBRARY_API double vector_get_x(void* vector);
extern "C" VECTORLIBRARY_API double vector_get_y(void* vector);
extern "C" VECTORLIBRARY_API double vector_get_radius(void* vector);
extern "C" VECTORLIBRARY_API double vector_get_theta(void* vector);
extern "C" VECTORLIBRARY_API void* vector_add(void* vector1, void* vector2);
extern "C" VECTORLIBRARY_API void* vector_zero();
extern "C" VECTORLIBRARY_API void* vector_one_one();