#include <vector>
#include <string>

template <typename Field>
class Matrix {
public:
	//constructors and destructor
	Matrix(size_t row, size_t col);
	Matrix(const std::vector<std::vector<Field>>& matrix);
	~Matrix() = default;
	Matrix(const Matrix& other);
	Matrix& operator=(const Matrix& other);

	// accessibility
	std::vector<Field> operator[](size_t position) const;
	std::vector<Field>& operator[](size_t position);

	// arithmetics
	Matrix& operator+=(const Matrix& rhs);
	Matrix& operator-=(const Matrix& rhs);
	Matrix operator+(const Matrix& rhs);
	Matrix operator-(const Matrix& rhs);
	Matrix& operator*=(const Field& rhs);
	Matrix operator*(const Field& rhs);
	Matrix operator*(const Matrix& rhs);
	Matrix& operator*=(const Matrix& rhs);

	// comparisons
	bool operator==(const Matrix& rhs) const;
	bool operator!=(const Matrix& rhs) const;

	// determinant
	Field det() const;

	// transposition
	Matrix<Field> transposed() const;

	// rank
	size_t rank() const;

	// invert matrix
	Matrix& invert();
	Matrix inverted() const;

	// trace
	Field trace() const;

	// reduced row echelon form
	Matrix getReducedRowEchelonForm() const;

	// getters
	size_t getRow() const;
	size_t getCol() const;
	std::vector<std::vector<Field>> getMatrix() const;

private:
	// row echelon form
	void getRowEchelonForm(Matrix& matrix) const;

	// swap 2 rows with each other
	void swapRow(Matrix<Field>& matrix, size_t first, size_t second) const;

	// nulify all entries below a given one
	void anihilate(Matrix<Field>& matrix, size_t row, size_t column) const;

	// nulify all entries in a column except the given one
	void fullAnihilate(Matrix<Field>& matrix, size_t row, size_t column) const;

	// turns the leading entry into 1
	void reduceToOne(Matrix<Field>& matrix, size_t row) const;

	// check if row is full of zeros
	bool isZero(size_t row) const;

	std::vector<std::vector<Field>> matrix_;
	size_t row_;
	size_t col_;
};

// multiplication by a number from the left
template <typename Field>
Matrix<Field> operator*(const Field& lhs, const Matrix<Field>& rhs);

// power of a matrix
template <typename Field>
Matrix<Field> pow(const Matrix<Field>& matrix, int power);


//------------------------------------------------------------------


// constructors and destructor
template <typename Field>
Matrix<Field>::Matrix(size_t row, size_t col): row_(row), col_(col) {
	matrix_.resize(row_);
    for (size_t i = 0; i < row_; ++i) {
    	matrix_[i].resize(col_);
        for (size_t j = 0; j < col_; ++j) {
            matrix_[i][j] = 0;
        }
    }
}

template <typename Field>
Matrix<Field>::Matrix(const std::vector<std::vector<Field>>& matrix): matrix_(matrix),
																	  row_(matrix.size()),
																	  col_(matrix[0].size())
{}

template <typename Field>
Matrix<Field>::Matrix(const Matrix& other): matrix_(other.matrix_),
											row_(other.row_),
											col_(other.col_)
{}

template <typename Field>
Matrix<Field>& Matrix<Field>::operator=(const Matrix& other) {
	matrix_ = other.matrix_;
	row_ = other.row_;
	col_ = other.col_;

	return *this;
}

// accessibility
template <typename Field>
std::vector<Field>& Matrix<Field>::operator[](size_t position) {
	return matrix_[position];
}

template <typename Field>
std::vector<Field> Matrix<Field>::operator[](size_t position) const {
	return matrix_[position];
}

// arithmetics
template <typename Field>
Matrix<Field>& Matrix<Field>::operator+=(const Matrix& rhs) {
    for (size_t i = 0; i < row_; ++i) {
        for (size_t j = 0; j < col_; ++j) {
            matrix_[i][j] += rhs[i][j];
        }
    }

    return *this;
}

template <typename Field>
Matrix<Field>& Matrix<Field>::operator-=(const Matrix& rhs) {
    for (size_t i = 0; i < row_; ++i) {
        for (size_t j = 0; j < col_; ++j) {
            matrix_[i][j] -= rhs[i][j];
        }
    }

    return *this;
}

