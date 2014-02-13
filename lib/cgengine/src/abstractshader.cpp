#include "abstractshader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

using namespace std;

namespace cg
{

AbstractShader::AbstractShader()
{
    init();
}

AbstractShader::~AbstractShader()
{
    // If our program object pointer is valid, free it
    if (mProgramHandle)
    {
        glDeleteShader(mProgramHandle);
        mProgramHandle = 0;
    }

}

void AbstractShader::init()
{
    mProgramHandle = glCreateProgram();
}

std::string AbstractShader::loadFile(const std::string &str)
{
    //First, let us load the vertex shader.
    std::fstream dataFile(str.c_str(), std::ios::in);
    std::string data;

    if (dataFile.is_open())
    {
        //This is to help store the file's buffer.
        std::stringstream buffer;
        //Here, get all of the file's data by streaming the file's stream into our buffer stream.
        buffer << dataFile.rdbuf();
        //Now, buffer contains the vertex shader's data
        data = buffer.str();
    }
    else
    {
        throw std::runtime_error("Error: Unable to load shader file " + str);
    }
    if (mShaderName.length() == 0)
        mShaderName = str;
    return data;
}

/**
 * @brief: Display compilation errors from the OpenGL shader compiler
 * WARNING: This doesn't display glErrors which might still happen depending on
 * the state machine's state.
 */
void AbstractShader::printLog(GLuint object)
{
    GLint log_length = 0;
    if (glIsShader(object))
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else if (glIsProgram(object))
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else
    {
        std::cerr <<  "AbstractShader::printLog: Not a shader or a program" << std::endl;
        return;
    }

    char* log = new char[log_length];

    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, NULL, log);
    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, NULL, log);

    std::cerr << log << std::endl;
    delete log;
}

/**
 * @brief Enables the shader. This will effectively replace the render pipeline functions by
 * the program shader
 * In case the geometry shader fails
 * - Check whether the input geometry primitive type sent by the draw call is
 *   the same as the shader's
 */
void AbstractShader::enable()
{
    // Validate the program
    /**
     * XXX: do only during development to avoid this overhead in release
     **/
    glValidateProgram(mProgramHandle);
    GLint err = 0;
    glGetProgramiv(mProgramHandle, GL_VALIDATE_STATUS, &err);
    if (err == GL_FALSE)
    {
        std::cout << "AbstractShader::enable() - validation failed for shader " << mShaderName << std::endl;
        GLenum errCode;
        if ((errCode = glGetError()) != GL_NO_ERROR)
        {
            std::cerr << "AbstractShader::enable: Error: " << std::endl;
            const GLubyte *errString;
            errString = gluErrorString(errCode);
            std::cerr << "OpenGL Error (" << errCode << "): " << errString << std::endl;
            if (errCode == GL_INVALID_OPERATION)
            {
                std::cerr << "GL_INVALID_OPERATION: possible reasons include: " << std::endl
                          <<  " a geometry shader is active and mode is incompatible with the input primitive type of the geometry shader." << std::endl
                          <<  " a texture unit isn't bound to the correct type (ie GL_TEXTURE_CUBE_MAP bound to sampler2D..." << std::endl;
                exit(1);
            }
        }
        exit(1);
    }
    GLenum errCode;
    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "AbstractShader::enable: Error: " << std::endl;
        const GLubyte *errString;
        errString = gluErrorString(errCode);
        std::cerr << "OpenGL Error (" << errCode << "): " << errString << std::endl;
        if (errCode == GL_INVALID_OPERATION)
        {
            std::cerr << "GL_INVALID_OPERATION is generated if a geometry shader is active and mode is incompatible with the input primitive type of the geometry shader in the currently installed program object." << std::endl
                      << "GL_INVALID_OPERATION is generated if a non-zero buffer object name is bound to an enabled array or the element array and the buffer object's data store is currently mapped." << std::endl;
            exit(1);
        }
    }


    // Enable the program
    glUseProgram(mProgramHandle);
}
void AbstractShader::disable()
{
    glUseProgram(0);
}



/**
 * @brief Returns the uniform ID of a shader's variable
 *
 * @param strVariable
 *      Name of the variable
 * @return
 *      Uniform ID of variable
 */
int AbstractShader::getVariableId(const std::string& strVariable) const
{
    // If we don't have an active program object, let's return -1
    if (!mProgramHandle)
        return -1;

    int uniformId = glGetUniformLocation(mProgramHandle, strVariable.c_str());
    if (uniformId == -1)
        cerr << "Uniform \"" << strVariable << "\" not found in shader " << mShaderName << std::endl;

    //cout << "getVariableId("<<strVariable<<") = " << uniformId << endl;
    // This returns the variable ID for a variable that is used to find
    // the address of that variable in memory.
    return uniformId;
}

}
