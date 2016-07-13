#ifndef MATRIXTEMPLATE_MATRIX_H
#define MATRIXTEMPLATE_MATRIX_H

#include <utility>
#include <stdexcept>

template <typename T>
class Matrix {
private:
    unsigned int rows;
    unsigned int columns;
    T *ptr;

public:
    Matrix(unsigned int rows, unsigned int columns);
    Matrix(unsigned int dim);
    Matrix(const Matrix<T>& M);
    virtual ~Matrix();

    T max() const;
    T min() const;
    T det() const;
    Matrix<T>& transpose();
    Matrix<T> reduced() const;
    unsigned int rank() const;
    unsigned int getRows() const;
    unsigned int getColumns() const;
    std::pair<unsigned int, unsigned int> size() const;
    Matrix<T> Row(unsigned int row) const throw(std::out_of_range);
    Matrix<T> Column(unsigned int column) const throw(std::out_of_range);
    Matrix<T> diag(int diag = 0) const throw(std::out_of_range);

    void setValue(const T& value, unsigned int row, unsigned int column) throw(std::out_of_range);
    T& getValue(unsigned int row, unsigned int column) const throw(std::out_of_range);

    const Matrix<T>& operator=(const Matrix<T>& rhs);

    Matrix<T> operator+(const Matrix<T>& rhs) const throw(std::out_of_range);
    Matrix<T> operator-(const Matrix<T>& rhs) const throw(std::out_of_range);
    Matrix<T> operator*(const Matrix<T>& rhs) const throw(std::out_of_range);
    Matrix<T> operator/(const T& rhs) const;
    Matrix<T> operator^(unsigned int pow) const;

    Matrix<T>& operator+=(const Matrix<T>& rhs) throw(std::out_of_range);
    Matrix<T>& operator-=(const Matrix<T>& rhs) throw(std::out_of_range);
    Matrix<T>& operator*=(const Matrix<T>& rhs) throw(std::out_of_range);
    Matrix<T>& operator/=(const T& rhs);
    Matrix<T>& operator^=(unsigned int pow) const;

    bool operator==(const Matrix<T>& rhs) const;
    bool operator!=(const Matrix<T>& rhs) const;

};


template <typename T>
Matrix<T>::Matrix(unsigned int rows, unsigned int columns)
        : rows(rows), columns(columns) , ptr(new T[rows * columns]) {

    for (int i = 0; i < rows * columns; i++)
        ptr[i] = 0;
}

template <typename T>
Matrix<T>::Matrix(unsigned int dim)
        : rows(dim), columns(dim), ptr(new T[rows * columns]) {
    for (int i = 0; i < rows * columns; i++)
        ptr[i] = 0;
}

template <typename T>
Matrix<T>::Matrix(const Matrix<T>& M)
        : rows(M.rows), columns(M.columns), ptr(new T[rows * columns]) {
    for (int i = 0; i < rows * columns; i++)
        ptr[i] = M.ptr[i];
}

template <typename T>
Matrix<T>::~Matrix() {
    delete[] ptr;
}

template <typename T>
Matrix<T> Matrix<T>::reduced() const {
    // implements the Gauss reduction algorithm to reduce the matrix

    if (rows != columns)
        throw std::logic_error("Matrix must be square");

    Matrix<T> app(*this);

    int n = rows;
    float m = 0.;
    for (int k = 0; k < n-1; k++) {
        if (app.ptr[k * columns + k] == 0)
            throw std::logic_error("Singular matrix");

        for (int i = k+1; i < n; i++) {
            m = app.ptr[i * columns + k] / app.ptr[k * columns + k];

            for (int j = k+1; j < n; j++)
                app.ptr[i * columns + j] = app.ptr[i * columns + j] - m * app.ptr[k * columns + j];

            app.ptr[i * columns + k] = 0;
        }
    }

    return app;
}

template <typename T>
T Matrix<T>::max() const {
    T max = this->ptr[0];
    for (int i = 1; i < rows * columns; i++)
        if (this->ptr[i] > max)
            max = this->ptr[i];
    return max;
}

template <typename T>
T Matrix<T>::min() const {
    T min = this->ptr[0];
    for (int i = 1; i < rows * columns; i++)
        if (this->ptr[i] < min)
            min = this->ptr[i];
    return min;
}

