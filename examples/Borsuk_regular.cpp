// constructing graphs of special kind
#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <functional>
#include <cmath>

#include "Graph.h"

int n;

std::vector<std::vector<size_t>> mult;
std::vector<size_t> inv;
std::vector<std::vector<size_t>> subsets;

std::vector<size_t> clique;

bool has_clique(const Graph& G, int level, int size) {
    if (level == size) {
        return true;
    }

    int prev = 0;
    if (level > 0) {
        prev = clique[level - 1] + 1;
    }

    for (clique[level] = prev; clique[level] < G.size(); ++clique[level]) {
        bool good = true;
        for (int k = 0; k < level && good; ++k) {
            if (!G.edge(clique[k], clique[level])) {
                good = false;
            }
        }
        if (good == false) {
            continue;
        }
        if (has_clique(G, level + 1, size)) {
            return true;
        }
    }
    return false;
}

int Clique_number(const Graph& G) {
    int c = 1;
    if (G.edges() == 0) {
        return c;
    }
    c = 2;

    for (c = 3;;++c) {
        clique.assign(c, 0);
        if (!has_clique(G, 0, c)) {
            std::cout << "Clique of size " << c << " has NOT been found" << std::endl;
            break;
        } else {
            std::cout << "Clique of size " << c << " has been found" << std::endl;
        }
    }
    --c;
    std::cout << "Maximum clique is of size " << c << std::endl;
    return c;
}


bool HasK3(const Graph& G) {
    size_t n = G.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (!G.edge(i, j)) {
                continue;
            }
            for (int k = j + 1; k < n; ++k) {
                if (G.edge(i, k) && G.edge(j, k)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool HasK4(const Graph& G) {
    size_t n = G.size();
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
    std::cout << "Check if chromatic number is at least " << chi << std::endl;
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
    //int chi = 2;
    int chi = 8;
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

Graph readAdjacencyMatrix(std::ifstream& stream, int n) {
    Graph G(n);
    std::string line;
    for (int i = 0; i < n; ++i) {
        std::getline(stream, line);
        for (int j = i + 1; j < n; ++j) {
            if (line[j] == '1') {
                G.addEdge(i, j);
            }
        }
    }
    return G;
}

Graph read276(std::ifstream& stream) {
    int n = 276;
    Graph G(n);
    std::string line;
    for (int i = 0; i < n; ++i) {
        std::getline(stream, line);
        for (int j = i + 1; j < n; ++j) {
            if (line[2 * j + 1] == '0') {
                G.addEdge(i, j);
            }
        }
    }
    return G;
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

void FAIL() {
    std::cout << "WE HAVE A PROBLEM" << std::endl;
    exit(1);
}


int main(int argc, char** argv) {

    //std::string path = "../strongly_reg_64.txt";
    //std::string path = "../strongly_reg_36.txt";
    //std::string path = "../strongly_reg_27.txt";
    //std::string path = "../strongly_reg_16.txt";
    //std::string path = "../strongly_reg_10.txt";
    //std::string path = "../strongly_reg_56.txt";
    //std::string path = "../strongly_reg_276.txt";
    //std::string path = "../strongly_reg_416.txt";
    //std::string path = "../two-dist_45.txt";
    std::string path = "../exponential_7.txt";

    std::string line;
    std::ifstream file(path);

    std::getline(file, line);

    int T = 1;
 
    for (int t = 0; t < T; ++t) {


    std::getline(file, line);

    int n = 126;
    //int n = 416;
    //int n = 276;
    //int n = 56;
    //int n = 36;
    //int n = 27;
    //int n = 16;
    //int n = 10;

    Graph G = readAdjacencyMatrix(file, n);
    //Graph G = read276(file);

    int k = 2 * G.edges() / n;

    std::cout << "Number of vertices is " << n << std::endl;
    std::cout << "Total number of edges is " << G.edges() << std::endl;
    std::cout << "Regularity is " << k << std::endl;

    std::cout << "Chromatic number is " << Chromatic(G) << std::endl;

    // assume the graph is indeed strongly regular
    // find out lambda and mu
    int a = -1;
    int b = -1;
    for (int i = 1; i < n; ++i) {
        if (G.edge(0, i)) {
            a = i;
            break;
        }
    }
    for (int i = 1; i < n; ++i) {
        if (!G.edge(0, i)) {
            b = i;
            break;
        }
    }

    int l = 0;
    int m = 0;

    for (int i = 1; i < n; ++i) {
        if (G.edge(0, i) && G.edge(a, i)) {
            ++l;
        }
        if (G.edge(0, i) && G.edge(b, i)) {
            ++m;
        }
    }

    std::cout << "Lambda is " << l << std::endl;
    std::cout << "Mu is " << m << std::endl;

    std::cout << "Zero check " << (n - k - 1) * m - k * (k - l - 1) << std::endl;

    int D = (l - m) * (l - m) + 4 * (k - m);
    int d = 1;
    while (d *d < D) {
        ++d;
    }

    if (D != d * d) {
        FAIL();
    }

    std::cout << "Discriminant " << D << " = " << d << " ^ 2" << std::endl;

    if ((2 * k + (n - 1) * (l - m)) % d != 0) {
        FAIL();
    }


    int f = n - 1 - (2 * k + (n - 1) * (l - m)) / d;
    int g = n - 1 + (2 * k + (n - 1) * (l - m)) / d;

    if ((f & 1) || (g & 1)) {
        FAIL();
    } else {
        f >>= 1;
        g >>= 1;
    }

    std::cout << "f and g are " << f <<  "  " << g << std::endl;  
    std::cout << "f + g + 1 = " << f + g + 1 << ". And this must be " << n << std::endl;

    int r = l - m + d;
    int s = l - m - d;

    if ((r & 1) || (s & 1)) {
        FAIL();
    } else {
        r >>= 1;
        s >>= 1;
    }

    std::cout << "r and s are " << r <<  "  " << s << std::endl;

    std::cout << "Has K3 " << HasK3(G) << std::endl;
    std::cout << "Has K4 " << HasK4(G) << std::endl;

    std::vector<std::vector<double>> M(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (G.edge(i, j)) {
                M[i][j] = 1;
            }
        }
        M[i][i] = -s;
    }

   /* for (int i = 0; i < n; ++i) {
        std::cout << "{";
        for (int j = 0; j < n; ++j) {
            std::cout << M[i][j] << ",";
        }
        std::cout << "},\n";
    }*/

    std::vector<double> mean(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            mean[j] += M[i][j];
        }
    }
    for (int j = 0; j < n; ++j) {
        mean[j] /= n;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            M[i][j] -= mean[j];
        }
    }

   /* for (int i = 0; i < n; ++i) {
        double norm = 0;
        for (int j = 0; j < n; ++j) {
            norm += M[i][j] * M[i][j];
            //std::cout << M[i][j] << " ";
        }
        std::cout << norm << std::endl;
        norm = sqrt(norm);
        for (int j = 0; j < n; ++j) {
            M[i][j] /= norm;
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double prod = 0;
            for (int k = 0; k < n; ++k) {
                prod += M[i][k] * M[j][k];
            }
            std::cout << prod << "," << G.edge(i, j) << " ";
        }
        std::cout << std::endl;
    }*/

    //std::cout << "Chromatic number is " << Chromatic(G) << std::endl;
    std::cout << "Clique number is " << Clique_number(G) << std::endl;

    }

    return 0;
}
