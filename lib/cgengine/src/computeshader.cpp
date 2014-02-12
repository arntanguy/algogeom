#include "computeshader.hpp"

namespace cg
{

ComputeShader::ComputeShader() :AbstractShader()
{
}
ComputeShader::~ComputeShader()
{
}
void ComputeShader::createComputeShader(const std::string& source)
{

    GLuint mComputeShaderHandle = glCreateShader(GL_COMPUTE_SHADER);

    // In order to write to a texture, we have to introduce it as image2D.
    // local_size_x/y/z layout variables define the work group size.
    // gl_GlobalInvocationID is a uvec3 variable giving the global ID of the thread,
    // gl_LocalInvocationID is the local index within the work group, and
    // gl_WorkGroupID is the work group's index

    std::string src = loadFile(source);
    const char *te = src.c_str();
    cout << te << endl;
    glShaderSource(mComputeShaderHandle, 1, &te, NULL);
    glCompileShader(mComputeShaderHandle);
    int rvalue;
    glGetShaderiv(mComputeShaderHandle, GL_COMPILE_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in compiling the compute shader\n");
        GLchar log[10240];
        GLsizei length;
        glGetShaderInfoLog(mComputeShaderHandle, 10239, &length, log);
        fprintf(stderr, "Compiler log:\n%s\n", log);
        exit(40);
    }
    glAttachShader(mProgramHandle, mComputeShaderHandle);

    glLinkProgram(mProgramHandle);
    glGetProgramiv(mProgramHandle, GL_LINK_STATUS, &rvalue);
    if (!rvalue) {
        fprintf(stderr, "Error in linking compute shader program\n");
        GLchar log[10240];
        GLsizei length;
        glGetProgramInfoLog(mProgramHandle, 10239, &length, log);
        fprintf(stderr, "Linker log:\n%s\n", log);
        exit(41);
    }   
}



}
