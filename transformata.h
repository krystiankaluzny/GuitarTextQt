#ifndef COMPLEXNUMBER_H
#define COMPLEXNUMBER_H

enum ComplexError{
    DividedByZero,
    ArgumentOfZero
};

class ComplexNumber
{
public:
    ComplexNumber();
    ComplexNumber(double real);
    ComplexNumber(double real, double imaginary);
    ComplexNumber(const ComplexNumber& a);

    ComplexNumber& operator=(const ComplexNumber& number);
    ComplexNumber operator+(const ComplexNumber& number) const;
    ComplexNumber& operator+=(const ComplexNumber& number);
    ComplexNumber operator-(const ComplexNumber& number) const;
    ComplexNumber& operator-=(const ComplexNumber& number);
    ComplexNumber operator*(const ComplexNumber& number) const;
    ComplexNumber& operator*=(const ComplexNumber& number);
    ComplexNumber operator/(const ComplexNumber& number)const;
    ComplexNumber& operator/=(const ComplexNumber& number);

    double real() const;
    double imaginary() const;
    double module() const;
    ComplexNumber conjugate() const; //sprzężenie
    double argument() const;
    ComplexNumber& power(int n);
private:
    bool isZero() const;
    double x, y;
};

template <typename T> ComplexNumber* FFT (T* arg, int size)
{
    if(size == 1) return arg[0];
}

int powerOf2(int n); //-1 jeśli tak, wprzeciwnym wypadku najbliższą większo od n potęgę 2
#endif // COMPLEXNUMBER_H
