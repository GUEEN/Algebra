#pragma once
#include <cinttypes>
#include <memory>

#include "Permutation.h"

class Group {
friend class SearchNode;
public:
    Group();
    explicit Group(size_t m);
    Group operator^(const Perm& P) const;
    Group operator*(const Group& G) const;

    bool contains(const Perm& P) const;
    void addGen(const Perm& P);
    uint64_t order() const;
    bool isAbelian() const;
    bool isEven() const;
    size_t size() const;

    PermList getElements() const;
    PermList getGenerators() const;

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
    PermList Cosets; // coset representatives
    mutable PermList Inverses; // inverses of coset representatives
    Perm Orbit; // orbit of u
    size_t NPoints; // number of points in Orbit	

    std::shared_ptr<Group> Gu; // stabilizer of u;
};

Group S(size_t m);
Group A(size_t m);
Group Z(size_t m);
Group D(size_t m);
Group N(size_t p, size_t q);
Group K4();
Group Q8();
Group M11();
Group M12();
Group M22();
Group M23();
Group M24();
