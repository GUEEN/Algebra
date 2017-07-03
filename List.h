//a class modelling an array of constant size

#ifndef __List__
#define __List__

template <class T>
class List
{
public:
	int n;
	
	List();
	List(int m);
	List(int m, bool empty);
	List(const List<T>& L);
	List(const List<T>& L, int s, int m); // copy of list L from s with m elements
	List(List<T>&& L);
	List<T>& operator = (const List<T>& L);
	List<T>& operator = (List<T>&& L);	

	~List();

	int size() const;
	bool empty() const;
	void clear();
	void null();
	void renew(int m);
	void constant(const T& V);

	inline T& operator [](int m)
	{
		return P[m];
	}
	
	inline const T& operator[](int m) const
	{
		return P[m];
	}
	
	void sort(bool (*compare)(const T& A, const T& B));
	T* ptr() const;

protected:
	void sort(int p, int q, bool (*compare)(const T& A, const T& B));
	
	T* P;
};
//constructor
template<class T>
List<T>::List(): n(0), P(nullptr)
{	
}
//constructor
template<class T>
List<T>::List(int m): n(m)
{
	P = new T[m];
}
//constructor
template<class T>
List<T>::List(int m, bool empty): n(m)
{
	if (empty)
		P = nullptr;
	else
		P = new T[m];
}
//constructor
template<class T>
List<T>::List(const List<T>& L, int s, int m): n(m), P(L.P + s)
{	
}
//copy constructor
template<class T>
List<T>::List(const List<T>& L): n(L.n)
{
	P = new T[n];
	for (int i = 0; i < n; i++)
		P[i] = L.P[i];
}
//move constructor
template<class T>
List<T>::List(List<T>&& L): n(L.n)
{
	P = new T[n];
	for (int i = 0; i < n; i++)
		P[i] = L.P[i];
}
//copy assignment
template<class T>
List<T>& List<T>::operator = (const List<T>& L)
{
	if (n != L.n)
	{
		n = L.n;
		delete[] P;
		P = new T[n];
	}
	else
		if (P == nullptr)
			P = new T[n];

	for (int i = L.n - 1; i >= 0; i--)
		P[i] = L[i];
	return *this;
}
//move assignment
template<class T>
List<T>& List<T>::operator = (List<T>&& L)
{
	if (n != L.n)
	{
		n = L.n;
		delete[] P;
		P = new T[n];
	}
	else
		if (P == nullptr)
			P = new T[n];

	for (int i = L.n - 1; i >= 0; i--)
		P[i] = L[i];
	return *this;
}

//destructor
template<class T>
List<T>::~List()
{	
	if (P)
	delete[] P;
}

template<class T>
inline bool List<T>::empty() const
{
	return (P == nullptr);
}

template<class T>
inline int List<T>::size() const
{
	return n;
}

template<class T>
inline void List<T>::clear()
{
	n = 0;
	if (P)
	delete[] P;
	P = nullptr;
}

template<class T>
inline void List<T>::null()
{
	P = nullptr;
}

template<class T>
inline void List<T>::renew(int m)
{
	if (P)
		delete[] P;
	n = m;
	P = new T[m];
}

template<class T>
inline void List<T>::constant(const T& V)
{
	for (int i = 0; i < n; i++)
		P[i] = V;
}

template<class T>
inline void List<T>::sort(bool (*compare)(const T& A, const T& B))
{
	sort(0, n - 1, compare);	
}

// sort all in increasing order. compare returns  A < B,
// i.e. i A, B is correct order
template<class T>
void List<T>::sort(int p, int q, bool (*compare)(const T& A, const T& B)) 
{
	if (q <= p) return;
	if (q == p + 1)
	{
		if (compare(P[q], P[p]))
		//if (P[p] > P[q])
		{
			T Q = P[p];
			P[p] = P[q];
			P[q] = Q;
		}
		return;
	}
	T X = P[q];
	int i = p - 1;

	for (int j = p; j < q; j++)
		if (compare(P[j], X))
		{
			i++;
			T Q = P[i];
			P[i] = P[j];
			P[j] = Q;
		}
	i++;
	T Q = P[i];
	P[i] = P[q];
	P[q] = Q;

	if (i - 1 > p) 
		sort(p, i - 1, compare);
	if (q > i + 1)
		sort(i + 1, q, compare);	
}

template<class T>
T* List<T>::ptr() const
{
	return P;
}

template<class T>
inline bool operator == (const List<T>& L, const List<T>& M)
{
	if (L.n != M.n) return false;
	int n = L.n;
	for (int i = 0; i < n; i++)
	{
		if (M[i] != L[i])
			return false;
	}
	return true;
}

template<class T>
inline bool operator != (const List<T>& L, const List<T>& M)
{
	if (L.n != M.n) return true;
	int n = L.n;
	for (int i = 0; i < n; i++)
	{
		if (M[i] != L[i])
			return true;
	}
	return false;
}


#endif