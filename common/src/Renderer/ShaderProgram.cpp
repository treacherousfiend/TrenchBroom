/*
 Copyright (C) 2010-2017 Kristian Duske

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ShaderProgram.h"

#include "Exceptions.h"
#include "Renderer/OpenGLWrapper.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderState.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderManager.h"

#include <vecmath/forward.h>
#include <vecmath/vec.h>
#include <vecmath/mat.h>

#include <memory>
#include <string>
#include <sstream>

namespace TrenchBroom {
    namespace Renderer {
        ShaderProgram::ShaderProgram(OpenGLWrapper& openGLWrapper, const std::string& name) :
        m_name(name),
        m_programId(openGLWrapper.glCreateProgram()),
        m_needsLinking(true),
        m_openGLWrapper(openGLWrapper) {
            if (m_programId == 0) {
                throw RenderException("Cannot create shader program " + m_name);
            }
        }

        ShaderProgram::~ShaderProgram() {
           if (m_programId != 0) {
                m_openGLWrapper.glDeleteProgram(m_programId);
                m_programId = 0;
            }
        }

        void ShaderProgram::attach(Shader& shader) {
            assert(m_programId != 0);
            shader.attach(m_programId);
            m_needsLinking = true;
        }

        void ShaderProgram::detach(Shader& shader) {
            assert(m_programId != 0);
            shader.detach(m_programId);
            m_needsLinking = true;
        }

        void ShaderProgram::activate(RenderState& renderState) {
            assert(m_programId != 0);

            if (m_needsLinking)
                link();

            m_openGLWrapper.glUseProgram(m_programId);
            assert(checkActive());

            renderState.shaderManager().setCurrentProgram(this);
        }

        void ShaderProgram::deactivate(RenderState& renderState) {
            m_openGLWrapper.glUseProgram(0);

            renderState.shaderManager().setCurrentProgram(nullptr);
        }

        void ShaderProgram::enableAttribute(const std::string& name, size_t size, GLenum type, bool normalized, const size_t stride, const size_t offset) {
            const GLint attributeIndex = findAttributeLocation(name);
            m_openGLWrapper.glEnableVertexAttribArray(static_cast<GLuint>(attributeIndex));
            m_openGLWrapper.glVertexAttribPointer(static_cast<GLuint>(attributeIndex), static_cast<GLint>(size), type, normalized, static_cast<GLsizei>(stride), reinterpret_cast<GLvoid*>(offset));
        }

        void ShaderProgram::disableAttribute(const std::string& name) {
            const GLint attributeIndex = findAttributeLocation(name);
            m_openGLWrapper.glDisableVertexAttribArray(static_cast<GLuint>(attributeIndex));
        }

        void ShaderProgram::set(const std::string& name, const bool value) {
            return set(name, static_cast<int>(value));
        }

        void ShaderProgram::set(const std::string& name, const int value) {
            assert(checkActive());
            m_openGLWrapper.glUniform1i(findUniformLocation(name), value);
        }

        void ShaderProgram::set(const std::string& name, const size_t value) {
            assert(checkActive());
            m_openGLWrapper.glUniform1i(findUniformLocation(name), static_cast<int>(value));
        }

        void ShaderProgram::set(const std::string& name, const float value) {
            assert(checkActive());
            m_openGLWrapper.glUniform1f(findUniformLocation(name), value);
        }

        void ShaderProgram::set(const std::string& name, const vm::vec2f& value) {
            assert(checkActive());
            m_openGLWrapper.glUniform2f(findUniformLocation(name), value.x(), value.y());
        }

        void ShaderProgram::set(const std::string& name, const vm::vec3f& value) {
            assert(checkActive());
            m_openGLWrapper.glUniform3f(findUniformLocation(name), value.x(), value.y(), value.z());
        }

        void ShaderProgram::set(const std::string& name, const vm::vec4f& value) {
            assert(checkActive());
            m_openGLWrapper.glUniform4f(findUniformLocation(name), value.x(), value.y(), value.z(), value.w());
        }

        void ShaderProgram::set(const std::string& name, const vm::mat2x2f& value) {
            assert(checkActive());
            m_openGLWrapper.glUniformMatrix2fv(findUniformLocation(name), 1, false, reinterpret_cast<const float*>(value.v));
        }

        void ShaderProgram::set(const std::string& name, const vm::mat3x3f& value) {
            assert(checkActive());
            m_openGLWrapper.glUniformMatrix3fv(findUniformLocation(name), 1, false, reinterpret_cast<const float*>(value.v));
        }

        void ShaderProgram::set(const std::string& name, const vm::mat4x4f& value) {
            assert(checkActive());
            m_openGLWrapper.glUniformMatrix4fv(findUniformLocation(name), 1, false, reinterpret_cast<const float*>(value.v));
        }

        void ShaderProgram::link() {
            m_openGLWrapper.glLinkProgram(m_programId);

            GLint linkStatus = 0;
            m_openGLWrapper.glGetProgramiv(m_programId, GL_LINK_STATUS, &linkStatus);

            if (linkStatus == 0) {
                auto str = std::stringstream();
                str << "Could not link shader program " << m_name << ": ";

                GLint infoLogLength = 0;
                m_openGLWrapper.glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &infoLogLength);
                if (infoLogLength > 0) {
                    auto infoLog = std::make_unique<char[]>(static_cast<size_t>(infoLogLength));
                    m_openGLWrapper.glGetProgramInfoLog(m_programId, infoLogLength, &infoLogLength, infoLog.get());
                    infoLog[static_cast<size_t>(infoLogLength-1)] = 0;

                    str << infoLog.get();
                } else {
                    str << "Unknown error";
                }

                throw RenderException(str.str());
            }

            m_variableCache.clear();
            m_needsLinking = false;
        }

        GLint ShaderProgram::findAttributeLocation(const std::string& name) const {
            auto it = m_attributeCache.find(name);
            if (it == std::end(m_attributeCache)) {
                GLint index;
                index = m_openGLWrapper.glGetAttribLocation(m_programId, name.c_str());
                if (index == -1) {
                    throw RenderException("Location of attribute '" + name + "' could not be found in shader program " + m_name);
                }

                m_attributeCache[name] = index;
                return index;
            }
            return it->second;
        }

        GLint ShaderProgram::findUniformLocation(const std::string& name) const {
            auto it = m_variableCache.find(name);
            if (it == std::end(m_variableCache)) {
                GLint index;
                index = m_openGLWrapper.glGetUniformLocation(m_programId, name.c_str());
                if (index == -1) {
                    throw RenderException("Location of uniform variable '" + name + "' could not be found in shader program " + m_name);
                }

                m_variableCache[name] = index;
                return index;
            }
            return it->second;
        }

        bool ShaderProgram::checkActive() const {
            GLint currentProgramId = -1;
            m_openGLWrapper.glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramId);
            return static_cast<GLuint>(currentProgramId) == m_programId;
        }
    }
}
