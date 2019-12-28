#include "Graph.h"

#include "catch.hpp"

#define CATCH_CONFIG_MAIN

TEST_CASE("simple") {
    Graph G(10);
    G.addEdge(1, 2);
    G.addEdge(2, 3);
    G.addEdge(6, 7);

    REQUIRE(G.size() == 10);
    REQUIRE(G.edges() == 3);
}

TEST_CASE("lower degree") {
    Graph G(10);
    for (int i = 1; i < 10; ++i) {
        G.addEdge(0, i);
    }
    REQUIRE(G.deg() == 1);

    G = K(123);
    REQUIRE(G.deg() == 122);

    G = K(20, 30);
    REQUIRE(G.deg() == 20);
}

TEST_CASE("special graphs") {
    Graph G;
    G = K(100);
    REQUIRE(G.size() == 100);
    REQUIRE(G.edges() == 50 * 99);

    G = C(100);
    REQUIRE(G.size() == 100);
    REQUIRE(G.edges() == 100);

    G = Q(10);
    REQUIRE(G.size() == (1 << 10));

    G = K(50, 50);
    REQUIRE(G.size() == 100);
    REQUIRE(G.edges() == 2500);
}

TEST_CASE("simple isomorphism") {
    Graph G(3);
    G.addEdge(0, 1);
    G.addEdge(1, 2);
    G.certify();

    Graph H(3);
    H.addEdge(2, 1);
    H.addEdge(0, 2);
    H.certify();

    REQUIRE(isomorphic(G, H));

    G = C(4);
    H = K(2, 2);
    G.certify();
    H.certify();

    REQUIRE(isomorphic(G, H));

    G = C(6);
    H = K(2, 3);
    G.certify();
    H.certify();

    REQUIRE(isomorphic(G, H) == false);
}

TEST_CASE("automorphism groups") {
    Graph G;
    uint64_t r;
    r = 1;
    for (size_t i = 1; i < 10; ++i, r *= i) {
        Graph G = K(i);
        Group A = G.aut();
        REQUIRE(A.order() == r);
    }
    for (size_t i = 3; i < 50; ++i) {
        Graph G = C(i);
        Group A = G.aut();
        REQUIRE(A.order() == 2 * i);
    }
    r = 2;
    for (size_t i = 1; i < 12; ++i, r *= i * i) {
        Graph G = K(i, i);
        Group A = G.aut();
        REQUIRE(A.order() == r);
    }
    r = 2;
    for (size_t i = 1; i < 8; ++i, r *= 2 * i) {
        Graph G = Q(i);
        Group A = G.aut();
        REQUIRE(A.order() == r);
    }

}
