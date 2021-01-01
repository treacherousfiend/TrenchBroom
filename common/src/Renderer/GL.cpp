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

#include "GL.h"

#include "Exceptions.h"

#include <string>

namespace TrenchBroom {
    GLenum glGetEnum(const std::string& name) {
        if (name == "GL_ONE") {
            return GL_ONE;
        } else if (name == "GL_ZERO") {
            return GL_ZERO;
        } else if (name == "GL_SRC_COLOR") {
            return GL_SRC_COLOR;
        } else if (name == "GL_DST_COLOR") {
            return GL_DST_COLOR;
        } else if (name == "GL_ONE_MINUS_SRC_COLOR") {
            return GL_ONE_MINUS_SRC_COLOR;
        } else if (name == "GL_ONE_MINUS_DST_COLOR") {
            return GL_ONE_MINUS_DST_COLOR;
        } else if (name == "GL_SRC_ALPHA") {
            return GL_SRC_ALPHA;
        } else if (name == "GL_DST_ALPHA") {
            return GL_DST_ALPHA;
        } else if (name == "GL_ONE_MINUS_SRC_ALPHA") {
            return GL_ONE_MINUS_SRC_ALPHA;
        } else if (name == "GL_ONE_MINUS_DST_ALPHA") {
            return GL_ONE_MINUS_DST_ALPHA;
        } else if (name == "GL_SRC_ALPHA_SATURATE") {
            return GL_SRC_ALPHA_SATURATE;
        } else {
            throw RenderException("Unknown GL enum: " + name);
        }
    }

    std::string glGetEnumName(const GLenum _enum) {
        switch (_enum) {
            case GL_ONE:
                return "GL_ONE";
            case GL_ZERO:
                return "GL_ZERO";
            case GL_SRC_COLOR:
                return "GL_SRC_COLOR";
            case GL_DST_COLOR:
                return "GL_DST_COLOR";
            case GL_ONE_MINUS_SRC_COLOR:
                return "GL_ONE_MINUS_SRC_COLOR";
            case GL_ONE_MINUS_DST_COLOR:
                return "GL_ONE_MINUS_DST_COLOR";
            case GL_SRC_ALPHA:
                return "GL_SRC_ALPHA";
            case GL_DST_ALPHA:
                return "GL_DST_ALPHA";
            case GL_ONE_MINUS_SRC_ALPHA:
                return "GL_ONE_MINUS_SRC_ALPHA";
            case GL_ONE_MINUS_DST_ALPHA:
                return "GL_ONE_MINUS_DST_ALPHA";
            case GL_SRC_ALPHA_SATURATE:
                return "GL_SRC_ALPHA_SATURATE";
            default:
                return "Unknown OpenGL enum";
        }
    }
}
