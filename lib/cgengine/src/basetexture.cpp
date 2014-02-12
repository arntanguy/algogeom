#include "basetexture.hpp"

#include <sstream>


namespace cg {

BaseTexture::BaseTexture()
{
    glGenTextures(1, &mId);
    mType = GL_TEXTURE_2D;
}


/**
 * @brief Loads a texture from existing OpenGL one
 *
 * @param id
 * @param textureType
 */
BaseTexture::BaseTexture(const GLuint id, const GLuint textureType)
{
    mId = id;
    mType = textureType;
    std::stringstream ss;
    ss << "Texture from id: " << id;
    mName = ss.str();
}

BaseTexture::~BaseTexture() {
    glDeleteTextures(1, &mId);
}


void BaseTexture::loadFromId(const GLuint& id)
{
    mId = id;
}

}
