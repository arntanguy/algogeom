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

        void test() 
        {
            GLuint buffer;
            GLfloat test_a[10] {0., 1., 2., 3., 4., 5., 6., 7., 8., 9.};
            glGenBuffers( 1, &buffer);
            glBindBuffer( GL_SHADER_STORAGE_BUFFER, buffer );
            glBufferData( GL_SHADER_STORAGE_BUFFER, sizeof(test_a), test_a, GL_STATIC_DRAW );
            //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

             glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 0, buffer );


            enable();
            //glBindBuffer( GL_SHADER_STORAGE_BUFFER, buffer );
            glDispatchCompute( 10, 1, 1 );
            //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

            glBindBuffer( GL_SHADER_STORAGE_BUFFER, buffer );
            GLfloat *test_res = new GLfloat[10];
            test_res = (GLfloat *) glMapBuffer( GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY );
            glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );
            std::cout << "Result: " << std::endl;
            for(int i=0; i<10; i++) {
                cout << test_res[i] << ", ";
            }
        }
        
};

}

#endif