template <typename T>
T Matrix<T>::det() const {
    if (rows != columns)
        throw std::logic_error("Matrix must be square in order to calculate determinant.");

    Matrix<T> r = reduced();

    T det = 1;
    for (int i = 0; i < rows; i++)
        det *= r.getValue(i, i);

    return det;
}

template <typename T>
Matrix<T> &Matrix<T>::transpose() {
    T* t = new T[rows * columns];

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++) {
            t[j*rows + i] = ptr[i*columns + j];
        }

    delete[] ptr;
    ptr = t;

    int app = rows;
    rows = columns;
    columns = rows;
    return *this;
}

template <typename T>
unsigned int Matrix<T>::rank() const {
    unsigned int rank = columns;

    Matrix<T> app(*this);

    int n = rows;
    float m = 0;
    for (int k = 0; k < rank; k++) {
        if (app.ptr[k * columns + k] != 0) {
            for (int i = k + 1; i < n; i++) {
                m = app.ptr[i * columns + k] / app.ptr[k * columns + k];

                for (int j = k + 1; j < n; j++)
                    app.ptr[i * columns + j] = app.ptr[i * columns + j] - m * app.ptr[k * columns + j];

                app.ptr[i * columns + k] = 0;
            }
        } else {     // if == 0
            bool reduced = true;
            for (int i = k + 1; i < n; i++) {
                if (app.ptr[i * columns + k] != 0) {
                    for (int j = 0; j < rank; i++) {
                        T temp = app.ptr[k * columns + j];
                        app.ptr[k * columns + j] = app.ptr[i * columns + j];
                        app.ptr[i * columns + j] = temp;
                    }

                    reduced = false;
                    break;
                }
            }

            if (reduced) {
                rank--;

                for (int i = 0; i < n; i++)
                    app.ptr[i * columns + k] = app.ptr[i * columns + rank];
            }

            k--;
        }
    }

    return rank;
}



template <typename T>
unsigned int Matrix<T>::getRows() const {
    return rows;
}

template <typename T>
unsigned int Matrix<T>::getColumns() const {
    return columns;
}

template <typename T>
std::pair<unsigned int, unsigned int> Matrix<T>::size() const {
    return std::pair<unsigned int, unsigned int>(rows, columns);
}




template <typename T>
Matrix<T> Matrix<T>::Row(unsigned int row) const throw(std::out_of_range) {
    if (row < 0 || row >= rows)
        throw std::out_of_range("Invalid row index");

    Matrix<T> r(1, columns);
    for (int i = 0; i < columns; i++)
        r.ptr[i] = ptr[row*columns + i];

    return r;
}

template <typename T>
Matrix<T> Matrix<T>::Column(unsigned int column) const throw(std::out_of_range) {
    if (column < 0 || column >= columns)
        throw std::out_of_range("Invalid row index");

    Matrix<T> c(rows, 1);
    for (int i = 0; i < rows; i++)
        c.ptr[i] = ptr[i*rows];

    return c;
}

template <typename T>
Matrix<T> Matrix<T>::diag(int diag) const throw(std::out_of_range) {
    Matrix<T> d(rows, columns);

    if ((diag < 0 && abs(diag) >= rows ) || (diag > 0 && abs(diag) >= columns))
        throw std::out_of_range("Cannot find diagonal");

    for (int i = 0; i < rows; i++) {
        int col = i + diag;
        if (col < columns)
            d.setValue(getValue(i, col), i, col);
    }

    return d;
}

template <typename T>
void Matrix<T>::setValue(const T &value, unsigned int row, unsigned int column) throw(std::out_of_range) {
    if (row < 0 || row >= rows || column < 0 || column >= columns)
        throw std::out_of_range("Invalid index");
    ptr[row * columns + column] = value;
}

template <typename T>
T &Matrix<T>::getValue(unsigned int row, unsigned int column) const throw(std::out_of_range) {
    if (row < 0 || row >= rows || column < 0 || column >= columns)
        throw std::out_of_range("Invalid index");
    return ptr[row*columns + column];
}


//ARITHMETIC OPERATORS

template <typename T>
Matrix<T> Matrix<T>::operator+(const Matrix<T> &rhs) const throw(std::out_of_range) {
    if (this->rows != rhs.rows || this->columns != rhs.columns)
        throw std::out_of_range("Invalid matrix size for operator +");

    Matrix<T> sum(rows, columns);

    for (int i = 0; i < rows * columns; i++)
        sum.ptr[i] = this->ptr[i] + rhs.ptr[i];

    return sum;
}

