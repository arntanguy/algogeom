/******************************************************************************
 Copyright (C) 2012-2013  TANGUY Arnaud arn.tanguy@gmail.com
  && FAVREAU Jean-Dominique
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

#include <GL/glew.h>
#include "shader.hpp"

using namespace std;

namespace cg
{

Shader::Shader() : BaseShader()
{
}

void Shader::setUniform(const std::string &uniformVarName,
                        const glm::vec2& vector)
{
    glUniform2f(getVariableId(uniformVarName), vector.x, vector.y);
}
void Shader::setUniform(const std::string &uniformVarName,
                        const glm::vec3& vector)
{
    glUniform3f(getVariableId(uniformVarName), vector.x, vector.y, vector.z);
}

void Shader::setUniform(const std::string &uniformVarName, const glm::vec4& vector)
{
    glUniform4f(getVariableId(uniformVarName), vector.x, vector.y, vector.z, vector.t);
}

void Shader::setUniform(const std::string &uniformVarName, const glm::mat4& mat)
{
    glUniformMatrix4fv(getVariableId(uniformVarName), 1, GL_FALSE,
                       glm::value_ptr(mat));
}

}
