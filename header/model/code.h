#include <iostream>
#include <type_traits>
#include <vector>
#include <string>

template <typename Field = float>
class Matrix {
public:
    Matrix(size_t row, size_t col): row_(row), col_(col) {
    	matrix_.resize(row_);
    	for (size_t i = 0; i < row_; ++i) {
    		matrix_[i].resize(col_);
            for (size_t j = 0; j < col_; ++j) {
                matrix_[i][j] = 0;
            }
        }
    }

    Matrix(std::vector<std::vector<Field>> matrix): matrix_(matrix),
    												row_(matrix_.size()),
    												col_(matrix_[0].size())
    {}

    ~Matrix() = default;

    Matrix(const Matrix& other): matrix_(other.matrix_),
    							 row_(other.row_),
    							 col_(other.col_)
	{}

    Matrix& operator=(const Matrix& other) {
        matrix_ = other.matrix_;
        row_ = other.row_;
        col_ = other.col_;

        return *this;
    }

    std::vector<Field>& operator[](size_t position) {
        return matrix_[position];
    }

    const std::vector<Field> operator[](size_t position) const {
        return matrix_[position];
    }

    Matrix& operator+=(const Matrix<Field>& rhs) {
        for (size_t i = 0; i < row_; ++i) {
            for (size_t j = 0; j < col_; ++j) {
                matrix_[i][j] += rhs[i][j];
            }
        }

        return *this;
    }

    Matrix& operator-=(const Matrix<Field>& rhs) {
        for (size_t i = 0; i < row_; ++i) {
            for (size_t j = 0; j < col_; ++j) {
                matrix_[i][j] -= rhs[i][j];
            }
        }

        return *this;
    }

    Matrix operator+(const Matrix<Field>& rhs) const {
        Matrix copy = *this;
        copy += rhs;

        return copy;
    }

    Matrix operator-(const Matrix<Field>& rhs) const {
        Matrix copy = *this;
        copy -= rhs;

        return copy;
    }

    Matrix& operator*=(const Field& number) {
        for (size_t i = 0; i < row_; ++i) {
            for (size_t j = 0; j < col_; ++j) {
                matrix_[i][j] *= number;
            }
        }

        return *this;
    }

    Matrix operator*(const Field& number) const {
        Matrix copy = *this;
        copy *= number;

        return copy;
    }

    Matrix operator*(const Matrix& rhs) const {
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

    Matrix& operator*=(const Matrix<Field>& rhs) {
        Matrix<Field> copy = *this * rhs;
        *this = copy;

        return *this;
    }

    bool operator==(const Matrix& rhs) const {
        for (size_t i = 0; i < row_; ++i) {
            for (size_t j = 0; j < col_; ++j) {
                if (matrix_[i][j] != rhs[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Matrix& rhs) const {
        return !(*this == rhs);
    }

    Field det() const {
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

    Matrix<Field> transposed() const {
        std::vector<std::vector<Field>> matrix(col_);
        for (size_t i = 0; i < col_; ++i) {
            matrix[i].resize(row_);
            for (size_t j = 0; j < row_; ++j) {
                matrix[i][j] = matrix_[j][i];
            }
        }

        return Matrix<Field>(matrix);
    }

    size_t rank() const {
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

    Matrix& invert() {
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

    Matrix inverted() const {
        Matrix copy = *this;

        return copy.invert();
    }

    Field trace() const {
        Field sum = 0;

        for (size_t i = 0; i < col_; ++i) {
            sum += matrix_[i][i];
        }

        return sum;
    }

    Matrix getReducedRowEchelonForm() const {
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

    size_t getRow() const {
    	return row_;
    }

    size_t getCol() const {
		return col_;
    }

	std::vector<std::vector<Field>> getMatrix() const {
		return matrix_;
	}

private:
    void getRowEchelonForm(Matrix& matrix) const {
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

    void swapRow(Matrix<Field>& matrix, size_t first, size_t second) const{
        for (size_t i = 0; i < matrix.col_; ++i) {
            std::swap(matrix[first][i], matrix[second][i]);
        }
    }

    void anihilate(Matrix<Field>& matrix, size_t row, size_t column) const {
       for (size_t i = row + 1; i < matrix.row_; ++i) {
           Field koef = matrix[i][column] / matrix[row][column];
           for (size_t j = 0; j < matrix.col_; ++j) {
                matrix[i][j] -= matrix[row][j] * koef;
           }
       }
    }

    void fullAnihilate(Matrix<Field>& matrix, size_t row, size_t column) const {
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

    void reduceToOne(Matrix<Field>& matrix, size_t row) const {
        Field koef =  matrix[row][row];
        for (size_t i = 0; i < matrix.col_; ++i) {
            matrix[row][i] /= koef;
        }
    }

    bool isZero(size_t row) const {
        for (size_t i = 0; i < row_; ++i) {
            if (matrix_[row][i] != 0) {
                return false;
            }
        }
        return true;
    }

    std::vector<std::vector<Field>> matrix_;
    size_t row_;
    size_t col_;
};

template <typename Field>
Matrix<Field> operator*(const Field& lhs, const Matrix<Field>& rhs) {
    Matrix<Field> copy = rhs;
    copy *= lhs;

    return copy;
}

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

