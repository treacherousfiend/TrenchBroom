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

#include "MapRenderer.h"

#include "PreferenceManager.h"
#include "Preferences.h"
#include "Assets/EntityDefinitionManager.h"
#include "Model/Brush.h"
#include "Model/BrushNode.h"
#include "Model/BrushFace.h"
#include "Model/EditorContext.h"
#include "Model/EntityNode.h"
#include "Model/GroupNode.h"
#include "Model/LayerNode.h"
#include "Model/Node.h"
#include "Model/WorldNode.h"
#include "Renderer/BrushRenderer.h"
#include "Renderer/EntityLinkRenderer.h"
#include "Renderer/GroupRenderer.h"
#include "Renderer/EntityRenderer.h"
#include "Renderer/RenderBatch.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderUtils.h"
#include "View/Selection.h"
#include "View/MapDocument.h"

#include <kdl/memory_utils.h>
#include <kdl/overload.h>
#include <kdl/vector_set.h>

#include <set>
#include <vector>

#include <QDebug>

namespace TrenchBroom {
    namespace Renderer {
        MapRenderer::MapRenderer(std::weak_ptr<View::MapDocument> document) :
        m_document(document),
        m_groupRenderer(std::make_unique<GroupRenderer>(kdl::mem_lock(document)->editorContext())),
        m_entityRenderer(std::make_unique<EntityRenderer>(
            *kdl::mem_lock(document),
            kdl::mem_lock(document)->entityModelManager(),
            kdl::mem_lock(document)->editorContext())),
        m_entityLinkRenderer(std::make_unique<EntityLinkRenderer>(m_document)),
        m_brushRenderer(std::make_unique<BrushRenderer>(kdl::mem_lock(document)->editorContext())) {
            bindObservers();
            setupRenderers();
        }

        MapRenderer::~MapRenderer() {
            unbindObservers();
            clear();
        }

        void MapRenderer::clear() {
            m_groupRenderer->clear();
            m_entityRenderer->clear();
            m_entityLinkRenderer->invalidate();
            m_brushRenderer->clear();
        }

        /**
         * Used to flash the selection color e.g. when duplicating
         */
        void MapRenderer::overrideSelectionColors(const Color& color, const float mix) {
            const Color edgeColor = pref(Preferences::SelectedEdgeColor).mixed(color, mix);
            const Color occludedEdgeColor = pref(Preferences::SelectedFaceColor).mixed(color, mix);
            const Color tintColor = pref(Preferences::SelectedFaceColor).mixed(color, mix);

            //m_selectionRenderer->setEntityBoundsColor(edgeColor);
            //m_selectionRenderer->setBrushEdgeColor(edgeColor);
            // m_selectionRenderer->setOccludedEdgeColor(occludedEdgeColor);
            // m_selectionRenderer->setTintColor(tintColor);
        }

        void MapRenderer::restoreSelectionColors() {
            //setupSelectionRenderer(*m_selectionRenderer);
        }

        void MapRenderer::render(RenderContext& renderContext, RenderBatch& renderBatch) {
            commitPendingChanges();
            setupGL(renderBatch);

            // renderDefaultOpaque(renderContext, renderBatch);
            // renderLockedOpaque(renderContext, renderBatch);
            // renderSelectionOpaque(renderContext, renderBatch);

            m_brushRenderer->renderOpaque(renderContext, renderBatch);
            m_entityRenderer->render(renderContext, renderBatch);
            m_groupRenderer->render(renderContext, renderBatch);

            // renderDefaultTransparent(renderContext, renderBatch);
            // renderLockedTransparent(renderContext, renderBatch);
            // renderSelectionTransparent(renderContext, renderBatch);
            m_brushRenderer->renderTransparent(renderContext, renderBatch);

            renderEntityLinks(renderContext, renderBatch);
        }

        void MapRenderer::commitPendingChanges() {
            auto document = kdl::mem_lock(m_document);
            document->commitPendingAssets();
        }

        class SetupGL : public Renderable {
        private:
            void doRender(RenderContext&) override {
                glAssert(glFrontFace(GL_CW))
                glAssert(glEnable(GL_CULL_FACE))
                glAssert(glEnable(GL_DEPTH_TEST))
                glAssert(glDepthFunc(GL_LEQUAL))
                glResetEdgeOffset();
            }
        };

