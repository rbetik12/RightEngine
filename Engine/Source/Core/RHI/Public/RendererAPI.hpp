#pragma once

#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "Types.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace RightEngine
{
    struct RendererSettings;

    struct Viewport
    {
        int32_t x;
        int32_t y;
        int32_t width;
        int32_t height;
    };

    class RendererAPI
    {
    public:
        virtual void Init() = 0;

        virtual void Configure(const RendererSettings& settings) = 0;

        virtual void SetClearColor(const glm::vec4& color) = 0;

        virtual void Clear(uint32_t clearBits) = 0;

        virtual void SetViewport(const Viewport& viewport) = 0;
        virtual Viewport GetViewport() = 0;

        virtual void DrawIndexed(const std::shared_ptr<IndexBuffer>& ib) = 0;
        virtual void Draw(const std::shared_ptr<VertexBuffer>& vb) = 0;

        static std::shared_ptr<RendererAPI> Create(GPU_API GpuApi);
        static GPU_API GetAPI();
    private:
        static GPU_API api;
    };
}
