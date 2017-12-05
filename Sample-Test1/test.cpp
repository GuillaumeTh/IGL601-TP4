#include "pch.h"

#include "../utils.h"

using vecf = std::vector<float>;
using veci = std::vector<int>;

TEST(EQUAL, equal) {
	vecf v1(10, 0.f);
	vecf v2(10, 0.f);
	vecf v3(10, 1.f);
	vecf v4(5, 0.f);
	EXPECT_TRUE(equals(v1, v2));
	EXPECT_TRUE(equals(v2, v1));
	EXPECT_FALSE(equals(v1, v3));
	EXPECT_FALSE(equals(v1, v4));
}

TEST(ADD, add) {
	vecf v1(5, 1.f);
	vecf v2(5, 2.f);
	vecf expect(5, 3.f);
	vecf& res = add(v1, v2);
	EXPECT_TRUE(equals(res, expect));
	EXPECT_TRUE(equals(res, v1));
	EXPECT_FALSE(equals(res, v2));
	EXPECT_TRUE(&v1 == &res);
}

TEST(MULT_SCALAR, mult_scalar) {
	vecf v1(5, 2.f);
	float f = 3.f;
	vecf expect(5, 6.f);
	auto& res = mult(v1, f);
	EXPECT_TRUE(equals(res, expect));
	EXPECT_TRUE(equals(res, v1));
	EXPECT_TRUE(&v1 == &res);
}


		TEST(MULT_VEC, mult_vec)
		{
			vecf v1(5, 2.f);
			vecf v2(5, 3.f);
			vecf expect(5, 6.f);
			vecf res = mult(v1, v2);
			EXPECT_TRUE(equals(res, expect));
			EXPECT_FALSE(equals(res, v1));
			EXPECT_FALSE(equals(res, v2));
		}
		TEST(SUM, sum)
		{
			vecf v1(5, 2.f);
			float expect = 10.f;
			float res = sum(v1);
			EXPECT_TRUE(epsilon(res - expect));
		}
		TEST(LENGTH, length)
		{
			vecf v1{ 3,4 };
			vecf v2(16, 4.f);
			float expect1 = 5.f;
			float expect2 = 16.f;
			float res1 = length(v1);
			float res2 = length(v2);
			EXPECT_TRUE(epsilon(expect1 - res1));
			EXPECT_TRUE(epsilon(expect2 - res2));
		}
		TEST(AVERAGE, average)
		{
			veci indexes{ 0,2 };
			vector<vecf> data{ vecf(5, -1.f), vecf(5, -100.f), vecf(5, 1.f) };
			vecf expect(5, 0.f);
			vecf res = average(indexes, data);
			EXPECT_TRUE(equals(expect, res));
		}
		TEST(VARIANCE, variance)
		{
			veci indexes{ 0,2 };
			vector<vecf> data{ vecf(5, -1.f), vecf(5, -100.f), vecf(5, 1.f) };
			vecf avg = average(indexes, data);
			vecf res = variance(indexes, data, avg);
			vecf expect(5, 1.f);
			EXPECT_TRUE(equals(expect, res));
		}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
