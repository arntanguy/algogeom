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

        GLuint getUniformBlockIndex(const std::string &name)
        {
            GLuint storage_block_index = glGetProgramResourceIndex( mProgramHandle, GL_SHADER_STORAGE_BLOCK, name.c_str()) ;
            
            //return storage_block_index;
            return glGetUniformBlockIndex(mProgramHandle, name.c_str());
        }

        template <typename T>
        GLuint createBuffer(const std::vector<T>& data)
        {
            cout << "setting data to buffer: " << endl;
            for(int i=0; i<10; i++)
            {
                cout << data[i] << ", " ;
            }
            cout << endl;

            GLuint buffer;
            glGenBuffers(1, &buffer);
            
            cout << "creating buffer of size (" << sizeof(T) <<"x" << data.size() << ")" << endl;
            // Create buffer
            glBindBuffer( GL_SHADER_STORAGE_BUFFER, buffer );
            // Upload data 
            glBufferData( GL_SHADER_STORAGE_BUFFER, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW );

            return buffer;
        }

        // Shader must be enabled!
        void bindBuffer(const std::string& uniformName, const GLuint& bufferId, const GLuint& bufferBinding)
        {
            cout << "binding buffer " << bufferId << " to uniform " << uniformName << endl;


            GLuint storage_block_index = glGetProgramResourceIndex( mProgramHandle, GL_SHADER_STORAGE_BLOCK, "MyBuffer") ;
            cout << "storage_block_index: " << storage_block_index << endl;
            glShaderStorageBlockBinding(mProgramHandle, storage_block_index, bufferBinding);
            glBindBufferBase( GL_SHADER_STORAGE_BUFFER, bufferBinding, bufferId );
        }

        template<typename T>
        T* getBuffer(GLuint bufferID)
        {
            cout << "getBuffer" << endl;
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
            // Copy data
            T *buffer = static_cast<T*>(glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, sizeof(T)*10, GL_MAP_READ_BIT));
            //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            return buffer;
        }

        GLuint genTexture(const unsigned int& width, const unsigned int& height) {
        	// We create a single uint32 channel 512^2 texture
            GLuint texHandle;
        	glGenTextures(1, &texHandle);
        
        	glBindTexture(GL_TEXTURE_2D, texHandle);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
