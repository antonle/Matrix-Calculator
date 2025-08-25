#pragma once

#include <iostream>

template <size_t N, size_t K, bool is_smaller>
struct isPrimeHelper {
	static const bool value = N % K == 0 ? false : isPrimeHelper<N, K + 1, K * K <= N>::value;
};

template <size_t N, size_t K>
struct isPrimeHelper<N, K, false> {
	static const bool value = true;
};

template <size_t N>
struct isPrime {
	static const bool value = isPrimeHelper<N, 2, N >= 4>::value;
};

template <>
struct isPrime<1> {
	static const bool value = false;
};

template <size_t N>
const bool is_prime_v = isPrime<N>::value;

template <size_t N>
class residue {
public:

	// constructor and operator =

	residue() = default;
	residue(int value);
	~residue() = default;
	residue(const residue& other);
	residue& operator=(const residue& other);

	// arithmetic assignments

	residue& operator+=(const residue& rhs);
	residue& operator-=(const residue& rhs);
	residue& operator*=(const residue& rhs);
	residue& operator/=(const residue& rhs);

	// presentations

	explicit operator int() const;

	// get the modulo value

	size_t getValue() const;

private:

	// quick power in O(logn)

	size_t power(size_t power_value);

	// get invert

	size_t invert() const;

	size_t value_; // value in modulo N field
};

// more arithmetics

template <size_t N>
residue<N> operator+(const residue<N>& lhs, const residue<N>& rhs);
template <size_t N>
residue<N> operator-(const residue<N>& lhs, const residue<N>& rhs);
template <size_t N>
residue<N> operator*(const residue<N>& lhs, const residue<N>& rhs);
template <size_t N>
residue<N> operator/(const residue<N>& lhs, const residue<N>& rhs);

// comparisons

template <size_t N>
bool operator<(const residue<N>& lhs, const residue<N>& rhs);
template <size_t N>
bool operator>(const residue<N>& lhs, const residue<N>& rhs);
template <size_t N>
bool operator==(const residue<N>& lhs, const residue<N>& rhs);
template <size_t N>
bool operator<=(const residue<N>& lhs, const residue<N>& rhs);
template <size_t N>
bool operator>=(const residue<N>& lhs, const residue<N>& rhs);
template <size_t N>
bool operator!=(const residue<N>& lhs, const residue<N>& rhs);

// output

template <size_t N>
std::ostream& operator<<(std::ostream& out, const residue<N>& number);

// ----------------------------------------------------------------------------------------

template <size_t N>
residue<N>::residue(int value) {
	if (value < 0) {
		value *= 1 - N;
	}

	value_ = value % N;
}

template <size_t N>
residue<N>::residue(const residue& other): value_(other.value_) {}

template <size_t N>
residue<N>& residue<N>::operator=(const residue& other) {
	value_ = other.value_;

	return *this;
}

template <size_t N>
residue<N>& residue<N>::operator+=(const residue& rhs) {
	value_ = (value_ + rhs.value_) % N;

	return *this;
}

template <size_t N>
residue<N>& residue<N>::operator-=(const residue& rhs) {
	value_ = value_ > rhs.value_ ? value_ - rhs.value_ : N + value_ - rhs.value_;

	return *this;
}

template <size_t N>
residue<N>& residue<N>::operator*=(const residue& rhs) {
	value_ = (value_ * rhs.value_) % N;

	return *this;
}

template <size_t N>
residue<N>& residue<N>::operator/=(const residue& rhs) {
	static_assert(is_prime_v<N>, "No inverts in non-prime fields");

	value_ = (value_ * rhs.invert()) % N;

	return *this;
}

template <size_t N>
residue<N>::operator int() const {
	return value_;
}

template <size_t N>
size_t residue<N>::power(size_t power_value) {
	if (power_value == 1) {
		return value_;
	}

	size_t helper;
	if (power_value % 2 == 0) {
		helper = power(power_value / 2);
		return (helper * helper) % N;
	}
	else {
		helper = power(power_value - 1);
		return helper * value_;
	}
}

template <size_t N>
size_t residue<N>::invert() const {
	return power(N - 2);
}

template <size_t N>
residue<N> operator+(const residue<N>& lhs, const residue<N>& rhs) {
	residue<N> copy = lhs;
	copy += rhs;

	return copy;
}

template <size_t N>
residue<N> operator-(const residue<N>& lhs, const residue<N>& rhs) {
	residue<N> copy = lhs;
	copy -= rhs;

	return copy;
}

template <size_t N>
residue<N> operator*(const residue<N>& lhs, const residue<N>& rhs) {
	residue<N> copy = lhs;
	copy *= rhs;

	return copy;
}

template <size_t N>
residue<N> operator/(const residue<N>& lhs, const residue<N>& rhs) {
	residue<N> copy = lhs;
	copy /= rhs;

	return copy;
}

template <size_t N>
bool operator<(const residue<N>& lhs, const residue<N>& rhs) {
	return lhs.getValue() < rhs.getValue();
}

template <size_t N>
bool operator>(const residue<N>& lhs, const residue<N>& rhs) {
	return rhs < lhs;
}

template <size_t N>
bool operator==(const residue<N>& lhs, const residue<N>& rhs) {
	return lhs.getValue() == rhs.getValue();
}

template <size_t N>
bool operator<=(const residue<N>& lhs, const residue<N>& rhs) {
	return !(lhs > rhs);
}

template <size_t N>
bool operator>=(const residue<N>& lhs, const residue<N>& rhs) {
	return !(lhs < rhs);
}

template <size_t N>
bool operator!=(const residue<N>& lhs, const residue<N>& rhs) {
	return !(lhs == rhs);
}

template <size_t N>
std::ostream& operator<<(std::ostream& out, const residue<N>& number) {
	out << number.getValue();

	return out;
}
