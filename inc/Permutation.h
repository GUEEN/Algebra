#pragma once

#include <vector>

class Perm {
public:
    Perm();
    explicit Perm(int m);
    Perm(const std::vector<int>& v);
    Perm(const Perm& perm);
    Perm(Perm&& perm);
    Perm& operator=(const Perm& perm);
    Perm& operator=(Perm&& perm);
    ~Perm();

    int& operator[](int i);
    const int operator[](int m) const;	
    Perm operator*(const Perm& S) const;
    Perm operator^(const Perm& S) const;
    Perm operator+(const Perm& S) const;
    bool operator||(const Perm& S) const;
    Perm operator+(int m) const;
    Perm& operator=(int m);
    friend Perm operator!(const Perm& P);
    friend Perm operator+(int m, const Perm& P);

    bool empty() const;
    void zero();
    void id();
    void id(int m);
	
    bool isValid() const;
    bool isConst() const;
    bool isConst(int m) const;
    bool isId() const;
    bool isInj() const;
    bool isBij() const;
    bool isEven() const;

    int length() const;

private:
    int size;
    int* data;
};

Perm Mult(const Perm& P, const Perm &Q, const Perm& R);
Perm Transposition(int n, int i, int j);
Perm Cycle(int n);

typedef std::vector<Perm> PermList;
typedef std::vector<Perm>::iterator PermIt;
typedef std::vector<Perm>::const_iterator ConstPermIt;
