#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using vecf = std::vector<float>;
using veci = std::vector<int>;

namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(EQUALS)
		{
			vecf v1(10, 0.f);
			vecf v2(10, 0.f);
			vecf v3(10, 1.f);
			vecf v4(5, 0.f);
			Assert::IsTrue(equals(v1, v2));
			Assert::IsTrue(equals(v2, v1));
			Assert::IsFalse(equals(v1, v3));
			Assert::IsFalse(equals(v1, v4));
		}

		TEST_METHOD(ADD)
		{
			vecf v1(5, 1.f);
			vecf v2(5, 2.f);
			vecf expect(5, 3.f);
			vecf& res = add(v1, v2);
			Assert::IsTrue(equals(res, expect));
			Assert::IsTrue(equals(res, v1));
			Assert::IsFalse(equals(res, v2));
			Assert::IsTrue(&v1 == &res);
		}

		TEST_METHOD(MULT_SCALAR)
		{
			vecf v1(5, 2.f);
			float f = 3.f;
			vecf expect(5, 6.f);
			auto& res = mult(v1, f);
			Assert::IsTrue(equals(res, expect));
			Assert::IsTrue(equals(res, v1));
			Assert::IsTrue(&v1 == &res);
		}
		TEST_METHOD(MULT_VEC)
		{
			vecf v1(5, 2.f);
			vecf v2(5, 3.f);
			vecf expect(5, 6.f);
			vecf res = mult(v1, v2);
			Assert::IsTrue(equals(res, expect));
			Assert::IsFalse(equals(res, v1));
			Assert::IsFalse(equals(res, v2));
		}
		TEST_METHOD(SUM)
		{
			vecf v1(5, 2.f);
			float expect = 10.f;
			float res = sum(v1);
			Assert::IsTrue(epsilon(res - expect));
		}
		TEST_METHOD(LENGTH)
		{
			vecf v1{ 3,4 };
			vecf v2(16, 4.f);
			float expect1 = 5.f;
			float expect2 = 16.f;
			float res1 = length(v1);
			float res2 = length(v2);
			Assert::IsTrue(epsilon(expect1 - res1));
			Assert::IsTrue(epsilon(expect2 - res2));
		}
		TEST_METHOD(AVERAGE)
		{
			veci indexes{ 0,2 };
			vector<vecf> data{ vecf(5, -1.f), vecf(5, -100.f), vecf(5, 1.f) };
			vecf expect(5, 0.f);
			vecf res = average(indexes, data);
			Assert::IsTrue(equals(expect, res));
		}
		TEST_METHOD(VARIANCE)
		{
			veci indexes{ 0,2 };
			vector<vecf> data{ vecf(5, -1.f), vecf(5, -100.f), vecf(5, 1.f) };
			vecf avg = average(indexes, data);
			vecf res = variance(indexes, data, avg);
			vecf expect(5, 1.f);
			Assert::IsTrue(equals(expect, res));
		}
	};
}