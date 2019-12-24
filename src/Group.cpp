#include "Group.h"

Group::Group() : Group(1) {
}

Group::Group(int n) : n(n), u(-1), Orbit(n), Generators(), NPoints(1), Gu(nullptr) {
    Cosets = new Perm[n];
    Inverses = new Perm[n];
}

Group::Group(const Group& G) : n(G.n), u(G.u), Orbit(G.Orbit), NPoints(G.NPoints), Generators(G.Generators) {
    Cosets = new Perm[n];
    Inverses = new Perm[n];
    for (int i = 1; i < n; ++i)	{
        Cosets[i] = G.Cosets[i];
        Inverses[i] = G.Inverses[i];
    }

    if (G.Gu) {
        Gu = new Group(*G.Gu);
    } else {
        Gu = nullptr;
    }
}

Group::Group(Group&& G) : n(G.n), Orbit(std::move(G.Orbit)), NPoints(G.NPoints), u(G.u), 
    Generators(std::move(G.Generators)), Cosets(G.Cosets), Inverses(G.Inverses), Gu(G.Gu) {
    G.Cosets = nullptr;
    G.Inverses = nullptr;
    G.Gu = nullptr;
}

Group& Group::operator=(const Group& G) {
    if (&G == this) {
        return *this;
    }

    delete[] Cosets;
    delete[] Inverses;
    delete Gu;

    n = G.n;
    Orbit = G.Orbit;
    NPoints = G.NPoints;	
    u = G.u;
    Generators = G.Generators;

    Cosets = new Perm[n];
    Inverses = new Perm[n];

    for (int i = 1; i < n; ++i)	{
        Cosets[i] = G.Cosets[i];
	Inverses[i] = G.Inverses[i];
    }
    if (G.Gu) {
        Gu = new Group(*G.Gu);
    } else {
        Gu = nullptr;
    }
    return *this;
}

Group& Group::operator=(Group&& G) {
    delete[] Cosets;
    delete[] Inverses;
    delete Gu;

    n = G.n;
    Orbit = std::move(G.Orbit);
    NPoints = G.NPoints;	
    u = G.u;
    Generators = std::move(G.Generators);

    Cosets = G.Cosets;
    Inverses = G.Inverses;
    Gu = G.Gu;

    G.Cosets = nullptr;
    G.Inverses = nullptr;
    G.Gu = nullptr;
    return *this;
}

Group::~Group() {
    delete[] Cosets;
    delete[] Inverses;
    delete Gu;
}

Group Group::operator^(const Perm& P) const {
    Group G(n);
    for (auto it = Generators.begin(); it != Generators.end(); ++it) {
        G.addGen((*it) ^ P);
    }
    return G;
}

Group Group::operator*(const Group& H) const {
    int m = H.n;
    Group G(n + m);
    for (auto it = Generators.begin(); it != Generators.end(); ++it) {
        G.addGen((*it) + m);
    }
    for (auto it = H.Generators.begin(); it != H.Generators.end(); ++it) {
        G.addGen(n + (*it));
    }
    return G;
}

bool Group::contains(const Perm& P) const {
    bool id = true;
    for (int k = 0; k < n; k++) {
        if (P[k] != k) {
            id = false;
            break;
        }
    }
    if (id) {
	    return true;
    }
    // these two conditions mean that we have a trivial group here
    if (Gu == nullptr) {
        return false;
    }
    if (Generators.empty()) {
        return false;
    }

    const int v = P[u];
    if (Cosets[v].length() == 0) {
        return false;
    }

    if (Inverses[v].length() == 0) {
        Inverses[v] = !Cosets[v];
    }
    return Gu->contains(Inverses[v] * P);
}
// adding a new generator to the group
void Group::addGen(const Perm& P) {
    if (Gu == nullptr) {
        Gu = new Group(n);
        u = 0;
        while (P[u] == u) {
            u++;
        }

        NPoints = 1;
        Orbit[0] = u;
        Cosets[u].id(n);
    } else if (Generators.empty()) {
        u = 0;
	while (P[u] == u) {
            u++;
        }

        NPoints = 1;		
        Orbit[0] = u;
        Cosets[u].id(n);
    }

    Generators.push_back(P);

    int M = NPoints;
    int k = 0;
    while (k < M) {
        int v = Orbit[k];
        int w = P[v];

        if (Cosets[w].empty()) {
            Orbit[NPoints] = w;
            NPoints++;
            Cosets[w] = P * Cosets[v];
        } else {
            if (Inverses[w].empty()) {
                Inverses[w] = !Cosets[w];			
	    }
            Perm Q = Mult(Inverses[w], P, Cosets[v]);
            if (!Gu->contains(Q)) {
                Gu->addGen(Q);
            }
        }
        k++;
    }
    
    // apply new generators to all points
    while (k < NPoints) {
       int v = Orbit[k];
       for (auto it = Generators.begin(); it != Generators.end(); ++it) {
           const Perm& Gen = *it;
           int w = Gen[v];
           if (Cosets[w].empty()) {				
               Orbit[NPoints] = w;
               NPoints++;
               Cosets[w] = Gen * Cosets[v];
           } else {
               if (Inverses[w].empty()) {
                   Inverses[w] = !Cosets[w];
               }
               Perm Q = Mult(Inverses[w], Gen, Cosets[v]);
               if (!Gu->contains(Q)) {
                   Gu->addGen(Q);
               }
           }
        }
        k++;
    }
}

