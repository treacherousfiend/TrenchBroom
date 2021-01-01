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

#include "TextRenderer.h"

#include "AttrString.h"
#include "Renderer/ActiveShader.h"
#include "Renderer/Camera.h"
#include "Renderer/FontManager.h"
#include "Renderer/PrimType.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderState.h"
#include "Renderer/RenderUtils.h"
#include "Renderer/ShaderManager.h"
#include "Renderer/Shaders.h"
#include "Renderer/TextAnchor.h"
#include "Renderer/TextureFont.h"

#include <vecmath/forward.h>
#include <vecmath/vec.h>
#include <vecmath/mat_ext.h>

namespace TrenchBroom {
    namespace Renderer {
        const float TextRenderer::DefaultMaxViewDistance = 768.0f;
        const float TextRenderer::DefaultMinZoomFactor = 0.5f;
        const vm::vec2f TextRenderer::DefaultInset = vm::vec2f(4.0f, 4.0f);
        const size_t TextRenderer::RectCornerSegments = 3;
        const float TextRenderer::RectCornerRadius = 3.0f;

        TextRenderer::Entry::Entry(std::vector<vm::vec2f>& i_vertices, const vm::vec2f& i_size, const vm::vec3f& i_offset, const Color& i_textColor, const Color& i_backgroundColor) :
        size(i_size),
        offset(i_offset),
        textColor(i_textColor),
        backgroundColor(i_backgroundColor) {
            using std::swap;
            swap(vertices, i_vertices);
        }

        TextRenderer::EntryCollection::EntryCollection() :
        textVertexCount(0),
        rectVertexCount(0) {}

        TextRenderer::TextRenderer(const FontDescriptor& fontDescriptor, const float maxViewDistance, const float minZoomFactor, const vm::vec2f& inset) :
        m_fontDescriptor(fontDescriptor),
        m_maxViewDistance(maxViewDistance),
        m_minZoomFactor(minZoomFactor),
        m_inset(inset) {}

        void TextRenderer::renderString(RenderState& renderState, const Color& textColor, const Color& backgroundColor, const AttrString& string, const TextAnchor& position) {
            renderString(renderState, textColor, backgroundColor, string, position, false);
        }

        void TextRenderer::renderStringOnTop(RenderState& renderState, const Color& textColor, const Color& backgroundColor, const AttrString& string, const TextAnchor& position) {
            renderString(renderState, textColor, backgroundColor, string, position, true);
        }

        void TextRenderer::renderString(RenderState& renderState, const Color& textColor, const Color& backgroundColor, const AttrString& string, const TextAnchor& position, const bool onTop) {

            const Camera& camera = renderState.camera();
            const float distance = camera.perpendicularDistanceTo(position.position(camera));
            if (distance <= 0.0f)
                return;

            if (!isVisible(renderState, string, position, distance, onTop))
                return;

            FontManager& fontManager = renderState.fontManager();
            TextureFont& font = fontManager.font(m_fontDescriptor);

            std::vector<vm::vec2f> vertices = font.quads(string, true);
            const float alphaFactor = computeAlphaFactor(renderState, distance, onTop);
            const vm::vec2f size = font.measure(string);
            const vm::vec3f offset = position.offset(camera, size);

            if (onTop)
                addEntry(m_entriesOnTop, Entry(vertices, size, offset,
                                               Color(textColor, alphaFactor * textColor.a()),
                                               Color(backgroundColor, alphaFactor * backgroundColor.a())));
            else
                addEntry(m_entries, Entry(vertices, size, offset,
                                          Color(textColor, alphaFactor * textColor.a()),
                                          Color(backgroundColor, alphaFactor * backgroundColor.a())));
        }

        bool TextRenderer::isVisible(RenderState& renderState, const AttrString& string, const TextAnchor& position, const float distance, const bool onTop) const {
            if (!onTop) {
                if (renderState.render3D() && distance > m_maxViewDistance)
                    return false;
                if (renderState.render2D() && renderState.camera().zoom() < m_minZoomFactor)
                    return false;
            }

            const Camera& camera = renderState.camera();
            const Camera::Viewport& viewport = camera.viewport();

            const vm::vec2f size = stringSize(renderState, string);
            const vm::vec2f offset = vm::vec2f(position.offset(camera, size)) - m_inset;
            const vm::vec2f actualSize = size + 2.0f * m_inset;

            return viewport.contains(offset.x(), offset.y(), actualSize.x(), actualSize.y());
        }

