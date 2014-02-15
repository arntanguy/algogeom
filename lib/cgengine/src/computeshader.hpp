#ifndef __computeshader__
#define __computeshader__   

#include "abstractshader.hpp"
#include <iostream>
#include <vector>

using namespace std;

namespace cg
{

class ComputeShader : public AbstractShader
{
    protected:
        void createComputeShader(const std::string& source);

        GLuint mComputeShaderHandle;

    public:
        ComputeShader();
        ~ComputeShader();

        void loadFromFile(const std::string& source) {
            createComputeShader(source);
        }

        GLint getUniformBlockIndex(const std::string &name)
        {
            return glGetUniformBlockIndex(mProgramHandle, name.c_str());
        }

        template <typename T>
        GLuint createBuffer(const std::vector<T>& data)
        {
            GLuint buffer;
            glGenBuffers(1, &buffer);
            
            // Create buffer
            glBindBuffer( GL_SHADER_STORAGE_BUFFER, buffer );
            // Upload data 
            glBufferData( GL_SHADER_STORAGE_BUFFER, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW );

            return buffer;
        }

        // Shader must be enabled!
        void bindBuffer(const std::string& uniformName, const GLuint& bufferId)
        {
            // Upload
            cout << uniformName << endl;
            GLint id = getUniformBlockIndex(uniformName.c_str());
            if(id != -1) 
                glBindBufferBase( GL_SHADER_STORAGE_BUFFER, id, bufferId );
            else
                cerr << "Warning: uniform block not found in shader" << endl;
        }

        template <typename T>
        void getBuffer(T* buffer)
        {
            buffer = static_cast<T>(glMapBuffer( GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY ));
        }

        GLuint genTexture() {
        	// We create a single uint32 channel 512^2 texture
            GLuint texHandle;
        	glGenTextures(1, &texHandle);
        
        	glBindTexture(GL_TEXTURE_2D, texHandle);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, 512, 512, 0, GL_RED, GL_INT, NULL);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
        
        	return texHandle;
        }
        void bindImageTexture(const std::string& uniformName, const GLuint& tex_id)
        {
            glBindTexture(GL_TEXTURE_2D, tex_id);
        	//// Because we're also using this tex as an image (in order to write to it),
        	//// we bind it to an image unit as well
            int texture_id = getVariableId(uniformName.c_str());
            if(texture_id != -1)  {
                cout << "bind image texture to unit " << texture_id << endl;
        	    glBindImageTexture(texture_id, tex_id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32I);
            }
            else
                cout << "Warning : texture not found in shader" << endl;
        }

};

}

#endif
