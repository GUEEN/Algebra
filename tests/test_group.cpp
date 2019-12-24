#include "Group.h"

#include "catch.hpp"

#define CATCH_CONFIG_MAIN

TEST_CASE("Cyclic groups") {
    Group G;
    for (int n = 1; n <= 100; ++n) {
        G = Z(n);
        REQUIRE(G.order() == n);
        REQUIRE(G.isAbelian());
    }
}

TEST_CASE("Dihedral groups") {
    Group G;
    for (int n = 3; n <= 100; ++n) {
        G = D(n);
        REQUIRE(G.order() == 2 * n);
        REQUIRE(G.isAbelian() == false);
    }
}

TEST_CASE("Symmetric groups") {
    uint64_t order = 1;
    Group G;
    for (int n = 1; n <= 20; ++n) {
        G = S(n);
        order *= n;
        REQUIRE(G.order() == order);
    }
}

TEST_CASE("Alternating groups") {
    uint64_t order = 1;
    Group G;
    for (int n = 3; n <= 20; ++n) {
        G = A(n);
        order *= n;
        REQUIRE(G.order() == order);
        REQUIRE(G.isEven());
    }
}

TEST_CASE("Mathieu groups") {
    REQUIRE(M11().order() == 7920);
    REQUIRE(M12().order() == 95040);
    REQUIRE(M22().order() == 443520);
    REQUIRE(M23().order() == 10200960);
    REQUIRE(M24().order() == 244823040);
}

TEST_CASE("Normal Subgroups") {
    for (int n = 3; n <= 100; ++n) {
        REQUIRE((Z(n) << D(n)));
    }
    for (int n = 1; n <= 20; ++n) {
        REQUIRE((A(n) << S(n)));
    }
}
