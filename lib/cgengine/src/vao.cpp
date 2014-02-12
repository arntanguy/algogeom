#include "vao.hpp"

namespace cg
{

VAO::VAO()
{
}
VAO::~VAO()
{
    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
    }
}

void VAO::create()
{
    glGenVertexArrays(1, &vao);
}


}