        void MapRenderer::setupGL(RenderBatch& renderBatch) {
            renderBatch.addOneShot(new SetupGL());
        }

        // void MapRenderer::renderDefaultOpaque(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     m_defaultRenderer->setShowOverlays(renderContext.render3D());
        //     m_defaultRenderer->renderOpaque(renderContext, renderBatch);
        // }
        //
        // void MapRenderer::renderDefaultTransparent(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     m_defaultRenderer->setShowOverlays(renderContext.render3D());
        //     //m_defaultRenderer->renderTransparent(renderContext, renderBatch);
        // }
        //
        // void MapRenderer::renderSelectionOpaque(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     if (!renderContext.hideSelection()) {
        //         m_selectionRenderer->renderOpaque(renderContext, renderBatch);
        //     }
        // }
        //
        // void MapRenderer::renderSelectionTransparent(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     // if (!renderContext.hideSelection()) {
        //     //     m_selectionRenderer->renderTransparent(renderContext, renderBatch);
        //     // }
        // }
        //
        // void MapRenderer::renderLockedOpaque(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     m_lockedRenderer->setShowOverlays(renderContext.render3D());
        //     m_lockedRenderer->renderOpaque(renderContext, renderBatch);
        // }
        //
        // void MapRenderer::renderLockedTransparent(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     m_lockedRenderer->setShowOverlays(renderContext.render3D());
        //     //m_lockedRenderer->renderTransparent(renderContext, renderBatch);
        // }

        void MapRenderer::renderEntityLinks(RenderContext& renderContext, RenderBatch& renderBatch) {
            m_entityLinkRenderer->render(renderContext, renderBatch);
        }

        void MapRenderer::setupRenderers() {
            // setupDefaultRenderer(*m_defaultRenderer);
            // setupSelectionRenderer(*m_selectionRenderer);
            // setupLockedRenderer(*m_lockedRenderer);
            setupEntityLinkRenderer();
        }

        // void MapRenderer::setupDefaultRenderer(ObjectRenderer& renderer) {
        //     // renderer.setEntityOverlayTextColor(pref(Preferences::InfoOverlayTextColor));
        //     // renderer.setGroupOverlayTextColor(pref(Preferences::GroupInfoOverlayTextColor));
        //     // renderer.setOverlayBackgroundColor(pref(Preferences::InfoOverlayBackgroundColor));
        //     // renderer.setTint(false);
        //     // m_brushRenderer->setTransparencyAlpha(pref(Preferences::TransparentFaceAlpha));
        //     //
        //     // renderer.setGroupBoundsColor(pref(Preferences::DefaultGroupColor));
        //     // renderer.setEntityBoundsColor(pref(Preferences::UndefinedEntityColor));
        //     //
        //     // m_brushRenderer->setEditorContext(&kdl::mem_lock(m_document)->editorContext());
        //     // m_brushRenderer->setFaceColor(pref(Preferences::FaceColor));
        //     // m_brushRenderer->setEdgeColor(pref(Preferences::EdgeColor));
        // }
        //
        // void MapRenderer::setupSelectionRenderer(ObjectRenderer& renderer) {
        //     // renderer.setEntityOverlayTextColor(pref(Preferences::SelectedInfoOverlayTextColor));
        //     // renderer.setGroupOverlayTextColor(pref(Preferences::SelectedInfoOverlayTextColor));
        //     // renderer.setOverlayBackgroundColor(pref(Preferences::SelectedInfoOverlayBackgroundColor));
        //     // // renderer.setShowBrushEdges(true);
        //     // renderer.setShowOccludedObjects(true);
        //     // renderer.setOccludedEdgeColor(pref(Preferences::OccludedSelectedEdgeColor));
        //     // renderer.setTint(true);
        //     // renderer.setTintColor(pref(Preferences::SelectedFaceColor));
        //     //
        //     // renderer.setOverrideGroupBoundsColor(true);
        //     // renderer.setGroupBoundsColor(pref(Preferences::SelectedEdgeColor));
        //     //
        //     // renderer.setOverrideEntityBoundsColor(true);
        //     // renderer.setEntityBoundsColor(pref(Preferences::SelectedEdgeColor));
        //     // renderer.setShowEntityAngles(true);
        //     // renderer.setEntityAngleColor(pref(Preferences::AngleIndicatorColor));
        //
        //     // renderer.setBrushFaceColor(pref(Preferences::FaceColor));
        //     // renderer.setBrushEdgeColor(pref(Preferences::SelectedEdgeColor));
        // }

