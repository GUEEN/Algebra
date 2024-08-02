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
        std::cout << ">= " << chi << std::endl;
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

int binom(int n, int k) {
    if (k == 0) {
       return 1;
    }
    return binom(n - 1, k - 1) * n / k;
}


// vertices are k-elements subsets that are joined with edge when the intersetion is empty
Graph Intersection(int n, int k) {
    std::vector<std::vector<int>> vertices;
    vertices.emplace_back(n);
    for (int i = 0; i < k; ++i) { // updating
        std::vector<std::vector<int>> new_verts;
        for (auto& v : vertices) {
            for (int j = 0; j < n; ++j) {
                if (v[j] != 0) {
                    break;
                }
                v[j] = 3;
                new_verts.push_back(v);
                v[j] = 0;
            }
        }
        std::swap(vertices, new_verts);
    }

    std::vector<int> w(n, 1);
    for (int i = 0; i < n; ++i) {
        w[i] = -2;
        vertices.push_back(w);
        w[i] = 1;
    }

    for (const auto& v : vertices) {
        for (int x : v) {
            std::cout << x << " ";
        }
        std::cout << std::endl;
    }

    Graph G(vertices.size());
    int s = G.size();

    for (int i = 0; i < s; ++i) {
        for (int j = i + 1; j < s; ++j) {
            int s = 0;       
            for (int k = 0; k < n; ++k) {
                s += (vertices[i][k] - vertices[j][k]) * (vertices[i][k] - vertices[j][k]);
            }
            if (s == 36) {
                G.addEdge(i, j);
            }
        }
    }

    return G;
}

Graph IntersectionModified(int n, int k) {
    int s = binom(n, k);
    Graph G(s + n);
    std::vector<std::vector<int>> vertices;
    vertices.emplace_back(n);
    for (int i = 0; i < k; ++i) { // updating
        std::vector<std::vector<int>> new_verts;
        for (auto& v : vertices) {
            for (int j = 0; j < n; ++j) {
                if (v[j] == 1) {
                    break;
                }
                v[j] = 1;
                new_verts.push_back(v);
                v[j] = 0;
            }
        }
        std::swap(vertices, new_verts);
    }

   /* for (const auto& v : vertices) {
        for (int x : v) {
            std::cout << x << " ";
        }
        std::cout << std::endl;
    }*/

    for (int i = 0; i < s; ++i) {
        for (int j = i + 1; j < s; ++j) {
            bool good = true;
            for (int k = 0; k < n; ++k) {
                if (vertices[i][k] == 1 && vertices[j][k] == 1) {
                    good = false;
                    break;
                }
            }
            if (good) {
                G.addEdge(i, j);
            }
        }
    }

    for (int i = s; i < s + n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            G.addEdge(i, j);
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < s; ++j) {
            if (vertices[j][i] == 0) {
                G.addEdge(i + s, j);
            }
        }
    }

    return G;
}

Graph Ferenc5(int n) {
    std::vector<std::vector<int>> vertices;
    std::vector<int> w(n, -1);

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            for (int k = j + 1; k < n; ++k) {
                w[i] = n;
                w[j] = n;
                w[k] = -n - 2;
                vertices.push_back(w);
                w[i] = n;
                w[j] = -n - 2;
                w[k] = n;
                vertices.push_back(w);
                w[i] = -n - 2;
                w[j] = n;
                w[k] = n;
                vertices.push_back(w);
                w[i] = -1;
                w[j] = -1;
                w[k] = -1;
            }
        }
    }

    Graph G(vertices.size());
    int s = G.size();

    for (int i = 0; i < s; ++i) {
        for (int j = i + 1; j < s; ++j) {
            int s = 0;       
            for (int k = 0; k < n; ++k) {
                s += (vertices[i][k] - vertices[j][k]) * (vertices[i][k] - vertices[j][k]);
            }
            if (s == 10 * (n + 1) * (n + 1)) {
                G.addEdge(i, j);
            }
        }
    }

    return G;
}

int main(int argc, char** argv) {
    // select a small group

    int n = 6;
    Graph G = Ferenc5(n);

    std::cout << n << std::endl;
    std::cout << "Graph is embeddable into R^" << n-1 << std::endl;
    std::cout << "Number of vertices " << G.size() << std::endl;
    std::cout << "Number of edges " << G.edges() << std::endl;
    std::cout << "Chromatic number " << Chromatic(G) << std::endl;
    //std::cout << "Chromatic number at least 10 " << ChromaticAtLeast(G, 8) << std::endl; // we need to check on 10
    //std::cout << std::endl;

    return 0;
}
