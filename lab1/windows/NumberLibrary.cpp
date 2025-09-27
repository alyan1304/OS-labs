#include "NumberLibrary.h"

namespace NumberLibrary
{
    const Number ZERO(0.0);
    const Number ONE(1.0);

    Number::Number() : value(0.0) {}

    Number::Number(double val) : value(val) {}

    Number Number::operator+(const Number& other) const
    {
        return Number(value + other.value);
    }

    Number Number::operator-(const Number& other) const
    {
        return Number(value - other.value);
    }

    Number Number::operator*(const Number& other) const
    {
        return Number(value * other.value);
    }

    Number Number::operator/(const Number& other) const
    {
        if (other.value == 0.0) {
            return Number(0.0);
        }
        return Number(value / other.value);
    }

    Number& Number::operator+=(const Number& other)
    {
        value += other.value;
        return *this;
    }

    Number& Number::operator-=(const Number& other)
    {
        value -= other.value;
        return *this;
    }

    Number& Number::operator*=(const Number& other)
    {
        value *= other.value;
        return *this;
    }

    Number& Number::operator/=(const Number& other)
    {
        if (other.value != 0.0) {
            value /= other.value;
        }
        return *this;
    }

    bool Number::operator==(const Number& other) const
    {
        return value == other.value;
    }

    bool Number::operator!=(const Number& other) const
    {
        return value != other.value;
    }

    double Number::getValue() const
    {
        return value;
    }

    void Number::setValue(double val)
    {
        value = val;
    }

    Number createNumber(double value)
    {
        return Number(value);
    }
}