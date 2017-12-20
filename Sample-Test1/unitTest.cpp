#define CATCH_CONFIG_MAIN
#include "../Catch2/single_include/catch.hpp"

#include "../utils.h"

using vecf = std::vector<float>;
using veci = std::vector<int>;

TEST_CASE("MAX_CHAR")
{
	string s1 = "aaaaabbbbccd";
	string s2 = "abbcccbb";
	string s3 = "aabbbaaabcde";
	string s4 = "";

	int res1 = maxRepeatingChar(s1);
	int expect1 = 5;
	
	int res2 = maxRepeatingChar(s2);
	int expect2 = 3;

	int res3 = maxRepeatingChar(s3);
	int expect3 = 3;

	int res4 = maxRepeatingChar(s4);
	int expect4 = 0;

	REQUIRE(res1 == expect1);
	REQUIRE(res2 == expect2);
	REQUIRE(res3 == expect3);
	REQUIRE(res4 == expect4);
}

TEST_CASE("LEVENSHTEIN_DIST")
{
	string w1 = "hello";
	string w2 = "holle";
	string w3 = "";
	string w4 = "abcde";

	int res1 = levenshteinDist(w1, w2);
	int expect1 = 2;
	
	int res2 = levenshteinDist(w1, w3);
	int expect2 = 5;

	int res3 = levenshteinDist(w1, w4);
	int expect3 = 5;

	REQUIRE(res1 == expect1);
	REQUIRE(res2 == expect2);
	REQUIRE(res3 == expect3);
}

TEST_CASE("NORM_LEVENSHTEIN_DIST")
{
	string w1 = "hello";
	string w2 = "holle";
	string w3 = "";
	string w4 = "abcde";
	
	float res1 = normLevenshteinDist(w1, w2);
	float expect1 = 2.f / 5.f;

	float res2 = normLevenshteinDist(w1, w3);
	float expect2 = 1.f;

	float res3 = normLevenshteinDist(w1, w4);
	float expect3 = 1.f;

	REQUIRE(epsilon(res1 - expect1));
	REQUIRE(epsilon(res2 - expect2));
	REQUIRE(epsilon(res3 - expect3));
}

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

TEST_CASE("LOAD")
{
	veci data;
	load("testIds.txt", data);
	cout << "DATA : ";
	for (int i : data)
		cout << i << " ";
	cout << " END\n";
	veci expect{ 0,2,4 };
	REQUIRE(equals(expect, data));
}