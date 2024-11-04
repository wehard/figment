#include <gtest/gtest.h>
#include "Arena.h"

TEST(Arena, ShouldAllocateMemoryWhenSizeIsWithinLimit)
{
    Arena arena(4);
    auto data = arena.Allocate<uint32_t>(1);
    ASSERT_NE(data, nullptr);
}

TEST(Arena, ShouldNotAllocateMemoryWhenSizeExceedsLimit)
{
    Arena arena(1);
    auto data = arena.Allocate(2);
    ASSERT_EQ(data, nullptr);
}

TEST(Arena, ShouldInitializeAllocatedMemoryBlockToZero)
{
    Arena arena(16);
    auto data = arena.Allocate<char>(16);

    ASSERT_EQ(*data, 0);
    ASSERT_EQ(data[15], 0);
}

TEST(Arena, ShouldNotAllocateMemoryWhenSizeIsZero)
{
    Arena arena(1);
    auto data = arena.Allocate(0);
    ASSERT_EQ(data, nullptr);
}

TEST(Arena, ShouldAllocateDifferentTypes)
{
    Arena arena(16);
    auto data1 = arena.Allocate<uint32_t>(1);
    auto data2 = arena.Allocate<char>(1);

    ASSERT_NE(data1, nullptr);
    ASSERT_NE(data2, nullptr);
}

TEST(Arena, ShouldNotAllocateMemoryWhenArenaIsFull)
{
    Arena arena(4);
    auto data1 = arena.Allocate<uint32_t>(1);
    auto data2 = arena.Allocate<uint32_t>(1);

    ASSERT_NE(data1, nullptr);
    ASSERT_EQ(data2, nullptr);
}
