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

#include "Shader.h"

#include "Exceptions.h"
#include "IO/IOUtils.h"
#include "IO/Path.h"
#include "Renderer/OpenGLWrapper.h"
#include "Renderer/RenderContext.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace TrenchBroom {
    namespace Renderer {
        Shader::Shader(OpenGLWrapper& openGLWrapper, const IO::Path& path, const GLenum type) :
        m_name(path.lastComponent().asString()),
        m_type(type),
        m_shaderId(0),
        m_openGLWrapper(openGLWrapper) {
            assert(m_type == GL_VERTEX_SHADER || m_type == GL_FRAGMENT_SHADER);
            m_shaderId = m_openGLWrapper.glCreateShader(m_type);

            if (m_shaderId == 0)
                throw RenderException("Could not create shader " + m_name);

            const std::vector<std::string> source = loadSource(path);
            const char** linePtrs = new const char*[source.size()];
            for (size_t i = 0; i < source.size(); i++)
                linePtrs[i] = source[i].c_str();

            m_openGLWrapper.glShaderSource(m_shaderId, static_cast<GLsizei>(source.size()), linePtrs, nullptr);
            delete[] linePtrs;

            m_openGLWrapper.glCompileShader(m_shaderId);
            GLint compileStatus;
            m_openGLWrapper.glGetShaderiv(m_shaderId, GL_COMPILE_STATUS, &compileStatus);

            if (compileStatus == 0) {
                auto str = std::stringstream();
                str << "Could not compile shader " << m_name << ": ";

                GLint infoLogLength;
                m_openGLWrapper.glGetShaderiv(m_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
                if (infoLogLength > 0) {
                    char* infoLog = new char[static_cast<size_t>(infoLogLength)];
                    m_openGLWrapper.glGetShaderInfoLog(m_shaderId, infoLogLength, &infoLogLength, infoLog);
                    infoLog[infoLogLength-1] = 0;

                    str << infoLog;
                    delete [] infoLog;
                } else {
                    str << "Unknown error";
                }

                throw RenderException(str.str());
            }
        }

        Shader::~Shader() {
            if (m_shaderId != 0) {
                m_openGLWrapper.glDeleteShader(m_shaderId);
                m_shaderId = 0;
            }
        }

        void Shader::attach(const GLuint programId) {
            m_openGLWrapper.glAttachShader(programId, m_shaderId);
        }

        void Shader::detach(const GLuint programId) {
            m_openGLWrapper.glDetachShader(programId, m_shaderId);
        }

        std::vector<std::string> Shader::loadSource(const IO::Path& path) {
            std::ifstream stream = openPathAsInputStream(path);
            if (!stream.is_open()) {
                throw RenderException("Could not load shader source from " + path.asString());
            }

            std::string line;
            std::vector<std::string> lines;

            while (!stream.eof()) {
                std::getline(stream, line);
                lines.push_back(line + '\n');
            }

            return lines;
        }
    }
}
