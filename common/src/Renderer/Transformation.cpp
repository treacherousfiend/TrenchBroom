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

#include "Transformation.h"

#include "Renderer/GL.h"

#include <vecmath/forward.h>
#include <vecmath/mat.h>

#include <cassert>

namespace TrenchBroom {
    namespace Renderer {
        Transformation::Transformation() {}

        Transformation Transformation::slice() const {
            Transformation transformation;
            transformation.pushTransformation(m_projectionStack.back(), m_viewStack.back(), m_modelStack.back());
            return transformation;
        }

        const vm::mat4x4f& Transformation::getProjection() {
            assert(m_projectionStack.size() > 1);
            return m_projectionStack.back();
        }

        const vm::mat4x4f& Transformation::getView() {
            assert(m_viewStack.size() > 1);
            return m_viewStack.back();
        }

        const vm::mat4x4f& Transformation::getModel() {
            assert(m_modelStack.size() > 1);
            return m_modelStack.back();
        }

        void Transformation::pushTransformation(const vm::mat4x4f& projection, const vm::mat4x4f& view, const vm::mat4x4f& model) {
            m_projectionStack.push_back(projection);
            m_viewStack.push_back(view);
            m_modelStack.push_back(model);
        }

        void Transformation::popTransformation() {
            assert(m_projectionStack.size() > 1);
            assert(m_viewStack.size() > 1);
            assert(m_modelStack.size() > 1);

            m_projectionStack.pop_back();
            m_viewStack.pop_back();
            m_modelStack.pop_back();
        }

        void Transformation::pushModelMatrix(const vm::mat4x4f& matrix) {
            m_modelStack.push_back(m_modelStack.back() * matrix);
        }

        void Transformation::replaceAndPushModelMatrix(const vm::mat4x4f& matrix) {
            m_modelStack.push_back(matrix);
        }

        void Transformation::popModelMatrix() {
            assert(m_modelStack.size() > 1);
            m_modelStack.pop_back();
        }

        ReplaceTransformation::ReplaceTransformation(Transformation& transformation, const vm::mat4x4f& projectionMatrix, const vm::mat4x4f& viewMatrix, const vm::mat4x4f& modelMatrix) :
        m_transformation(transformation) {
            m_transformation.pushTransformation(projectionMatrix, viewMatrix, modelMatrix);
        }

        ReplaceTransformation::~ReplaceTransformation() {
            m_transformation.popTransformation();
        }

        MultiplyModelMatrix::MultiplyModelMatrix(Transformation& transformation, const vm::mat4x4f& modelMatrix) :
        m_transformation(transformation) {
            m_transformation.pushModelMatrix(modelMatrix);
        }

        MultiplyModelMatrix::~MultiplyModelMatrix() {
            m_transformation.popModelMatrix();
        }

        ReplaceModelMatrix::ReplaceModelMatrix(Transformation& transformation, const vm::mat4x4f& modelMatrix) :
        m_transformation(transformation) {
            m_transformation.replaceAndPushModelMatrix(modelMatrix);
        }

        ReplaceModelMatrix::~ReplaceModelMatrix() {
            m_transformation.popModelMatrix();
        }
    }
}
