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

#ifndef TrenchBroom_MapRenderer
#define TrenchBroom_MapRenderer

#include "Macros.h"

#include <map>
#include <memory>
#include <vector>

namespace TrenchBroom {
    class Color;

    namespace IO {
        class Path;
    }

    namespace View {
        // FIXME: Renderer should not depend on View
        class MapDocument;
        class Selection;
    }

    namespace Model {
        class EntityNode;
        class BrushNode;
        class BrushFaceHandle;
        class GroupNode;
        class LayerNode;
        class Node;
    }

    namespace Renderer {
        class EntityRenderer;
        class GroupRenderer;
        class BrushRenderer;
        class EntityLinkRenderer;
        class RenderBatch;
        class RenderContext;

        class MapRenderer {
        private:
            class SelectedBrushRendererFilter;
            class LockedBrushRendererFilter;
            class UnselectedBrushRendererFilter;

            std::weak_ptr<View::MapDocument> m_document;

            std::unique_ptr<GroupRenderer> m_groupRenderer;
            std::unique_ptr<EntityRenderer> m_entityRenderer;
            std::unique_ptr<EntityLinkRenderer> m_entityLinkRenderer;
            std::unique_ptr<BrushRenderer> m_brushRenderer;
        public:
            explicit MapRenderer(std::weak_ptr<View::MapDocument> document);
            ~MapRenderer();

            deleteCopyAndMove(MapRenderer)
        private:
            void clear();
        public: // color config
            void overrideSelectionColors(const Color& color, float mix);
            void restoreSelectionColors();
        public: // rendering
            void render(RenderContext& renderContext, RenderBatch& renderBatch);
        private:
            void commitPendingChanges();
            void setupGL(RenderBatch& renderBatch);
            // void renderDefaultOpaque(RenderContext& renderContext, RenderBatch& renderBatch);
            // void renderDefaultTransparent(RenderContext& renderContext, RenderBatch& renderBatch);
            // void renderSelectionOpaque(RenderContext& renderContext, RenderBatch& renderBatch);
            // void renderSelectionTransparent(RenderContext& renderContext, RenderBatch& renderBatch);
            // void renderLockedOpaque(RenderContext& renderContext, RenderBatch& renderBatch);
            // void renderLockedTransparent(RenderContext& renderContext, RenderBatch& renderBatch);
            void renderEntityLinks(RenderContext& renderContext, RenderBatch& renderBatch);

            void setupRenderers();
            // void setupDefaultRenderer(ObjectRenderer& renderer);
            // void setupSelectionRenderer(ObjectRenderer& renderer);
            // void setupLockedRenderer(ObjectRenderer& renderer);
            void setupEntityLinkRenderer();

            class CollectRenderableNodes;

            /**
             * This moves nodes between default / selection / locked renderers as needed,
             * but doesn't otherwise invalidate them.
             * (in particular, brushes are not updated unless they move between renderers.)
             * If brushes are modified, you need to call invalidateRenderers() or invalidateObjectsInRenderers()
             */
            void updateRenderers();
            void invalidateRenderers();
            void invalidateEntityLinkRenderer();
            void reloadEntityModels();
        private: // notification
            void bindObservers();
            void unbindObservers();

            void documentWasCleared(View::MapDocument* document);
            void documentWasNewedOrLoaded(View::MapDocument* document);

            void nodesWereAdded(const std::vector<Model::Node*>& nodes);
            void nodesWereRemoved(const std::vector<Model::Node*>& nodes);
            void nodesDidChange(const std::vector<Model::Node*>& nodes);

            void nodeVisibilityDidChange(const std::vector<Model::Node*>& nodes);
            void nodeLockingDidChange(const std::vector<Model::Node*>& nodes);

            void groupWasOpened(Model::GroupNode* group);
            void groupWasClosed(Model::GroupNode* group);

            void brushFacesDidChange(const std::vector<Model::BrushFaceHandle>& faces);

            void selectionDidChange(const View::Selection& selection);

            void textureCollectionsWillChange();
            void entityDefinitionsDidChange();
            void modsDidChange();

            void editorContextDidChange();
            void mapViewConfigDidChange();

            void preferenceDidChange(const IO::Path& path);
        private: // invalidating specific nodes
            class InvalidateNode;
            friend class InvalidateNode;

            void invalidateNodes(const std::vector<Model::Node*>& nodes);
            void invalidateBrushFaces(const std::vector<Model::BrushFaceHandle>& faces);
        };
    }
}

#endif /* defined(TrenchBroom_MapRenderer) */
