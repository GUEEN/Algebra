#include "Graph.h"

#include "catch.hpp"

#define CATCH_CONFIG_MAIN

TEST_CASE("simple") {
    Graph G(10, 20);
    REQUIRE(G.size() == 10);
    REQUIRE(G.edges() == 20);
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
