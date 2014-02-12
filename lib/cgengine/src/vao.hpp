#ifndef __vao__
#define __vao__

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
namespace cg
{

class VAO
{
private:
    GLuint vao;

public:
    VAO();
    virtual ~VAO();

    void create();

    void bind()
    {
        if (vao)
        {
            glBindVertexArray(vao);
        }
        else
        {
            std::cerr << "Null VAO: " << vao << std::endl;
        }
    }

    static void unbind()
    {
        glBindVertexArray(0);
    }

    GLuint getId() const
    {
        return vao;
    }
};

}

#endif
