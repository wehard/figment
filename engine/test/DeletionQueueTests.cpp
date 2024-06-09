#include <gtest/gtest.h>
#include "DeletionQueue.h"

TEST(DeletionQueue, Initialize)
{
    DeletionQueue deletionQueue;

    ASSERT_EQ(deletionQueue.deletors.size(), 0);
}

TEST(DeletionQueue, PushDeletor)
{
    DeletionQueue deletionQueue;
    deletionQueue.Push([]()
    { });

    ASSERT_EQ(deletionQueue.deletors.size(), 1);
}

TEST(DeletionQueue, FlushEmpty)
{
    DeletionQueue deletionQueue;
    deletionQueue.Flush();
    ASSERT_NO_THROW();
}

TEST(DeletionQueue, FlushNonEmpty)
{
    DeletionQueue deletionQueue;
    deletionQueue.Push([]()
    { });
    deletionQueue.Flush();
    ASSERT_EQ(deletionQueue.deletors.size(), 0);
}

TEST(DeletionQueue, FlushMultiple)
{
    DeletionQueue deletionQueue;
    deletionQueue.Push([]()
    { });
    deletionQueue.Flush();
    deletionQueue.Flush();
    ASSERT_EQ(deletionQueue.deletors.size(), 0);
}

TEST(DeletionQueue, FlushCallsDeletors)
{
    bool called = false;
    DeletionQueue deletionQueue;
    deletionQueue.Push([&called]()
    { called = true; });
    deletionQueue.Flush();
    ASSERT_EQ(called, true);
}
