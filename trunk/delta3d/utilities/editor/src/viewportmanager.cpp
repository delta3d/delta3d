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
#include <QtGui/QWidget>
#include <QtOpenGL/QGLWidget>
#include <osg/Texture>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <dtUtil/log.h>
#include "dtDAL/actorproxyicon.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/orthoviewport.h"
#include "dtEditQt/perspectiveviewport.h"
#include "dtEditQt/viewportoverlay.h"
#include "dtEditQt/camera.h"
#include "dtEditQt/editorevents.h"

namespace dtEditQt
{

    //Singleton global variable for the library manager.
   dtCore::RefPtr<ViewportManager> ViewportManager::instance(NULL);

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
        shareMasterContext = true;
        masterViewport = NULL;
        masterScene = new dtCore::Scene();
        viewportOverlay = new ViewportOverlay();
        worldCamera = new Camera();
        inChangeTransaction = false;
        isPagingEnabled = false;
        startTick = 0;

        EditorEvents* editorEvents = &EditorEvents::getInstance();

        connect(editorEvents, SIGNAL(actorProxyCreated(proxyRefPtr, bool)),
                this,SLOT(onActorProxyCreated(proxyRefPtr, bool)));

        connect(editorEvents,
                SIGNAL(actorPropertyChanged(proxyRefPtr,propertyRefPtr)),
                this,
                SLOT(onActorPropertyChanged(proxyRefPtr,propertyRefPtr)));

        connect(editorEvents,SIGNAL(projectChanged()),
                this,SLOT(refreshAllViewports()));
        connect(editorEvents,SIGNAL(currentMapChanged()),
                this,SLOT(onCurrentMapChanged()));
        connect(editorEvents, SIGNAL(beginChangeTransaction()),
            this, SLOT(onBeginChangeTransaction()));
        connect(editorEvents, SIGNAL(endChangeTransaction()),
            this, SLOT(onEndChangeTransaction()));

        connect(&EditorEvents::getInstance(), SIGNAL(editorCloseEvent()),
                 this, SLOT(onEditorShutDown()));
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
        this->viewportList.clear();
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
        if (isPagingEnabled)
           EnablePaging(false);
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
    void ViewportManager::EnablePaging(bool enable)
    {
       if (enable)
       {
          osgDB::DatabasePager *databasePager = osgDB::Registry::instance()->getOrCreateDatabasePager();
          databasePager->setTargetFrameRate(60);
          databasePager->registerPagedLODs(masterScene->GetSceneNode());
          databasePager->setUseFrameBlock(false);

          for(std::map<std::string, Viewport*>::iterator i = viewportList.begin(); i != viewportList.end();
             ++i)
          {
             i->second->getSceneView()->getCullVisitor()->setDatabaseRequestHandler(databasePager);

             databasePager->setCompileGLObjectsForContextID(i->second->getSceneView()->getState()->getContextID(),true);
          }
          startTick = osg::Timer::instance()->tick();
          isPagingEnabled = true;
          LOG_INFO("Paging is enabled");
       }
       else
       {
          if(isPagingEnabled && osgDB::Registry::instance()->getDatabasePager() != NULL)
          {
             osgDB::Registry::instance()->getDatabasePager()->clear();
             osgDB::Registry::instance()->setDatabasePager(NULL);
             isPagingEnabled = false;
             LOG_INFO("Paging is disabled");
          }
          else
          {
             LOG_INFO("Paging is already disabled.");
          }
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
                billBoard->LoadImages();
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
            else
            {
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
                billBoard->LoadImages();
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
        else
        {
            //If we got here, then the proxy wishes the system to determine how to display
            //the proxy.
        }

        // only redraw if we're doing a single change.  Otherwise, all events will be
        // redrawn in the endChangeTransaction
        if (!inChangeTransaction)
            refreshAllViewports();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::onEditorShutDown()
    {
       if (isPagingEnabled)
          EnablePaging(false);
          
       masterScene->RemoveAllDrawables();
    }


    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::onActorProxyCreated(
            osg::ref_ptr<dtDAL::ActorProxy> proxy, bool forceNoAdjustments)
    {
        dtCore::Scene *scene = this->masterScene.get();
        dtDAL::ActorProxyIcon *billBoard = NULL;

        const dtDAL::ActorProxy::RenderMode &renderMode = proxy->GetRenderMode();
        if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
        {
            billBoard = proxy->GetBillBoardIcon();

            // Load the textures for the billboard and orientation arrow.
            // This is only done here so the files will only be loaded when
            // they are actually going to be rendered. Previously this was
            // done inside ActorProxyIcon's constructor, even if billboards
            // will never be displayed. It was broken out and placed here
            // to fix that. -osb
            billBoard->LoadImages();
            if (billBoard == NULL)
            {
                LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
            }
            else
                scene->AddDrawable(billBoard->GetDrawable());
        }
        else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR)
        {
            scene->AddDrawable(proxy->GetActor());
        }
        else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
        {
            scene->AddDrawable(proxy->GetActor());

            billBoard = proxy->GetBillBoardIcon();

            // Load the textures for the billboard and orientation arrow.
            // This is only done here so the files will only be loaded when
            // they are actually going to be rendered. Previously this was
            // done inside ActorProxyIcon's constructor, even if billboards
            // will never be displayed. It was broken out and placed here
            // to fix that. -osb
            billBoard->LoadImages();
            if (billBoard == NULL)
            {
                LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
            }
            else
                scene->AddDrawable(billBoard->GetDrawable());
        }
        else
        {
            //If we got here, then the proxy wishes the system to determine how to display
            //the proxy. Currently, not implemented, defaults to DRAW_ACTOR).
            scene->AddDrawable(proxy->GetActor());
        }

        // update the viewports unless we're getting lots of changes back to back, in which
        // case our super class handles that.
        if (!inChangeTransaction)
            refreshAllViewports();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::placeProxyInFrontOfCamera(dtDAL::ActorProxy *proxy)
    {
        //Get the current position and direction the camera is facing.
        osg::Vec3 pos = getWorldViewCamera()->getPosition();
        osg::Vec3 viewDir = getWorldViewCamera()->getViewDir();

        //If the object is a transformable (can have a position in the scene)
        //add it to the scene in front of the camera.
        dtDAL::TransformableActorProxy *tProxy =
                dynamic_cast<dtDAL::TransformableActorProxy *>(proxy);
        dtDAL::ActorProperty *prop = proxy->GetProperty("Translation");

        if (tProxy != NULL && prop != NULL)
        {
            const osg::BoundingSphere &bs = tProxy->GetActor()->GetOSGNode()->getBound();

            //Position it along the camera's view direction.  The distance from
            //the camera is the object's bounding volume so it appears
            //just in front of the camera.  If the object is very large, it is
            //just created at the origin.
            std::string oldValue = prop->GetStringValue();

            float offset = (bs.radius() < 1000.0f) ? bs.radius() : 1.0f;
            if (offset <= 0.0f)
                offset = 10.0f;
            tProxy->SetTranslation(pos+(viewDir*offset*2));

            std::string newValue = prop->GetStringValue();
            EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, prop, oldValue, newValue);
            EditorEvents::getInstance().emitActorPropertyChanged(proxy,prop);
        }

        if (!inChangeTransaction)
            refreshAllViewports();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::onBeginChangeTransaction()
    {
        inChangeTransaction = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::onEndChangeTransaction()
    {
        inChangeTransaction = false;
        refreshAllViewports();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void ViewportManager::onCurrentMapChanged()
    {
        refreshScene();
        refreshAllViewports();
    }

}