        // void MapRenderer::setupLockedRenderer(ObjectRenderer& renderer) {
        //     renderer.setEntityOverlayTextColor(pref(Preferences::LockedInfoOverlayTextColor));
        //     renderer.setGroupOverlayTextColor(pref(Preferences::LockedInfoOverlayTextColor));
        //     renderer.setOverlayBackgroundColor(pref(Preferences::LockedInfoOverlayBackgroundColor));
        //     renderer.setShowOccludedObjects(false);
        //     renderer.setTint(true);
        //     renderer.setTintColor(pref(Preferences::LockedFaceColor));
        //     // renderer.setTransparencyAlpha(pref(Preferences::TransparentFaceAlpha));
        //
        //     renderer.setOverrideGroupBoundsColor(true);
        //     renderer.setGroupBoundsColor(pref(Preferences::LockedEdgeColor));
        //
        //     renderer.setOverrideEntityBoundsColor(true);
        //     renderer.setEntityBoundsColor(pref(Preferences::LockedEdgeColor));
        //     renderer.setShowEntityAngles(false);
        //
        //     // renderer.setBrushFaceColor(pref(Preferences::FaceColor));
        //     // renderer.setBrushEdgeColor(pref(Preferences::LockedEdgeColor));
        // }

        void MapRenderer::setupEntityLinkRenderer() {
        }

        void MapRenderer::updateRenderers() {
            auto document = kdl::mem_lock(m_document);
            Model::WorldNode* world = document->world();

            std::vector<Model::EntityNode*> entities;
            std::vector<Model::GroupNode*> groups;
            std::vector<Model::BrushNode*> brushes;

            world->accept(kdl::overload(
                [&](auto&& thisLambda, Model::WorldNode* world) { world->visitChildren(thisLambda); },
                [&](auto&& thisLambda, Model::LayerNode* layer) { layer->visitChildren(thisLambda); },
                [&](auto&& thisLambda, Model::GroupNode* group) {
                    groups.push_back(group);
                    group->visitChildren(thisLambda);
                },
                [&](auto&& thisLambda, Model::EntityNode* entity) {
                    entities.push_back(entity);
                    entity->visitChildren(thisLambda);
                },
                [&](Model::BrushNode* brush) {
                    brushes.push_back(brush);
                }
            ));

            m_entityRenderer->setEntities(entities);
            m_groupRenderer->setGroups(groups);
            m_brushRenderer->setBrushes(brushes);

            invalidateEntityLinkRenderer();
        }

        void MapRenderer::invalidateRenderers() {
            m_groupRenderer->invalidate();
            m_entityRenderer->invalidate();
            m_entityLinkRenderer->invalidate();
            m_brushRenderer->invalidate();
        }

        void MapRenderer::invalidateEntityLinkRenderer() {
            m_entityLinkRenderer->invalidate();
        }

        void MapRenderer::reloadEntityModels() {
            m_entityRenderer->reloadModels();
        }

        void MapRenderer::bindObservers() {
            assert(!kdl::mem_expired(m_document));
            auto document = kdl::mem_lock(m_document);
            document->documentWasClearedNotifier.addObserver(this, &MapRenderer::documentWasCleared);
            document->documentWasNewedNotifier.addObserver(this, &MapRenderer::documentWasNewedOrLoaded);
            document->documentWasLoadedNotifier.addObserver(this, &MapRenderer::documentWasNewedOrLoaded);
            document->nodesWereAddedNotifier.addObserver(this, &MapRenderer::nodesWereAdded);
            document->nodesWereRemovedNotifier.addObserver(this, &MapRenderer::nodesWereRemoved);
            document->nodesDidChangeNotifier.addObserver(this, &MapRenderer::nodesDidChange);
            document->nodeVisibilityDidChangeNotifier.addObserver(this, &MapRenderer::nodeVisibilityDidChange);
            document->nodeLockingDidChangeNotifier.addObserver(this, &MapRenderer::nodeLockingDidChange);
            document->groupWasOpenedNotifier.addObserver(this, &MapRenderer::groupWasOpened);
            document->groupWasClosedNotifier.addObserver(this, &MapRenderer::groupWasClosed);
            document->brushFacesDidChangeNotifier.addObserver(this, &MapRenderer::brushFacesDidChange);
            document->selectionDidChangeNotifier.addObserver(this, &MapRenderer::selectionDidChange);
            document->textureCollectionsWillChangeNotifier.addObserver(this, &MapRenderer::textureCollectionsWillChange);
            document->entityDefinitionsDidChangeNotifier.addObserver(this, &MapRenderer::entityDefinitionsDidChange);
            document->modsDidChangeNotifier.addObserver(this, &MapRenderer::modsDidChange);
            document->editorContextDidChangeNotifier.addObserver(this, &MapRenderer::editorContextDidChange);

            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.preferenceDidChangeNotifier.addObserver(this, &MapRenderer::preferenceDidChange);
        }

