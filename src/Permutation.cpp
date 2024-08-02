#include <iostream>

#include "Permutation.h"

Perm::Perm() : size_(0), data_(nullptr) {
}

Perm::Perm(size_t m): size_(m) {
    data_ = new int[m];
    id();
}

Perm::~Perm() {
    delete[] data_;
}

Perm::Perm(const std::vector<int>& v) : size_(v.size()) {
    data_ = new int[size_];
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = v[i];
    }
}

Perm::Perm(const Perm& perm) : size_(perm.size_) {
    data_ = new int[size_];
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = perm.data_[i];
    }
}

Perm::Perm(Perm&& perm) : size_(perm.size_), data_(perm.data_) {
    perm.size_ = 0;
    perm.data_ = nullptr;
}

Perm& Perm::operator=(const Perm& perm) {
    if (perm.data_ == data_) {
        return *this;
    }
    if (size_ != perm.size_) {
        delete[] data_;
        size_ = perm.size_;
        data_ = new int[size_];
    }
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = perm.data_[i];
    }
    return *this;
}

Perm& Perm::operator=(Perm&& perm) {
    if (perm.data_ == data_) {
        return *this;
    }
    delete[] data_;
    size_ = perm.size_;
    data_ = perm.data_;
    perm.size_ = 0;
    perm.data_ = nullptr;
    return *this;
}

int& Perm::operator[](size_t i) {
    return data_[i];
}

const int& Perm::operator[](size_t i) const {
    return data_[i];
}

size_t Perm::size() const {
    return size_;
}

void Perm::print() const {
    for (size_t i = 0; i < size_; ++i) {
        std::cout << data_[i] << " ";
    }
    std::cout << std::endl;
}

bool Perm::empty() const {
    return size_ == 0;
}

void Perm::clear() {
    delete[] data_;
    data_ = nullptr;
    size_ = 0;
}

void Perm::id() {
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = i;
    }
}

void Perm::id(size_t m) {
    if (size_ != m) {
        delete[] data_;
        data_ = new int[m];
        size_ = m;
    }
    id();
}

bool Perm::isValid() const {
    for (size_t i = 0; i < size_; ++i) {
        if (data_[i] < 0 || data_[i] >= size_) {
            return false;
        }
    }
    return true;
}

bool Perm::isConst() const {
    for (size_t i = 0; i + 1 < size_; ++i) {
        if (data_[i] != data_[i + 1]) {
            return false;
        }
    }
    return true;
}

bool Perm::isConst(int m) const {
    for (size_t i = 0; i < size_; i++) {
        if (data_[i] != m) {
            return false;
        }
    }
    return true;
}

bool Perm::isId() const {
    for (size_t i = 0; i < size_; i++) {
        if (data_[i] != i) {
            return false;
        }
    }
    return true;
}

bool Perm::isInj() const {
    Perm Q(size_);
    Q = -1;

    for (size_t i = 0; i < size_; i++) {
        if (Q[data_[i]] != -1) {
            return false;
        } else {
            Q[data_[i]] = i;
	}
    }
    return true;
}

bool Perm::isBij() const {
    return isValid() && isInj();
}

bool Perm::isEven() const {
    if (isBij() == false)
        return false;
    int* b = new int[size_];
    for (int i = 0; i < size_; ++i) {
        b[i] = 0;
    }
    int Q = 0; // number of even cycles
    for (size_t i = 0; i < size_; i++) {
        if (b[i])
            continue;
        size_t j = i;	
        int q = 0;
        while (b[j] == 0) {
            b[j] = 1;
            j = data_[j];
            q++;
	}		
        if ((q & 1) == 0) {
            Q++;
        }
    }
    delete[] b;
    if (Q & 1) {
        return false;
    } else {
        return true;
    }
}

Perm Perm::operator*(const Perm& T) const {
    size_t m = T.size_;
    Perm U(m);
    for (size_t i = 0; i < m; i++) {
        int k = T[i];
        if (k >= 0 && k < size_) {
            U[i] = data_[k];
        } else {
            U[i] = 0;
        }
    }
    return U;
}

Perm Perm::operator^(const Perm& T) const {
    return Mult(!T, *this, T);
}

Perm Perm::operator+(const Perm& T) const {
    size_t m = T.size_;
    Perm U(size_ + m);
    for (size_t i = 0; i < size_; i++) {
        U[i] = data_[i];
    }
    for (size_t i = 0; i < m; i++)	{
        U[size_ + i] = T[i] + size_;
    }
    return U;
}

Perm Perm::operator+(size_t m) const {
    Perm Q(m);
    return *this + Q;
}

Perm Perm::operator^(int m) const {
    if (m < 0) {
        return (!*this)^(-m);
    }

    Perm R(size_);
    Perm P = *this;
    while (m) {
        if (m & 1) {
            --m;
            R = R * P;
        } else {
            m >>= 1;
            P = P * P;
        }
    }
    return R;
}

Perm Perm::operator[](const Perm& T) const {
    return (!(*this)) * ((*this) ^ T);
}

bool Perm::operator||(const Perm& T) const {
    if (size_ != T.size_) {
        return false;
    }
    for (size_t i = 0; i < size_; i++) {
        int k = T[i];
        if (k < 0 || k >= size_) {
            return false;
        } else {
            if (data_[k] != T[data_[i]]) {
                return false;
            }
        }
    }
    return true;
}

bool Perm::operator==(const Perm& T) const {
    if (size_ != T.size_) {
        return false;
    }
    for (size_t i = 0; i < size_; i++) {
        if (data_[i] != T[i]) {
            return false;
        }
    }
    return true;
}

Perm& Perm::operator=(int m) {
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = m;
    }
    return *this;
}

Perm operator+(size_t m, const Perm& P) {
    Perm Q(m);
    Q.id();
    return Q + P;
}

Perm operator!(const Perm& P) {
    Perm Q(P.size_);
    for (size_t i = 0; i < P.size_; ++i) {
        Q[P[i]] = i;
    }
    return Q;
}

Perm Transposition(size_t n, size_t i, size_t j) {
    Perm T(n);
    T.id();
    if (0 <= i && i < n && 0 <= j && j < n) {
        T[i] = j;
        T[j] = i;
    }
    return T;
}

Perm Cycle(size_t n) {
    Perm C(n);	
    for (size_t i = 0; i < n; i++) {
        C[i] = (i + 1) % n;
    }
    return C;
}

Perm Mult(const Perm& P, const Perm& Q, const Perm& R) {
    size_t n = R.size();
    size_t m = Q.size();
    size_t l = P.size();

    Perm U(n);
    for (size_t i = 0; i < n; i++) {
        int k = R[i];
        if (k >= 0 && k < m) {
            k = Q[k];
            if (k >= 0 && k < l) {
                U[i] = P[k];
            } else {
                U[i] = 0;
            }
        } else {
            U[i] = 0;
        }
    }
    return U;
}
