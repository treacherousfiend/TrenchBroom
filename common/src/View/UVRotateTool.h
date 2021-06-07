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

#include "Model/HitType.h"
#include "View/Tool.h"
#include "View/ToolController.h"

#include <memory>

namespace TrenchBroom {
    namespace Model {
        class PickResult;
    }

    namespace Renderer {
        class RenderBatch;
        class RenderContext;
    }

    namespace View {
        class DragTracker;
        class MapDocument;
        class UVViewHelper;

        class UVRotateTool : public ToolControllerBase<PickingPolicy, NoKeyPolicy, NoMousePolicy, NoMouseDragPolicy, RenderPolicy, NoDropPolicy>, public Tool {
        public:
            static const Model::HitType::Type AngleHandleHitType;
        private:
            std::weak_ptr<MapDocument> m_document;
            UVViewHelper& m_helper;
        public:
            UVRotateTool(std::weak_ptr<MapDocument> document, UVViewHelper& helper);
        private:
            Tool* doGetTool() override;
            const Tool* doGetTool() const override;

            void doPick(const InputState& inputState, Model::PickResult& pickResult) override;

            std::unique_ptr<DragTracker> acceptMouseDrag(const InputState& inputState) override;

            void doRender(const InputState& inputState, Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch) override;

            bool doCancel() override;
        };
    }
}

