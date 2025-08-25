#include "complex.h"

#include <cmath>
#include <cctype>
#include <type_traits>
#include <cmath>

bool cmp(float val1, float val2) {
	float eps = 1e-7;

	return val1 - val2 < eps && val1 - val2 > -eps;
}

complexNumber::complexNumber(float re, float im): real_(re),
												  imaginary_(im)
{}

complexNumber::complexNumber(const complexNumber& other): complexNumber(other.real_, other.imaginary_) {}

complexNumber& complexNumber::operator=(const complexNumber& other) {
	real_ = other.real_;
	imaginary_ = other.imaginary_;

	return *this;
}

complexNumber& complexNumber::operator+=(const complexNumber& rhs) {
	real_ += rhs.real_;
	imaginary_ = rhs.imaginary_;

	return *this;
}

complexNumber& complexNumber::operator-=(const complexNumber& rhs) {
	real_ -= rhs.real_;
	imaginary_ -= rhs.imaginary_;

	return *this;
}

complexNumber& complexNumber::operator*=(float rhs) {
	real_ *= rhs;
	imaginary_ *= rhs;

	return *this;
}

complexNumber& complexNumber::operator*=(const complexNumber& rhs) {
	real_ = real_ * rhs.real_ - imaginary_ * rhs.imaginary_;
	imaginary_ = real_ * rhs.imaginary_ + imaginary_ * rhs.real_;

	return *this;
}

complexNumber& complexNumber::operator/=(const complexNumber& rhs) {
	float len = rhs.length();

	real_ = (real_ * rhs.real_ + imaginary_ * rhs.imaginary_) / (len * len);
	imaginary_ = (imaginary_ * rhs.real_ - real_ * rhs.imaginary_) / (len * len);

	return *this;
}

float complexNumber::length() const {
	return sqrt(real_ * real_ + imaginary_ * imaginary_);
}

float complexNumber::arg() const {
	if (real_ > 0 && imaginary_ > 0) {
		return atan(imaginary_ / real_);
	}

	if (real_ > 0 && imaginary_ < 0) {
		return 2 * M_PI - atan(-imaginary_ / real_);
	}

	if (real_ < 0 && imaginary_ > 0) {
		return M_PI - atan(imaginary_ / (-real_));
	}

	return M_PI + atan(-imaginary_ / (-real_));
}

complexNumber complexNumber::conjugate() const {
	return complexNumber(real_, -imaginary_);
}

float complexNumber::re() const {
	return real_;
}

float complexNumber::im() const {
	return imaginary_;
}

complexNumber operator+(const complexNumber& lhs, const complexNumber& rhs) {
	complexNumber copy = lhs;
	copy += rhs;

	return copy;
}

complexNumber operator-(const complexNumber& lhs, const complexNumber& rhs) {
	complexNumber copy = lhs;
	copy -= rhs;

	return copy;
}

complexNumber operator*(float lhs, const complexNumber& rhs) {
	complexNumber copy = rhs;
	copy *= lhs;

	return copy;
}

complexNumber operator*(const complexNumber& lhs, const complexNumber& rhs) {
	complexNumber copy = lhs;
	copy *= rhs;

	return copy;
}

complexNumber operator/(const complexNumber& lhs, const complexNumber& rhs) {
	complexNumber copy = lhs;
	copy /= rhs;

	return copy;
}

bool operator==(const complexNumber& lhs, const complexNumber& rhs)	{
	return cmp(lhs.re(), rhs.re()) && cmp(lhs.im(), rhs.im());
}

bool operator!=(const complexNumber& lhs, const complexNumber& rhs) {
	return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const complexNumber& number) {
	if (number.re() != 0) {
		out << number.re() << " ";
	}

	if (number.im() > 0) out << "+ ";
	else {
		out << "- ";
	}

	if (number.im() != 0) {
		out << number.im() << "i";
	}

	return out;
}

std::istream& operator>>(std::istream& in, complexNumber& number) {
	std::string str;
	in >> str;
	number = complexNumber(str);

	return in;
}
