// constructing graphs of special kind
#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <functional>

#include "Graph.h"

int n;

std::vector<std::vector<size_t>> mult;
std::vector<size_t> inv;
std::vector<std::vector<size_t>> subsets;

void next(int level, std::vector<size_t>& ch) {
    if (level == n) {
        subsets.push_back(ch);
        return;
    }

    if (ch[level] == 1) {
        next(level + 1, ch);
        return;
    }

    next(level + 1, ch);

    int y = inv[level];
    if (y >= level) {
        ch[level] = 1;
        ch[y] = 1;
        next(level + 1, ch);
        ch[level] = 0;
        ch[y] = 0;
    }
}


bool HasK4(const Graph& G) {
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (!G.edge(i, j)) {
                continue;
            }

            for (int k = j + 1; k < n; ++k) {
                if (!G.edge(i, k) || !G.edge(j, k)) {
                    continue;
                }

                for (int l = k + 1; l < n; ++l) {
                    if (G.edge(i, l) && G.edge(j, l) && G.edge(k, l)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Connected(const Graph& G) {
    std::vector<int> b(n);

    std::vector<std::vector<int>> e(n);
    for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j) {
        if (G.edge(i, j)) {
            e[i].push_back(j);
        }
    }

    std::function<void(int)> dfs = [&b, &e, &dfs](int v) {
        b[v] = 1;
        for (int c : e[v]) {
            if (b[c] == 0) {
                dfs(c);
            }
        }
    };

    dfs(0);
    for (int i = 0; i < n; ++i) {
        if (b[i] == 0) {
            return false;
        }
    }
    return true;
}

// check if the chromatic number is at least chi
bool ChromaticAtLeast(const Graph& G, int chi) {
    size_t n = G.size();
    std::vector<int> colors(chi - 1);
    for (int i = 0; i < chi - 1; ++i) {
        colors[i] = i + 1;
    }

    std::vector<int> c(n);

    bool found = false;

    std::function<void(int)> chrom = [&c, &G, &chrom, &colors, &found, &n, &chi](int level) {
        if (level == n) {
            found = true;
            return;
        }
        std::vector<int> used(chi);

        for (int u = 0; u < level; ++u) {
            if (G.edge(u, level)) {
                used[c[u]] = 1;
            }
        }

        for (int col : colors) {
            if (used[col] == 0) {
                c[level] = col;
                chrom(level + 1);
            }
            if (found) {
                return;
            }
        }
    };

    // start from any color
    c[0] = colors[0];
    chrom(0);

    return found == false;
}

int Chromatic(const Graph& G) {
    // assume G is connected
    int chi = 2;
    while (ChromaticAtLeast(G, chi)) {
        ++chi;
    }

    return chi - 1;
}

void writeAdjacencyMatrix(const std::string& filename, const Graph& G) {
    std::ofstream file(filename);

    int n = G.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            file << G.edge(i, j);
        }
        file << std::endl;
    }
}

void print_cycles(const Perm& P) {
    int n = P.size();
    std::vector<int> b(n);
    for (int i = 0; i < n; ++i) {
        if (b[i] == 0)  {
            if (P[i] == i) {
                b[i] = 1;
                continue;
            }

            std::cout << "(";
            int j = i;
            while (b[j] == 0) {
                b[j] = 1;
                std::cout << j + 1;
                j = P[j];
                if (b[j] == 0) {
                    std::cout << ",";
                }
            }
            std::cout << ")";
        }
    }
    //std::cout << ", ";
    std::cout << std::endl;
}

// class for unitriangular group
//
//  1 a b
//    1 c
//      1


template<int p>
class UT3 {
public:
    UT3(size_t n) {
        a = (n / p / p) % p;
        b = (n / p) % p;
        c = n % p;
    }

    UT3(int a, int b, int c) : a(a % p), b(b % p), c(c % p) {
    }

    size_t hash() const {
        return (a * p + b) * p + c;
    }

    UT3 operator*(const UT3& x) const {
        int a1 = x.a + a;
        int b1 = x.b + a * x.c + b;
        int c1 = c + x.c;

        return UT3(a1, b1, c1);
    }

private:
    int a;
    int b;
    int c;
};



int main(int argc, char** argv) {
    // select a small group
    //Group H = D(11);
    //Group H = S(4);
    //Group H = D(6) * Z(2);
    //Group H = Q8() * Z(3);
    //Group H = D(13);
    //Group H = D(14);
    //Group H = S(3) * K4();
    //Group H = A(4) * Z(2);
    //Group H = D(15);
    //Group H = D(3) * Z(5);
    //Group H = D(3) * Z(5);
    //Group H = N(3, 7);

    std::string group_name = "UT(3,3)";

   // PermList elements = H.getElements();
   // n = elements.size();

    n = 27;

    // multiplication matrix
    mult.assign(n, std::vector<size_t>(n));
    // inversion table
    inv.resize(n);

/*
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            Perm p = elements[i] * elements[j];
            if (p.isId()) {
                inv[i] = j;
            }

            for (int k = 0; k < n; ++k) {
                if (p == elements[k]) {
                    mult[i][j] = k;
                    break;
                }
            }
        }
    }
*/
    for (int i = 0; i < 27; ++i) {
        UT3<3> p(i); 
        for (int j = 0; j < 27; ++j) {
            UT3<3> q(j);
            UT3<3> r = p * q;

            size_t k = r.hash();

            mult[i][j] = k;
            if (k == 0) {
                inv[i] = j;
            }
        }
    }


    // get all subsets of [1, ..., n - 1] that are inv-invariant
    std::vector<size_t> ch(n);
    next(1, ch);

    // get all Cayley graphs for all these subsets

    GraphSet graphs(n);

    size_t j = 0;
    for (const auto& subset : subsets) {
        ++j;
        // construct graph
        Graph G(n);
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                int k = mult[i][inv[j]];
                if (subset[k]) {
                    G.addEdge(i, j);
                }
            }
        }

       /* for (int x : subset) {
            std::cout << x << " ";
        }
        std::cout << std::endl;*/

        // verify conditions
        // connected
        if (!Connected(G)) {
            continue;
        }
        // no K4
        if (HasK4(G)) {
            continue;
        }

        // chromatic number >= 6
       /* if (!ChromaticAtLeast(G, 6)) {
            continue;
        }*/

        /*
        int chi = Chromatic(G);

        std::cout << G.edges() << " " << chi << std::endl;

        if (chi >= 6) {
           q++;
        }*/
        G.certify();

       /* std::cout << "chi = " << Chromatic(G) << std::endl;
        std::cout << "edges = " << G.edges() << std::endl;
        std::cout << "aut =" << G.aut().order() << std::endl;*/
       /* if (G.aut().order() == 96) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    std::cout << G.edge(i, j);
                }
                std::cout << std::endl;
            }
        }*/

        graphs.insert(G);
        std::cout << j << " / " << subsets.size() << std::endl; 

    }

    std::cout << graphs.size() << std::endl;

    size_t q = 0;
    for (Graph G : graphs) {
        G.certify();

        int v = G.size();
        int e = G.edges();
        int a = G.aut().order();
        int chi = Chromatic(G);

        if (chi >= 6) {
            ++q;
            std::string folder = "../data/BORSUK/";
            std::string filename = group_name + "_#" + std::to_string(q) + "__v=" + std::to_string(v) + "_e=" + std::to_string(e)
                                 + "_a=" + std::to_string(a) + "_chi=" + std::to_string(chi) + ".txt";
            writeAdjacencyMatrix(folder + filename, G);

            std::cout << filename << std::endl;

            PermList list = G.aut().getElements();
            for (int i = 0; i < list.size(); ++i) {
                for (int j = i + 1; j < list.size(); ++j) {
                //for (int k = j + 1; k < list.size(); ++k) {
                    const auto& P = list[i];
                    const auto& Q = list[j];
                    //const auto& R = list[k];

                    Group H(n);
                    H.addGen(P);
                    if (!H.contains(Q)) {
                        H.addGen(Q);
                    }
                    //H.addGen(R);

                    if (H.isAbelian()) {
                        std::cout << H.order() << " is ab? " << H.isAbelian() << std::endl;
                    }

                }
            }
            std::cout << "----------------------------------------------------\n";
        }

    }

    std::cout << q << std::endl;


    return 0;
}
