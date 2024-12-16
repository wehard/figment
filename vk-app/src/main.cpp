#include "context.h"
#include "vk_debug_tools.h"

using namespace figment;

int main()
{
    figment::vulkan::Context context;
    vkdt::Presenter presenter;
    context.Init(1280, 720);
    presenter.init({
        .width          = 1280,
        .height         = 720,
        .instance       = context.GetInstance(),
        .device         = context.GetDevice(),
        .physicalDevice = context.GetPhysicalDevice(),
        .queue          = context.GetGraphicsQueue(),
        .queueIndex     = 0,
    });

    auto imguiRenderer = vkdt::ImGuiRenderer({
        .window          = presenter.getWindow(),
        .instance        = context.GetInstance(),
        .device          = context.GetDevice(),
        .physicalDevice  = context.GetPhysicalDevice(),
        .queue           = context.GetGraphicsQueue(),
        .minImageCount   = 2,
        .swapchainFormat = presenter.getSurfaceFormat().format,
    });

    while (!presenter.shouldClose())
    {
        presenter.prepare();
        imguiRenderer.begin(presenter.getCommandBuffer(),
                            {
                                .image      = presenter.getImage(),
                                .imageView  = presenter.getImageView(),
                                .renderArea = {0, 0, presenter.getWindowExtent().width,
                                               presenter.getWindowExtent().height},
                            });
        ImGui::Begin("Window");
        ImGui::Text("Hello, world!");
        ImGui::End();
        imguiRenderer.end(presenter.getCommandBuffer());
        presenter.present();
    }
    presenter.cleanup();

    return 0;
}
