#include "gtest/gtest.h"

TEST(HelloTest, BasicAssertions)
{
	EXPECT_STREQ("hello", "world") << "Strings are not equal";
	EXPECT_EQ(7*6,42);
}