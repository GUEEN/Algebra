#pragma once
#include <cinttypes>

#include "Permutation.h"

class Group {
friend class SearchNode;
public:
    Group();
    explicit Group(int m);
    Group(const Group& G);
    Group(Group&& G);
    Group& operator=(const Group& G);
    Group& operator=(Group&& G);
    Group operator^(const Perm& P) const;
    Group operator*(const Group& G) const;
    ~Group();

    bool contains(const Perm& P) const;
    void addGen(const Perm& P);
    uint64_t order() const;
    bool isAbelian() const;
    bool isEven() const;

    bool operator<=(const Group& G) const;
    bool operator>=(const Group& G) const;
    bool operator==(const Group& G) const;
    bool operator< (const Group& G) const;
    bool operator> (const Group& G) const;
    bool operator<<(const Group& G) const;
    bool operator>>(const Group& G) const;

private:
    int n; // order of the permutation presentation
    int u; // a fixed element

    PermList Generators; // generators of the group

    Perm Orbit; // orbit of u
    int NPoints; // number of points in Orbit	

    Perm* Cosets; // coset representatives
    Perm* Inverses; // inverses of coset representatives
    Group* Gu; // stabilizer of u;
};

Group S(int m);
Group A(int m);
Group Z(int m);
Group D(int m);
Group K4();
Group Q8();
Group M11();
Group M12();
Group M22();
Group M23();
Group M24();
