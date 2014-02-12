#ifndef __abstracttexture__
#define __abstracttexture__

#include <string>
#include <GL/gl.h>

namespace cg
{
class BaseTexture
{
protected:
    GLuint mId;
    std::string mName;
    GLuint mType;

public:
    BaseTexture();
    BaseTexture(const GLuint id, const GLuint textureType = GL_TEXTURE_2D);
    virtual ~BaseTexture();


    void bind() const
    {
        glBindTexture(mType, mId);
    }

    void unbind()
    {
        glBindTexture(mType, 0);
    }


    GLuint getTextureId() const
    {
        return mId;
    }

    GLuint getTextureType() const
    {
        return mType;
    }

    std::string getName() const
    {
        return mName;
    }



    void loadFromId(const GLuint& id);

};
};

#endif