template <typename T>
Matrix<T> Matrix<T>::operator-(const Matrix<T> &rhs) const throw(std::out_of_range) {
    if (this->rows != rhs.rows || this->columns != rhs.columns)
        throw std::out_of_range("Invalid matrix size for operator -");

    Matrix<T> sub(rows, columns);

    for (int i = 0; i < rows * columns; i++)
        sub.ptr[i] = this->ptr[i] - rhs.ptr[i];

    return sub;
}

template <typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T> &rhs) const throw(std::out_of_range) {
    if (columns != rhs.rows)
        throw std::logic_error("Invalid matrix sizes for operator *");

    Matrix<T> mul(rows, rhs.columns);

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < rhs.columns; j++)
            for (int k = 0; k < columns; ++k)
                mul.ptr[i * rhs.columns + j] += ptr[i * columns + k] * rhs.ptr[k * rhs.columns + j];

    return mul;
}

template <typename T>
Matrix<T> Matrix<T>::operator/(const T &rhs) const {
    Matrix<T> div(rows, columns);

    for (int i = 0; i < rows * columns; i++)
        div.ptr[i] = this->ptr[i] / rhs;

    return div;
}

template <typename T>
Matrix<T> Matrix<T>::operator^(unsigned int pow) const {
    Matrix<T> p(*this);

    for (int i = 1; i < pow; i++)
        p = p * *this;

    return p;
}

template <typename T>
const Matrix<T>& Matrix<T>::operator=(const Matrix<T>& rhs) {
    this->rows = rhs.rows;
    this->columns = rhs.columns;

    delete[] ptr;
    ptr = new T[rows * columns];

    for (int i = 0; i < rows * columns; i++)
        ptr[i] = rhs.ptr[i];
}

//BINARY OPERATOR

template <typename T>
Matrix<T> &Matrix<T>::operator+=(const Matrix<T> &rhs) throw(std::out_of_range) {
    if (this->rows != rhs.rows || this->columns != rhs.columns)
        throw std::out_of_range("Invalid matrix size for operator +=");

    for (int i = 0; i < rows * columns; i++)
        this->ptr[i] = this->ptr[i] + rhs.ptr[i];

    return *this;
}

template <typename T>
Matrix<T> &Matrix<T>::operator-=(const Matrix<T> &rhs) throw(std::out_of_range) {
    if (this->rows != rhs.rows || this->columns != rhs.columns)
        throw std::out_of_range("Invalid matrix size for operator -=");

    for (int i = 0; i < rows * columns; i++)
        this->ptr[i] = this->ptr[i] - rhs.ptr[i];

    return *this;
}

template <typename T>
Matrix<T> &Matrix<T>::operator*=(const Matrix<T> &rhs) throw(std::out_of_range) {
    if (columns != rhs.rows)
        throw std::logic_error("Invalid matrix sizes for operator *");

    T* mul = new T[rows * rhs.columns];

    for (int i = 0; i < rows * rhs.columns; i++)
        mul[i] = 0;

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < rhs.columns; j++)
            for (int k = 0; k < columns; ++k)
                mul[i * rhs.columns + j] += ptr[i * columns + k] * rhs.ptr[k * rhs.columns + j];

    columns = rhs.columns;

    delete[] ptr;
    ptr = mul;

    return *this;
}

template <typename T>
Matrix<T> &Matrix<T>::operator/=(const T &rhs) {
    for (int i = 0; i < rows * columns; i++)
        this->ptr[i] = this->ptr[i] / rhs;

    return *this;
}

template <typename T>
Matrix<T> &Matrix<T>::operator^=(unsigned int pow) const {
    Matrix<T> p(*this);

    for (int i = 1; i < pow; i++)
        p *= *this;

    return (Matrix<T> &) *this;
}


//LOGICAL OPERATORS

template <typename T>
bool Matrix<T>::operator==(const Matrix<T> &rhs) const {
    if (this->rows != rhs.rows || this->columns != rhs.columns)
        return false;
    for (int i = 0; i < rows * columns; i++)
        if (this->ptr[i] != rhs.ptr[i])
            return false;
    return true;
}

template <typename T>
bool Matrix<T>::operator!=(const Matrix<T> &rhs) const {
    return !(*this == rhs);
}

#endif //MATRIXTEMPLATE_MATRIX_H