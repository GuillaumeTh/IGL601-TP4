#define CATCH_CONFIG_MAIN
#include "../Catch2/single_include/catch.hpp"

#include "../utils.h"

using vecf = std::vector<float>;
using veci = std::vector<int>;

TEST_CASE("KMEAN")
{
	vector<vecf> freq(10, vecf(20, 5.f));
	veci docIds{ 0,2,4,6,8 };
	vector<veci> clusterToDoc;
	vector<vecf> weights;
	kMean(10, 1, freq, docIds, clusterToDoc, weights, false, false, 1);

	int expectSize = 10;
    REQUIRE(clusterToDoc.size() == expectSize);

	vector<vecf> expectWeights(10, vector<float>(5, 1.0f));
	for (int i = 0; i < expectWeights.size(); ++i)
	{
		REQUIRE(equals(expectWeights[i], weights[i]));
	}
}

TEST_CASE("RECURS_KMEAN")
{
	vector<vecf> freq(10, vecf(20, 5.f));
	veci docIds{ 0,2,4,6,8 };
	vector<veci> clusterToDoc;
	vector<vecf> weights;
	recursiveKMean(16, 1, 1, ".", freq, docIds, clusterToDoc, weights, false);

	int expectSize = 16;
	REQUIRE(clusterToDoc.size() == 16);

	vector<vecf> expectWeights(16, vector<float>(5, 1.0f));
	for (int i = 0; i < expectWeights.size(); ++i)
	{
		REQUIRE(equals(expectWeights[i], weights[i]));
	}
}
