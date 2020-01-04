#pragma once

#include "Structure.h"

// a class representing simple graph
class Graph : public Structure {
public:
    Graph();
    explicit Graph(size_t n);
    Graph(size_t n, const Certificate& cert);

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

bool readGraph(std::fstream&, Graph& G);

Graph operator+(const Graph& G, size_t m);
Graph operator+(size_t m, const Graph& G);
Graph operator+(const Graph& G, const Graph& H);

// some special types of graphs
Graph K(size_t n);
Graph K(size_t n, size_t m);
Graph C(size_t n);
Graph P(size_t n);
Graph Q(size_t n);

// a class for a collection of pairwise non-isomorphic
// simple graphs of same size
class GraphSet : public StructSet {
public:
    GraphSet(size_t n) : n(n) {
    }

    class iterator {
    public:
        iterator(const GraphSet* gset, const std::unordered_set<std::string>::const_iterator& it) : gset_(gset), it_(it) {
        }

        Graph operator*() const {
            return Graph(gset_->n, Cert(*it_));
        }

        bool operator!=(const iterator& it) {
            return gset_ != it.gset_ || it_ != it.it_;
        }

        bool operator==(const iterator& it) {
            return gset_ == it.gset_ && it_ == it.it_;
        }

        iterator& operator++() {
            ++it_;
            return *this;
        }

        iterator operator++(int) {
            iterator it = *this;
            ++it_;
            return it;
        }

    private:
        const GraphSet* gset_;
        std::unordered_set<std::string>::const_iterator it_;
    };

    iterator begin() const {
        return iterator(this, data_.begin());
    }

    iterator end() const {
        return iterator(this, data_.end());
    }

private:
    size_t n;
};
