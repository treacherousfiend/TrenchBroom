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
#include "Renderer/FontDescriptor.h"
#include "Renderer/Renderable.h"
#include "Renderer/VertexArray.h"
#include "Renderer/GLVertexType.h"

#include <vecmath/forward.h>
#include <vecmath/vec.h>

#include <vector>

namespace TrenchBroom {
    namespace Renderer {
        class AttrString;
        class RenderContext;
        class RenderState;
        class TextAnchor;

        class TextRenderer : public DirectRenderable {
        private:
            static const float DefaultMaxViewDistance;
            static const float DefaultMinZoomFactor;
            static const vm::vec2f DefaultInset;
            static const size_t RectCornerSegments;
            static const float RectCornerRadius;

            struct Entry {
                std::vector<vm::vec2f> vertices;
                vm::vec2f size;
                vm::vec3f offset;
                Color textColor;
                Color backgroundColor;

                Entry(std::vector<vm::vec2f>& i_vertices, const vm::vec2f& i_size, const vm::vec3f& i_offset, const Color& i_textColor, const Color& i_backgroundColor);
            };

            using EntryList = std::vector<Entry>;

            struct EntryCollection {
                EntryList entries;
                size_t textVertexCount;
                size_t rectVertexCount;

                VertexArray textArray;
                VertexArray rectArray;

                EntryCollection();
            };

            using TextVertex = GLVertexTypes::P3T2C4::Vertex;
            using RectVertex = GLVertexTypes::P3C4::Vertex;

            FontDescriptor m_fontDescriptor;
            float m_maxViewDistance;
            float m_minZoomFactor;
            vm::vec2f m_inset;

            EntryCollection m_entries;
            EntryCollection m_entriesOnTop;
        public:
            explicit TextRenderer(const FontDescriptor& fontDescriptor, float maxViewDistance = DefaultMaxViewDistance, float minZoomFactor = DefaultMinZoomFactor, const vm::vec2f& inset = DefaultInset);

            void renderString(RenderState& renderState, const Color& textColor, const Color& backgroundColor, const AttrString& string, const TextAnchor& position);
            void renderStringOnTop(RenderState& renderState, const Color& textColor, const Color& backgroundColor, const AttrString& string, const TextAnchor& position);
        private:
            void renderString(RenderState& renderState, const Color& textColor, const Color& backgroundColor, const AttrString& string, const TextAnchor& position, bool onTop);

            bool isVisible(RenderState& renderState, const AttrString& string, const TextAnchor& position, float distance, bool onTop) const;
            float computeAlphaFactor(const RenderState& renderState, float distance, bool onTop) const;
            void addEntry(EntryCollection& collection, const Entry& entry);

            vm::vec2f stringSize(RenderState& renderState, const AttrString& string) const;
        private:
            void doPrepareVertices(RenderContext& renderContext) override;
            void prepare(EntryCollection& collection, bool onTop, RenderContext& renderContext);

            void addEntry(const Entry& entry, bool onTop, std::vector<TextVertex>& textVertices, std::vector<RectVertex>& rectVertices);

            void doRender(RenderState& renderState) override;
            void render(EntryCollection& collection, RenderState& renderState);

            void clear();
        };
    }
}

