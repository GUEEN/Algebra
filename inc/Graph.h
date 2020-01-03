#pragma once

#include "Structure.h"

// a class representing simple graph
class Graph : public Structure {
public:
    Graph();
    explicit Graph(size_t n);
    size_t edges() const;
    bool edge(size_t i, size_t j) const;
    size_t deg();
    bool subClique(size_t k) const;
    void resize(size_t m);
    std::vector<size_t> getDegrees() const;
    void addEdge(size_t i, size_t j);
    void killEdge(size_t i, size_t j);

    friend Graph operator+(const Graph& G, const Graph& H);
    friend Graph operator+(const Graph& G, size_t m);
    friend Graph operator+(size_t m, const Graph& G);
    friend bool readGraph(Graph& G);

protected:
    size_t e;
    std::vector<byte> A;
    bool nextS(int level, Perm& Q) const;

    virtual size_t degsize() const override;
    virtual int color(size_t i, size_t j) const override;
    virtual int compareOrders(const Perm& F, const Perm& B, size_t p, size_t q) const override;
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
Graph P(size_t n);
Graph Q(size_t n);
