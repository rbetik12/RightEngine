#include "UniformBuffer.hpp"
#include "Renderer.hpp"
#include "OpenGL/OpenGLUniformBuffer.hpp"
#include "Assert.hpp"

using namespace RightEngine;

std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::OpenGL:
            return std::make_shared<OpenGLUniformBuffer>(size, binding);
        default:
            R_CORE_ASSERT(false, "Unknown GPU API!");
    }
}
