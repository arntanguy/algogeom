#ifndef __computeshader__
#define __computeshader__   

#include "abstractshader.hpp"
#include <iostream>

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
            glGetUniformBlockIndex(mProgramHandle, name.c_str());
        }

        GLuint texHandle;
        void test() 
        {
            GLuint buffer;
            GLfloat test_a[10] {0., 1., 2., 3., 4., 5., 6., 7., 8., 9.};
            glGenBuffers( 1, &buffer);
            // Create buffer
            glBindBuffer( GL_SHADER_STORAGE_BUFFER, buffer );
            // Upload data 
            glBufferData( GL_SHADER_STORAGE_BUFFER, sizeof(test_a), test_a, GL_STATIC_DRAW );
            // Upload
            GLint id = getUniformBlockIndex("MyBuffer");
            if(id != -1) 
                glBindBufferBase( GL_SHADER_STORAGE_BUFFER, id, buffer );
            else
                cerr << "Warning: uniform block MyBuffer not found in shader";

            enable();
            genTexture();

            glDispatchCompute( 512/16, 512/16, 1 ); // 512^2 threads in blocks of 16 
            //glDispatchCompute( 50, 50, 1 );
            glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

            GLfloat *test_res = new GLfloat[10];
            test_res = (GLfloat *) glMapBuffer( GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY );
            std::cout << "Result: " << std::endl;
            for(int i=0; i<10; i++) {
                cout << test_res[i] << ", ";
            }
            // Delete data?
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        GLuint genTexture() {
        	// We create a single uint32 channel 512^2 texture
        	glGenTextures(1, &texHandle);
        
        	glBindTexture(GL_TEXTURE_2D, texHandle);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, 512, 512, 0, GL_RED, GL_INT, NULL);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        
        	//// Because we're also using this tex as an image (in order to write to it),
        	//// we bind it to an image unit as well
            int texture_id = getVariableId("destTex");
            if(texture_id != -1)  {
                cout << "bind image texture to unit " << texture_id << endl;
        	    glBindImageTexture(texture_id, texHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32I);
            }
            else
                cout << "Warning : texture destTex not found in shader" << endl;

        	return texHandle;
        }
};

}

#endif
