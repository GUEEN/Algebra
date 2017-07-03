#include "List.h"

#include <vector>

#ifndef __Permutation__
#define __Permutation__

using std::vector;

class Perm
{
protected:
	List<int> P;
public:
	// default constructor makes an empty permutation
	Perm(): P(0, true)
	{		
	}

	Perm(int m): P(m)
	{
	}

	inline int& operator [](int m)
	{
		return P[m];
	}
	
	inline const int operator [](int m) const
	{
		return P[m];
	}
	
	Perm operator *(const Perm& S) const;

	Perm operator ^(const Perm& S) const;

	Perm operator +(const Perm& S) const;

	Perm operator +(int m) const;

	void operator =(int m);

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

	inline int length() const
	{
		return P.n;
	}
};

Perm operator ! (const Perm& P);

Perm operator + (int m, const Perm& P);

Perm Mult(const Perm& P, const Perm &Q, const Perm& R);

Perm Transposition(int n, int i, int j);

Perm Cycle(int n);

typedef vector<Perm> PermList;
typedef vector<Perm>::iterator PermIt;

#endif