        void MapRenderer::unbindObservers() {
            if (!kdl::mem_expired(m_document)) {
                auto document = kdl::mem_lock(m_document);
                document->documentWasClearedNotifier.removeObserver(this, &MapRenderer::documentWasCleared);
                document->documentWasNewedNotifier.removeObserver(this, &MapRenderer::documentWasNewedOrLoaded);
                document->documentWasLoadedNotifier.removeObserver(this, &MapRenderer::documentWasNewedOrLoaded);
                document->nodesWereAddedNotifier.removeObserver(this, &MapRenderer::nodesWereAdded);
                document->nodesWereRemovedNotifier.removeObserver(this, &MapRenderer::nodesWereRemoved);
                document->nodesDidChangeNotifier.removeObserver(this, &MapRenderer::nodesDidChange);
                document->nodeVisibilityDidChangeNotifier.removeObserver(this, &MapRenderer::nodeVisibilityDidChange);
                document->nodeLockingDidChangeNotifier.removeObserver(this, &MapRenderer::nodeLockingDidChange);
                document->groupWasOpenedNotifier.removeObserver(this, &MapRenderer::groupWasOpened);
                document->groupWasClosedNotifier.removeObserver(this, &MapRenderer::groupWasClosed);
                document->brushFacesDidChangeNotifier.removeObserver(this, &MapRenderer::brushFacesDidChange);
                document->selectionDidChangeNotifier.removeObserver(this, &MapRenderer::selectionDidChange);
                document->textureCollectionsWillChangeNotifier.removeObserver(this, &MapRenderer::textureCollectionsWillChange);
                document->entityDefinitionsDidChangeNotifier.removeObserver(this, &MapRenderer::entityDefinitionsDidChange);
                document->modsDidChangeNotifier.removeObserver(this, &MapRenderer::modsDidChange);
                document->editorContextDidChangeNotifier.removeObserver(this, &MapRenderer::editorContextDidChange);
            }

            PreferenceManager& prefs = PreferenceManager::instance();
            prefs.preferenceDidChangeNotifier.removeObserver(this, &MapRenderer::preferenceDidChange);
        }

        static void debugLog(const char* functionName) {
            qDebug() << functionName;
        }

        static void debugLog(const char* functionName, const std::vector<Model::Node*>& nodes) {
            qDebug() << functionName << nodes.size() << "nodes";
        }

        static void debugLog(const char* functionName, const Model::Node*) {
            qDebug() << functionName << "1 node";
        }

        static void debugLog(const char* functionName, const std::vector<Model::BrushFaceHandle>& faces) {
            qDebug() << functionName << faces.size() << "face handles";
        }

        static void debugLog(const char* functionName, const View::Selection& selection) {
            qDebug() << functionName <<
                QString::fromLatin1("%1/%2 nodes selected/deselected, %3/%4 faces selected/deselected")
                .arg(selection.selectedNodes().size())
                .arg(selection.deselectedNodes().size())
                .arg(selection.selectedBrushFaces().size())
                .arg(selection.deselectedBrushFaces().size());
        }

        void MapRenderer::documentWasCleared(View::MapDocument*) {
            debugLog(__func__);
            clear();
        }

        void MapRenderer::documentWasNewedOrLoaded(View::MapDocument*) {
            debugLog(__func__);
            clear();
            updateRenderers();
        }

