#include "Permutation.h"

bool Perm::empty() const
{
	return P.empty();
}

void Perm::zero()
{
	P.clear();
}

void Perm::id()
{
	for (int i = P.n - 1; i >= 0; i--)
		P[i] = i;
}

void Perm::id(int m)
{
	if (P.empty())
		P.renew(m);
	else
		if (P.n != m)
		{
			P.clear();
			P.renew(m);
		}
	id();
}

bool Perm::isValid() const
{
	bool B = true;
	int n = P.n;
	for (int i = 0; i < n && B; i++)
		if (P[i] < 0 || P[i] >= n)
			B = false;
	return B;
}

bool Perm::isConst() const
{
	bool B = true;
	int n = P.n;
	for (int i = 0; i < n - 1 && B; i++)
		if (P[i] != P[i+1])
			B = false;
	return B;
}

bool Perm::isConst(int m) const
{
	bool B = true;
	int n = P.n;
	for (int i = 0; i < n && B; i++)
		if (P[i] != m)
			B = false;
	return B;
}

bool Perm::isId() const
{
	bool B = true;
	int n = P.n;
	for (int i = 0; i < n && B; i++)
		if (P[i] != i)
			B = false;
	return B;
}

bool Perm::isInj() const
{
	int n = P.n;
	bool B = true;
	int * Q = new int[n];
	for (int i = 0; i < n; i++) Q[n] = -1;
	for (int i = 0; i < n && B; i++)
	{
		if (Q[P[i]] != -1) B = false;
		else 
			Q[P[i]] = i;
	}
	return B;
}

bool Perm::isBij() const
{
	return isValid() && isInj();
}

// we assume here it is bijective 
bool Perm::isEven() const
{
	if (isBij() == false)
		return false;
	int n = P.n;
	List<int> b(n);
	int Q = 0; // number of even cycles
	for (int i = 0; i < n; i++)
	{
		if (b[i])
			continue;
		int j = i;	
		int q = 0;
		while (b[j] == 0)
		{
			b[j] = 1;
			j = P[j];
			q++;
		}
		if ((q & 1) == 0)
			Q++;
	}
	if (Q & 1)
		return false;
	else
		return true;
}

Perm Perm::operator * (const Perm& T) const
{
	int m = T.length();	

	Perm U(m);
	for (int i = 0; i < m; i++)
	{
		int k = T[i];				
		if (k >= 0 && k < m)
			U[i] = P[k];
		else
			U[i] = 0;		
	}	
	return U;
}

Perm Perm::operator ^ (const Perm& T) const
{
	return Mult(!T, *this, T);
}

Perm Perm::operator + (const Perm& T) const
{
	int m = T.length();	
	int n = P.n;

	Perm U(n+m);
	for (int i = 0; i < n; i++)
		U[i] = P[i];

	for (int i = 0; i < m; i++)	
		U[n+i] = T[i]+n;		
	return U;
}

Perm Perm::operator + (int m) const
{
	Perm Q(m);
	Q.id();
	return *this + Q;
}

void Perm::operator = (int m)
{
	for (int i = P.n - 1; i >= 0; i-- )
		P[i] = m;
}

Perm operator + (int m, const Perm& P)
{
	int n = P.length();
	Perm Q(m);
	Q.id();
	return Q + P;
}

Perm operator ! (const Perm& P)
{
	int n = P.length();

	Perm Q(n);
	for (int i = 0; i < n; i++)
		Q[P[i]] = i;
	return Q;
}

Perm Transposition(int n, int i, int j)
{
	Perm T(n);
	T.id();
	if (0 <= i < n && 0 <= j < n)
	{
		T[i] = j;
		T[j] = i;
	}
	return T;
}

Perm Cycle(int n)
{
	Perm C(n);
	for (int i = 0; i < n; i++)
		C[i] = (i+1) % n;
	return C;
}

Perm Mult(const Perm& P, const Perm& Q, const Perm& R)
{
	int n = R.length();	
	int m = Q.length();
	int l = P.length();

	Perm U(n);
	for (int i = 0; i < n; i++)
	{
		int k = R[i];				
		if (k >= 0 && k < m)
		{
			k = Q[k];
			if (k >= 0 && k < l)
				U[i] = P[k];
			else
				U[i] = 0;
		}
		else U[i] = 0;		
	}	
	return U;
}
