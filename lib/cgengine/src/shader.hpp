/******************************************************************************
 Copyright (C) 2012-2013  TANGUY Arnaud arn.tanguy@gmail.com
 &&  FAVREAU Jean-Dominique
 *                                                                             *
 * This program is free software; you can redistribute it and/or modify        *
 * it under the terms of the GNU General Public License as published by        *
 * the Free Software Foundation; either version 2 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
 * GNU General Public License for more details.                                *
 *                                                                             *
 * You should have received a copy of the GNU General Public License along     *
 * with this program; if not, write to the Free Software Foundation, Inc.,     *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                 *
 ******************************************************************************/

/**
 * Inspired by
 * http://content.gpwiki.org/index.php/OpenGL:Tutorials:Loading_and_using_GLSL_shaders
 **/

#ifndef __Shader__
#define __Shader__

#include <memory>
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include "include_glm.hpp"
#include "baseshader.hpp"


namespace cg
{

class BaseTexture;

/**
 * @brief Provides an easy way to load, enable and disable shaders (vertex,
 * geometry, fragment)
 * Uses the glew library, don't forget to initialize it!
 * This class only depends on Glew and Opengl
 */
class Shader : public BaseShader
{
public:
    Shader();
    void setUniform(const std::string &uniformVarName, const glm::vec2& vector);
    void setUniform(const std::string &uniformVarName, const glm::vec3& vector);
    void setUniform(const std::string &uniformVarName, const glm::vec4& vector);
    void setUniform(const std::string &uniformVarName, const glm::mat4& mat);

};


}

#endif
