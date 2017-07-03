#include "Group.h"

Group::Group(int m) : n(m), u(-1), Orbit(m), Generators(), NPoints(1), Gu(nullptr)
{
	Cosets = new Perm[m];
	Inverses = new Perm[m];
}
// copy constructor
Group::Group(const Group& G) : n(G.n), u(G.u), Orbit(G.Orbit), NPoints(G.NPoints), Generators(G.Generators)
{
	Cosets = new Perm[n];
	Inverses = new Perm[n];

	for (int i = 1; i < n; i++)
	{
		Cosets[i] = G.Cosets[i];
		Inverses[i] = G.Cosets[i];
	}

	if (G.Gu)
	{
		Gu = new Group(*G.Gu);
	}
	else
		Gu = nullptr;
}
// move constructor
Group::Group(Group&& G) : n(G.n), Orbit(G.Orbit), NPoints(G.NPoints), u(G.u), Generators(G.Generators)
{
	Cosets = new Perm[n];
	Inverses = new Perm[n];

	for (int i = 1; i < n; i++)
	{
		Cosets[i] = G.Cosets[i];
		Inverses[i] = G.Cosets[i];
	}

	if (G.Gu)
	{
		Gu = new Group(*G.Gu);
	}
	else
		Gu = nullptr;
}
// copy assignment
Group& Group::operator= (const Group& G)
{
	delete[] Cosets;
	delete[] Inverses;
	if (Gu)
		delete Gu;

	n = G.n;	 
	Orbit = G.Orbit;
	NPoints = G.NPoints;	
	u = G.u;
	Generators = G.Generators;

	Cosets = new Perm[n];
	Inverses = new Perm[n];

	for (int i = 1; i < n; i++)
	{
		Cosets[i] = G.Cosets[i];
		Inverses[i] = G.Cosets[i];
	}
	Gu = new Group(*G.Gu);

	return *this;
}
// move assignment
Group& Group::operator= (Group&& G)
{
	delete[] Cosets;
	delete[] Inverses;
	if (Gu)
		delete Gu;

	n = G.n;	 
	Orbit = G.Orbit;
	NPoints = G.NPoints;	
	u = G.u;
	Generators = G.Generators;

	Cosets = new Perm[n];
	Inverses = new Perm[n];

	for (int i = 1; i < n; i++)
	{
		Cosets[i] = G.Cosets[i];
		Inverses[i] = G.Cosets[i];
	}
	Gu = new Group(*G.Gu);

	return *this;
}
// destructor
Group::~Group()
{
	delete[] Cosets;
	delete[] Inverses;
	if (Gu)
		delete Gu;
}

bool Group::contains(const Perm& P) const
{
	for (int k = 0; k < n; k++)
			if (P[k] != k) goto label;
		
	return true;

label:
	// these two conditions mean that we have a trivial group here
	if (Gu == nullptr) return false;
	if (Generators.empty()) return false;

	const int v = P[u];
	if (Cosets[v].length() == 0) return false;

	if(Inverses[v].length() == 0)			
				Inverses[v] = !Cosets[v];

	return Gu->contains(Inverses[v] * P);
}

void Group::addGen(const Perm& P)
{
	if (Gu == nullptr)
	{
		Gu = new Group(n);
		u = 0;
		while (P[u] == u) u++;

		NPoints = 1;		
		Orbit[0] = u;
		Cosets[u].id(n);		
	}
	else
	if (Generators.empty())
	{
		u = 0;
		while (P[u] == u ) u++;

		NPoints = 1;		
		Orbit[0] = u;
		Cosets[u].id(n);
	}

	Generators.push_back(P);

	int M = NPoints;

	int k = 0;
	while (k < M)
	{
		int v = Orbit[k];
		int w = P[v];

		if (Cosets[w].empty())
		{			
			Orbit[NPoints] = w;
			NPoints++;
			Cosets[w] = P * Cosets[v];			
		}
		else
		{
			if(Inverses[w].empty())			
				Inverses[w] = !Cosets[w];			
			
			Perm Q = Mult(Inverses[w], P, Cosets[v]);

			if (!Gu->contains(Q))
				Gu->addGen(Q);
		}
		k++;
	}
	// apply new generators to all points
	while (k < NPoints)
	{
		int v = Orbit[k];

		//for (vector<Perm *>::iterator it = Generators.begin(); it!=Generators.end(); ++it)
		//for (PermIterator it(Generators); !it.end(); ++it)
				
		//for (int i = Generators.n - 1; i >=0; i--)
		for (PermIt it = Generators.begin(); it!= Generators.end(); ++it)
		{			
			Perm Gen = *it;

			int w = Gen[v];
			if (Cosets[w].empty())
			{				
				Orbit[NPoints] = w;
				NPoints++;
				Cosets[w] = Gen * Cosets[v];				
			}
			else
			{
				if (Inverses[w].empty())
					Inverses[w] = !Cosets[w];
				Perm Q = Mult(Inverses[w], Gen, Cosets[v]);

				if (!Gu->contains(Q))
					Gu->addGen(Q);
			}			
		}
		k++;
	}
}

long long Group::order() const
{
	if (Gu == nullptr)
		return 1;
	else
		return Gu->order() * NPoints;
}

Group S(int m)
{
	Group S(m);
	// symmetric group on  n  points
	if (m > 1)
		S.addGen(Cycle(m));
	if (m > 2)
		S.addGen(Cycle(2) + (m - 2));
	return S;
}

Group Z(int m)
{
	Group G(m);
	G.addGen(Cycle(m));
	return G;
}