template <typename Field>
Matrix<Field> Matrix<Field>::operator+(const Matrix& rhs) {
	Matrix copy = *this;
    copy += rhs;

    return copy;
}

template <typename Field>
Matrix<Field> Matrix<Field>::operator-(const Matrix& rhs) {
    Matrix copy = *this;
    copy -= rhs;

    return copy;
}

template <typename Field>
Matrix<Field>& Matrix<Field>::operator*=(const Field& rhs) {
	for (size_t i = 0; i < row_; ++i) {
        for (size_t j = 0; j < col_; ++j) {
            matrix_[i][j] *= rhs;
        }
    }

    return *this;
}

template <typename Field>
Matrix<Field> Matrix<Field>::operator*(const Field& rhs) {
	Matrix copy = *this;
    copy *= rhs;

    return copy;
}

template <typename Field>
Matrix<Field> Matrix<Field>::operator*(const Matrix& rhs) {
	std::vector<std::vector<Field>> new_matrix(row_);
    for (size_t i = 0; i < row_; ++i) {
        new_matrix[i].resize(rhs.col_);
        for (size_t z = 0; z < rhs.col_; ++z) {
            Field sum = 0;
            for (size_t j = 0; j < col_; ++j) {
                sum += matrix_[i][j] * rhs[j][z];
            }
            new_matrix[i][z] = sum;
        }
    }

    return Matrix(new_matrix);
}

template <typename Field>
Matrix<Field>& Matrix<Field>::operator*=(const Matrix& rhs) {
    Matrix<Field> copy = *this * rhs;
    *this = copy;

    return *this;
}

// comparisons
template <typename Field>
bool Matrix<Field>::operator==(const Matrix& rhs) const {
    for (size_t i = 0; i < row_; ++i) {
        for (size_t j = 0; j < col_; ++j) {
            if (matrix_[i][j] != rhs[i][j]) {
                return false;
            }
        }
    }

    return true;
}

template <typename Field>
bool Matrix<Field>::operator!=(const Matrix& rhs) const {
	return !(*this == rhs);
}

// determinant
template <typename Field>
Field Matrix<Field>::det() const {
	if (matrix_.size() == 1) {
		return matrix_[0][0];
    }

   	if (matrix_.size() == 2) {
		return matrix_[0][0] * matrix_[1][1] - matrix_[0][1] * matrix_[1][0];
   	}

	Field determinant = 0;
   	for (int i = 0; i < matrix_.size(); ++i) {
   		if (matrix_[0][i] == 0) continue;
		std::vector<std::vector<Field>> minor;
		minor.resize(matrix_.size() - 1);
		for (int j = 1; j < matrix_.size(); ++j) {
			for (int z = 0; z < matrix_.size(); ++z) {
				if (z != i) {
					minor[j - 1].push_back(matrix_[j][z]);
				}
			}
		}

		if (i % 2 == 0) {
			determinant += matrix_[0][i] * Matrix<Field>(minor).det();
		}
		else {
			determinant -= matrix_[0][i] * Matrix<Field>(minor).det();
		}
   	}

   	return determinant;
}

// transposition
template <typename Field>
Matrix<Field> Matrix<Field>::transposed() const {
	std::vector<std::vector<Field>> matrix(col_);
    for (size_t i = 0; i < col_; ++i) {
        matrix[i].resize(row_);
        for (size_t j = 0; j < row_; ++j) {
            matrix[i][j] = matrix_[j][i];
        }
    }

    return Matrix<Field>(matrix);
}

//rank
template <typename Field>
size_t Matrix<Field>::rank() const {
	Matrix copy = *this;
    getRowEchelonForm(copy);

    size_t answer = 0;
    for (size_t i = 0 ; i < row_; ++i) {
        if (!copy.isZero(i)) {
            ++answer;
        }
    }

    return answer;
}

// invert a matrix
template <typename Field>
Matrix<Field>& Matrix<Field>::invert() {
	Matrix<Field> copy(row_, 2 * col_);

	for (int i = 0; i < copy.getRow(); ++i) {
		for (int j = 0; j < copy[i].size(); ++j) {
			copy[i][j] = j < col_ ? matrix_[i][j] : j == i + col_;
		}
	}

    copy = copy.getReducedRowEchelonForm();

    for (size_t i = 0; i < row_; ++i) {
        for (size_t j = 0; j < col_; ++j) {
            matrix_[i][j] = copy[i][j + col_];
        }
    }

    return *this;
}

