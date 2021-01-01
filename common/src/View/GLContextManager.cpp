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

#include "GLContextManager.h"

#include "Exceptions.h"
#include "Renderer/OpenGLWrapper.h"
#include "Renderer/RenderContext.h"

#include <sstream>
#include <string>

namespace TrenchBroom {
    namespace View {
        std::string GLContextManager::GLVendor = "unknown";
        std::string GLContextManager::GLRenderer = "unknown";
        std::string GLContextManager::GLVersion = "unknown";

        GLContextManager::GLContextManager(Renderer::GLContext& context) :
        m_initialized(false) {
            m_renderContext = std::make_unique<Renderer::RenderContext>(context);
        }

        GLContextManager::~GLContextManager() = default;

        bool GLContextManager::initialized() const {
            return m_initialized;
        }

        bool GLContextManager::initialize() {
            if (!m_initialized) {
                m_renderContext->gl().initialize();

                GLVendor   = reinterpret_cast<const char*>(m_renderContext->gl().glGetString(GL_VENDOR));
                GLRenderer = reinterpret_cast<const char*>(m_renderContext->gl().glGetString(GL_RENDERER));
                GLVersion  = reinterpret_cast<const char*>(m_renderContext->gl().glGetString(GL_VERSION));

                m_initialized = true;
                return true;
            }
            return false;
        }

        Renderer::RenderContext& GLContextManager::renderContext() {
            return *m_renderContext;
        }
    }
}
