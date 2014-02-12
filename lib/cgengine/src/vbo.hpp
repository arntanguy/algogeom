#ifndef __vbo__
#define __vbo__

#include <GL/glew.h>
#include <GL/gl.h>
#include <list>
#include <vector>
#include <cstring>
#include <iostream>

/**
 * XXX: is it possible to do it otherwise?
 * Defines a macro setting up the templates needed for infinite parameter list
 * setup
 **/
#define INFINITE_INITIALIZER \
void create_vbo_final(void* data, size_t size) \
{ \
glBindBuffer(mType, mBuffer); \
glBufferData(mType, size, data, GL_STATIC_DRAW); \
glBindBuffer(mType, 0); \
} \
void create_vbo_tmp(std::vector<const void*>&) {} \
template<typename T, typename... Args> \
void create_vbo_tmp(std::vector<const void*>& v, const \
                    std::vector<T>& vect, int size, const Args&... args) \
{ \
    v.push_back(vect.data()); \
    mSizes.push_back(vect.size()*sizeof(T)); \
    mDimensions.push_back(size);\
create_vbo_tmp(v, args...); \
} \
template<typename... Args> \
void create_vbo(const Args&... args) \
{ \
    std::vector<const void*> v; \
    create_vbo_tmp(v, args...); \
    size_t size(0); \
    for (const auto& a : mSizes) \
        size += a; \
    unsigned char* data = new unsigned char[size]; \
    unsigned char* ptr_data = data; \
    auto ptr_v = v.data(); \
    for (auto size : mSizes) \
    { \
        memcpy(ptr_data, *ptr_v, size); \
        ptr_data += size; \
        ++ptr_v; \
    } \
    create_vbo_final(data, size); \
    delete[] data; \
}


namespace cg
{

/**
 * @brief Inherits this class and redifine createVBO.
 * class YourVBO: public VBO
 * {
 *      private:
 *          INFINITE_INITIALIZER
 *      public:
 *          void createVBO() {
 *              std::vector<vec3> vertices = {};
 *              std::vector<vec3> normals = {};
 *              std::vector...
 *
 *              create_vbo(vertices, 3, normals, 3, texcoords, 2 ...);
 *          }
 *
 * };
 * Set your VBO creation inside createVBO. Create a std::vector for each uniform
 * you want to set in shader, and initialize them using create_vbo, method
 * taking an infinite number of parameters.
 * create_vbo will then concatene the data contained within all the vectors, and
 * send it to the GPU
 */
class VBO
{
protected:
    std::vector<size_t> mSizes;
    std::vector<int> mDimensions;
    GLuint mBuffer;


public:
    INFINITE_INITIALIZER
    GLuint mType;
    VBO(const GLuint type = GL_ARRAY_BUFFER);
    virtual ~VBO();
    GLuint getId() const
    {
        return mBuffer;
    }
    int getDimension(const int& i) const
    {
        return mDimensions[i];
    }
    int getSize(const int& i) const
    {
        return (i < 0) ? 0 : mSizes[0];
    }

    int getSize() const
    {
        int size = 0;
        for (auto t : mSizes)
        {
            size += t;
        }
        return size;
    }

    virtual void createVBO() {}

    void bind()
    {
        glBindBuffer(mType, mBuffer);
    }

    static void unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }


};


}


#endif