        void MapRenderer::nodesWereAdded(const std::vector<Model::Node*>& nodes) {
            debugLog(__func__, nodes);
            // FIXME: selective
            updateRenderers();
        }

        void MapRenderer::nodesWereRemoved(const std::vector<Model::Node*>& nodes) {
            debugLog(__func__, nodes);
            // FIXME: selective
            updateRenderers();
        }

        void MapRenderer::nodesDidChange(const std::vector<Model::Node*>& nodes) {
            debugLog(__func__, nodes);
            invalidateNodes(nodes);
            invalidateEntityLinkRenderer();
        }

        void MapRenderer::nodeVisibilityDidChange(const std::vector<Model::Node*>& nodes) {
            debugLog(__func__, nodes);
            // FIXME: do we need to add/remove from the renderers?
            invalidateNodes(nodes);
            //invalidateRenderers();
        }

        void MapRenderer::nodeLockingDidChange(const std::vector<Model::Node*>& nodes) {
            debugLog(__func__, nodes);
            invalidateNodes(nodes);
            //updateRenderers();
        }

        void MapRenderer::groupWasOpened(Model::GroupNode* group) {
            debugLog(__func__, group);
            updateRenderers();
        }

        void MapRenderer::groupWasClosed(Model::GroupNode* group) {
            debugLog(__func__, group);
            updateRenderers();
        }

        void MapRenderer::brushFacesDidChange(const std::vector<Model::BrushFaceHandle>& faces) {
            debugLog(__func__, faces);
            invalidateBrushFaces(faces);
        }

        void MapRenderer::selectionDidChange(const View::Selection& selection) {
            debugLog(__func__, selection);
            invalidateNodes(selection.selectedNodes());
            invalidateNodes(selection.deselectedNodes());
            invalidateBrushFaces(selection.selectedBrushFaces());
            invalidateBrushFaces(selection.deselectedBrushFaces());
        }

        void MapRenderer::textureCollectionsWillChange() {
            debugLog(__func__);
            invalidateRenderers();
        }

        void MapRenderer::entityDefinitionsDidChange() {
            reloadEntityModels();
            invalidateRenderers();
            invalidateEntityLinkRenderer();
        }

        void MapRenderer::modsDidChange() {
            reloadEntityModels();
            invalidateRenderers();
            invalidateEntityLinkRenderer();
        }

        void MapRenderer::editorContextDidChange() {
            invalidateRenderers();
            invalidateEntityLinkRenderer();
        }

        void MapRenderer::preferenceDidChange(const IO::Path& path) {
            setupRenderers();

            auto document = kdl::mem_lock(m_document);
            if (document->isGamePathPreference(path)) {
                reloadEntityModels();
                invalidateRenderers();
                invalidateEntityLinkRenderer();
            }

            if (path.hasPrefix(IO::Path("Map view"), true)) {
                invalidateRenderers();
                invalidateEntityLinkRenderer();
            }
        }

        // invalidating specific nodes

        void MapRenderer::invalidateNodes(const std::vector<Model::Node*>& nodes) {
            size_t invalidatedNodes = 0;

            Model::Node::visitAll(nodes, kdl::overload(
                [](auto&& thisLambda, Model::WorldNode* world)  { world->visitChildren(thisLambda); },
                [](auto&& thisLambda, Model::LayerNode* layer)  { layer->visitChildren(thisLambda); },
                [&](auto&& thisLambda, Model::GroupNode* group) {
                    m_groupRenderer->invalidateGroup(group);
                    ++invalidatedNodes;
                    group->visitChildren(thisLambda);
                },
                [&](auto&& thisLambda, Model::EntityNode* entity) {
                    m_entityRenderer->invalidateEntity(entity);
                    ++invalidatedNodes;
                    entity->visitChildren(thisLambda);
                },
                [&](auto&&, Model::BrushNode* brush) {
                    m_brushRenderer->invalidateBrush(brush);
                    ++invalidatedNodes;
                }
            ));
        }

        void MapRenderer::invalidateBrushFaces(const std::vector<Model::BrushFaceHandle>& faces) {
            for (const auto& face : faces) {
                m_brushRenderer->invalidateBrush(face.node());
            }
        }
    }
}
