#ifndef __Utils__
#define __Utils__

#include "glm/glm.hpp"
#include <iostream>
#include <sstream>

#include <GL/glu.h>
#include <GL/gl.h>

//-----------------------------------------------------------------
// Print for OpenGL errors
//
// Returns 1 if an OpenGL error occurred, 0 otherwise.
//
#define glErr() printOglError(__FILE__, __LINE__)
int printOglError(const char* file, int line);

/**
 * Overload some glm stream operators
 **/
std::ostream &operator<< (std::ostream &out, const glm::vec3 &vec);
std::ostream &operator<< (std::ostream &out, const glm::mat3 &vec);
std::ostream &operator<< (std::ostream &out, const glm::vec4 &vec);
std::ostream &operator<< (std::ostream &out, const glm::mat4 &vec);

#endif
