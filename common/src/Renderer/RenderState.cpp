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

#include "Renderer/Camera.h"
#include "Renderer/RenderContext.h"

namespace TrenchBroom {
    namespace Renderer {
        RenderState::RenderState(const RenderMode renderMode, const Camera& camera, RenderContext& renderContext) :
        m_renderMode(renderMode),
        m_camera(camera),
        m_transformation(*this),
        m_renderContext(renderContext),
        m_showTextures(true),
        m_showFaces(true),
        m_showEdges(true),
        m_shadeFaces(true),
        m_showPointEntities(true),
        m_showPointEntityModels(true),
        m_showEntityClassnames(true),
        m_showGroupBounds(true),
        m_showBrushEntityBounds(true),
        m_showPointEntityBounds(true),
        m_showFog(false),
        m_showGrid(true),
        m_gridSize(4),
        m_hideSelection(false),
        m_tintSelection(true),
        m_showSelectionGuide(ShowSelectionGuide::Hide) {
            m_transformation.pushTransformation(m_camera.projectionMatrix(), m_camera.viewMatrix());
        }

        bool RenderState::render2D() const {
            return m_renderMode == RenderMode::Render2D;
        }

        bool RenderState::render3D() const {
            return m_renderMode == RenderMode::Render3D;
        }

        const Camera& RenderState::camera() const {
            return m_camera;
        }

        Transformation& RenderState::transformation() {
            return m_transformation;
        }

        RenderContext& RenderState::renderContext() {
            return m_renderContext;
        }

        FontManager& RenderState::fontManager() {
            return m_renderContext.fontManager();
        }

        ShaderManager& RenderState::shaderManager() {
            return m_renderContext.shaderManager();
        }

        VboManager& RenderState::vboManager() {
            return m_renderContext.vboManager();
        }

        OpenGLWrapper& RenderState::gl() {
            return m_renderContext.openGLWrapper();
        }

        bool RenderState::showTextures() const {
            return m_showTextures;
        }

        void RenderState::setShowTextures(const bool showTextures) {
            m_showTextures = showTextures;
        }

        bool RenderState::showFaces() const {
            return m_renderMode == RenderMode::Render3D && m_showFaces;
        }

        void RenderState::setShowFaces(const bool showFaces) {
            m_showFaces = showFaces;
        }

        bool RenderState::showEdges() const {
            return m_renderMode == RenderMode::Render2D || m_showEdges;
        }

        void RenderState::setShowEdges(const bool showEdges) {
            m_showEdges = showEdges;
        }

        bool RenderState::shadeFaces() const {
            return m_shadeFaces;
        }

        void RenderState::setShadeFaces(const bool shadeFaces) {
            m_shadeFaces = shadeFaces;
        }

        bool RenderState::showPointEntities() const {
            return m_showPointEntities;
        }

        void RenderState::setShowPointEntities(const bool showPointEntities) {
            m_showPointEntities = showPointEntities;
        }

        bool RenderState::showPointEntityModels() const {
            return m_showPointEntityModels;
        }

        void RenderState::setShowPointEntityModels(const bool showPointEntityModels) {
            m_showPointEntityModels = showPointEntityModels;
        }

        bool RenderState::showEntityClassnames() const {
            return m_showEntityClassnames;
        }

        void RenderState::setShowEntityClassnames(const bool showEntityClassnames) {
            m_showEntityClassnames = showEntityClassnames;
        }

        bool RenderState::showGroupBounds() const {
            return m_showGroupBounds;
        }

        void RenderState::setShowGroupBounds(const bool showGroupBounds) {
            m_showGroupBounds = showGroupBounds;
        }

        bool RenderState::showBrushEntityBounds() const {
            return m_showBrushEntityBounds;
        }

        void RenderState::setShowBrushEntityBounds(const bool showBrushEntityBounds) {
            m_showBrushEntityBounds = showBrushEntityBounds;
        }

        bool RenderState::showPointEntityBounds() const {
            return m_showPointEntityBounds;
        }

        void RenderState::setShowPointEntityBounds(const bool showPointEntityBounds) {
            m_showPointEntityBounds = showPointEntityBounds;
        }

        bool RenderState::showFog() const {
            return m_showFog;
        }

        void RenderState::setShowFog(const bool showFog) {
            m_showFog = showFog;
        }

        bool RenderState::showGrid() const {
            return m_showGrid;
        }

        void RenderState::setShowGrid(const bool showGrid) {
            m_showGrid = showGrid;
        }

        FloatType RenderState::gridSize() const {
            return m_gridSize;
        }

        void RenderState::setGridSize(const FloatType gridSize) {
            m_gridSize = gridSize;
        }

        const vm::bbox3f& RenderState::softMapBounds() const {
            return m_sofMapBounds;
        }

        void RenderState::setSoftMapBounds(const vm::bbox3f& softMapBounds) {
            m_sofMapBounds = softMapBounds;
        }

        bool RenderState::hideSelection() const {
            return m_hideSelection;
        }

        void RenderState::setHideSelection() {
            m_hideSelection = true;
        }

        bool RenderState::tintSelection() const {
            return m_tintSelection;
        }

        void RenderState::clearTintSelection() {
            m_tintSelection = false;
        }

        bool RenderState::showSelectionGuide() const {
            return m_showSelectionGuide == ShowSelectionGuide::Show || m_showSelectionGuide == ShowSelectionGuide::ForceShow;
        }

        void RenderState::setShowSelectionGuide() {
            setShowSelectionGuide(ShowSelectionGuide::Show);
        }

        void RenderState::setHideSelectionGuide() {
            setShowSelectionGuide(ShowSelectionGuide::Hide);
        }

        void RenderState::setForceShowSelectionGuide() {
            setShowSelectionGuide(ShowSelectionGuide::ForceShow);
        }

        void RenderState::setForceHideSelectionGuide() {
            setShowSelectionGuide(ShowSelectionGuide::ForceHide);
        }

        void RenderState::setShowSelectionGuide(const ShowSelectionGuide showSelectionGuide) {
            switch (showSelectionGuide) {
                case ShowSelectionGuide::Show:
                    if (m_showSelectionGuide == ShowSelectionGuide::Hide)
                        m_showSelectionGuide = ShowSelectionGuide::Show;
                    break;
                case ShowSelectionGuide::Hide:
                    if (m_showSelectionGuide == ShowSelectionGuide::Show)
                        m_showSelectionGuide = ShowSelectionGuide::Hide;
                    break;
                case ShowSelectionGuide::ForceShow:
                    m_showSelectionGuide = ShowSelectionGuide::ForceShow;
                    break;
                case ShowSelectionGuide::ForceHide:
                    if (m_showSelectionGuide != ShowSelectionGuide::ForceShow)
                        m_showSelectionGuide = ShowSelectionGuide::ForceHide;
                    break;
            }
        }
    }
}
