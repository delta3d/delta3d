/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
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
#include <osg/Billboard>
#include <osg/Geometry>
#include <osg/NodeVisitor>
#include <dtCore/transformable.h>

#include "dtDAL/actorproxyicon.h"

#include "dtEditQt/viewportoverlay.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/editoractions.h"

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////

    /**
     * This node visitor is invoked when we select an object in the scene.  This
     * ensures that the color binding on the object does not affect the wireframe
     * overlay used to convey selected objects.
     */
    class ExtractSelectionVisitor : public osg::NodeVisitor {
    public:

        /**
         * Constructs the visitor object.
         */
        ExtractSelectionVisitor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {
            this->copyOp = osg::CopyOp::DEEP_COPY_OBJECTS |
                           osg::CopyOp::DEEP_COPY_NODES |
                           osg::CopyOp::DEEP_COPY_DRAWABLES;
        }

        /**
         * Empty destructor.
         */
        virtual ~ExtractSelectionVisitor() { }

        /**
         * Overridden to ensure that no action is taken on billboards in the scene.
         * @param billBoard The billboard scene node.
         */
        virtual void apply(osg::Billboard &billBoard) {
            traverse(billBoard);
        }

        /**
         * Copies the incoming geode and turns off any color binding that
         * may have been set on the copy.
         * @param geode
         */
        virtual void apply(osg::Geode &geode)
        {
            osg::Geode *copyGeode = new osg::Geode(geode,this->copyOp);
            unsigned int i;

            //Make sure we turn off color binding so the wireframe color is
            //correct.
            for (i=0; i<copyGeode->getNumDrawables(); i++) {
                osg::Geometry *geom = copyGeode->getDrawable(i)->asGeometry();
                if (geom != NULL) {
                    geom->setColorBinding(osg::Geometry::BIND_OFF);
                }
            }

            for (i=0; i<geode.getNumParents(); i++) {
                geode.getParent(i)->replaceChild(&geode,copyGeode);
            }

            traverse(*copyGeode);
        }

    private:
        osg::CopyOp copyOp;
    };
    ///////////////////////////////////////////////////////////////////////////////


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
            select(NULL);
            this->currentActorSelection.clear();
            EditorActions::getInstance().actionEditDeleteActor->setEnabled(false);
            EditorActions::getInstance().actionEditDuplicateActor->setEnabled(false);
            EditorActions::getInstance().actionEditGroundClampActors->setEnabled(false);
        }
        else 
        {
            if (!this->multiSelectMode) 
            {
                select(NULL);
                this->currentActorSelection.clear();
            }

            for (unsigned int i=0; i<actors.size(); i++)
            {
                const dtDAL::ActorProxy::RenderMode &renderMode = actors[i]->GetRenderMode();
                dtDAL::ActorProxyIcon *billBoardIcon = actors[i]->GetBillBoardIcon();

                if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON) 
                {
                    if (billBoardIcon != NULL)
                        select(actors[i]->GetBillBoardIcon()->GetDrawable());
                    else
                        LOG_ERROR("ActorProxy: " + actors[i]->GetName() + " has NULL billboard.");
                }
                else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR) 
                {
                    select(actors[i]->GetActor());
                }
                else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON) 
                {
                    select(actors[i]->GetActor());

                    if (billBoardIcon != NULL)
                        select(actors[i]->GetBillBoardIcon()->GetDrawable());
                    else
                        LOG_ERROR("ActorProxy: " + actors[i]->GetName() + " has NULL billboard.");
                }
                else 
                {
                    //If we got here, then the proxy wishes the system to determine how to display
                    //the proxy.
                }

                this->currentActorSelection.insert(actors[i]);
            }

            EditorActions::getInstance().actionEditDeleteActor->setEnabled(true);
            EditorActions::getInstance().actionEditDuplicateActor->setEnabled(true);
            EditorActions::getInstance().actionEditGroundClampActors->setEnabled(true);
        }

        ViewportManager::getInstance().refreshAllViewports();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::select(dtCore::DeltaDrawable *drawable)
    {
        if (!this->selectionDecorator.valid())
            setupSelectionDecorator();

        osg::Group *selectionNode = this->selectionDecorator.get();
        if (drawable == NULL) {
            selectionNode->removeChild(0,selectionNode->getNumChildren());
            this->overlayGroup->removeChild(selectionNode);
            return;
        }

        if (selectionNode->containsNode(drawable->GetOSGNode()))
            return;

        ExtractSelectionVisitor sv;
        drawable->GetOSGNode()->accept(sv);
        selectionNode->addChild(drawable->GetOSGNode());

        if (!this->overlayGroup->containsNode(selectionNode))
            this->overlayGroup->addChild(selectionNode);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::unSelect(dtCore::DeltaDrawable *drawable)
    {
        if (!this->selectionDecorator.valid())
            setupSelectionDecorator();
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
    void ViewportOverlay::removeActorFromCurrentSelection(dtDAL::ActorProxy *proxy)
    {
        if (!this->selectionDecorator.valid())
            setupSelectionDecorator();

        ActorProxyList::iterator itor = this->currentActorSelection.find(proxy);
        if (itor != this->currentActorSelection.end()) {
            this->currentActorSelection.erase(itor);
            if (proxy->GetActor())
                this->selectionDecorator->removeChild(proxy->GetActor()->GetOSGNode());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::setupSelectionDecorator()
    {
        this->selectionDecorator = new osg::Group();
        osg::StateSet* ss = new osg::StateSet;
        osg::StateAttribute::GLModeValue turnOn = osg::StateAttribute::PROTECTED |
            osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON;
        osg::StateAttribute::GLModeValue turnOff = osg::StateAttribute::PROTECTED |
                osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF;

        osg::PolygonOffset* po = new osg::PolygonOffset;
        osg::PolygonMode *pm = new osg::PolygonMode();
        osg::Material *mat = new osg::Material();
        osg::CullFace *cull = new osg::CullFace();

        cull->setMode(osg::CullFace::BACK);
        pm->setMode(osg::PolygonMode::FRONT,osg::PolygonMode::LINE);
        po->setFactor(-1.0f);
        po->setUnits(-1.0f);
        mat->setDiffuse(osg::Material::FRONT,osg::Vec4(1,0,0,1));
        mat->setDiffuse(osg::Material::BACK,osg::Vec4(1,0,0,1));

        ss->setAttributeAndModes(cull,turnOn);
        ss->setAttributeAndModes(mat,turnOn);
        ss->setAttribute(mat,turnOn);
        ss->setAttributeAndModes(pm,turnOn);
        ss->setAttributeAndModes(po,turnOn);
        ss->setMode(GL_LIGHTING,turnOff);
        for (int i=0; i<ViewportManager::getInstance().getNumTextureUnits(); i++)
          ss->setTextureMode(i,GL_TEXTURE_2D,turnOff);

        this->selectionDecorator->setStateSet(ss);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportOverlay::listenForEvents()
    {
        EditorEvents &ge = EditorEvents::getInstance();
        //EditorActions &ga = EditorActions::getInstance();

        connect(&ge,SIGNAL(selectedActors(std::vector<osg::ref_ptr<dtDAL::ActorProxy> >&)),
                this,SLOT(onActorsSelected(std::vector<osg::ref_ptr<dtDAL::ActorProxy> >&)));
    }
}
