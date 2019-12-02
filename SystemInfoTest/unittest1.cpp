#include "stdafx.h"
#include "CppUnitTest.h"

#include "../SystemInfo/network/rest/rest.h"
#include "../SystemInfo/network/rest/rest.cpp"
#include <winhttp.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SystemInfoTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			bool res = uploadImage(NULL, L"C:\\Users\\Theta3\\AppData\\Roaming\\SystemInfo\\_temp.png");
			Assert::IsTrue(res);
		}

	};
}