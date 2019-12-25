#include "Permutation.h"

#include "catch.hpp"

#define CATCH_CONFIG_MAIN

TEST_CASE("simple") {
    Perm P(100);
    P = 33;
    REQUIRE(P.isBij() == false);
    REQUIRE(P.isConst() == true);
    REQUIRE(P.isConst(3) == false);
    REQUIRE(P.isConst(33) == true);

    P = Perm(100);
    REQUIRE(P.size() == 100);

    P = Cycle(100);
    REQUIRE(P.isBij());
    REQUIRE((P || P));
}

TEST_CASE("commutators") {
    Perm P = Cycle(100);
    REQUIRE(P[P].isId());
}

TEST_CASE("power") {
    Perm C = Cycle(10000);
    REQUIRE((C ^ 10000).isId());
    REQUIRE((C ^ 9999).isId() == false);
}
