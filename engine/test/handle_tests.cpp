#include <gtest/gtest.h>
#include "Handle.h"

using namespace Figment;

struct TestStruct
{
    uint32_t x;
};

TEST(Handle, TestHandleCreation)
{
    auto handle = Handle<TestStruct>();

    ASSERT_EQ(handle.index, 0);
    ASSERT_EQ(handle.generation, 0);
}

TEST(Handle, TestHandleEquality)
{
    auto handle1 = Handle<TestStruct>();
    auto handle2 = Handle<TestStruct>();
    auto handle3 = Handle<TestStruct>();
    handle3.index = 1;

    ASSERT_EQ(handle1.index, handle2.index);
    ASSERT_EQ(handle1.generation, handle2.generation);
    ASSERT_NE(handle1.index, handle3.index);
}
