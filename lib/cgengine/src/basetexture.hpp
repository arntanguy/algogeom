#ifndef __abstracttexture__
#define __abstracttexture__

#include "GL/glew.h"
#include <string>
#include <GL/gl.h>
#include <iostream>

namespace cg
{
class BaseTexture
{
    protected:
        GLuint mId;
        std::string mName;
        GLuint mType;
        bool mTextureImage;

        GLint mColorFormat;
        GLenum mImageFormat;

        GLenum mAccess = GL_READ_WRITE;

    public:
        BaseTexture(const GLint& colorFormat = GL_RGBA, const GLint& imageFormat = GL_RGBA32F, const GLuint& type=GL_TEXTURE_2D);
        BaseTexture(const GLuint& id, const GLuint& textureType = GL_TEXTURE_2D, const GLint& colorFormat = GL_RGBA, const GLint& imageFormat = GL_RGBA32F);
        virtual ~BaseTexture();

        void setTextureImage(bool val=true) {
            mTextureImage = val;
        }

        void setAccess(GLenum access) {
            mAccess = access;
        }

        void bind() const
        {
            glBindTexture(mType, mId);
        }

        void bind(const GLuint& uniformID) const
        {
            bind();
            if(mTextureImage) {
        	    //// Because we're also using this tex as an image (in order to write to it),
        	    //// we bind it to an image unit as well
                if(uniformID != -1)  {
                    std::cout << "bind image texture to unit " << uniformID << std::endl;
        	        glBindImageTexture(uniformID, mId, 0, GL_FALSE, 0, mAccess, mImageFormat);
                }
                else
                    std::cout << "Warning : texture not found in shader" << std::endl;
            }
        }

        void unbind()
        {
            glBindTexture(mType, 0);
        }

        // GL_RGBA
        void genTexture(const unsigned int& width, const unsigned int& height) {
            glBindTexture(mType, mId);
            glTexParameteri(mType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(mType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(mType, 0, mColorFormat, width, height, 0, mImageFormat, GL_UNSIGNED_BYTE, NULL);
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

        bool isTextureImage() const 
        {
            return mTextureImage;
        }


        void loadFromId(const GLuint& id);

};

}

#endif
