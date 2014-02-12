#ifndef __baseshader__
#define __baseshader__

#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include "vao.hpp"
#include "vbo.hpp"

namespace cg
{
class BaseTexture;

class BaseShader
{
protected:
    std::string mShaderName;
    GLenum shader_type;
    GLhandleARB mVertexHandle;
    GLhandleARB mFragmentHandle;
    GLhandleARB mGeometryHandle;
    GLhandleARB mProgramHandle;

    VAO mVAO;
    // uniform ID, texture_id
    std::map<GLuint, std::shared_ptr<BaseTexture>> m_textures;
    void bindTextures();

    std::string loadFile(const std::string &str);
    GLhandleARB createSharedObjects(const std::string shader,
                                    GLenum shaderType);
    void linkShader();

    void init();

    int getVariableId(const std::string& strVariable) const;


    void printLog(GLuint object);


public:
    BaseShader();
    BaseShader(const std::string &vertexShaderPath,
               const std::string &fragmentShaderPath);
    virtual ~BaseShader();

    void loadFromFile(const std::string& vertex, const std::string& fragment);
    void loadFromMemory(const std::string& vertex, const std::string& fragment);

    void loadVertexShaderFromFile(const std::string &);
    void loadVertexShader(const std::string &);

    void loadFragmentShaderFromFile(const std::string &);
    void loadFragmentShader(const std::string &);

    void loadGeometryShaderFromFile(const std::string &shader);
    void loadGeometryShader(const std::string &shader);

    void setName(const std::string& name)
    {
        mShaderName = name;
    }
    std::string getName() const
    {
        return mShaderName;
    }

    void enable();
    static void disable();

    void setTexture(const std::string &uniformLocation, GLuint texture);
    void setTexture(const std::string &uniformLocation, const std::shared_ptr<BaseTexture>& texture);
    void clear_texture();

    bool hasShaderSupport();

    GLhandleARB getProgramHandle() const;


    operator GLuint()
    {
        return mProgramHandle;
    }
    void operator()()
    {
        enable();
    }



    virtual void setUniform(const std::string &uniformVarName, float value);
    virtual void setUniform(const std::string &uniformVarName, int value);

    // VBO set input variable
    bool setIn(std::shared_ptr<cg::VBO>& vbo, const std::vector<std::string>& inNames);
    bool setIn(std::shared_ptr<cg::VBO>& vbo, const std::initializer_list<std::string>& inNames)
    {
        std::vector<std::string> v(inNames);
        return setIn(vbo, v);
    }
};

}

#endif
