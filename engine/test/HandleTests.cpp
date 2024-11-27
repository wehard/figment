#include <gtest/gtest.h>
#include "Handle.h"

using namespace figment;

struct TestStruct
{
    uint32_t x;
};

TEST(Handle, TestHandleCreation)
{
    auto handle = Handle<TestStruct>();

    ASSERT_EQ(handle.Index(), 0);
    ASSERT_EQ(handle.Generation(), 0);
}

TEST(Handle, TestHandleEquality)
{
    auto handle1 = Handle<TestStruct>();
    auto handle2 = Handle<TestStruct>();

    ASSERT_EQ(handle1.Index(), handle2.Index());
    ASSERT_EQ(handle1.Generation(), handle2.Generation());
}
