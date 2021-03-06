#include "OpenGLConverters.hpp"

using namespace RightEngine;

GLenum OpenGLConverters::GLSamplerFilter(SamplerFilter minFilter, SamplerFilter mipmapFilter, bool isMipMapped)
{
    if (!isMipMapped)
    {
        return GLSamplerFilter(minFilter);
    }

    switch (minFilter)
    {
        case SamplerFilter::Nearest:
        {
            switch (mipmapFilter)
            {
                case SamplerFilter::Nearest:
                    return GL_NEAREST_MIPMAP_NEAREST;
                case SamplerFilter::Linear:
                    return GL_NEAREST_MIPMAP_LINEAR;
            }
        }
        case SamplerFilter::Linear:
        {
            switch (mipmapFilter)
            {
                case SamplerFilter::Nearest:
                    return GL_LINEAR_MIPMAP_NEAREST;
                case SamplerFilter::Linear:
                    return GL_LINEAR_MIPMAP_LINEAR;
            }
        }
    }
    return 0;
}

GLenum OpenGLConverters::GLSamplerFilter(SamplerFilter magFilter)
{
    switch (magFilter)
    {
        case SamplerFilter::Nearest:
            return GL_NEAREST;
        case SamplerFilter::Linear:
            return GL_LINEAR;
    }
}

GLenum OpenGLConverters::GLTextureType(TextureType type)
{
    switch (type)
    {
        case TextureType::TEXTURE_2D:
            return GL_TEXTURE_2D;
        case TextureType::CUBEMAP:
            return GL_TEXTURE_CUBE_MAP;
    }
}

GLenum OpenGLConverters::GLCubemapFace(uint8_t face)
{
    R_CORE_ASSERT(face < 6, "");
    return GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
}
