#include <gtest/gtest.h>

namespace laplace
{
namespace test
{

TEST(ExampleTest,TrivialTest)
{
    bool testBoolean = true;
    
    EXPECT_EQ(testBoolean,true);
}

} // namespace test
} // namespace laplace
