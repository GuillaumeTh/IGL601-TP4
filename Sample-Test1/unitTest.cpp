#define CATCH_CONFIG_MAIN
#include "../Catch2/single_include/catch.hpp"

#include "../utils.h"

using vecf = std::vector<float>;
using veci = std::vector<int>;

TEST_CASE("EQUAL") {
	vecf v1(10, 0.f);
	vecf v2(10, 0.f);
	vecf v3(10, 1.f);
	vecf v4(5, 0.f);
	REQUIRE(equals(v1, v2));
	REQUIRE(equals(v2, v1));
	REQUIRE(!equals(v1, v3));
	REQUIRE(!equals(v1, v4));
}

TEST_CASE("ADD") {
	vecf v1(5, 1.f);
	vecf v2(5, 2.f);
	vecf expect(5, 3.f);
	vecf& res = add(v1, v2);
	REQUIRE(equals(res, expect));
	REQUIRE(equals(res, v1));
	REQUIRE(!equals(res, v2));
	REQUIRE(&v1 == &res);
}

TEST_CASE("MULT_SCALAR") {
	vecf v1(5, 2.f);
	float f = 3.f;
	vecf expect(5, 6.f);
	auto& res = mult(v1, f);
	REQUIRE(equals(res, expect));
	REQUIRE(equals(res, v1));
	REQUIRE(&v1 == &res);
}


TEST_CASE("MULT_VEC")
{
    vecf v1(5, 2.f);
    vecf v2(5, 3.f);
    vecf expect(5, 6.f);
    vecf res = mult(v1, v2);
    REQUIRE(equals(res, expect));
    REQUIRE(!equals(res, v1));
    REQUIRE(!equals(res, v2));
}
TEST_CASE("SUM")
{
    vecf v1(5, 2.f);
    float expect = 10.f;
    float res = sum(v1);
    REQUIRE(epsilon(res - expect));
}
TEST_CASE("LENGTH")
{
    vecf v1{ 3,4 };
    vecf v2(16, 4.f);
    float expect1 = 5.f;
    float expect2 = 16.f;
    float res1 = length(v1);
    float res2 = length(v2);
    REQUIRE(epsilon(expect1 - res1));
    REQUIRE(epsilon(expect2 - res2));
}
TEST_CASE("AVERAGE")
{
    veci indexes{ 0,2 };
    vector<vecf> data{ vecf(5, -1.f), vecf(5, -100.f), vecf(5, 1.f) };
    vecf expect(5, 0.f);
    vecf res = average(indexes, data);
    REQUIRE(equals(expect, res));
}