uint64_t Group::order() const {
    if (Gu == nullptr) {
        return 1;
    } else {
        return Gu->order() * NPoints;
    }
}

bool Group::isAbelian() const {
    for (auto it1 = Generators.begin(); it1 != Generators.end(); ++it1) {
        for (auto it2 = it1 + 1; it2 != Generators.end(); ++it2) {
            if (((*it1) || (*it2)) == false) {
                return false;
            }
        }
    }
    return true;
}

bool Group::isEven() const {
    for (auto it = Generators.begin(); it != Generators.end(); ++it) {
        if ((*it).isEven() == false) {
            return false;
        }
    }
    return true;
}

bool Group::operator<=(const Group& G) const {
    for (auto it = Generators.begin(); it != Generators.end(); ++it) {
        if (!G.contains(*it)) {
            return false;
        }
    }
    return true;
}

bool Group::operator >=(const Group& G) const {
    return G <= *this;
}

bool Group::operator==(const Group& G) const {
    return (G <= *this) && (*this <= G);
}

bool Group::operator<(const Group& G) const {
    return *this <= G && !(G <= *this);
}

bool Group::operator>(const Group& G) const {
    return G < *this;
}
// normal subgroup
bool Group::operator<<(const Group& G) const {
    for (auto it_g = G.Generators.begin(); it_g != G.Generators.end(); ++it_g) {
        for (auto it = Generators.begin(); it != Generators.end(); ++it) {
            if (!contains((*it)^(*it_g))) {
                return false;
            }
        }
    }
    return true;
}

bool Group::operator>>(const Group& G) const {
    return G << *this;
}

// symmetric group
Group S(int m) {
    Group S(m);
    // symmetric group on  n  points
    if (m > 1) {
        S.addGen(Cycle(m));
    } if (m > 2) {
        S.addGen(Cycle(2) + (m - 2));
    }
    return S;
}
// alternating group
Group A(int m) {
    Group G(m);
    if (m > 2) {
        if (m & 1) {
            G.addGen(Cycle(m));
        } else {
            G.addGen(1 + Cycle(m - 1));
        }
        G.addGen(Cycle(3) + (m - 3));
    }
    return G;
}
// cyclic group
Group Z(int m) {
    Group G(m);
    if (m > 1) {
        G.addGen(Cycle(m));
    }
    return G;
}
// dyhedral group
Group D(int m) {
    Group G(m);
    if (m == 1) {
        return Z(2);
    }
    if (m == 2) {
        return K4();
    }
    // m > 2
    G.addGen(Cycle(m));
    Perm P(m);
    P[0] = 0;
    for (int i = 1; 2 * i <= m; i++) {
        P[i] = m - i;
        P[m - i] = i;
    }		
    G.addGen(P);
    return G;
}
// Klein four-group
Group K4() {
    Group G(4);
    G.addGen(Perm({1,0,3,2}));
    G.addGen(Perm({2,3,0,1}));
    return G;
}
// quaternion group
Group Q8() {
    Group G(8);
    G.addGen(Perm({1,3,5,6,2,7,0,4}));
    G.addGen(Perm({2,4,3,7,6,1,5,0}));
    return G;
}
// Mathieu sporadic groups
Group M11() {
    Group G(11);
    G.addGen(Cycle(11));
    G.addGen(Perm({0,1,6,9,5,3,10,2,8,4,7}));
    return G;
}

Group M12() {
    Group G(12);
    G.addGen(Cycle(11)+1);
    G.addGen(Perm({0,1,6,9,5,3,10,2,8,4,7,11}));
    G.addGen(Perm({11,10,5,7,8,2,9,3,4,6,1,0}));
    return G;
}

Group M22() {
    Group G(22);
    G.addGen(Cycle(11)+Cycle(11));
    //(0,3,4,8,2)(1,7,9,6,5)(11,14,15,19,13)(12,18,20,17,16)
    //(0,20)(1,9,7,5)(2,12,3,16)(4,18,8,17)(10,21)(11,13,15,19)
    G.addGen(Perm({3,7,0,4,8,1,5,9,2,6,10,14,18,11,15,19,12,16,20,13,17,21}));
    G.addGen(Perm({20,9,12,16,18,1,6,5,17,7,21,13,3,15,14,19,2,4,8,11,0,10}));
    return G;
}

Group M23() {
    Group G(23);
    G.addGen(Cycle(23));	
    //(2, 16, 9, 6, 8)(3,12, 13,18,4)(7,17,10,11,22)(14,19,21,20,15)
    G.addGen(Perm({0,1,16,12,3,5,8,17,2,6,11,22,13,18,19,14,9,10,4,21,15,20,7}));
    return G;
}

Group M24() {
    Group G(24);	
    //(0, 15, 7, 22, 12, 13, 4)(1, 6, 10, 18, 19, 23, 11)(2, 3, 16, 8, 21, 20, 14)
    //(0, 23)(1, 20)(2, 9)(3, 21)(4, 8)(5, 22)(6, 7)(10, 17)(11, 19)(12, 13)(14, 18)(15, 16)	
    G.addGen(Perm({15,6,3,16,0,5,10,22,21,9,18,1,13,4,2,7,8,17,19,23,14,20,12,11}));
    G.addGen(Perm({23,20,9,21,8,22,7,6,4,2,17,19,13,12,18,16,15,10,14,11,1,3,5,0}));	
    return G;
}
