#define CATCH_CONFIG_MAIN
#include "../Catch2/single_include/catch.hpp"

#include "../utils.h"

using vecf = std::vector<float>;
using veci = std::vector<int>;

TEST_CASE("AVERAGE")
{
	veci indexes{ 0,2 };
	vector<vecf> data{ vecf(5, -1.f), vecf(5, -100.f), vecf(5, 1.f) };
	vecf expect(5, 0.f);
	vecf res = average(indexes, data);
	REQUIRE(equals(expect, res));
}

TEST_CASE("VARIANCE")
{
    veci indexes{ 0,2 };
    vector<vecf> data{ vecf(5, -1.f), vecf(5, -100.f), vecf(5, 1.f) };
    vecf avg = average(indexes, data);
    vecf res = variance(indexes, data, avg);
    vecf expect(5, 1.f);
    REQUIRE(equals(expect, res));
}

//TEST_CASE("LOADING")
//{
//	veci indexes;
//	load("./testIds.txt", indexes);
//
//	vector<vecf> data;
//	loadClusterData("./testData.txt", data);
//
//	vecf avg = average(indexes, data);
//	vecf expect(4, 3.f);
//
//	REQUIRE(equals(avg, expect));
//}
