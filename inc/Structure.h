#pragma once

#include <fstream>
#include <unordered_set>
#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <list>

#include "Group.h"

typedef uint8_t byte;
typedef std::vector<int> Deg;
typedef std::vector<byte> Certificate;

std::string CertToString(const Certificate& cert);
Certificate Cert(const std::string& s);

int compareCertificates(const Certificate& C, const Certificate& D);

struct Cell : public Perm {
    Cell();
    explicit Cell(size_t m);
    Cell(const Cell& W, int s, int m);
    void sort(const std::function<int(int,int)>& comp);

    bool counted;
    bool discrete;
};

typedef std::list<Cell> Part;

class SearchNode;
// abstract class containing all algebraic structures such as
// graphs, digraphs, hypergraphs, semigroups, posets, lattices
class Structure {
friend class SearchNode;
friend class StructSet;
public:
    explicit Structure(size_t n);
    size_t size() const;
    void certify();
    Group aut();

    static std::fstream stream;	
    static void writeStruct(const Certificate& cert);
    static void readStruct(Certificate& cert);
    static void setReadStream(const std::string& path);

protected:
    virtual int compareOrders(const Perm& P, const Perm& Q, size_t p, size_t q) const = 0;
    virtual size_t degsize() const = 0;
    virtual int color(size_t i, size_t j) const = 0;
    virtual Certificate getCertificate(const Perm& P) const = 0;

    friend bool isomorphic(const Structure& s, const Structure& t);

    size_t n;
    Certificate cert;
    std::shared_ptr<Group> auto_group;

    static SearchNode* TopSearchNode;
};

class SearchNode {
friend class Structure;
public:
    SearchNode(size_t n);
    ~SearchNode();
	
    int orbitRep(size_t v);
    void merge(size_t u, size_t v);
    void updateOrbits(const Perm& Q);
    void addGen(const Perm& Q);
    inline void refine();
    void stabilise();
    inline void changeBase(int d);

private:
    Part P;
    int FixedPoint;
    std::shared_ptr<Group> G;
    Perm CellOrbits;
    size_t Depth;
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
    void add(const Structure& s);
    size_t size() const;
    void write(std::string path, bool Append = false) const;
    void clear();
    bool contains(const Structure& s) const;

private:
    std::unordered_set<std::string> set;
};
