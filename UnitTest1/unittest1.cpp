#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using vecf = std::vector<float>;

namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(DUMMY)
		{
			Assert::AreEqual(1, 1);
		}

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
			vecf v2(5, 0.f);

			vecf res = add(v1, v2);
			Assert::IsTrue(equals(res, v1));
		}
	};
}