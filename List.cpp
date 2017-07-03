#include "List.h"

template<class T>
List<T>::List(int m): n(m)
{
	P = new T[n];
}

template<class T>
List<T>::List(const List<T>& L)
{
	List<T> M(L.n);

	for (int i = 0; i < n; i++)
		M[i] = L[i];

	return M;
}
/*
template<class T>
List<T>::~List()
{
	delete[] P;
}*/
/*
template<class T>
T& operator[](int m)
{
	return P[m];
}*/