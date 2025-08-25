#include "complex.h"

#include <iostream>
#include <iomanip>

int main() {
	complexNumber a(3, 6);
	complexNumber b(2, 8);

	std::cout << "Test1: plus" << std::endl;
	complexNumber c = a + b;
	std::cout << "Expected: 5 + 14i" << std::endl;
	std::cout << "Got: " << c << std::endl;
	std::cout << "--------------------" << std::endl;

	std::cout << "Test2: minus" << std::endl;
	complexNumber d = a - b;
	std::cout << "Expected: 1 - 2i" << std::endl;
	std::cout << "Got: " << d << std::endl;
	std::cout << "-------------------" << std::endl;

	std::cout << "Test3: multiply by another complex number" << std::endl;
	complexNumber e = a * b;
	std::cout << "Expected: -42 + 36i" << std::endl;
	std::cout << "Got: " << e << std::endl;
	std::cout << "-------------------" << std::endl;

	std::cout << "Test4: multiply by a number" << std::endl;
	complexNumber f = 0.5 * a;
	std::cout << "Expected: 1.5 + 3i" << std::endl;
	std::cout << "Got: " << f << std::endl;
	std::cout << "-------------------" << std::endl;

	std::cout << "Test5: division" << std::endl;
	complexNumber g = a / b;
	std::cout << "Expected: 27/34 - 3/17i" << std::endl;
	std::cout << "Got: " << g << std::endl;
	std::cout << "-------------------" << std::endl;

	std::cout << "Test6: conjugate" << std::endl;
	complexNumber con = a.conjugate();
	std::cout << "Expected: 3 - 6i" << std::endl;
	std::cout << "Got: " << con << std::endl;
}
