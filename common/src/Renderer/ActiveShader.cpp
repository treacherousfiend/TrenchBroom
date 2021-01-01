/*
 Copyright (C) 2010-2019 Kristian Duske

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

#include "ActiveShader.h"

#include "Renderer/RenderState.h"
#include "Renderer/ShaderManager.h"
#include "Renderer/Transformation.h"

namespace TrenchBroom {
    namespace Renderer {

        ActiveShader::ActiveShader(RenderState& renderState, const ShaderConfig& shaderConfig) :
            m_program(renderState.shaderManager().program(shaderConfig)),
            m_renderState(renderState) {
            m_program.activate(m_renderState);
            
            Transformation& transformation = m_renderState.transformation();
            m_program.set("gl_Projection", transformation.getProjection());
            m_program.set("gl_View", transformation.getView());
        }

        void ActiveShader::applyModelTransform() {
            Transformation& transformation = m_renderState.transformation();
            m_program.set("gl_Model", transformation.getModel());
        }

        ActiveShader::~ActiveShader() {
            m_program.deactivate(m_renderState);
        }
    }
}
