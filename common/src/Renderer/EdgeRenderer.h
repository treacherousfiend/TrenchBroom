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

#include "Color.h"
#include "Renderer/IndexRangeMap.h"
#include "Renderer/Renderable.h"
#include "Renderer/VertexArray.h"

#include <memory>

namespace TrenchBroom {
    namespace Renderer {
        class BrushIndexArray;
        class BrushVertexArray;
        class RenderBatch;
        class RenderContext;

        class EdgeRenderer {
        public:
            struct Params {
                float width;
                float offset;
                bool onTop;
                bool useColor;
                Color color;
                Params(float i_width, float i_offset, bool i_onTop);
                Params(float i_width, float i_offset, bool i_onTop, const Color& i_color);
                Params(float i_width, float i_offset, bool i_onTop, bool i_useColor, const Color& i_color);
            };

            class RenderBase {
            private:
                const Params m_params;
            public:
                RenderBase(const Params& params);
                virtual ~RenderBase();
            protected:
                void renderEdges(RenderState& renderState);
            private:
                virtual void doRenderVertices(RenderState& renderState) = 0;
            };
        public:
            virtual ~EdgeRenderer();

            void render(RenderState& renderState, RenderBatch& renderBatch, float width = 1.0f, float offset = 0.0f);
            void render(RenderState& renderState, RenderBatch& renderBatch, const Color& color, float width = 1.0f, float offset = 0.0f);
            void render(RenderState& renderState, RenderBatch& renderBatch, bool useColor, const Color& color, float width = 1.0f, float offset = 0.0f);
            void renderOnTop(RenderState& renderState, RenderBatch& renderBatch, float width = 1.0f, float offset = 0.2f);
            void renderOnTop(RenderState& renderState, RenderBatch& renderBatch, const Color& color, float width = 1.0f, float offset = 0.2f);
            void renderOnTop(RenderState& renderState, RenderBatch& renderBatch, bool useColor, const Color& color, float width = 1.0f, float offset = 0.2f);
            void render(RenderState& renderState, RenderBatch& renderBatch, bool useColor, const Color& color, bool onTop, float width, float offset);
        private:
            virtual void doRender(RenderState& renderState, RenderBatch& renderBatch, const Params& params) = 0;
        };

        class DirectEdgeRenderer : public EdgeRenderer {
        private:
            class Render : public RenderBase, public DirectRenderable {
            private:
                VertexArray m_vertexArray;
                IndexRangeMap m_indexRanges;
            public:
                Render(const Params& params, VertexArray& vertexArray, IndexRangeMap& indexRanges);
            private:
                void doPrepareVertices(RenderContext& renderContext) override;
                void doRender(RenderState& renderState) override;
                void doRenderVertices(RenderState& renderState) override;
            };
        private:
            VertexArray m_vertexArray;
            IndexRangeMap m_indexRanges;
        public:
            DirectEdgeRenderer();
            DirectEdgeRenderer(const VertexArray& vertexArray, const IndexRangeMap& indexRanges);
            DirectEdgeRenderer(const VertexArray& vertexArray, PrimType primType);

            DirectEdgeRenderer(const DirectEdgeRenderer& other);
            DirectEdgeRenderer& operator=(DirectEdgeRenderer other);

            friend void swap(DirectEdgeRenderer& left, DirectEdgeRenderer& right);
        private:
            void doRender(RenderState& renderState, RenderBatch& renderBatch, const EdgeRenderer::Params& params) override;
        };

        class IndexedEdgeRenderer : public EdgeRenderer {
        private:
            class Render : public RenderBase, public IndexedRenderable {
            private:
                std::shared_ptr<BrushVertexArray> m_vertexArray;
                std::shared_ptr<BrushIndexArray> m_indexArray;
            public:
                Render(const Params& params, std::shared_ptr<BrushVertexArray> vertexArray, std::shared_ptr<BrushIndexArray> indexArray);
            private:
                void prepareVerticesAndIndices(RenderContext& renderContext) override;
                void doRender(RenderState& renderState) override;
                void doRenderVertices(RenderState& renderState) override;
            };
        private:
            std::shared_ptr<BrushVertexArray> m_vertexArray;
            std::shared_ptr<BrushIndexArray> m_indexArray;
        public:
            IndexedEdgeRenderer();
            IndexedEdgeRenderer(std::shared_ptr<BrushVertexArray> vertexArray, std::shared_ptr<BrushIndexArray> indexArray);

            IndexedEdgeRenderer(const IndexedEdgeRenderer& other);
            IndexedEdgeRenderer& operator=(IndexedEdgeRenderer other);

            friend void swap(IndexedEdgeRenderer& left, IndexedEdgeRenderer& right);
        private:
            void doRender(RenderState& renderState, RenderBatch& renderBatch, const EdgeRenderer::Params& params) override;
        };
    }
}

