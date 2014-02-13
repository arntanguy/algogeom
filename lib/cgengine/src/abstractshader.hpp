#ifndef __abstractshader__
#define __abstractshader__

#include <GL/glew.h>
#include <GL/gl.h>

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

        GLuint getProgramHandle() const
        {
            return mProgramHandle;
        }

        operator GLuint()
        {
            return mProgramHandle;
        }
        void operator()()
        {
            enable();
        }

        virtual void enable();
        static void disable();

        int getVariableId(const std::string& strVariable) const;


        //virtual void setUniform(const std::string &uniformVarName, float value);
        //virtual void setUniform(const std::string &uniformVarName, int value);

    public:
        AbstractShader();
        virtual ~AbstractShader();
};
}


#endif
