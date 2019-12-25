#include <iostream>

#include "Permutation.h"

Perm::Perm() : size(0), data(nullptr) {
}

Perm::Perm(int m): size(m) {
    data = new int[m];
    id();
}

Perm::~Perm() {
    delete[] data;
}

Perm::Perm(const std::vector<int>& v) : size(v.size()) {
    data = new int[size];
    for (int i = 0; i < size; ++i) {
        data[i] = v[i];
    }
}

Perm::Perm(const Perm& perm) : size(perm.size) {
    data = new int[size];
    for (int i = 0; i < size; ++i) {
        data[i] = perm.data[i];
    }
}

Perm::Perm(Perm&& perm) : size(perm.size), data(perm.data) {
    perm.size = 0;
    perm.data = nullptr;
}

Perm& Perm::operator=(const Perm& perm) {
    if (perm.data == data) {
        return *this;
    }
    if (size != perm.size) {
        delete[] data;
        size = perm.size;
        data = new int[size];
    }
    for (int i = 0; i < size; ++i) {
        data[i] = perm.data[i];
    }
    return *this;
}

Perm& Perm::operator=(Perm&& perm) {
    if (perm.data == data) {
        return *this;
    }
    delete[] data;
    size = perm.size;
    data = perm.data;
    perm.size = 0;
    perm.data = nullptr;
    return *this;
}

int& Perm::operator[](int i) {
    return data[i];
}

const int Perm::operator[](int i) const {
    return data[i];
}

int Perm::length() const {
    return size;
}

void Perm::print() const {
    for (int i = 0; i < size; ++i) {
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;
}

bool Perm::empty() const {
    return size == 0;
}

void Perm::zero() {
    delete[] data;
    data = nullptr;
    size = 0;
}

void Perm::id() {
    for (int i = 0; i < size; ++i) {
        data[i] = i;
    }
}

void Perm::id(int m) {
    if (size != m) {
        delete[] data;
        data = new int[m];
        size = m;
    }
    id();
}

bool Perm::isValid() const {
    for (int i = 0; i < size; ++i) {
        if (data[i] < 0 || data[i] >= size) {
            return false;
        }
    }
    return true;
}

bool Perm::isConst() const {
    for (int i = 0; i < size - 1; ++i) {
        if (data[i] != data[i + 1]) {
            return false;
        }
    }
    return true;
}

bool Perm::isConst(int m) const {
    for (int i = 0; i < size; i++) {
        if (data[i] != m) {
            return false;
        }
    }
    return true;
}

bool Perm::isId() const {
    for (int i = 0; i < size; i++) {
        if (data[i] != i) {
            return false;
        }
    }
    return true;
}

bool Perm::isInj() const {
    Perm Q(size);
    Q = -1;

    for (int i = 0; i < size; i++) {
        if (Q[data[i]] != -1) {
            return false;
        } else {
            Q[data[i]] = i;
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
    int* b = new int[size];
    for (int i = 0; i < size; ++i) {
        b[i] = 0;
    }
    int Q = 0; // number of even cycles
    for (int i = 0; i < size; i++) {
        if (b[i])
            continue;
        int j = i;	
        int q = 0;
        while (b[j] == 0) {
            b[j] = 1;
            j = data[j];
            q++;
	}		
        if ((q & 1) == 0)
            Q++;
    }
    delete[] b;
    if (Q & 1) {
        return false;
    } else {
        return true;
    }
}

Perm Perm::operator*(const Perm& T) const {
    int m = T.size;
    Perm U(m);
    for (int i = 0; i < m; i++) {
        int k = T[i];
        if (k >= 0 && k < size) {
            U[i] = data[k];
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
    int m = T.size;
    Perm U(size + m);
    for (int i = 0; i < size; i++) {
        U[i] = data[i];
    }
    for (int i = 0; i < m; i++)	{
        U[size + i] = T[i] + size;
    }
    return U;
}

Perm Perm::operator+(int m) const {
    Perm Q(m);
    Q.id();
    return *this + Q;
}

Perm Perm::operator^(int m) const {
    Perm R(size);
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
    if (size != T.size) {
        return false;
    }
    for (int i = 0; i < size; i++) {
        int k = T[i];
        if (k < 0 || k >= size) {
            return false;
        } else {
            if (data[k] != T[data[i]]) {
                return false;
            }
        }
    }
    return true;
}

Perm& Perm::operator=(int m) {
    for (int i = 0; i < size; ++i) {
        data[i] = m;
    }
    return *this;
}

Perm operator+(int m, const Perm& P) {
    Perm Q(m);
    Q.id();
    return Q + P;
}

Perm operator!(const Perm& P) {
    Perm Q(P.size);
    for (int i = 0; i < P.size; ++i) {
        Q[P[i]] = i;
    }
    return Q;
}

Perm Transposition(int n, int i, int j) {
    Perm T(n);
    T.id();
    if (0 <= i && i < n && 0 <= j && j < n) {
        T[i] = j;
        T[j] = i;
    }
    return T;
}

Perm Cycle(int n) {
    Perm C(n);	
    for (int i = 0; i < n; i++) {
        C[i] = (i + 1) % n;
    }
    return C;
}

Perm Mult(const Perm& P, const Perm& Q, const Perm& R) {
    int n = R.length();	
    int m = Q.length();
    int l = P.length();

    Perm U(n);
    for (int i = 0; i < n; i++) {
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