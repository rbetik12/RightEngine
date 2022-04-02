#include "SandboxLayer.hpp"
#include "Renderer.hpp"
#include "LightNode.hpp"
#include "EditorCamera.hpp"
#include "RendererCommand.hpp"
#include <glad/glad.h>
#include <imgui.h>

enum class GeometryType
{
    CUBE,
    PLANE
};

std::shared_ptr<RightEngine::SceneNode> CreateTestSceneNode(GeometryType type, const std::string& texturePath)
{
    std::shared_ptr<RightEngine::Texture> texture;
    if (!texturePath.empty())
    {
        texture = std::make_shared<RightEngine::Texture>(texturePath);
    }
    auto node = std::make_shared<RightEngine::SceneNode>();
    std::shared_ptr<RightEngine::Geometry> geometry;
    switch (type)
    {
        case GeometryType::CUBE:
            geometry = RightEngine::GeometryBuilder::CubeGeometry();
            break;
        case GeometryType::PLANE:
            geometry = RightEngine::GeometryBuilder::PlaneGeometry();
            break;
    }
    node->SetGeometry(geometry);
    node->GetGeometry()->GetMaterial()->SetBaseTexture(texture);
    return node;
}

void SandboxLayer::OnAttach()
{
    const auto camera = std::make_shared<RightEngine::EditorCamera>(glm::vec3(0, 5, -15),
                                                                    glm::vec3(0, 1, 0));
    scene = std::make_shared<RightEngine::Scene>();

    const auto cube1 = CreateTestSceneNode(GeometryType::CUBE, "/Assets/Textures/WoodAlbedo.png");
    cube1->SetPosition({ 0, 0.0f, 0 });
    const auto cube2 = CreateTestSceneNode(GeometryType::CUBE, "");
    cube2->SetPosition({ 5.0f, 2.0f, 0 });
    scene->SetCamera(camera);
    scene->GetRootNode()->AddChild(cube1);
    scene->GetRootNode()->AddChild(cube2);
    shader = std::make_shared<RightEngine::Shader>("/Assets/Shaders/Basic/basic.vert",
                                                   "/Assets/Shaders/Basic/basic.frag");
    renderer = std::make_shared<RightEngine::Renderer>();

    RightEngine::FramebufferSpecification fbSpec;
    fbSpec.width = 1280;
    fbSpec.height = 720;
    fbSpec.attachments = RightEngine::FramebufferAttachmentSpecification(
            {
                    RightEngine::FramebufferTextureSpecification(RightEngine::FramebufferTextureFormat::RGBA8),
                    RightEngine::FramebufferTextureSpecification(RightEngine::FramebufferTextureFormat::RGBA8),
                    RightEngine::FramebufferTextureSpecification(RightEngine::FramebufferTextureFormat::Depth),
            }
    );

    frameBuffer = std::make_shared<RightEngine::Framebuffer>(fbSpec);
}

void SandboxLayer::OnUpdate(float ts)
{
    scene->OnUpdate();

    frameBuffer->Bind();
    RightEngine::RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer->Configure();
    renderer->BeginScene(scene);

    const auto children = scene->GetRootNode()->GetAllChildren();
    shader->Bind();
    for (const auto& child: children)
    {
        shader->SetMaterialUniforms(child->GetGeometry()->GetMaterial());
        renderer->SubmitGeometry(shader, child->GetGeometry(), child->GetWorldTransformMatrix());
    }

    renderer->EndScene();
    frameBuffer->UnBind();
}

void SandboxLayer::OnImGuiRender()
{
    ImGui::Begin("Scene view");
    id = frameBuffer->GetColorAttachment();
    ImGui::Image((void*)id, ImVec2(1280, 720));
    ImGui::End();
}