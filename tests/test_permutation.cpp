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
    REQUIRE(P.length() == 100);

    P = Cycle(100);
    REQUIRE((P || P));
}
