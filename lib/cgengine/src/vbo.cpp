#include "vbo.hpp"

// For memcpy
#include <cstring>

using namespace std;

namespace cg
{

VBO::VBO(const GLuint type) : mType(type)
{
    glGenBuffers(1, &mBuffer);
}

VBO::~VBO()
{
}

}
