#include "model.h" // include header file
#include <iostream>
#include <system_error>

// calculation

// set up values
void Token::setUpMatrix(const std::vector<std::vector<float>>& matrix, std::string& error) {
	is_ans_number_ = false;
	ans_matrix_ = matrix;
}

void Token::setUpNumber(const std::string& num, std::string& error) {
	char* pend;
	is_ans_number_ = true;
	ans_float_ = std::strtof(num.c_str(), &pend);
	if (pend == num.c_str()) {
		error = "Syntax error: wrong format for a real number";
		return;
	}
	return;
}

void Token::setUpNumber(float num) {
	is_ans_number_ = true;
	ans_float_ = num;
	return;
}

// universal calculate function
void Var::calc(std::string& error) {}

void Number::calc(std::string& error) {}

// token's children
void Plus::calc(std::string& error) {
	if (error != "") {
		return;
	}

	if (!left_ || !right_) {
		error = "Syntax error: not enough operands for plus";
		return;
	}

	if (left_->is_ans_number_ && !right_->is_ans_number_ ||
		!left_->is_ans_number_ && right_->is_ans_number_) {
		error = "Semantic error: can't add number and matrix";
		return;
	}

	if (!left_->is_ans_number_) {
		Matrix<float> matr1(left_->ans_matrix_);
		Matrix<float> matr2(right_->ans_matrix_);
		if (matr1.getRow() != matr2.getRow() || matr1.getCol() != matr2.getCol()) {
			error = "Semantic error: can't add matrices of different dimensions";
			return;
		}

		is_ans_number_ = false;
		ans_matrix_ = (matr1 + matr2).getMatrix();
		return;
	}

	is_ans_number_ = true;
	ans_float_ = left_->ans_float_ + right_->ans_float_;
	return;
}

void Minus::calc(std::string& error) {
	if (error != "") {
		return;
	}

	if (!left_ || !right_) {
		error = "Syntax error: not enough operands for minus";
		return;
	}

	if (left_->is_ans_number_ && !right_->is_ans_number_ ||
		!left_->is_ans_number_ && right_->is_ans_number_) {
		error = "Semantic error: can't subtract number and matrix";
		return;
	}

	if (!left_->is_ans_number_) {
		Matrix<float> matr1(left_->ans_matrix_);
		Matrix<float> matr2(right_->ans_matrix_);
		if (matr1.getRow() != matr2.getRow() || matr1.getCol() != matr2.getCol()) {
			error = "Semantic error: can not subtract matrices of different dimensions";
			return;
		}

		is_ans_number_ = false;
		ans_matrix_ = (matr1 - matr2).getMatrix();
		return;
	}

	is_ans_number_ = true;
	ans_float_ = left_->ans_float_ - right_->ans_float_;
	return;
}

void Multiply::calc(std::string& error) {
	if (error != "") {
		return;
	}

	if (!left_ || !right_) {
		error = "Syntax error:: not enough operands for multiply";
		return;
	}

	if (left_->is_ans_number_ && !right_->is_ans_number_) {
		is_ans_number_ = false;
		ans_matrix_ = (left_->ans_float_ * Matrix<float>(right_->ans_matrix_)).getMatrix();
		return;
	}

	if (!left_->is_ans_number_ && right_->is_ans_number_) {
		is_ans_number_ = false;
		ans_matrix_ = (right_->ans_float_ * Matrix<float>(left_->ans_matrix_)).getMatrix();
		return;
	}

	if (left_->is_ans_number_) {
		is_ans_number_ = true;
		ans_float_ = left_->ans_float_ * right_->ans_float_;
		return;
	}

	Matrix<float> matr1(left_->ans_matrix_);
	Matrix<float> matr2(right_->ans_matrix_);
	if (matr1.getCol() != matr2.getRow()) {
		error = "Semantic error: can't multiply such matrices";
		return;
	}

	is_ans_number_ = false;
	ans_matrix_ = (matr1 * matr2).getMatrix();
	return;
}

