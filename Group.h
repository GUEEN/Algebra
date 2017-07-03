#include "Permutation.h"

#ifndef __Group__
#define __Group__

class Group
{
friend class SearchNode;
public:	
	Group(int m);
	Group(const Group& G);
	Group(Group&& G);
	Group& operator = (const Group& G);
	Group& operator = (Group&& G);
	~Group();

	bool contains(const Perm& P) const;
	void addGen(const Perm& P);

	long long order() const;	

protected:
	int n; // order of the permutation presentation
	int u; // a fixed element

	//vector<Perm*> Generators; // generators of the group
	PermList Generators;

	Perm Orbit; // orbit of u
	int NPoints; // number of points in Orbit	

	Perm* Cosets; // coset representatives
	Perm* Inverses; // inverses of coset representatives

	Group* Gu; // stabilizer of u;
};
/*
Long Order(const Group& G)
{
	return G.order();
}
*/
Group S(int m);
Group Z(int m);

#endif

