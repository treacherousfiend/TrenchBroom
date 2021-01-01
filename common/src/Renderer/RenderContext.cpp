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

#include "RenderContext.h"

#include "Renderer/FontManager.h"
#include "Renderer/OpenGLWrapper.h"
#include "Renderer/ShaderManager.h"
#include "Renderer/VboManager.h"

namespace TrenchBroom {
    namespace Renderer {
        RenderContext::RenderContext(GLContext& context) :
        m_fontManager(nullptr),
        m_shaderManager(nullptr),
        m_vboManager(nullptr),
        m_openGLWrapper(nullptr) {
            m_openGLWrapper = new OpenGLWrapper(context);
            m_vboManager = new VboManager(*m_openGLWrapper);
            m_fontManager = new FontManager(*m_openGLWrapper);
            m_shaderManager = new ShaderManager(*m_openGLWrapper);
        }

        RenderContext::~RenderContext() {
            delete m_shaderManager;
            delete m_fontManager;
            delete m_vboManager;
            delete m_openGLWrapper;
        }

        FontManager& RenderContext::fontManager() {
            return *m_fontManager;
        }

        ShaderManager& RenderContext::shaderManager() {
            return *m_shaderManager;
        }

        VboManager& RenderContext::vboManager() {
            return *m_vboManager;
        }

        OpenGLWrapper& RenderContext::gl() {
            return *m_openGLWrapper;
        }
    }
}
