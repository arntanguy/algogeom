#include "baseshader.hpp"
#include "basetexture.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#define BUFFER_OFFSET(i)   ((char *)NULL + (i))

using namespace std;

namespace cg
{

/**
 * @brief Creates a shader class
 * @throws runtime_error if the shaders aren't supported
 */
BaseShader::BaseShader()
{
    init();
}


/**
 * @brief Loads a vertex and fragment shader from filesystem
 *
 * @param vertexShaderPath
 *      Path to the vertex shader
 * @param fragmentShaderPath
 *      Path to the fragment shader
 */
BaseShader::BaseShader(const std::string &vertexShaderPath,
                       const std::string &fragmentShaderPath)
{
    init();
    loadFromFile(vertexShaderPath, fragmentShaderPath);
}

BaseShader::~BaseShader()
{
    // To free a shader we need to detach the vertex and fragment
    // shader pointers from the program object, then free each one.
    // Once that is done we can finally delete the program object.

    // If our vertex shader pointer is valid, free it
    if (mVertexHandle)
    {
        glDetachShader(mProgramHandle, mVertexHandle);
        glDeleteShader(mVertexHandle);
        mVertexHandle = 0;
    }

    // If our fragment shader pointer is valid, free it
    if (mFragmentHandle)
    {
        glDetachShader(mProgramHandle, mFragmentHandle);
        glDeleteShader(mFragmentHandle);
        mFragmentHandle = 0;
    }

    // If our geometry shader pointer is valid, free it
    if (mGeometryHandle)
    {
        glDetachShader(mProgramHandle, mGeometryHandle);
        glDeleteShader(mGeometryHandle);
        mGeometryHandle = 0;
    }

    // If our program object pointer is valid, free it
    if (mProgramHandle)
    {
        glDeleteShader(mProgramHandle);
        mProgramHandle = 0;
    }
}

void BaseShader::loadFromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    std::string vertex = loadFile(vertexShaderPath);
    std::string fragment = loadFile(fragmentShaderPath);
    loadVertexShader(vertex);
    loadFragmentShader(fragment);
}

void BaseShader::loadFromMemory(const std::string& vertex, const std::string& fragment)
{
    loadVertexShader(vertex);
    loadFragmentShader(fragment);
}

/**
 * @brief Checks for shader support
 */
void BaseShader::init()
{
    if (hasShaderSupport())
    {
        //Create a mProgramHandle handle.
        mProgramHandle = glCreateProgram();
        mVertexHandle = 0;
        mFragmentHandle = 0;
        mGeometryHandle = 0;
        mVAO.create();

    }
    else
    {
        throw std::runtime_error(
            "Error: GL Shaders not supported or Glew not initialized properly!");
    }
}

std::string BaseShader::loadFile(const std::string &str)
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
 * @brief Loads a vertex shader
 *
 * @param vertexPath
 *      Vertex program
 *
 */
void BaseShader::loadVertexShaderFromFile(const std::string &vertexShader)
{
    loadVertexShader(loadFile(vertexShader));
}

/**
 * @brief Loads a fragment shader
 *
 * @param fragmentShader
 *      Fragment program
 */
void BaseShader::loadFragmentShaderFromFile(const std::string &fragmentShader)
{
    loadFragmentShader(loadFile(fragmentShader));
}
/**
 * @brief Loads a vertex shader
 *
 * @param vertexPath
 *      Vertex program
 *
 */
void BaseShader::loadVertexShader(const std::string &vertexShader)
{
    mVertexHandle = createSharedObjects(vertexShader, GL_VERTEX_SHADER);
    linkShader();
}

/**
 * @brief Loads a fragment shader
 *
 * @param fragmentShader
 *      Fragment program
 */
void BaseShader::loadFragmentShader(const std::string &fragmentShader)
{
    mFragmentHandle = createSharedObjects(fragmentShader, GL_FRAGMENT_SHADER);
    linkShader();
}

/**
 * @brief Loads a geometry shader from path
 *
 * @param shaderPath
 *      Path of the geometric shader
 */
