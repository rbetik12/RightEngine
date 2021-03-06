#include "Renderer.hpp"
#include "RendererCommand.hpp"
#include "Scene.hpp"
#include "OpenGL/OpenGLRendererAPI.hpp"
#include <glm/ext/matrix_clip_space.hpp>

using namespace RightEngine;

void Renderer::SubmitMesh(const std::shared_ptr<Shader>& shader,
                              const MeshComponent& mesh,
                              const glm::mat4& transform)
{
    shader->Bind();
    shader->SetUniformMat4f("u_ViewProjection", sceneData.viewProjectionMatrix);
    shader->SetUniformMat4f("u_Transform", transform);
    const auto& va = mesh.GetVertexArray();
    const auto& ib = va->GetIndexBuffer();
    const auto& vb = va->GetVertexBuffer();
    va->Bind();
    if (ib)
    {
        ib->Bind();
        RendererCommand::DrawIndexed(ib);
    }
    else
    {
        RendererCommand::Draw(vb);
    }
}

void Renderer::BeginScene(const std::shared_ptr<Scene>& scene)
{
    const auto& camera = scene->GetCamera();
    const auto projectionMatrix = glm::perspective(camera->GetFOV(true),
                                                   camera->GetAspectRatio(),
                                                   camera->GetNear(),
                                                   camera->GetFar());
    const auto viewMatrix = camera->GetViewMatrix();
    sceneData.viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Renderer::EndScene()
{}

void Renderer::Configure()
{
    RendererCommand::Configure(settings);
}

RendererSettings& Renderer::GetSettings()
{
    return settings;
}

GPU_API Renderer::GetAPI()
{
    return RendererAPI::GetAPI();
}
