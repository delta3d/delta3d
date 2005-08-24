/*
 * Delta3D Open Source Game and Simulation Engine
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author Matthew W. Campbell
*/
#include <QAction>

#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Material>
#include <osg/CullFace>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/NodeVisitor>
#include <osgDB/WriteFile>
#include <dtCore/transformable.h>

#include "dtDAL/actorproxyicon.h"

#include "dtEditQt/viewportoverlay.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/editoractions.h"
#include "dtEditQt/editordata.h"

namespace dtEditQt {

    ///////////////////////////////////////////////////////////////////////////////
    ViewportOverlay::ViewportOverlay()
    {
        this->multiSelectMode = false;
        this->overlayGroup = new osg::Group();
        this->selectionDecorator = NULL;
        listenForEvents();
    }

    ///////////////////////////////////////////////////////////////////////////////
    ViewportOverlay::~ViewportOverlay()
    {

    }

    ///////////////////////////////////////////////////////////////////////////////
    ViewportOverlay::ViewportOverlay(const ViewportOverlay &rhs)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    ViewportOverlay &ViewportOverlay::operator=(const ViewportOverlay &rhs)
    {
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::onActorsSelected(std::vector<osg::ref_ptr<dtDAL::ActorProxy> > &actors)
    {
        if (actors.empty()) {
            clearCurrentSelection();
            EditorActions::getInstance().actionEditDeleteActor->setEnabled(false);
            EditorActions::getInstance().actionEditDuplicateActor->setEnabled(false);
            EditorActions::getInstance().actionEditGroundClampActors->setEnabled(false);
            EditorActions::getInstance().actionEditGotoActor->setEnabled(false);
            ViewportManager::getInstance().refreshAllViewports();
            return;
        }

        if (!this->multiSelectMode)
            clearCurrentSelection();

        for (unsigned int i=0; i<actors.size(); i++) {
            const dtDAL::ActorProxy::RenderMode &renderMode = actors[i]->GetRenderMode();
            dtDAL::ActorProxyIcon *billBoardIcon = NULL;

            if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON) {
                billBoardIcon = actors[i]->GetBillBoardIcon();
                if (billBoardIcon != NULL)
                    select(actors[i]->GetBillBoardIcon()->GetDrawable());
                else
                    LOG_ERROR("ActorProxy: " + actors[i]->GetName() + " has NULL billboard.");
            }
            else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR) {
                select(actors[i]->GetActor());
            }
            else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON) {
                billBoardIcon = actors[i]->GetBillBoardIcon();
                if (billBoardIcon != NULL)
                    select(actors[i]->GetBillBoardIcon()->GetDrawable());
                else
                    LOG_ERROR("ActorProxy: " + actors[i]->GetName() + " has NULL billboard.");

                select(actors[i]->GetActor());
            }
            else {
                select(actors[i]->GetActor());
            }

