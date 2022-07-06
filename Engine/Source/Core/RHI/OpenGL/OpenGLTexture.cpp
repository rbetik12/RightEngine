#include "OpenGLTexture.hpp"
#include "Logger.hpp"
#include "Path.hpp"
#include "Assert.hpp"
#include "TextureLoader.hpp"
#include "OpenGLConverters.hpp"

using namespace RightEngine;

OpenGLTexture::OpenGLTexture(const TextureSpecification& aSpecification, const std::vector<uint8_t>& data)
{
    specification = aSpecification;
    Generate(data.data());
}

OpenGLTexture::OpenGLTexture(const TextureSpecification& aSpecification, const CubemapFaces& faces)
{
    specification = aSpecification;
    Generate(faces);
}

OpenGLTexture::~OpenGLTexture()
{
    glDeleteTextures(1, &id);
}

void OpenGLTexture::Bind(uint32_t slot) const
{
    glBindTextureUnit(slot, id);
}

void OpenGLTexture::UnBind() const
{
    glBindTexture(OpenGLConverters::textureType(specification.type), 0);
}

void OpenGLTexture::Generate(const void* buffer)
{
    ValidateTextureData();
    Init();
    GenerateTexture(buffer, OpenGLConverters::textureType(specification.type));
    UnBind();
}

void OpenGLTexture::Generate(const CubemapFaces& faces)
{
    ValidateTextureData();
    Init();
    for (int i = 0; i < 6; i++)
    {
        GenerateTexture(faces.GetFaceData(i).data(), OpenGLConverters::cubeMapFace(i));
    }
    UnBind();
}

void OpenGLTexture::GenerateTexture(const void* buffer, GLenum type)
{
    switch (specification.format)
    {
        case TextureFormat::RED8:
            glTexImage2D(type, 0,
                         GL_RED,
                         specification.width,
                         specification.height,
                         0,
                         GL_RED,
                         GL_UNSIGNED_BYTE,
                         buffer);
            break;
        case TextureFormat::RG32F:
            glTexImage2D(type,
                         0,
                         GL_RG32F,
                         specification.width,
                         specification.height,
                         0,
                         GL_RG,
                         GL_FLOAT,
                         buffer);
            break;
        case TextureFormat::RGB8:
            glTexImage2D(type, 0,
                         GL_RGB,
                         specification.width,
                         specification.height,
                         0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE,
                         buffer);
            break;
        case TextureFormat::RGB16F:
            glTexImage2D(type, 0,
                         GL_RGB16F,
                         specification.width,
                         specification.height,
                         0,
                         GL_RGB,
                         GL_FLOAT,
                         buffer);
            break;
        case TextureFormat::RGB32F:
            glTexImage2D(type, 0,
                         GL_RGB32F,
                         specification.width,
                         specification.height,
                         0,
                         GL_RGB,
                         GL_FLOAT,
                         buffer);
            break;
        case TextureFormat::RGBA8:
            glTexImage2D(type, 0,
                         GL_RGBA,
                         specification.width,
                         specification.height,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         buffer);
            break;
        default:
        R_CORE_ASSERT(false, "Unsupported texture format!");
    }
}

void OpenGLTexture::GenerateMipmaps() const
{
    Bind();
    glGenerateMipmap(OpenGLConverters::textureType(specification.type));
    UnBind();
}

void OpenGLTexture::Init()
{
    glGenTextures(1, &id);
    glBindTexture(OpenGLConverters::textureType(specification.type), id);
    glTexParameteri(OpenGLConverters::textureType(specification.type), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(OpenGLConverters::textureType(specification.type), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(OpenGLConverters::textureType(specification.type), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(OpenGLConverters::textureType(specification.type), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (specification.type == TextureType::CUBEMAP)
    {
        glTexParameteri(OpenGLConverters::textureType(specification.type), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
}

void OpenGLTexture::ValidateTextureData() const
{
    R_CORE_ASSERT(specification.width > 0
                  && specification.height > 0
                  && specification.componentAmount > 0
                  && specification.format != TextureFormat::NONE
                  && specification.type != TextureType::NONE, "Texture data is incorrect!");
}