void Divide::calc(std::string& error) {
	if (error != "") {
		return;
	}

	if (!left_ || !right_) {
		error = "Syntax error: not enough operands for divide";
		return;
	}

	if (!left_->is_ans_number_ || !right_->is_ans_number_) {
		error = "Semantic error: can not divide matrices";
		return;
	}

	if (std::abs(right_->ans_float_) == 0.0) {
		error = "Semantic error: can not divide by 0";
		return;
	}

	is_ans_number_ = true;
	ans_float_ = left_->ans_float_ / right_->ans_float_;
	return;
}

void Power::calc(std::string& error) {
	if (error != "") {
		return;
	}

	if (!left_ || !right_) {
		error = "Syntax error: not enough operands to do power";
		return;
	}

	if (!right_->is_ans_number_) {
		error = "Semantic error: can not take matrix as the power";
		return;
	}

	if (left_->is_ans_number_) {
		is_ans_number_ = true;
		ans_float_ = std::pow(left_->ans_float_, right_->ans_float_);
		return;
	}

	is_ans_number_ = false;
	int exponent = right_->ans_float_;
	float diff = right_->ans_float_ - exponent;
	if (diff > 0) {
		error = "Semantic error: can not take a maatrix to a float power";
		return;
	}

	Matrix<float> matr(left_->ans_matrix_);
	if (matr.getRow() != matr.getCol()) {
		error = "Semantic error: can not take a power of a non square matrix";
		return;
	}

	is_ans_number_ = false;
	ans_matrix_ = pow(matr, exponent).getMatrix();
	return;
}

void Trace::calc(std::string& error) {
	if (error != "") {
		return;
	}

	if (!left_) {
		error = "Syntax error: not enough operands to do trace";
		return;
	}

	if (left_->is_ans_number_) {
		error = "Semantic error: can not take trace of a number";
		return;
	}

	Matrix<float> matr(left_->ans_matrix_);
	if (matr.getRow() != matr.getCol()) {
		error = "Semantic error: can not take trace of a non square matrix";
		return;
	}

	is_ans_number_ = true;
	ans_float_ = matr.trace();
	return;
}

void Determinant::calc(std::string& error) {
	if (error != "") {
		return;
	}

	if (!left_) {
		error = "Syntax error: not enough operands to find determinant";
		return;
	}

	if (left_->is_ans_number_) {
		error = "Semantic error: can not take determinant of a number";
		return;
	}

	Matrix<float> matr(left_->ans_matrix_);
	if (matr.getRow() != matr.getCol()) {
		error = "Semantic error: can not find determinant of a non square matrix";
		return;
	}

	is_ans_number_ = true;
	ans_float_ = matr.det();
	return;
}

void Rank::calc(std::string& error) {
	if (error != "") {
		return;
	}

	if (!left_) {
		error = "Syntax error: not enough operands to find rank";
		return;
	}

	if (left_->is_ans_number_) {
		error = "Semantic error: can not find rank of a number";
		return;
	}

	Matrix<float> matr(left_->ans_matrix_);
	is_ans_number_ = true;
	ans_float_ = matr.rank();
}

void Transpose::calc(std::string& error) {
	if (error != "") {
		return;
	}

	if (!left_) {
		error = "Syntax error: not enough operands to find the tranpose";
		return;
	}

	if (left_->is_ans_number_) {
		error = "Semantic error: can not tranpose a number";
		return;
	}

	Matrix<float> matr(left_->ans_matrix_);
	is_ans_number_ = false;
	ans_matrix_ = matr.transposed().getMatrix();
	return;
}

