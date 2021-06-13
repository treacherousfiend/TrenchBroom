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

#include "View/Tool.h"
#include "View/ToolController.h"

#include <vecmath/forward.h>
#include <vecmath/vec.h>

#include <memory>

namespace TrenchBroom {
    namespace Renderer {
        class PerspectiveCamera;
    }

    namespace View {
        class DragTracker;
        class MapDocument;

        class CameraTool3D : public ToolControllerBase<NoPickingPolicy, NoKeyPolicy, MousePolicy, NoMouseDragPolicy, NoRenderPolicy, NoDropPolicy>, public Tool {
        private:
            std::weak_ptr<MapDocument> m_document;
            Renderer::PerspectiveCamera& m_camera;
        public:
            CameraTool3D(std::weak_ptr<MapDocument> document, Renderer::PerspectiveCamera& camera);
        private:
            Tool* doGetTool() override;
            const Tool* doGetTool() const override;

            void doMouseScroll(const InputState& inputState) override;
            void doMouseUp(const InputState& inputState) override;

            std::unique_ptr<DragTracker> acceptMouseDrag(const InputState& inputState) override;

            bool doCancel() override;
        };
    }
}

