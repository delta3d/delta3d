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
#include <QWidget>
#include <QGLWidget>
#include <osg/Texture>
#include "dtDAL/log.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/orthoviewport.h"
#include "dtEditQt/perspectiveviewport.h"
#include "dtEditQt/viewportoverlay.h"
#include "dtEditQt/camera.h"
#include "dtEditQt/editorevents.h"

namespace dtEditQt 
{

    //Singleton global variable for the library manager.
    osg::ref_ptr<ViewportManager> ViewportManager::instance(NULL);

    ///////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_ENUM(ViewportManager::ViewportType);
    ViewportManager::ViewportType
        ViewportManager::ViewportType::ORTHOGRAPHIC("ORTHOGRAPHIC");
    ViewportManager::ViewportType
        ViewportManager::ViewportType::PERSPECTIVE("PERSPECTIVE");
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    ViewportManager::ViewportManager()
    {
        this->shareMasterContext = true;
        this->masterViewport = NULL;
        this->masterScene = new dtCore::Scene();
        this->viewportOverlay = new ViewportOverlay();
        this->worldCamera = new Camera();

         connect(&EditorEvents::getInstance(),
                 SIGNAL(actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy>,osg::ref_ptr<dtDAL::ActorProperty>)),
                 this,
                 SLOT(onActorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy>,osg::ref_ptr<dtDAL::ActorProperty>)));

         connect(&EditorEvents::getInstance(),SIGNAL(projectChanged()),
                 this,SLOT(refreshAllViewports()));
         connect(&EditorEvents::getInstance(),SIGNAL(currentMapChanged()),
                 this,SLOT(onCurrentMapChanged()));
    }

    ///////////////////////////////////////////////////////////////////////////////
    ViewportManager::ViewportManager(const ViewportManager &rhs)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    ViewportManager &ViewportManager::operator=(const ViewportManager &rhs)
    {
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    ViewportManager::~ViewportManager()
    {
        std::cout << "Destroying viewport manager." << std::endl;
        this->viewportList.clear();
        std::cout << "Done with viewport manager" << std::endl;
    }

    ///////////////////////////////////////////////////////////////////////////////
    Viewport *ViewportManager::createViewport(const std::string &name,
        ViewportType &type, QWidget *parent)
    {
        Viewport *vp = NULL;

        //The master viewport for context sharing is the first one created.
        //Therefore, see if we have it and if so grab its context to pass
        //on to the new viewport.
        if (this->shareMasterContext) {
            if (this->masterViewport == NULL) {
                //Must be the first viewport.  Nothing to share with in this case.
                vp = createViewportImpl(name,type,parent,NULL);
                this->masterViewport = vp;
            }
            else {
                vp = createViewportImpl(name,type,parent,this->masterViewport);
            }
        }
        else {
            vp = createViewportImpl(name,type,parent,NULL);
        }

        //Now make sure the viewport we created has a valid OpenGL context.
        if (!vp || !vp->isValid()) {
            LOG_ERROR("Error creating viewport.");
            return NULL;
        }

        vp->setScene(this->masterScene.get());
        this->viewportList[vp->getName()] = vp;
        return vp;
    }

    ///////////////////////////////////////////////////////////////////////////////
    ViewportManager &ViewportManager::getInstance()
    {
        if (ViewportManager::instance.get() == NULL)
            ViewportManager::instance = new ViewportManager();
        return *(ViewportManager::instance.get());
    }

    ///////////////////////////////////////////////////////////////////////////////
    Viewport *ViewportManager::createViewportImpl(const std::string &name,
        ViewportType &type, QWidget *parent, QGLWidget *shareWith)
    {
        if (type == ViewportType::ORTHOGRAPHIC)
            return new OrthoViewport(name,parent,shareWith);
        else if (type == ViewportType::PERSPECTIVE)
            return new PerspectiveViewport(name,parent,shareWith);
        else
            return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::refreshAllViewports()
    {
        std::map<std::string,Viewport *>::iterator itor;
        for (itor = this->viewportList.begin(); itor!=this->viewportList.end(); ++itor) {
            if (itor->second->getAutoInteractionMode())
                itor->second->refresh();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::refreshScene()
    {
        std::map<std::string,Viewport *>::iterator itor;
        for (itor = this->viewportList.begin(); itor!=this->viewportList.end(); ++itor) {
            if (itor->second->getAutoSceneUpdate())
                itor->second->setScene(this->masterScene.get());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::clearMasterScene(
            const std::map<dtCore::UniqueId, osg::ref_ptr<dtDAL::ActorProxy> > &proxies)
    {
        std::map<dtCore::UniqueId,osg::ref_ptr<dtDAL::ActorProxy> >::const_iterator itor;

        for (itor = proxies.begin(); itor != proxies.end(); ++itor) 
        {
            dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->second.get());
            const dtDAL::ActorProxy::RenderMode &renderMode = proxy->GetRenderMode();
            dtDAL::ActorProxyIcon *billBoard;

            if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON) 
            {
                billBoard = proxy->GetBillBoardIcon();
                if (billBoard != NULL)
                    this->masterScene->RemoveDrawable(billBoard->GetDrawable());
            }
            else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR) 
            {
                this->masterScene->RemoveDrawable(proxy->GetActor());
            }
            else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON) 
            {
                billBoard = proxy->GetBillBoardIcon();
                if (billBoard != NULL)
                    this->masterScene->RemoveDrawable(billBoard->GetDrawable());
                this->masterScene->RemoveDrawable(proxy->GetActor());
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::initializeGL()
    {
        static bool hasBeenInitialized = false;
        if (!hasBeenInitialized) {
            hasBeenInitialized = true;

             //Get the texture extensions so we can query for some device information.
            osg::Texture::Extensions *ext = osg::Texture::getExtensions(0,true);
            if (ext != NULL)
                this->numTextureUnits = ext->numTextureUnits();
            else
                this->numTextureUnits = 2;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::onActorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
            osg::ref_ptr<dtDAL::ActorProperty> property)
    {
        unsigned int billBoardIndex, actorIndex;
        const dtDAL::ActorProxy::RenderMode &renderMode = proxy->GetRenderMode();
        dtDAL::ActorProxyIcon *billBoard = proxy->GetBillBoardIcon();

        if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON) 
        {
            this->masterScene->RemoveDrawable(proxy->GetActor());
            this->viewportOverlay->unSelect(proxy->GetActor());
            if (billBoard == NULL) 
            {
                LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
            }
            else 
            {
                billBoardIndex = this->masterScene->GetDrawableIndex(billBoard->GetDrawable());
                if (billBoardIndex == (unsigned)this->masterScene->GetNumberOfAddedDrawable()) 
                {
                    this->masterScene->AddDrawable(billBoard->GetDrawable());
                    this->viewportOverlay->select(billBoard->GetDrawable());
                }
            }
        }
        else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR)
        {
            if (billBoard == NULL) 
            {
                LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
            }
            else {
                this->viewportOverlay->unSelect(billBoard->GetDrawable());
                this->masterScene->RemoveDrawable(billBoard->GetDrawable());
            }

            actorIndex = this->masterScene->GetDrawableIndex(proxy->GetActor());
            if (actorIndex == (unsigned)this->masterScene->GetNumberOfAddedDrawable()) 
            {
                this->masterScene->AddDrawable(proxy->GetActor());
                this->viewportOverlay->select(proxy->GetActor());
            }
        }
        else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON) 
        {
            if (billBoard == NULL) 
            {
                LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
            }
            else 
            {
                billBoardIndex = this->masterScene->GetDrawableIndex(billBoard->GetDrawable());
                if (billBoardIndex == (unsigned)this->masterScene->GetNumberOfAddedDrawable()) 
                {
                    this->masterScene->AddDrawable(billBoard->GetDrawable());
                    this->viewportOverlay->select(billBoard->GetDrawable());
                }
            }

            actorIndex = this->masterScene->GetDrawableIndex(proxy->GetActor());
            if (actorIndex == (unsigned)this->masterScene->GetNumberOfAddedDrawable()) 
            {
                this->masterScene->AddDrawable(proxy->GetActor());
                this->viewportOverlay->select(proxy->GetActor());
            }
        }
        else {
            //If we got here, then the proxy wishes the system to determine how to display
            //the proxy.
        }

        refreshAllViewports();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::onCurrentMapChanged()
    {
        refreshScene();
        refreshAllViewports();
    }

}
