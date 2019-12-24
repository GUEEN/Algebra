#include "Permutation.h"

#include "catch.hpp"

#define CATCH_CONFIG_MAIN

TEST_CASE("simple") {
    Perm P(100);
    P = 33;
    REQUIRE(P.isBij());
    REQUIRE(P.isConst());
}
