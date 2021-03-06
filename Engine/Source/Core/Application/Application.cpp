#include "Application.hpp"
#include "Assert.hpp"
#include "Logger.hpp"
#include "DebugRHI.hpp"
#include "RendererCommand.hpp"
#include "Input.hpp"
#include "Types.hpp"
#include "ImGuiLayer.hpp"
#include <memory>

namespace RightEngine
{
    Application& Application::Get()
    {
        static Application application;
        return application;
    }

    Application::Application()
    {
        Init();
    }

    Application::~Application()
    {
        for (auto& layer: layers)
        {
            layer->OnDetach();
            layer.reset();
        }

        layers.clear();
    }

    void Application::Init()
    {
        window.reset(Window::Create("Right Editor", 1920, 1080));
        RendererCommand::Init(GPU_API::OpenGL);
        DebugRHI::Init();

        imGuiLayer = std::make_shared<ImGuiLayer>();

        R_CORE_INFO("Successfully initialized application!");
    }

    void Application::PostInit()
    {
        static bool wasCalled = false;
        R_CORE_ASSERT(!wasCalled, "PostInit was called twice!");
        wasCalled = true;

        PushOverlay(imGuiLayer);
    }

    void Application::OnUpdate()
    {
        Input::OnUpdate();
        RendererCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
        RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window->OnUpdate();

        for (const auto& layer: layers)
        {
            layer->OnUpdate(Input::deltaTime);
        }

        imGuiLayer->Begin();
        {
            for (const auto& layer : layers)
            {
                layer->OnImGuiRender();
            }
        }
        imGuiLayer->End();
    }

    void Application::OnUpdateEnd()
    {
        window->Swap();
    }

    void Application::PushLayer(const std::shared_ptr<Layer>& layer)
    {
        // TODO: Move to LayerStack data structure
        static int layerIndex = 0;
        layers.emplace(layers.begin() + layerIndex, layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(const std::shared_ptr<Layer>& layer)
    {
        layers.emplace_back(layer);
        layer->OnAttach();
    }

    const std::shared_ptr<Window>& Application::GetWindow() const
    {
        return window;
    }

}
