#include "OpenGLTexture3D.hpp"
#include "TextureLoader.hpp"
#include <glad/glad.h>

using namespace RightEngine;

OpenGLTexture3D::OpenGLTexture3D(const std::array<std::string, 6>& texturesPath)
{
    CubeMapFaces faces;
    const auto textureLoader = TextureLoader();
    for (int i = 0; i < texturesPath.size(); i++)
    {
        const auto [data, spec] = textureLoader.Load(texturesPath[i]);
        if (specification.format != TextureFormat::None)
        {
            R_CORE_ASSERT(specification == spec, "");
        }
        specification = spec;
        faces.SetFaceData(data, i);
    }

    Generate(faces);
}

OpenGLTexture3D::~OpenGLTexture3D()
{
    glDeleteTextures(1, &id);
}

void OpenGLTexture3D::Bind(uint32_t slot) const
{
    glBindTextureUnit(slot, id);
}

void OpenGLTexture3D::UnBind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void OpenGLTexture3D::Generate(const CubeMapFaces& faces)
{
    for (int i = 0; i < 6; i++)
    {
        switch (specification.format)
        {
            case TextureFormat::RGB32F:
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB32F,
                             specification.width,
                             specification.height,
                             0, GL_RGB,
                             GL_FLOAT, faces.GetFaceData(i).data());
                break;
            default:
            R_CORE_ASSERT(false, "");
        }
    }
}