            this->currentActorSelection.insert(actors[i]);
        }

        EditorActions::getInstance().actionEditDeleteActor->setEnabled(true);
        EditorActions::getInstance().actionEditDuplicateActor->setEnabled(true);
        EditorActions::getInstance().actionEditGroundClampActors->setEnabled(true);
        EditorActions::getInstance().actionEditGotoActor->setEnabled(true);
        ViewportManager::getInstance().refreshAllViewports();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::select(dtCore::DeltaDrawable *drawable)
    {
        if (drawable == NULL || drawable->GetOSGNode() == NULL)
            return;

        if (!this->selectionDecorator.valid())
            setupSelectionDecorator();
        else {
            if (this->selectionDecorator->containsNode(drawable->GetOSGNode()))
                return;
        }

        this->selectionVisitor.setRestoreMode(false);
        drawable->GetOSGNode()->accept(this->selectionVisitor);
        this->selectionDecorator->addChild(drawable->GetOSGNode());
        if (!this->overlayGroup->containsNode(this->selectionDecorator.get()))
            this->overlayGroup->addChild(this->selectionDecorator.get());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::unSelect(dtCore::DeltaDrawable *drawable)
    {
        if (drawable == NULL || drawable->GetOSGNode() == NULL)
            return;

        if (!this->selectionDecorator.valid())
            setupSelectionDecorator();

        this->selectionVisitor.setRestoreMode(true);
        drawable->GetOSGNode()->accept(this->selectionVisitor);
        this->selectionDecorator->removeChild(drawable->GetOSGNode());
    }

    ///////////////////////////////////////////////////////////////////////////////
    bool ViewportOverlay::isActorSelected(dtDAL::ActorProxy *proxy) const
    {
        ActorProxyList::const_iterator itor = this->currentActorSelection.find(proxy);
        if (itor != this->currentActorSelection.end())
            return true;
        else
            return false;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::removeActorFromCurrentSelection(dtDAL::ActorProxy *proxy, bool clearAll)
    {
        if (!this->selectionDecorator.valid())
            setupSelectionDecorator();

        ActorProxyList::iterator itor = this->currentActorSelection.find(proxy);
        if (itor == this->currentActorSelection.end())
            return;

        const dtDAL::ActorProxy::RenderMode &renderMode = proxy->GetRenderMode();
        dtDAL::ActorProxyIcon *billBoardIcon = NULL;

        //Make sure we remove the correct drawable from the selection list depending
        //on the render mode of the actor.
        if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON) {
            billBoardIcon = proxy->GetBillBoardIcon();
            if (billBoardIcon != NULL)
                unSelect(proxy->GetBillBoardIcon()->GetDrawable());
            else
                LOG_ERROR("ActorProxy: " + proxy->GetName() + " has NULL billboard.");
        }
        else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR) {
            unSelect(proxy->GetActor());
        }
        else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON) {
            billBoardIcon = proxy->GetBillBoardIcon();
            if (billBoardIcon != NULL)
                unSelect(proxy->GetBillBoardIcon()->GetDrawable());
            else
                LOG_ERROR("ActorProxy: " + proxy->GetName() + " has NULL billboard.");

            unSelect(proxy->GetActor());
        }
        else {
            unSelect(proxy->GetActor());
        }

        //Finally remove the actor proxy from the selection list.
        if (clearAll)
            this->currentActorSelection.erase(itor);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::clearCurrentSelection()
    {
        ActorProxyList::iterator itor = this->currentActorSelection.begin();
        while (itor != this->currentActorSelection.end()) {
            removeActorFromCurrentSelection(const_cast<dtDAL::ActorProxy *>(itor->get()),false);
            ++itor;
        }

        this->currentActorSelection.clear();
        this->selectionVisitor.reset();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::setupSelectionDecorator()
    {
        this->selectionDecorator = new osg::Group();
        this->selectionMaterial = new osg::Material();

        osg::StateSet* ss = new osg::StateSet;
        osg::StateAttribute::GLModeValue turnOn =
                            osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON;
        osg::StateAttribute::GLModeValue turnOff = osg::StateAttribute::PROTECTED |
                osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF;

        //Set the material color using the editor's current preference.
        QColor qtColor = EditorData::getInstance().getSelectionColor();
        osg::Vec4 color;

        color.set(qtColor.redF(),qtColor.greenF(),qtColor.blueF(),1.0f);
        this->selectionMaterial->setDiffuse(osg::Material::FRONT,color);
        this->selectionMaterial->setDiffuse(osg::Material::BACK,color);

        //Create the required state attributes for wireframe overlay selection.
        osg::PolygonOffset* po = new osg::PolygonOffset;
        osg::PolygonMode *pm = new osg::PolygonMode();

        pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
        po->setFactor(-1.0f);
        po->setUnits(-1.0f);

        ss->setAttributeAndModes(this->selectionMaterial.get(),turnOn);
        ss->setAttribute(this->selectionMaterial.get(),turnOn);
        ss->setAttributeAndModes(pm,turnOn);
        ss->setAttributeAndModes(po,turnOn);
        ss->setMode(GL_LIGHTING,turnOff);
        for (int i=0; i<ViewportManager::getInstance().getNumTextureUnits(); i++) {
            ss->setTextureMode(i,GL_TEXTURE_1D,turnOff);
            ss->setTextureMode(i,GL_TEXTURE_2D,turnOff);
            ss->setTextureMode(i,GL_TEXTURE_3D,turnOff);
        }

        this->selectionDecorator->setStateSet(ss);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::onEditorPreferencesChanged()
    {
        QColor qtColor = EditorData::getInstance().getSelectionColor();
        osg::Vec4 color;

        if (!this->selectionDecorator.valid())
            setupSelectionDecorator();

        color.set(qtColor.redF(),qtColor.greenF(),qtColor.blueF(),1.0f);
        this->selectionMaterial->setDiffuse(osg::Material::FRONT,color);
        this->selectionMaterial->setDiffuse(osg::Material::BACK,color);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::listenForEvents()
    {
        EditorEvents &ge = EditorEvents::getInstance();

        connect(&ge,SIGNAL(selectedActors(proxyRefPtrVector&)),
                this,SLOT(onActorsSelected(proxyRefPtrVector&)));

        connect(&ge, SIGNAL(editorPreferencesChanged()),
                this,SLOT(onEditorPreferencesChanged()));
    }
}
