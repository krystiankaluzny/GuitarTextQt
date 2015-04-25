#include "transformata.h"
#include <cmath>
#include <iostream>

ComplexNumber::ComplexNumber()
    : x(0.0),
      y(0.0)
{
}

ComplexNumber::ComplexNumber(double real)
    : x(real),
      y(0.0)
{
}

ComplexNumber::ComplexNumber(double real, double imaginary)
    : x(real),
      y(imaginary)
{
}

ComplexNumber::ComplexNumber(const ComplexNumber &a)
{
    x = a.x;
    y = a.y;
}

ComplexNumber &ComplexNumber::operator=(const ComplexNumber &number)
{
    x = number.x;
    y = number.y;
    return *this;
}

ComplexNumber ComplexNumber::operator+(const ComplexNumber &number) const
{
    return ComplexNumber(x + number.x, y + number.y);
}

ComplexNumber &ComplexNumber::operator+=(const ComplexNumber &number)
{
    x+=number.x;
    y+=number.y;
    return *this;
}

ComplexNumber ComplexNumber::operator-(const ComplexNumber &number) const
{
    return ComplexNumber(x - number.x, y - number.y);
}

ComplexNumber &ComplexNumber::operator-=(const ComplexNumber &number)
{
    x-=number.x;
    y-=number.y;
    return *this;
}

ComplexNumber ComplexNumber::operator*(const ComplexNumber &number) const
{
    return ComplexNumber (x*number.x - y*number.y, x*number.y - number.x*y);
}

ComplexNumber &ComplexNumber::operator*=(const ComplexNumber &number)
{
    x = x*number.x - y*number.y;
    y = x*number.y - number.x*y;
    return *this;
}

ComplexNumber ComplexNumber::operator/(const ComplexNumber &number) const
{
    if(number.isZero())
        throw ComplexError::DividedByZero;

    return ComplexNumber(this->operator *(number.conjugate()).operator *( 1 / (number.module() * number.module())));
}

ComplexNumber &ComplexNumber::operator/=(const ComplexNumber &number)
{
    ComplexNumber tmp = this->operator /(number);
    x = tmp.x;
    y = tmp.y;

    return *this;
}

double ComplexNumber::real() const
{
    return x;
}

double ComplexNumber::imaginary() const
{
    return y;
}

double ComplexNumber::module() const
{
    return sqrt(x*x + y*y);
}

ComplexNumber ComplexNumber::conjugate() const //sprzężenie
{
    return ComplexNumber (x, -y);
}

double ComplexNumber::argument() const
{
    if(isZero())
        throw ComplexError::ArgumentOfZero; //argument zera jest nieokreślony
    return atan2(y, x);
}

ComplexNumber &ComplexNumber::power(int n)
{
    double m = module();
    double arg = argument();

    m = pow(m, n);
    arg = n * arg;

    x = m * cos(arg);
    y = m * sin(arg);

    return *this;
}

bool ComplexNumber::isZero() const
{
    return (x == 0.0 && y == 0.0);
}


int powerOf2(int n)
{
    if((n ^ (n-1)) == (2*n - 1)) //!!!! ważne nawiasy
    {

    }
}