template <typename Field>
Matrix<Field> Matrix<Field>::inverted() const {
	Matrix copy = *this;

    return copy.invert();
}

// trace
template <typename Field>
Field Matrix<Field>::trace() const {
	Field sum = 0;

    for (size_t i = 0; i < col_; ++i) {
        sum += matrix_[i][i];
    }

    return sum;
}

// get reduced row echelon form
template <typename Field>
Matrix<Field> Matrix<Field>::getReducedRowEchelonForm() const {
	Matrix<Field> matrix(matrix_);

    for (size_t i = 0; i < col_ && i < row_; ++i) {
        bool flag = false;
        for (size_t j = i; j < row_; ++j) {
            if (matrix[j][i] != 0.0) {
                swapRow(matrix, i, j);
                flag = true;
                break;
            }
        }
        if (flag) {
            fullAnihilate(matrix, i, i);
        }
        flag = false;
        if (matrix[i][i] != 0.0) {
            reduceToOne(matrix, i);
        }
    }

    return matrix;
}

// getters
template <typename Field>
size_t Matrix<Field>::getRow() const {
	return row_;
}

template <typename Field>
size_t Matrix<Field>::getCol() const {
	return col_;
}

template <typename Field>
std::vector<std::vector<Field>> Matrix<Field>::getMatrix() const {
	return matrix_;
}

// get row echelon form
template <typename Field>
void Matrix<Field>::getRowEchelonForm(Matrix& matrix) const {
	    for (size_t i = 0; i < row_; ++i) {
        bool flag = false;
        size_t position;
        for (size_t j = 0; j < col_; ++j) {
            if (matrix[i][j] != 0) {
                flag = true;
                position = j;
                break;
            }
        }
        if (flag) {
            anihilate(matrix, i, position);
        }
        flag = false;
    }
}

// swap 2 rows of a matrix
template <typename Field>
void Matrix<Field>::swapRow(Matrix& matrix, size_t first, size_t second) const {
	for (size_t i = 0; i < matrix.col_; ++i) {
        std::swap(matrix[first][i], matrix[second][i]);
    }
}

// nulify every entries under a given one
template <typename Field>
void Matrix<Field>::anihilate(Matrix& matrix, size_t row, size_t column) const {
	for (size_t i = row + 1; i < matrix.row_; ++i) {
       Field koef = matrix[i][column] / matrix[row][column];
       for (size_t j = 0; j < matrix.col_; ++j) {
            matrix[i][j] -= matrix[row][j] * koef;
       }
   }
}

// nulify every entries in a given column expcept the given one
template <typename Field>
void Matrix<Field>::fullAnihilate(Matrix& matrix, size_t row, size_t column) const {
	for (size_t i = 0; i < matrix.row_; ++i) {
        if (i == row) {
            continue;
        }
        Field koef = matrix[i][column] / matrix[row][column];
        for (size_t j = 0; j < matrix.col_; ++j) {
            matrix[i][j] -= matrix[row][j] * koef;
        }
    }
}

// change leading entry of a row to 1
template <typename Field>
void Matrix<Field>::reduceToOne(Matrix& matrix, size_t row) const {
	Field koef =  Field(1) / matrix[row][row];
    for (size_t i = 0; i < matrix.col_; ++i) {
        matrix[row][i] *= koef;
    }
}

// check if the row is full of zeros
template <typename Field>
bool Matrix<Field>::isZero(size_t row) const {
	for (size_t i = 0; i < col_; ++i) {
        if (matrix_[row][i] != 0) {
            return false;
        }
    }
    return true;
}

// multiplication by a number to the left
template <typename Field>
Matrix<Field> operator*(const Field& lhs, const Matrix<Field>& rhs) {
	Matrix<Field> copy = rhs;
	copy *= lhs;

	return copy;
}

// power of a matrix to a natural number
template <typename Field>
Matrix<Field> pow(const Matrix<Field>& matrix, int power) {
	if (power == 1) {
		return matrix;
	}

	if (power % 2 == 0) {
		Matrix<Field> helper = pow(matrix, power / 2);
		return helper * helper;
	}

	return pow(matrix, power - 1) * matrix;
}
