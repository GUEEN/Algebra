#include <fstream>
#include <unordered_set>
#include <vector>
#include <string>

#include "Group.h"

#ifndef __Structure__
#define __Structure__

using std::vector;
using std::unordered_set;

typedef char byte;
typedef List<int> Deg;
typedef List<byte> Certificate;

std::string CertToString(const Certificate& cert);
Certificate Cert(const std::string& s);

int compareCertificates(const Certificate& C, const Certificate& D);

class Cell
{
public:	
	List<int> V;

    bool counted;
    bool discrete;

	Cell() : counted(false), discrete(false), V(0) {};
	Cell(int m): counted(false), discrete(false), V(m) {};
	Cell(const List<int>& W, int s, int m): counted(false), discrete(false), V(W, s, m) {};

	inline int& operator [](int m)
	{
		return V[m];
	}

	inline int operator [](int m) const
	{
		return V[m];
	}

	void clear()
	{
		V.clear();
	}
};

typedef vector<Cell> Part;
typedef vector<Cell>::iterator PartIt;

// abstract class containing all algebraic structures such as
// graphs, digraphs, hypergraphs, semigroups, posets, lattices
class Structure
{
friend class SearchNode;
public:
	int n;
	Structure(int n): n(n) {} ;

	void certify();
	Group aut();

	Certificate cert;

	static std::fstream stream;
	
	static void writeStruct(const Certificate& cert);
	static void readStruct(Certificate& cert);
	static void setReadStream(std::string path);

private:
	
	static SearchNode* TopSearchNode;	
	
	virtual int compareOrders(const Perm& P, const Perm& Q, int p, int q) const = 0;
	virtual int degsize() const = 0;
	virtual int color(int ii, int jj) const = 0;
	virtual Certificate getCertificate(const Perm& P) const = 0;
};

class SearchNode
{
friend class Structure;
public:
	SearchNode(int n) : G(nullptr), Next(nullptr), OnBestPath(false), CellOrbits(n) {};
	~SearchNode()
	{
		delete Next;
	};
		
	int orbitRep(int v);
	inline void merge(int uRep, int vRep);
	inline void updateOrbits(const Perm& Q);
	void addGen(const Perm& Q);
	inline void refine();
	void stabilise();
	inline void changeBase(int d);

private:
	Part P;
	int FixedPoint;
	Group* G;
	Perm CellOrbits;
	int Depth;
	int NFixed;
	bool OnBestPath;
	SearchNode* Next;
	
	static Perm F;
	static Perm B;
	static Deg* Degg;
	static const Structure* S;

	static bool AutoFound;
	static bool Bexists;
	static int BasisOK;
	static bool IsDiscrete;
	static SearchNode* LastBaseChange;

	static bool Compare(const int& x, const int& y);
};

// class modelling an unordered collection of non-isomorphic structures. 
class StructSet
{
public:
	void add(const Certificate& cert);
	inline int size() { return set.size(); }
	void write(std::string path, bool Append = false) const;
	void clear();
	bool contains(const Certificate& cert) const;

private:
	unordered_set<std::string> set;
};

#endif