        float TextRenderer::computeAlphaFactor(const RenderState& renderState, const float distance, const bool onTop) const {
            if (onTop)
                return 1.0f;

            if (renderState.render3D()) {
                const float a = m_maxViewDistance - distance;
                if (a > 128.0f)
                    return 1.0f;
                return a / 128.0f;
            } else {
                const float z = renderState.camera().zoom();
                const float d = z - m_minZoomFactor;
                if (d > 0.3f)
                    return 1.0f;
                return d / 0.3f;
            }
        }

        void TextRenderer::addEntry(EntryCollection& collection, const Entry& entry) {
            collection.entries.push_back(entry);
            collection.textVertexCount += entry.vertices.size();
            collection.rectVertexCount += roundedRect2DVertexCount(RectCornerSegments);
        }

        vm::vec2f TextRenderer::stringSize(RenderState& renderState, const AttrString& string) const {
            FontManager& fontManager = renderState.fontManager();
            TextureFont& font = fontManager.font(m_fontDescriptor);
            return round(font.measure(string));
        }

        void TextRenderer::doPrepareVertices(RenderContext& renderContext) {
            prepare(m_entries, false, renderContext);
            prepare(m_entriesOnTop, true, renderContext);
        }

        void TextRenderer::prepare(EntryCollection& collection, const bool onTop, RenderContext& renderContext) {
            std::vector<TextVertex> textVertices;
            textVertices.reserve(collection.textVertexCount);

            std::vector<RectVertex> rectVertices;
            rectVertices.reserve(collection.rectVertexCount);

            for (const Entry& entry : collection.entries) {
                addEntry(entry, onTop, textVertices, rectVertices);
            }

            collection.textArray = VertexArray::move(std::move(textVertices));
            collection.rectArray = VertexArray::move(std::move(rectVertices));

            collection.textArray.prepare(renderContext);
            collection.rectArray.prepare(renderContext);
        }

        void TextRenderer::addEntry(const Entry& entry, const bool /* onTop */, std::vector<TextVertex>& textVertices, std::vector<RectVertex>& rectVertices) {
            const std::vector<vm::vec2f>& stringVertices = entry.vertices;
            const vm::vec2f& stringSize = entry.size;

            const vm::vec3f& offset = entry.offset;

            const Color& textColor = entry.textColor;
            const Color& rectColor = entry.backgroundColor;

            for (size_t i = 0; i < stringVertices.size() / 2; ++i) {
                const vm::vec2f& position2 = stringVertices[2 * i];
                const vm::vec2f& texCoords = stringVertices[2 * i + 1];
                textVertices.emplace_back(vm::vec3f(position2 + offset.xy(), -offset.z()), texCoords, textColor);
            }

            const std::vector<vm::vec2f> rect = roundedRect2D(stringSize + 2.0f * m_inset, RectCornerRadius, RectCornerSegments);
            for (size_t i = 0; i < rect.size(); ++i) {
                const vm::vec2f& vertex = rect[i];
                rectVertices.emplace_back(vm::vec3f(vertex + offset.xy() + stringSize / 2.0f, -offset.z()), rectColor);
            }
        }

        void TextRenderer::doRender(RenderState& renderState) {
            const Camera::Viewport& viewport = renderState.camera().viewport();
            const vm::mat4x4f projection = vm::ortho_matrix(
                0.0f, 1.0f,
                static_cast<float>(viewport.x),
                static_cast<float>(viewport.height),
                static_cast<float>(viewport.width),
                static_cast<float>(viewport.y));
            const vm::mat4x4f view = vm::view_matrix(vm::vec3f::neg_z(), vm::vec3f::pos_y());
            ReplaceTransformation ortho(renderState.transformation(), projection, view);

            render(m_entries, renderState);

            renderState.gl().glDisable(GL_DEPTH_TEST);
            render(m_entriesOnTop, renderState);
            renderState.gl().glEnable(GL_DEPTH_TEST);
        }

        void TextRenderer::render(EntryCollection& collection, RenderState& renderState) {
            FontManager& fontManager = renderState.fontManager();
            TextureFont& font = fontManager.font(m_fontDescriptor);

            renderState.gl().glDisable(GL_TEXTURE_2D);

            ActiveShader backgroundShader(renderState, Shaders::TextBackgroundShader);
            collection.rectArray.render(renderState, PrimType::Triangles);

            renderState.gl().glEnable(GL_TEXTURE_2D);

            ActiveShader textShader(renderState, Shaders::ColoredTextShader);
            textShader.set("Texture", 0);
            font.activate();
            collection.textArray.render(renderState, PrimType::Quads);
            font.deactivate();
        }
    }
}
