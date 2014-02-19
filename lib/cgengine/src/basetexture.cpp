#include "basetexture.hpp"

#include <sstream>


namespace cg {

BaseTexture::BaseTexture(const GLint& colorFormat, const GLint& imageFormat, const GLuint& type) :
    mColorFormat(colorFormat), mImageFormat(imageFormat), mType(type)
{
    glGenTextures(1, &mId);
}


/**
 * @brief Loads a texture from existing OpenGL one
 *
 * @param id
 * @param textureType
 */
BaseTexture::BaseTexture(const GLuint& id, const GLuint& textureType, const GLint& colorFormat, const GLint& imageFormat) :
    mId(id), mColorFormat(colorFormat), mImageFormat(imageFormat), mType(textureType)
{
    std::stringstream ss;
    ss << "Texture from id: " << id;
    mName = ss.str();
}

BaseTexture::~BaseTexture() {
    glDeleteTextures(1, &mId);
}


}
