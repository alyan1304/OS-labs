#pragma once
namespace NumberLibrary
{
    class Number
    {
    private:
        double value;

    public:
        Number();
        Number(double val);

        Number operator+(const Number& other) const;
        Number operator-(const Number& other) const;
        Number operator*(const Number& other) const;
        Number operator/(const Number& other) const;

        Number& operator+=(const Number& other);
        Number& operator-=(const Number& other);
        Number& operator*=(const Number& other);
        Number& operator/=(const Number& other);

        bool operator==(const Number& other) const;
        bool operator!=(const Number& other) const;

        double getValue() const;
        void setValue(double val);
    };


    extern const Number ZERO;
    extern const Number ONE;
    extern Number createNumber(double value);
}