void Inverse::calc(std::string& error) {
	if (error != "") {
		return;
	}

	if (!left_) {
		error = "Syntax error: not enough operands to find the inverse";
		return;
	}

	if (left_->is_ans_number_) {
		error = "Semantic error: can not take an inverse of a number";
		return;
	}

	Matrix<float> matr(left_->ans_matrix_);
	if (matr.getRow() != matr.getCol()) {
		error = "Semantic error: can not take an inverse of a non square matrix";
		return;
	}

	if (matr.det() == 0) {
		error = "Semantic error: matrix is a singular matrix";
		return;
	}

	is_ans_number_ = false;
	ans_matrix_ = matr.inverted().getMatrix();
	return;
}

// initialize static member

Model::sptrModel Model::model_ = nullptr;

// create a single object of the class

Model::sptrModel Model::createModel() {
	if (model_ == nullptr) {
		model_ = Model::sptrModel(new Model());
	}

	return model_;
}

Answer Model::processQuery(const Query& query) {
	if (query.type_of_query_ == init) {
		return handleInitQuery(query);
	}
	else if (query.type_of_query_ == solveEq) {
		return handleSolveEqQuery(query);
	}
	return handleCalcExpQuery(query);
}

// private constructor for singleton pattern

Model::Model(): is_ans_number_(false)
{
	priority_["+"] = 4;
	priority_["-"] = 4;
	priority_["*"] = 3;
	priority_["/"] = 3;
	priority_["^"] = 2;
	priority_["tr"] = 1;
	priority_["inv"] = 1;
	priority_["det"] = 1;
	priority_["rk"] = 1;
	priority_["trans"] = 1;
	variables_.resize(26);
}

// query handlers

Answer Model::handleInitQuery(const Query& query) {
	Answer ans;

	if (query.is_ans_used_) {
		if (is_ans_number_) {
			ans.error_message_ = "Cannot initialize variable with number";
			return ans;
		}

		variables_[query.variable_used_] = ans_matrix_float_;

		return ans;
	}

	isMatrixValid(query.matrix_, query.variable_used_, init, ans.error_message_);
	if (ans.error_message_ != "") {
		return ans;
	}

	return ans;
}

Answer Model::handleSolveEqQuery(const Query& query) {
	Answer ans;

	isMatrixValid(query.matrix_, 0, solveEq, ans.error_message_);

	if (ans.error_message_ != "") {
		return ans;
	}

	Matrix<float> equation(system_);
	ans.ans_matrix_ = equation.getReducedRowEchelonForm().getMatrix();

	for (int i = 0; i < ans.ans_matrix_.size(); ++i) {
		for (int j = 0; j < ans.ans_matrix_[i].size(); ++j) {
			if (ans.ans_matrix_[i][j] == -0.0) {
				ans.ans_matrix_[i][j] = 0;
			}
		}
	}

	return ans;
}

Answer Model::handleCalcExpQuery(const Query& query) {
	Answer ans;
	std::string exp = query.exp_;
	std::vector<std::string> tokens;
	splitIntoTokens(tokens, exp);

	if (!correctBrackets(tokens)) {
		ans.error_message_ = "Invalid syntax: brackets";

		return ans;
	}

	std::shared_ptr<Token> calc_tree = nullptr;
	calc_tree = getCalcTree(tokens, 0, tokens.size(), ans.error_message_);

	printTree(calc_tree);
	std::cout << "---------------" << std::endl;

	calc(calc_tree, ans.error_message_);

	if (ans.error_message_ == "") {
		ans.is_ans_number_ = calc_tree->is_ans_number_;
		is_ans_number_ = calc_tree->is_ans_number_;
		if (is_ans_number_) {
			ans.ans_float_ = calc_tree->ans_float_;
			ans_float_ = calc_tree->ans_float_;
		}
		else {
			ans_matrix_float_.resize(calc_tree->ans_matrix_.size());
			for (int i = 0; i < calc_tree->ans_matrix_.size(); ++i) {
				ans_matrix_float_[i].resize(calc_tree->ans_matrix_[i].size());
				for (int j = 0; j < calc_tree->ans_matrix_[i].size(); ++j) {
					int int_value = calc_tree->ans_matrix_[i][j];
					if (calc_tree->ans_matrix_[i][j] - int_value < 0.00001) {
						ans_matrix_float_[i][j] = int_value;
					}
					else {
						ans_matrix_float_[i][j] = calc_tree->ans_matrix_[i][j];
					}
				}
			}
			ans.ans_matrix_ = ans_matrix_float_;
		}
	}

	return ans;
}

