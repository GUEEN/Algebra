#pragma once

#include <vector>
#include <cstddef>

class Perm {
public:
    Perm();
    explicit Perm(size_t m);
    Perm(const std::vector<int>& v);
    Perm(const Perm& perm);
    Perm(Perm&& perm);
    Perm& operator=(const Perm& perm);
    Perm& operator=(Perm&& perm);
    ~Perm();

    int& operator[](size_t i);
    const int& operator[](size_t i) const;
    Perm operator*(const Perm& S) const;
    Perm operator^(const Perm& S) const;
    Perm operator+(const Perm& S) const;
    Perm operator[](const Perm& S) const;
    bool operator||(const Perm& S) const;
    bool operator==(const Perm& S) const;
    Perm operator^(int m) const;
    Perm operator+(size_t m) const;
    Perm& operator=(int m);
    friend Perm operator!(const Perm& P);
    friend Perm operator+(size_t m, const Perm& P);

    size_t size() const;
    void print() const;
    bool empty() const;
    void clear();
    void id();
    void id(size_t m);
	
    bool isValid() const;
    bool isConst() const;
    bool isConst(int m) const;
    bool isId() const;
    bool isInj() const;
    bool isBij() const;
    bool isEven() const;

protected:
    size_t size_;
    int* data_;
};

Perm Mult(const Perm& P, const Perm &Q, const Perm& R);
Perm Transposition(size_t n, size_t i, size_t j);
Perm Cycle(size_t n);

typedef std::vector<Perm> PermList;
