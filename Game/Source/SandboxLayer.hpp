#pragma once

#include "Core.hpp"
#include "FrameBuffer.hpp"
#include "Texture.hpp"

class SandboxLayer : public RightEngine::Layer
{
public:
    SandboxLayer(): Layer("Sandbox") {}

    virtual void OnAttach() override;
    virtual void OnUpdate(float ts) override;
    virtual void OnImGuiRender() override;
private:
    std::shared_ptr<RightEngine::Scene> scene;
    std::shared_ptr<RightEngine::Shader> shader;
    std::shared_ptr<RightEngine::Renderer> renderer;
    std::shared_ptr<RightEngine::Framebuffer> frameBuffer;
    std::shared_ptr<RightEngine::Texture> texture;
    uint32_t id;
};