// checkers

// check if cells really represent real numbers
void Model::isMatrixValid(const std::vector<std::vector<std::string>>& matrix, int variable, int query_type, std::string& error) {
	std::vector<std::vector<float>> copy;
	int row = matrix.size();
	int col = matrix[0].size();
	copy.resize(row);
	for (int i = 0; i < row; ++i) {
		copy[i].resize(col);
	}

	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			std::string first = "";
			std::string second = "";
			int cur = 0;

			while (cur < matrix[i][j].length() && matrix[i][j][cur] != '/') {
				first += matrix[i][j][cur++];
			}

			for (int k = cur + 1; k < matrix[i][j].length(); ++k) {
				second += matrix[i][j][k];
			}

			if (first == "") {
				error = "Syntax error: the cells do not represent real numbers";
				return;
			}

			char* pend1;
			copy[i][j] = std::strtof(first.c_str(), &pend1);

			if (pend1 == first.c_str()) {
				error = "Syntax error: the cells do not represent real numbers";
				return;
			}

			if (second != "") {
				char* pend2;
				float second_num = std::strtof(second.c_str(), &pend2);
				if (pend2 == second.c_str()) {
					error = "Syntax error: the cells do not represent real numbers";
					return;
				}
				if (second_num == 0.0) {
					error = "Division by zero";
					return;
				}

				copy[i][j] /= second_num;
			}
		}
	}

	if (query_type == init) {
		variables_[variable] = copy;
	}
	else {
		system_ = copy;
	}

	return;
}

bool Model::isDigit(char symbol) {
	return symbol == '.' || (symbol >= '0' && symbol <= '9');
}

// checks if brackets sequence is correct
bool Model::correctBrackets(const std::vector<std::string>& tokens) {
	std::stack<std::string> br;
	int cnt = 0;
	for (int i = 0; i < tokens.size(); ++i) {
		if (tokens[i] == "(") {
			++cnt;
			br.push("(");
		}
		if (tokens[i] == ")") {
			if (br.empty()) return false;
			--cnt;
		}
	}

	if (cnt) {
		return false;
	}

	return true;
}

bool Model::isOperator(const std::string& token) {
	return token == "+" || token == "-" || token == "*" || token == "/" || token == "^" ||
		   token == "tr" || token == "inv" || token == "det" || token == "rk" ||
		   token == "trans";
}

// split expression into tokens
void Model::splitIntoTokens(std::vector<std::string>& tokens, const std::string& exp) {
	std::string token = "";
	for (int i = 0; i < exp.length(); ++i) {
		token += exp[i];

		if (token == "(" || token == ")" || isOperator(token) || (token[0] >= 'A' && token[0] <= 'Z') || token == "ans") {
			if (token == "tr" && i + 1 < exp.length() && exp[i + 1] == 'a') {
				continue;
			}
			tokens.push_back(token);
			token = "";
		}
		else if (i + 1 == exp.length() || i + 1 < exp.length() && isDigit(exp[i]) && !isDigit(exp[i + 1])) {
			tokens.push_back(token);
			token = "";
		}
	}
}

