#ifndef __abstractshader__
#define __abstractshader__

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

#include <string>

namespace cg
{
class AbstractShader
{
    protected:
        GLhandleARB mProgramHandle;

        std::string mShaderName;
        GLenum shader_type;

        std::string loadFile(const std::string &str);
        void printLog(GLuint object);

        void init();


        void setName(const std::string& name)
        {
            mShaderName = name;
        }
        std::string getName() const
        {
            return mShaderName;
        }

        operator GLuint()
        {
            return mProgramHandle;
        }
        void operator()()
        {
            enable();
        }

        int getVariableId(const std::string& strVariable) const;


    public:
        AbstractShader();
        virtual ~AbstractShader();

        virtual void enable();
        static void disable();

        GLuint getProgramHandle() const
        {
            return mProgramHandle;
        }

        virtual void setUniform(const std::string &uniformVarName, float value);
        virtual void setUniform(const std::string &uniformVarName, int value);
        virtual void setUniform(const std::string &uniformVarName, const glm::vec2& vector);
        virtual void setUniform(const std::string &uniformVarName, const glm::vec3& vector);
        virtual void setUniform(const std::string &uniformVarName, const glm::vec4& vector);
        virtual void setUniform(const std::string &uniformVarName, const glm::mat4& mat);


};
}


#endif