void BaseShader::loadGeometryShaderFromFile(const std::string &shaderPath)
{
    loadGeometryShader(loadFile(shaderPath));
}


void BaseShader::loadGeometryShader(const std::string &shader)
{
    mGeometryHandle = createSharedObjects(shader, GL_GEOMETRY_SHADER);
    linkShader();
}

/**
 * @brief: Display compilation errors from the OpenGL shader compiler
 * WARNING: This doesn't display glErrors which might still happen depending on
 * the state machine's state.
 */
void BaseShader::printLog(GLuint object)
{
    GLint log_length = 0;
    if (glIsShader(object))
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else if (glIsProgram(object))
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else
    {
        std::cerr <<  "BaseShader::printLog: Not a shader or a program" << std::endl;
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
 * @brief Compiles the given shader
 *
 * @param shader
 *        A string containing the shader program
 * @param shaderType
 *        BaseShader type. Must be one of GL_COMPUTE_SHADER, GL_VERTEX_SHADER,
 *        GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER or GL_FRAGMENT_SHADER
 *
 * @return
 *        The handle of the built shader
 */
GLuint BaseShader::createSharedObjects(const std::string shader,
                                       GLenum shaderType)
{

    //Obtain a valid handle to a vertex shader object.
    GLuint shaderHandle = glCreateShader(shaderType);

    if (!shaderHandle)
    {
        //We have failed creating the vertex shader object.
        std::cerr << "BaseShader::createSharedObjects: Failed creating shader object";
    }

    //Now, compile the shader source.
    //Note that glShaderSource takes an array of chars. This is so that one can load multiple vertex shader files at once.
    //This is similar in function to linking multiple C++ files together. Note also that there can only be one "void main" definition
    //In all of the linked source files that are compiling with this funciton.
    const char *te = (shader.c_str());
    glShaderSource(shaderHandle, //The handle to our shader
                   1, //The number of files.
                   &te, //An array of const char * data, which represents the source code of the shaders
                   NULL); //An array of string lengths. For have null terminated strings, pass NULL.

    //Attempt to compile the shader.
    glCompileShader(shaderHandle);

    GLint compile_ok = GL_FALSE;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE)
    {
        std::cerr << "BaseShader::createSharedObjects: BaseShader " + shader + " failed compilation." << std::endl;
        printLog(shaderHandle);
        glDeleteShader(shaderHandle);
        return 0;
    }

    //Attach the shaders.
    // This is very similar to the linking process in C, only the attached objects take part of the linking process.
    glAttachShader(mProgramHandle, shaderHandle);

    return shaderHandle;
}

/**
 * @brief Links the shader object to the shader program
 * The shader has to be built first by calling BaseShader::createSharedObjects
 *
 * @param shaderHandle
 *    Handle of the shader to link
 */
//XXX: wtf
void BaseShader::linkShader()
{

    //Link the mProgramHandle.
    glLinkProgram(mProgramHandle);

    GLint Success;
    glGetProgramiv(mProgramHandle, GL_LINK_STATUS, &Success);
    if (Success == 0)
    {
        printLog(mProgramHandle);
    }
    //Now the program can be used.
}

/**
 * @brief Enables the shader. This will effectively replace the render pipeline functions by
 * the program shader
 * In case the geometry shader fails
 * - Check whether the input geometry primitive type sent by the draw call is
 *   the same as the shader's
 */
void BaseShader::enable()
{
    // Validate the program
    /**
     * XXX: do only during developpement to avoid this overhead in release
     **/
    glValidateProgram(mProgramHandle);
    GLint err = 0;
    glGetProgramiv(mProgramHandle, GL_VALIDATE_STATUS, &err);
    if (err == GL_FALSE)
    {
        std::cout << "BaseShader::enable() - validation failed for shader " << mShaderName << std::endl;
        GLenum errCode;
        if ((errCode = glGetError()) != GL_NO_ERROR)
        {
            std::cerr << "BaseShader::enable: Error: " << std::endl;
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
        std::cerr << "BaseShader::enable: Error: " << std::endl;
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

    // Bind the textures
    bindTextures();

    mVAO.bind();
}

/**
 * @brief Restores the render pipeline
 */
void BaseShader::disable()
{
    cg::VAO::unbind();
    glUseProgram(0);
}

/**
 * @brief Checks for vertex and fragment shader support
 *
 * @return
 */
bool BaseShader::hasShaderSupport()
{
    return GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader;
}

/**
 * @brief Sets a texture from an OpenGL texture ID
 *
 * @param uniformLocation
 * @param texture
 */
void BaseShader::setTexture(const std::string &uniformLocation, GLuint texture)
{
    int uniformID = getVariableId(uniformLocation);
    //cout << "setTexture " << texture << " for uniform " << uniformLocation << "("<<uniformID<<")" << endl;
    m_textures[uniformID] = shared_ptr<BaseTexture>(new BaseTexture(texture));
}

void BaseShader::setTexture(const std::string &uniformLocation, const shared_ptr<BaseTexture>& texture)
{
    int uniformID = getVariableId(uniformLocation);
    //cout << "setTexture " << texture << " for uniform " << uniformLocation << "(" << uniformID << ")" << endl;
    m_textures[uniformID] = texture;
}

void BaseShader::clear_texture()
{
    m_textures.clear();
}
/**
 * Bind textures to GL_TEXTURE0 + some number
 * For all the textures assigned to the shader, bind them.
 */
void BaseShader::bindTextures()
{
    int texture_slot_index = 0;
    for (auto it = m_textures.begin(); it != m_textures.end(); it++)
    {
        glActiveTexture(GL_TEXTURE0 + texture_slot_index);
        //std::cout << "bind: " << it->second->getTextureType() << ", " << it->second->getTextureId() << std::endl;
        glBindTexture(it->second->getTextureType(), it->second->getTextureId());
        // Binds to the shader
        glUniform1i(it->first, texture_slot_index);
        texture_slot_index++;
    }
    // Make sure that the texture unit which is left active is the number 0
    glActiveTexture(GL_TEXTURE0);
}

GLuint BaseShader::getProgramHandle() const
{
    return mProgramHandle;
}

/**
 * @brief Returns the uniform ID of a shader's variable
 *
 * @param strVariable
 *      Name of the variable
 * @return
 *      Uniform ID of variable
 */
int BaseShader::getVariableId(const std::string& strVariable) const
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


/**
 * @brief Sets an uniform GLSL float value
 * WARNING: This has to be called after the shader has been enabled (by calling the enable() function)!
 *
 * @param uniformVarName
 *      The uniform variable name in the GLSL shader
 * @param value
 *      The value to set
 */
void BaseShader::setUniform(const std::string &uniformVarName, float value)
{
    glUniform1f(getVariableId(uniformVarName), value);
}

void BaseShader::setUniform(const std::string &uniformVarName, int value)
{
    glUniform1i(getVariableId(uniformVarName), value);
}

bool BaseShader::setIn(std::shared_ptr<cg::VBO>& vbo, const std::vector<std::string>& inNames)
{
    mVAO.bind();
    vbo->bind();
    std::cout << "VBO setIn id: " << vbo->getId() << std::endl;

    bool status = true;
    int i = 0;
    int offset = 0;
    for (auto in : inNames)
    {
        GLint loc = glGetAttribLocation(mProgramHandle, in.c_str());
        std::cout << "in: " << in << ", loc: " << loc << std::endl;
        if (loc != -1)
        {
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc , vbo->getDimension(i), GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
        }
        else
        {
            status = false;
        }
        ++i;
        offset += vbo->getSize(i);
    }

    GLenum errCode;
    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "Shader::setIn: error1" << std::cerr;
        const GLubyte *errString;
        errString = gluErrorString(errCode);
        std::cerr << "\tOpenGL Error (" << errCode << "): " << errString << std::endl;
        exit(0);
    }
    cg::VBO::unbind();
    cg::VAO::unbind();

    return status;
}

}