// turn tokens into calc tree
std::shared_ptr<Token> Model::getCalcTree(const std::vector<std::string>& tokens, int start, int end, std::string& error) {
	// remove outer brackets
	bool stop = false;
	while (!stop) {
		int cnt = 0;
		bool has_outer_br = true;
		for (int i = start; i < end; ++i) {
			if (tokens[i] == "(") ++cnt;
			if (tokens[i] == ")") --cnt;

			if (cnt == 0 && i + 1 < end || start + 1 == end) {
				has_outer_br = false;
				break;
			}
		}

		if (has_outer_br) {
			++start;
			--end;
		}
		else {
			stop = true;
		}
	}

	// find inflection point
	int max_priority = -1;
	int pos = -1;
	int br = 0;
	for (int i = end - 1; i >= start; --i) {
		if (tokens[i] == "(") --br;
		if (tokens[i] == ")") ++br;
		if (isOperator(tokens[i])) {
			if (priority_[tokens[i]] - 4 * br > max_priority) {
				max_priority = priority_[tokens[i]] - 4 * br;
				pos = i;
			}
		}
	}

	// base case
	if (pos == -1) {
		std::shared_ptr<Token> node;
		if (end - start != 1 || isOperator(tokens[start])) {
			error = "Invalid syntax";
			return nullptr;
		}

		if ((tokens[start][0] >= 'A' && tokens[start][0] <= 'Z') || tokens[start] == "ans") {
			if (tokens[start] == "ans") {
				if (is_ans_number_) {
					node = std::shared_ptr<Number>(new Number());
					node->type_ = "number";
					node->setUpNumber(ans_float_);
					return node;
				}
				else {
					node = std::shared_ptr<Var>(new Var());
					node->type_ = "var";
					node->setUpMatrix(ans_matrix_float_, error);
					return node;
				}
			}
			node = std::shared_ptr<Var>(new Var());
			node->type_ = "var";
			node->setUpMatrix(variables_[tokens[start][0] - 'A'], error);
			return node;
		}

		node = std::shared_ptr<Number>(new Number());
		node->type_ = "number";
		node->setUpNumber(tokens[start], error);
		return node;
	}

	// split at inflection point
	std::shared_ptr<Token> node;
	if (tokens[pos] == "+") {
		node = std::shared_ptr<Plus>(new Plus());
	}
	else if (tokens[pos] == "-") {
		node = std::shared_ptr<Minus>(new Minus());
	}
	else if (tokens[pos] == "*") {
		node = std::shared_ptr<Multiply>(new Multiply());
	}
	else if (tokens[pos] == "/") {
		node = std::shared_ptr<Divide>(new Divide());
	}
	else if (tokens[pos] == "^") {
		node = std::shared_ptr<Power>(new Power());
	}
	else if (tokens[pos] == "tr") {
		node = std::shared_ptr<Trace>(new Trace());
	}
	else if (tokens[pos] == "det") {
		node = std::shared_ptr<Determinant>(new Determinant());
	}
	else if (tokens[pos] == "rk") {
		node = std::shared_ptr<Rank>(new Rank());
	}
	else if (tokens[pos] == "trans") {
		node = std::shared_ptr<Transpose>(new Transpose());
	}
	else if (tokens[pos] == "inv") {
		node = std::shared_ptr<Inverse>(new Inverse());
	}

	node->type_ = tokens[pos];

	if (tokens[pos].length() == 1) {
		node->left_ = getCalcTree(tokens, start, pos, error);
		node->right_ = getCalcTree(tokens, pos + 1, end, error);
	}
	else {
		node->left_ = getCalcTree(tokens, start + 1, end, error);
		node->right_ = nullptr;
	}

	node->type_ = tokens[pos];

	return node;
}

// calculate expression
void Model::calc(std::shared_ptr<Token> tree, std::string& error) {
	if (tree->left_) calc(tree->left_, error);
	if (tree->right_) calc(tree->right_, error);
	tree->calc(error);
}

// print out tree
void Model::printTree(std::shared_ptr<Token> node) {
	if (node == nullptr) {
		return;
	}

	std::cerr << node->type_ << std::endl;
	printTree(node->left_);
	printTree(node->right_);
}
