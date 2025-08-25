#include "residue.h"

#include <iostream>

int main() {
	residue<7> a(8);
	residue<7> b(6);

	std::cout << "Test1: values" << std::endl;
	std::cout << "Expected: a = 1 and b = 6" << std::endl;
	std::cout << "Got: a = " << a << " b = " << b << std::endl;
	std::cout << "---------------" << std::endl;

	std::cout << "Test2: plus" << std::endl;
	residue<7> pl = a + b;
	std::cout << "Expected: 0" << std::endl;
	std::cout << "Got: " << pl << std::endl;
	std::cout << "--------------" << std::endl;

	std::cout << "Test3: minus" << std::endl;
	residue<7> mi = a - b;
	std::cout << "Expected: 2" << std::endl;
	std::cout << "Got: " << mi << std::endl;
	std::cout << "--------------" << std::endl;

	std::cout << "Test4: multiply" << std::endl;
	residue<7> mul = a * b;
	std::cout << "Expected: 6" << std::endl;
	std::cout << "Got: " << mul << std::endl;
	std::cout << "--------------" << std::endl;

	std::cout << "Test5: division" << std::endl;
	residue<7> div = a / b;
	std::cout << "Expected: 6" << std::endl;
	std::cout << "Got: " << div << std::endl;
}
