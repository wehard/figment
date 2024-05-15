#include <gtest/gtest.h>

#include "ResourceManager.h"

using namespace Figment;

struct MockRenderContext : public RenderContext
{
    MockRenderContext() : RenderContext() { }
    void Init() override { };
};

TEST(ResourceManager, TestResourceManagerInitialization)
{
    auto renderContext = MockRenderContext();
    auto resourceManager = ResourceManager(renderContext);

    ASSERT_EQ(resourceManager.ResourceCount(), 0);
}

TEST(ResourceManager, TestResourceManagerResourceCreation)
{
    auto renderContext = MockRenderContext();
    auto resourceManager = ResourceManager(renderContext);

    auto textureHandle = resourceManager.CreateTexture({ .width = 1024, .height = 1024 });

    ASSERT_EQ(textureHandle.index, 0);
    ASSERT_EQ(resourceManager.ResourceCount(), 1);
}

TEST(ResourceManager, TestResourceManagerResourceRetrieval)
{
    static constexpr uint32_t size = 1024;
    auto renderContext = MockRenderContext();
    auto resourceManager = ResourceManager(renderContext);

    auto textureHandle = resourceManager.CreateTexture({ .width = size, .height = size });
    auto texture = resourceManager.GetTexture(textureHandle);

    ASSERT_NE(texture, nullptr);
    ASSERT_EQ(texture->width, size);
    ASSERT_EQ(texture->height, size);
}

TEST(ResourceManager_TextureHandle, TestResourceManagerCreateTextureResourceHandle)
{
    static constexpr uint32_t size = 1024;
    auto renderContext = MockRenderContext();
    auto resourceManager = ResourceManager(renderContext);

    auto textureHandle = resourceManager.CreateTexture({ .width = size, .height = size });

    EXPECT_EQ(textureHandle.index, 0);
}

TEST(ResourceManager_TextureHandle, TestResourceManagerRetrieveTextureResource)
{
    static constexpr uint32_t size = 1024;
    auto renderContext = MockRenderContext();
    auto resourceManager = ResourceManager(renderContext);

    auto textureHandle = resourceManager.CreateTexture({ .width = size, .height = size });
    auto texture = resourceManager.GetTexture(textureHandle);

    EXPECT_NE(texture, nullptr);
}

TEST(ResourceManager_BindGroupHandle, TestResourceManagerCreateBindGroupResourceHandle)
{
    static constexpr uint32_t size = 1024;
    auto renderContext = MockRenderContext();
    auto resourceManager = ResourceManager(renderContext);

    auto bindGroupHandle = resourceManager.CreateBindGroup({});

    EXPECT_EQ(bindGroupHandle.index, 0);
}

TEST(ResourceManager_BindGroupHandle, TestResourceManagerRetrieveBindGroupResource)
{
    static constexpr uint32_t size = 1024;
    auto renderContext = MockRenderContext();
    auto resourceManager = ResourceManager(renderContext);

    auto bindGroupHandle = resourceManager.CreateBindGroup({});
    auto bindGroup = resourceManager.GetBindGroup(bindGroupHandle);

    EXPECT_NE(bindGroup, nullptr);
}
