#pragma once

#include <string>
#include <vector>

enum typeOfQuery {
	noQuery = 0,
	calcExp,
	init,
	solveEq
};

struct Query {
	std::string type_; // type used for matrix
	int type_of_query_  = noQuery; // type of query
	bool is_ans_used_; // whether ans was used or not for init
	std::string exp_; // expression to calculated
	int variable_used_; // variable used to store matrix
	std::vector<std::vector<std::string>> matrix_; // matrix that is used for init

	Query() = default;
};

struct Answer {
	std::string error_message_ = ""; // error message if any
	float ans_float_; // answer if its a number
	bool is_ans_number_; // flag whether ans was number or not
	std::vector<std::vector<float>> ans_matrix_; // answer if its a matrix

	Answer() = default;
};
