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

#include "FloatType.h"

#include <vecmath/bbox.h>

namespace TrenchBroom {
    namespace Renderer {
        class RenderBatch;
        class RenderState;

        class SelectionBoundsRenderer {
        private:
            const vm::bbox3 m_bounds;

            class SizeTextAnchor2D;
            class SizeTextAnchor3D;
            class MinMaxTextAnchor3D;
        public:
            SelectionBoundsRenderer(const vm::bbox3& bounds);

            void render(RenderState& renderState, RenderBatch& renderBatch);
        private:
            void renderBounds(RenderState& renderState, RenderBatch& renderBatch);
            void renderSize(RenderState& renderState, RenderBatch& renderBatch);
            void renderSize2D(RenderState& renderState, RenderBatch& renderBatch);
            void renderSize3D(RenderState& renderState, RenderBatch& renderBatch);
            void renderMinMax(RenderState& renderState, RenderBatch& renderBatch);
        };
    }
}

