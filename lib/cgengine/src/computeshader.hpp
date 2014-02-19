#ifndef __computeshader__
#define __computeshader__   

#include "abstractshader.hpp"
#include <iostream>
#include <vector>
#include "basetexture.hpp"
#include <memory>

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
        T* getBuffer(GLuint bufferID, int size)
        {
            cout << "getBuffer " << sizeof(T) << endl;
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
            // Copy data
            T *buffer = (T*)(glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, sizeof(T)*size, GL_MAP_READ_BIT ));
           //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            return buffer;
        }

        void bindTextureImage(const std::string& uniformName, std::shared_ptr<BaseTexture>& tex) {
            GLint uniformID = getVariableId(uniformName);
            tex->bind(uniformID);
        }


};

}

#endif
