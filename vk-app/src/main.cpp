#include "context.h"
#include "vk_imgui_presenter.h"

using namespace figment;

int main()
{
    figment::vulkan::Context context;
    vk_imgui_presenter::VkImGuiPresenter vkImGuiPresenter;
    context.Init(1280, 720);

    vkImGuiPresenter.init(context.GetInstance(), context.GetDevice(), context.GetPhysicalDevice(),
                          context.GetGraphicsQueue(), 0, 1280, 720);

    while (true)
    {
        vkImGuiPresenter.prepare(context.GetDevice());
        ImGui::Begin("Window");
        ImGui::Text("Hello, world!");
        ImGui::End();
        vkImGuiPresenter.present(context.GetGraphicsQueue());
    }

    return 0;
}
