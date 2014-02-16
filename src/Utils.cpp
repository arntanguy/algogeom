#include "Utils.h"


int printOglError(const char *file, int line)
{

    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n",
			     file, line, gluErrorString(glErr));
        retCode = 1;
    }
    return retCode;
}

/**
 * Overload some glm stream operators
 **/
std::ostream &operator<< (std::ostream &out, const glm::vec3 &vec) {
    out << "{"
        << vec.x << " " << vec.y << " "<< vec.z
        << "}";

    return out;
}

std::ostream &operator<< (std::ostream &out, const glm::mat3 &vec) {
    out << "{\n"
        << "\t" << vec[0] << "\n"
        << "\t" << vec[1] << "\n"
        << "\t" << vec[2] << "\n"
        << "\n}";

    return out;
}

std::ostream &operator<< (std::ostream &out, const glm::vec4 &vec) {
    out << "{"
        << vec.x << " " << vec.y << " "<< vec.z << " " << vec.t
        << "}";

    return out;
}

std::ostream &operator<< (std::ostream &out, const glm::mat4 &vec) {
    out << "{\n"
        << "\t" << vec[0] << "\n"
        << "\t" << vec[1] << "\n"
        << "\t" << vec[2] << "\n"
        << "\t" << vec[3] << "\n"
        << "\n}";

    return out;
}
