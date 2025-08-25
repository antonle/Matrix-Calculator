#pragma once

#include <memory> // for shared pointers
#include <cstdlib> // convert string to float strtof()
#include <string>
#include <map> // priority of operations
#include <stack> // for checking correctness of brackets
#include <cmath> // for pow of 2 floats

#include "query.h"
#include "matrix.h"

// class for node of the tree
struct Token {
	using ptr = std::shared_ptr<Token>;

	Token() = default;
	Token(const std::string& type): type_(type) {}
	~Token() = default;

	// set up values
	void setUpMatrix(const std::vector<std::vector<float>>& matrix, std::string& error);
	void setUpNumber(const std::string& num, std::string& error);
	void setUpNumber(float num);

	// universal calculate function
	virtual void calc(std::string& error) = 0;

	std::string type_;
	ptr left_ = nullptr; // pointer to left child
	ptr right_ = nullptr; // pointer to right child
	bool is_ans_number_ = false; // whether answer of subtree is a number
	float ans_float_ = 0.0; // answer of subtree if its a number
	std::vector<std::vector<float>> ans_matrix_; // answer of subtree if its a matrix
};

// token's children
struct Var: Token {
	Var() = default;

	void calc(std::string& error);
};

struct Number: Token {
	Number() = default;

	void calc(std::string& error);
};

struct Plus: Token {
	Plus() = default;

	void calc(std::string& error);
};

struct Minus: Token {
	Minus() = default;

	void calc(std::string& error);
};

struct Multiply: Token {
	Multiply() = default;

	void calc(std::string& error);
};

struct Divide: Token {
	Divide() = default;

	void calc(std::string& error);
};

struct Power: Token {
	Power() = default;

	void calc(std::string& error);
};

struct Trace: Token {
	Trace() = default;

	void calc(std::string& error);
};

struct Determinant: Token {
	Determinant() = default;

	void calc(std::string& error);
};

struct Rank: Token {
	Rank() = default;

	void calc(std::string& error);
};

struct Transpose: Token {
	Transpose() = default;

	void calc(std::string& error);
};

struct Inverse: Token {
	Inverse() = default;

	void calc(std::string& error);
};

class Model {
public:
	// types definitons

	using sptrModel = std::shared_ptr<Model>;
	using sptrToken = std::shared_ptr<Token>;

	// constructor and destructor

	~Model() = default;
	Model(const Model& other) = default;
	Model& operator=(const Model& other) = default;

	// create a single object of the class

	static sptrModel createModel();

	// process given query
	Answer processQuery(const Query& query);

private:
	// private constructor for singleton pattern

	Model();

	// query handlers
	Answer handleInitQuery(const Query& query);
	Answer handleSolveEqQuery(const Query& query);
	Answer handleCalcExpQuery(const Query& query);

	// checker
	void isMatrixValid(const std::vector<std::vector<std::string>>& matrix, int variable, int query_type, std::string& error);
	bool isDigit(char symbol);
	bool correctBrackets(const std::vector<std::string>& tokens);
	bool isOperator(const std::string& token);

	// split expression into tokens
	void splitIntoTokens(std::vector<std::string>& tokens, const std::string& exp);

	// turn tokens into calc tree
	std::shared_ptr<Token> getCalcTree(const std::vector<std::string>& tokens, int start, int end, std::string& error);

	// calculate expression
	void calc(std::shared_ptr<Token> tree, std::string& error);

	// print out tree
	void printTree(std::shared_ptr<Token> node);

	bool is_ans_number_; // indicates whether answer is number
	float ans_float_; // answer if it's a number
	std::vector<std::vector<float>> ans_matrix_float_; // answer if it's a matrix
	std::vector<std::vector<std::vector<float>>> variables_; // stores the variables
	std::vector<std::vector<float>> system_; // stores coefs of system
	std::map<std::string, int> priority_; // priority of operators
	static sptrModel model_; // singleton pattern
};
