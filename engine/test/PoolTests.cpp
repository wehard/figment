#include <gtest/gtest.h>
#include "Pool.h"

using namespace Figment;

struct TestStruct
{
    int x;
    int y;
    uint32_t generation;
};

TEST(Pool, TestPoolInitializationWithDefaultCapacity)
{
    auto pool = Pool<TestStruct>(Pool<TestStruct>::DefaultCapacity);
    ASSERT_EQ(pool.Capacity(), Pool<TestStruct>::DefaultCapacity);
}

TEST(Pool, TestPoolInitializationWithSpecificCapacity)
{
    constexpr int capacity = 10;
    auto pool = Pool<TestStruct>(capacity);
    ASSERT_EQ(pool.Capacity(), capacity);
}

TEST(Pool, TestPoolCapacityExpansionAfterCreation)
{
    constexpr int capacity = 1;
    auto pool = Pool<TestStruct>(capacity);
    pool.Create();
    pool.Create();
    ASSERT_EQ(pool.Capacity(), capacity * 2);
}

TEST(Pool, TestPoolCapacityExpansionAfterMultipleCreations)
{
    constexpr int capacity = 5;
    auto pool = Pool<TestStruct>(capacity);

    for (int i = 0; i < capacity * 2; i++)
    {
        pool.Create();
    }
    ASSERT_EQ(pool.Capacity(), capacity * 2);
}

TEST(Pool, TestHandleCreationAndRetrieval)
{
    constexpr int capacity = 1;
    auto pool = Pool<TestStruct>(capacity);
    auto handle = pool.Create();

    auto data = pool.Get(handle);
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(handle.Index(), 0);
    ASSERT_EQ(handle.Generation(), 0);
}

TEST(Pool, TestMultipleHandleCreationAndRetrieval)
{
    constexpr int capacity = 10;
    auto pool = Pool<TestStruct>(capacity);

    for (int i = 0; i < capacity; i++)
    {
        auto handle = pool.Create();
        auto data = pool.Get(handle);
        ASSERT_NE(data, nullptr);
        ASSERT_EQ(handle.Index(), i);
        ASSERT_EQ(handle.Generation(), 0);
    }
}

TEST(Pool, TestHandleGenerationAfterDeletionAndCreation)
{
    auto pool = Pool<TestStruct>(1);
    auto h1 = pool.Create();
    pool.Delete(h1);
    auto h2 = pool.Create();

    ASSERT_NE(h1.Generation(), h2.Generation());
    ASSERT_EQ(h1.Index(), h2.Index());
    ASSERT_EQ(h1.Generation(), h2.Generation() - 1);
}

TEST(Pool, TestHandleGetDataStaleHandle)
{
    auto pool = Pool<TestStruct>(1);
    auto h1 = pool.Create();
    pool.Delete(h1);
    auto data = pool.Get(h1);

    ASSERT_EQ(data, nullptr);
}
