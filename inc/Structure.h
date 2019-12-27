#pragma once

#include <fstream>
#include <unordered_set>
#include <functional>
#include <vector>
#include <string>

#include "Group.h"

typedef char byte;
typedef std::vector<int> Deg;
typedef std::vector<byte> Certificate;

std::string CertToString(const Certificate& cert);
Certificate Cert(const std::string& s);

int compareCertificates(const Certificate& C, const Certificate& D);

struct Cell : public Perm {
    Cell() : Perm(), counted(false), discrete(false) {};
    explicit Cell(size_t m): Perm(m), counted(false), discrete(false) {};
    Cell(const Cell& W, int s, int m): counted(false), discrete(false), Perm(m) {
        for (int i = 0; i < m; ++i) {
            data_[i] = W[s + i];
        }
    };
    void sort(const std::function<int(int,int)>& comp);

    bool counted;
    bool discrete;
};

typedef std::vector<Cell> Part;

class SearchNode;
// abstract class containing all algebraic structures such as
// graphs, digraphs, hypergraphs, semigroups, posets, lattices
class Structure {
friend class SearchNode;
public:
    explicit Structure(size_t n): n(n) {} ;
    size_t size() const;

    void certify();
    Group aut();
    Certificate cert;

    static std::fstream stream;	
    static void writeStruct(const Certificate& cert);
    static void readStruct(Certificate& cert);
    static void setReadStream(const std::string& path);

protected:
    size_t n;	
    static SearchNode* TopSearchNode;

    virtual int compareOrders(const Perm& P, const Perm& Q, int p, int q) const = 0;
    virtual size_t degsize() const = 0;
    virtual int color(size_t ii, size_t jj) const = 0;
    virtual Certificate getCertificate(const Perm& P) const = 0;
};

class SearchNode {
friend class Structure;
public:
    SearchNode(size_t n) : G(nullptr), Next(nullptr), OnBestPath(false), CellOrbits(n) {};
    ~SearchNode() {
        delete Next;
    };
	
    int orbitRep(int v);
    void merge(int uRep, int vRep);
    void updateOrbits(const Perm& Q);
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
    size_t NFixed;
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

    static bool Compare(int x, int y);
};

// class modelling an unordered collection of non-isomorphic structures. 
class StructSet {
public:
    void add(const Certificate& cert);
    size_t size() { return set.size(); }
    void write(std::string path, bool Append = false) const;
    void clear();
    bool contains(const Certificate& cert) const;

private:
    std::unordered_set<std::string> set;
};
