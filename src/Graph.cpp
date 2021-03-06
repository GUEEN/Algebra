#include "Graph.h"

Graph::Graph() : Structure(0), e(0) {
}

Graph::Graph(size_t n) : Structure(n), A(n * n, 0), e(0) {
}

Graph::Graph(size_t n, const Certificate& cert) : Structure(n, cert), A(n * n, 0), e(0) {
    size_t l = n * (n - 1) / 2;
    if (l % 8 == 0) {
        l >>= 3;
    } else {
        l >>= 3;
        l++;
    }
    // cert must be of length l
    size_t ii = 0;
    size_t jj = 1;
	
    for (size_t i = 0; i < l; i++) {
        byte b = static_cast<byte>(cert[i]);
        for (int j = 7; j >= 0; j--) {
            byte c = (b >> j) & 1;

            if (c) {                
                addEdge(ii, jj);
            }

            jj++;

            if (jj >= n) {
                ii++;
                jj = ii + 1;
            }
	    if (ii >= n - 1) {
		return;
            }
        }
    }
}

bool Graph::edge(size_t i, size_t j) const {
    return A[n * i + j] != 0;
}

void Graph::resize(size_t m) {
    n = m;
    A.assign(m * m, 0);
    e = 0;
}

std::vector<size_t> Graph::getDegrees() const {
    std::vector<size_t> degrees(n);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (A[i * n + j]) {
                degrees[i]++;
            }
        }
    }
    return degrees;
}

void Graph::addEdge(size_t i, size_t j) {
    byte c = A[n * i + j];
    if (c == 0) {
        A[n * i + j] = 1;
        A[n * j + i] = 1;
        e++;
    }
}

void Graph::killEdge(size_t i, size_t j) {
    byte c = A[n * i + j];
    if (c == 1) {
        A[n * i + j] = 0;
        A[n * j + i] = 0;
        e--;
    }
}

size_t Graph::degsize() const {
    return 1;
}

int Graph::color(size_t i, size_t j) const {
    return static_cast<int>(A[n * i + j]);
}

size_t Graph::edges() const {
    return e;
}

int Graph::compareOrders(const Perm& F, const Perm& B, size_t p, size_t q) const {
    for (size_t i = p; i < q; i++) {
        for (size_t j = 0; j < i; j++) {
             if (A[n * F[i] + F[j]] > A[n * B[i] + B[j]]) {
                 return 1;
             } else {
                 if (A[n * F[i] + F[j]] < A[n * B[i] + B[j]]) {
                     return -1;
                 }
             }
        }
    }
    return 0;
}

Certificate Graph::getCertificate(const Perm& P) const {
    size_t l = certSize(n);
    Certificate C(l);

    size_t q = 0;
    byte b = 0;

    for (size_t i = 0; i + 1 < n; i++) {
        for (size_t j = i + 1; j < n; j++) {			
            b = b * 2 + A[n * P[i] + P[j]];
            q++;
            if (q % 8 == 0) {
                C[q / 8 - 1] = b;
                b = 0;
            }
        }
    }
    if (q < 8 * l) {
        while (q < 8 * l) {
            q++;
            b = b * 2;		
        }
        C[q / 8 - 1] = b;
    }
    return C;
}

size_t Graph::deg() const {
    if (n == 0) {
        return 0;
    }

    size_t d = n - 1;
    for (size_t i = 0; i < n; i++) {
        size_t dd = 0;
        for (size_t j = 0; j < n; j++) {
            if (edge(i, j)) {
                dd++;
                if (dd >= d) {
                    break;
                }
            }
        }
        if (dd < d) {
            d = dd;
        }
        if (dd == 0) {
           return 0;
        }
    }
    return d;
}

void Graph::clear() {
    e = 0;
    A.assign(n * n , 0);
}

bool Graph::subClique(size_t k) const {
    Perm Q(k);
    for (size_t i = 0; i + k <= n; i++) {
        Q[0] = i;
        if (nextS(1, Q)) {
            return true;
        }
    }
    return false;
}

bool Graph::nextS(int level, Perm& Q) const {
    if (level >= Q.size()) {
        return true;
    }

    if (level < Q.size()) {
        for (size_t i = Q[level - 1] + 1; i < n; i++) {
            bool B = true;
            for (size_t j = 0; j < level; j++) {
                if (edge(Q[j], i)) {
                    B = false;
                }
            }
            if (B) {
                Q[level] = i;
                if (nextS(level + 1, Q)) {
                    return true;
                }
            }           
        }
    } 
    return false;
}

bool readGraph(std::fstream& stream, Graph& G) {
    size_t n = G.size();
    size_t l = Graph::certSize(n);
    Certificate cert(l);
    Structure::readStruct(stream, cert);

    if (stream.eof()) {
        return false;
    }

    G = Graph(n, cert);
    return true;
}

size_t Graph::certSize(size_t n) {
    size_t l = n * (n - 1) / 2;
    if (l % 8 == 0) {
        l >>= 3;
    } else {
        l >>= 3;
        l++;
    }
    if (l == 0) {
        l = 1;
    }
    return l;
}

Graph operator+(const Graph& G, size_t m) {
    size_t n = G.n;
    Graph H(m + n);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            if (G.edge(i, j)) {
                H.addEdge(i, j);
            }
        }
    }
    return H;
}

Graph operator+(size_t m, const Graph& G) {
    size_t n = G.n;
    Graph H(m + n);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            if (G.edge(i, j)) {
                H.addEdge(i + m, j + m);
            }
        }
    }
    return H;
}

Graph operator+(const Graph& G, const Graph& H) {
    size_t n = G.n;
    size_t m = H.n;

    Graph F(n + m);

    for (size_t i = 0; i + 1 < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            if (G.edge(i, j)) {
                F.addEdge(i, j);
            }
        }
    }

    for (size_t i = 0; i + 1 < m; i++) {
        for (size_t j = i + 1; j < m; j++) {
            if (H.edge(i, j)) {
                F.addEdge(n + i, n + j);
            }
        }
    }
    return F;
}

Graph K(size_t n) {
    if (n <= 1) {
        return Graph(1);
    }
    Graph G(n);
    for (size_t i = 0; i + 1 < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            G.addEdge(i, j);
        }
    }
    return G;
}

Graph K(size_t n, size_t m) {
    Graph G(n + m);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = n; j < n + m; j++) {
            G.addEdge(i, j);
        }
    }
    return G;
}

Graph C(size_t n) {
    Graph G(n);
    for (size_t i = 0; i + 1 < n; i++) {
        G.addEdge(i, i + 1);
    }
    G.addEdge(0, n - 1);
    return G;
}

Graph P(size_t n) {
    Graph G(n);
    for (size_t i = 0; i + 1 < n; i++) {
        G.addEdge(i, i + 1);
    }
    return G;
}

Graph Q(size_t n) {
    if (n == 0) {
        return Graph(1);
    }
    Graph G = Q(n - 1) + Q(n - 1);
    size_t m = G.size() >> 1;
    for (size_t i = 0; i < m; i++) {
        G.addEdge(i, i + m);
    }
    return G;
}
