#include "Graph.h"

/*
Degrees Graph::getDegrees()
{
	Degrees D(n);

	for (int i = 0; i < n; i++)
	{
		Deg d(1);

		for (int j = 0; j < n; j++)
		if (A[i,j] == 1)
			d[0]++;

		D[i] = d;
	}

	return D;
}*/

size_t Graph::edges() const {
    return e;
}

int Graph::compareOrders(const Perm& F, const Perm& B, int p, int q) const {
    for (int i = p; i < q; i++) {
        for (int j = 0; j < i; j++) {
	//for (int j = p; j < q; j++)
	//	for (int i = 0; i < j; i++)
             if (A[n * F[i] + F[j]] > A[n * B[i] + B[j]]) {
                 return 1;
             } else {
                 if (A[n * F[i] + F[j]] < A[n * B[i] + B[j]]) {
                     return  -1;
                 }
             }
        }
    }
    return 0;
}

Certificate Graph::getCertificate(const Perm& P) const {
    int l = n * (n - 1) / 2;
    if (l % 8 == 0) {
        l /= 8;
    } else {
        l = l / 8  + 1;
    }

    //Certificate C(n * (n - 1) / 2);
    Certificate C(l);

    int q = 0;
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
	if (q < 8 * l) {
            while (q < 8 * l) {
                q++;
                b = b * 2;		
            }
            C[q / 8 - 1] = b;
        }
    }
    return C;
}

size_t Graph::deg() {
    if (n == 0) {
        return 0;
    }

    size_t d = n - 1;
    for (size_t i = 0; i < n; i++) {
        size_t dd = 0;
        for (size_t j = 0; j < n; j++) {
            if (edge(i, j)) {
                dd++;
            }
        }
        if (dd < d) {
            d = dd;
        }
    }
    return d;
}

bool Graph::subClique(size_t k) const {
    Perm Q(k);
    for (size_t ii = 0; ii + k <= n; ii++) {
        Q[0] = ii;
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
        for (size_t ii = Q[level - 1] + 1; ii < n; ii++) {
            bool B = true;
            for (size_t jj = 0; jj < level; jj++) {
                if (edge(Q[jj], ii)) {
                    B = false;
                }
            }
            if (B) {
                Q[level] = ii;
                if (nextS(level + 1, Q)) {
                    return true;
                }
            }           
        }
    } 
    return false;
}

bool readGraph(Graph& G) {
    size_t n = G.size();
    size_t l = (n * (n - 1)) / 2;
    if (l % 8 == 0) {
        l /= 8;
    } else {
        l = l / 8 + 1;
    }

    G.resize(n);
    Certificate cert(l);

    Structure::readStruct(cert);

    if (G.stream.eof()) {
        return false;
    }

    G.cert = cert;

    size_t ii = 0;
    size_t jj = 1;
	
    for (size_t i = 0; i < l; i++) {
        unsigned char b = static_cast<unsigned char>(cert[i]);
        for (int j = 7; j >= 0; j--) {
            byte c = (b >> j) % 2;
            //byte c = (cert[i] >> j) % 2;
            //std::cout << c << std::endl;

            if (c != 0) {
                G.addEdge(ii, jj);
            }

            jj++;

            if (jj >= n) {
                ii++;
                jj = ii + 1;
            }
            if (ii + 1 >= n) {
                return true;
            }
        }
    }
    return true;
}

Graph operator+(const Graph& G, size_t m) {
    size_t n = G.n;
    Graph H(m + n);
    for (size_t ii = 0; ii < n; ii++) {
        for (size_t jj = ii + 1; jj < n; jj++) {
            if (G.edge(ii, jj)) {
                H.addEdge(ii, jj);
            }
        }
    }
    return H;
}

Graph operator+(size_t m, const Graph& G) {
    size_t n = G.n;
    Graph H(m + n);
    for (size_t ii = 0; ii < n; ii++) {
        for (size_t jj = ii + 1; jj < n; jj++) {
            if (G.edge(ii, jj)) {
                H.addEdge(ii + m, jj + m);
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
