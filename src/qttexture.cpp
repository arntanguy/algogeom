#include "qttexture.hpp"
#include "GL/gl.h"
#include <QImage>

namespace cg
{

QtTexture::QtTexture() : BaseTexture()
{
    mType = GL_TEXTURE_2D;
}

bool QtTexture::load(const QImage& img)
{
    if (img.isNull()) return false;

    glBindTexture(mType, mId); //we activate this id to tell opengl we are going to use this texture
    glTexParameteri(mType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(mType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    /**
     * To eliminate white edge effect
     **/
    glTexParameteri(mType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(mType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // After this, there is no need to keep track of the image, it has been uploaded to the GPU
    glTexImage2D(mType, 0, GL_RGBA, img.width(),
                 img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 img.bits());
    glBindTexture(mType, 0);
    return true;
}

}
