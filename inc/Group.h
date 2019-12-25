#pragma once
#include <cinttypes>

#include "Permutation.h"

class Group {
friend class SearchNode;
public:
    Group();
    explicit Group(size_t m);
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
    size_t n; // order of the permutation presentation
    int u; // a fixed element

    PermList Generators; // generators of the group

    Perm Orbit; // orbit of u
    size_t NPoints; // number of points in Orbit	

    Perm* Cosets; // coset representatives
    Perm* Inverses; // inverses of coset representatives
    Group* Gu; // stabilizer of u;
};

Group S(size_t m);
Group A(size_t m);
Group Z(size_t m);
Group D(size_t m);
Group K4();
Group Q8();
Group M11();
Group M12();
Group M22();
Group M23();
Group M24();
