#include <gtest/gtest.h>
#include "Pool.h"

using namespace Figment;

struct TestStruct
{
    int x;
    int y;
};

TEST(Pool, TestPoolInitializationWithDefaultCapacity)
{
    auto pool = Pool<TestStruct>();
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
    pool.Create({});
    pool.Create({});
    ASSERT_EQ(pool.Capacity(), capacity * 2);
}

TEST(Pool, TestPoolCapacityExpansionAfterMultipleCreations)
{
    constexpr int capacity = 5;
    auto pool = Pool<TestStruct>(capacity);

    for (int i = 0; i < capacity * 2; i++)
    {
        pool.Create({});
    }
    ASSERT_EQ(pool.Capacity(), capacity * 2);
}

TEST(Pool, TestHandleCreationAndRetrieval)
{
    constexpr int capacity = 1;
    auto pool = Pool<TestStruct>(capacity);
    auto handle = pool.Create({});

    auto data = pool.Get(handle);
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(handle.index, 0);
    ASSERT_EQ(handle.generation, 0);
}

TEST(Pool, TestMultipleHandleCreationAndRetrieval)
{
    constexpr int capacity = 10;
    auto pool = Pool<TestStruct>(capacity);

    for (int i = 0; i < capacity; i++)
    {
        auto handle = pool.Create({});
        auto data = pool.Get(handle);
        ASSERT_NE(data, nullptr);
        ASSERT_EQ(handle.index, i);
        ASSERT_EQ(handle.generation, 0);
    }
}

TEST(Pool, TestHandleGenerationAfterDeletionAndCreation)
{
    auto pool = Pool<TestStruct>(1);
    auto h1 = pool.Create({});
    pool.Delete(h1);
    auto h2 = pool.Create({});

    ASSERT_NE(h1.generation, h2.generation);
    ASSERT_EQ(h1.index, h2.index);
    ASSERT_EQ(h1.generation, h2.generation - 1);
}

TEST(Pool, TestHandleGetDataStaleHandle)
{
    auto pool = Pool<TestStruct>(1);
    auto h1 = pool.Create({});
    pool.Delete(h1);
    auto data = pool.Get(h1);

    ASSERT_EQ(data, nullptr);
}
