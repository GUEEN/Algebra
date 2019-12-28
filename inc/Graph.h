#pragma once

#include "Structure.h"

// a class representing a graph
class Graph : public Structure {
public:

    Graph() : Structure(0), e(0) {}

    explicit Graph(size_t n) : Structure(n), A(n * n, 0), e(0) {
    }

    size_t edges() const;

    bool edge(size_t ii, size_t jj) const {
        if (A[n * ii + jj] == 0) {
            return false;
        } else {
            return true;
        }
    }

    size_t deg();
    bool subClique(size_t k) const;

    void resize(size_t m) {
        n = m;
        A.assign(m * m, 0);
        e = 0;
    }

    void addEdge(size_t ii, size_t jj) {
        unsigned char c = A[n * ii + jj];
        if (c == 0) {
            A[n * ii + jj] = 1;
            A[n * jj + ii] = 1;
            e++;
        }
    }

    inline void killEdge(size_t ii, size_t jj) {
        unsigned char c = A[n * ii + jj];
	if (c == 1) {
            A[n * ii + jj] = 0;
            A[n * jj + ii] = 0;
            e--;
        }
    }

    friend Graph operator+(const Graph& G, const Graph& H);
    friend Graph operator+(const Graph& G, size_t m);
    friend Graph operator+(size_t m, const Graph& G);

protected:
    size_t e;
    std::vector<byte> A;

    bool nextS(int level, Perm& Q) const;

    virtual size_t degsize() const override {
        return 1;
    }

    virtual int color(size_t ii, size_t jj) const override {
        return static_cast<int>(A[n * ii + jj]);
    }

    virtual int compareOrders(const Perm& F, const Perm& B, int p, int q) const override;
    virtual Certificate getCertificate(const Perm& P) const override;
};

bool readGraph(Graph& G);

Graph operator+(const Graph& G, size_t m);
Graph operator+(size_t m, const Graph& G);
Graph operator+(const Graph& G, const Graph& H);

// some types of graphs
Graph K(size_t n);
Graph K(size_t n, size_t m);
Graph C(size_t n);
Graph Q(size_t n);
