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

#pragma once

#include <string>
#include <vector>

#include <QGL>
#include <QOpenGLContext>

namespace TrenchBroom {
    namespace Renderer {
        using GLIndices = std::vector<GLint>;
        using GLCounts = std::vector<GLsizei>;
        using GLContext = QOpenGLContext;

        GLenum glGetEnum(const std::string& name);
        std::string glGetEnumName(GLenum _enum);

        template <GLenum T> struct GLType               { using Type = GLvoid;   };
        template <> struct GLType<GL_BYTE>              { using Type = GLbyte;   };
        template <> struct GLType<GL_UNSIGNED_BYTE>     { using Type = GLubyte;  };
        template <> struct GLType<GL_SHORT>             { using Type = GLshort;  };
        template <> struct GLType<GL_UNSIGNED_SHORT>    { using Type = GLushort; };
        template <> struct GLType<GL_INT>               { using Type = GLint;    };
        template <> struct GLType<GL_UNSIGNED_INT>      { using Type = GLuint;   };
        template <> struct GLType<GL_FLOAT>             { using Type = GLfloat;  };

        template <typename T> struct GLEnum { static const GLenum Value = GL_INVALID_ENUM; };
        template <> struct GLEnum<GLbyte>   { static const GLenum Value = GL_BYTE; };
        template <> struct GLEnum<GLubyte>  { static const GLenum Value = GL_UNSIGNED_BYTE; };
        template <> struct GLEnum<GLshort>  { static const GLenum Value = GL_SHORT; };
        template <> struct GLEnum<GLushort> { static const GLenum Value = GL_UNSIGNED_SHORT; };
        template <> struct GLEnum<GLint>    { static const GLenum Value = GL_INT; };
        template <> struct GLEnum<GLuint>   { static const GLenum Value = GL_UNSIGNED_INT; };
        template <> struct GLEnum<GLfloat>  { static const GLenum Value = GL_FLOAT; };

        template <typename T> GLenum glType() { return GLEnum<T>::Value; }
    }
}
