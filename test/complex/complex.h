#pragma once

#include <iostream>
#include <string>

// compare to float values
bool cmp(float val1, float val2);

class complexNumber {
public:
	// constructors and operator =

	complexNumber(float re, float im);
	complexNumber(const std::string& number);
	complexNumber(const complexNumber& other);
	complexNumber& operator=(const complexNumber& other);

	// arithmetics

	complexNumber& operator+=(const complexNumber& rhs);
	complexNumber& operator-=(const complexNumber& rhs);
	complexNumber& operator*=(float rhs);
	complexNumber& operator*=(const complexNumber& rhs);
	complexNumber& operator/=(const complexNumber& rhs);

	// converting to string

	std::string toString() const;

	// arg and length

	float length() const;
	float arg() const;

	// conjugate

	complexNumber conjugate() const;

	// getters

	float re() const;
	float im() const;


private:
	float real_; // real part of the number
	float imaginary_; // imaginary part of the number
};

// more arithmetics

complexNumber operator+(const complexNumber& lhs, const complexNumber& rhs);
complexNumber operator-(const complexNumber& lhs, const complexNumber& rhs);
complexNumber operator*(float lhs, const complexNumber& rhs);
complexNumber operator*(const complexNumber& lhs, const complexNumber& rhs);
complexNumber operator/(const complexNumber& lhs, const complexNumber& rhs);

// comparisons

bool operator==(const complexNumber& lhs, const complexNumber& rhs);
bool operator!=(const complexNumber& lhs, const complexNumber& rhs);

// input and output streams

std::ostream& operator<<(std::ostream& out, const complexNumber& number);
