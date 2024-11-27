#include <gtest/gtest.h>

#include "ResourceManager.h"
#include "VulkanContext.h"

using namespace Figment;

struct MockRenderContext : public VulkanContext
{
    MockRenderContext() : VulkanContext(nullptr) { }
    void Init(uint32_t width, uint32_t height) override { }
    void onResize(uint32_t width, uint32_t height) override { }
};

TEST(ResourceManager, TestResourceManagerInitialization)
{
    auto renderContext = MockRenderContext();
    auto resourceManager = ResourceManager(renderContext, {});

    // ASSERT_EQ(resourceManager.ResourceCount(), 0);
}

TEST(ResourceManager_BufferHandle, TestResourceManagerCreateBufferResourceHandle)
{
    auto renderContext = MockRenderContext();
    auto resourceManager = ResourceManager(renderContext, {});

    auto bufferHandle = resourceManager.CreateBuffer({});

    EXPECT_EQ(bufferHandle.Index(), 0);
}

TEST(ResourceManager_BufferHandle, TestResourceManagerRetrieveBufferResource)
{
    static constexpr uint32_t byteSize = 42;
    auto renderContext = MockRenderContext();
    auto resourceManager = ResourceManager(renderContext, {});

    auto bufferHandle = resourceManager.CreateBuffer({ .ByteSize = byteSize, .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT });
    auto buffer = resourceManager.GetBuffer(bufferHandle);

    EXPECT_EQ(buffer->ByteSize(), byteSize);
    EXPECT_NE(buffer, nullptr);
}
