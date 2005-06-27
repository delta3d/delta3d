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
#include "dtEditQt/viewport.h"

#include <osg/StateSet>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/CullFace>
#include <osg/Viewport>
#include <osg/FrameStamp>
#include <osg/StateSet>
#include <osg/ClearNode>

#include <osgUtil/SceneView>

#include <dtCore/scene.h>
#include <osg/AlphaFunc>

#include <dtCore/system.h>
#include <dtCore/pointaxis.h>

#include "dtEditQt/viewportoverlay.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/editoractions.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/editordata.h"
#include "dtDAL/intersectionquery.h"
#include "dtDAL/exception.h"
#include "dtDAL/map.h"
#include "dtDAL/transformableactorproxy.h"

namespace dtEditQt
{

    ///////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_ENUM(Viewport::RenderStyle);
    const Viewport::RenderStyle Viewport::RenderStyle::WIREFRAME("WIREFRAME");
    const Viewport::RenderStyle Viewport::RenderStyle::LIT("LIT");
    const Viewport::RenderStyle Viewport::RenderStyle::TEXTURED("TEXTURED");
    const Viewport::RenderStyle Viewport::RenderStyle::LIT_AND_TEXTURED("LIT_AND_TEXTURED");
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_ENUM(Viewport::InteractionMode);
    const Viewport::InteractionMode Viewport::InteractionMode::CAMERA("CAMERA");
    const Viewport::InteractionMode Viewport::InteractionMode::SELECT_ACTOR("SELECT_ACTOR");
    const Viewport::InteractionMode Viewport::InteractionMode::TRANSLATE_ACTOR("TRANSLATE_ACTOR");
    const Viewport::InteractionMode Viewport::InteractionMode:: ROTATE_ACTOR("ROTATE_ACTOR");
    const Viewport::InteractionMode Viewport::InteractionMode::SCALE_ACTOR("SCALE_ACTOR");
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    Viewport::Viewport(ViewportManager::ViewportType &type, const std::string &name,
        QWidget *parent, QGLWidget *shareWith) :
            QGLWidget(parent,shareWith), name(name), viewPortType(type)
    {
        //this->messageListener = new Delta3DMessageListener(this);
        this->frameStamp = new osg::FrameStamp();
        this->mouseSensitivity = 10.0f;
        this->interactionMode = &InteractionMode::CAMERA;
        this->renderStyle = &RenderStyle::WIREFRAME;

        this->useAutoInteractionMode = false;
        this->initialized = false;
        this->autoSceneUpdate = true;
        this->enableKeyBindings = true;

        this->rootNodeGroup = new osg::Group();
        this->sceneView = new osgUtil::SceneView();

        this->sceneView->setDefaults();
        this->sceneView->setFrameStamp(this->frameStamp.get());
        this->sceneView->setSceneData(this->rootNodeGroup.get());
        setOverlay(ViewportManager::getInstance().getViewportOverlay());

        setMouseTracking(false);
        this->cacheMouseLocation = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    Viewport::~Viewport()
    {

    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::initializeGL()
    {
        setupInitialRenderState();
        ViewportManager::getInstance().initializeGL();
        this->initialized = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::setScene(dtCore::Scene *scene)
    {
        //First, remove the old scene, then add the new one.
        if (this->sceneView.valid()) {
            if (this->scene != NULL) {
                this->rootNodeGroup->replaceChild(this->scene->GetSceneNode(),
                                                  scene->GetSceneNode());
            }
            else {
                this->rootNodeGroup->addChild(scene->GetSceneNode());
            }

            this->scene = scene;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::setOverlay(ViewportOverlay *overlay)
    {
        if (this->sceneView.valid()) {
            //If the new overlay is NULL, clear the current overlay.
            if (overlay == NULL) {
                this->rootNodeGroup->removeChild(this->overlay->getOverlayGroup());
                this->overlay = NULL;
                return;
            }

            //Else update the current overlay in both the scene and in the viewport.
            if (this->overlay != NULL) {
                this->rootNodeGroup->replaceChild(this->overlay->getOverlayGroup(),
                    overlay->getOverlayGroup());
            }
            else {
                this->rootNodeGroup->addChild(overlay->getOverlayGroup());
            }

            this->overlay = overlay;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::resizeGL(int width, int height)
    {
        this->sceneView->setViewport(0,0,width,height);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::paintGL()
    {
        if (!this->sceneView.valid() || !this->scene.valid() || !this->camera.valid())
            return;

        renderFrame();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::refresh()
    {
        updateGL();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::setClearColor(const osg::Vec4 &color)
    {
        if (this->clearNode.valid())
            this->clearNode->setClearColor(color);
    }


    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::renderFrame()
    {
        this->sceneView->setProjectionMatrix(this->camera->getProjectionMatrix());
        this->sceneView->setViewMatrix(this->camera->getWorldViewMatrix());

        this->frameStamp->setFrameNumber(this->frameStamp->getFrameNumber()+1);
        this->sceneView->update();
        this->sceneView->cull();
        this->sceneView->draw();
        //mClock.update();
       //mFrameStamp->setReferenceTime(mClock.getAbsTime() );
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::setRenderStyle(const RenderStyle &style, bool refreshView)
    {
        int i;
        int numTextureUnits = ViewportManager::getInstance().getNumTextureUnits();

        this->renderStyle = &style;
        if (!this->sceneView.valid())
            EXCEPT(dtDAL::ExceptionEnum::BaseException,"Cannot set render style "
                   "because the current scene view is invalid.");

        osg::StateAttribute::GLModeValue turnOn =
                osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON;
        osg::StateAttribute::GLModeValue turnOff =
                osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF;

        osg::PolygonMode *pm = dynamic_cast<osg::PolygonMode *>(
                this->globalStateSet->getAttribute(osg::StateAttribute::POLYGONMODE));

        if (this->renderStyle == &RenderStyle::WIREFRAME) {
            for (i=0; i<numTextureUnits; i++)
                this->globalStateSet->setTextureMode(i,GL_TEXTURE_2D,turnOff);
            this->globalStateSet->setMode(GL_LIGHTING,turnOff);
            pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
        }
        else if (this->renderStyle == &RenderStyle::TEXTURED) {
            for (i=0; i<numTextureUnits; i++)
                this->globalStateSet->setTextureMode(i,GL_TEXTURE_2D,turnOn);
            this->globalStateSet->setMode(GL_LIGHTING,turnOff);
            pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);
        }
        else if (this->renderStyle == &RenderStyle::LIT) {
            for (i=0; i<numTextureUnits; i++)
                this->globalStateSet->setTextureMode(i,GL_TEXTURE_2D,turnOff);
            this->globalStateSet->setMode(GL_LIGHTING,turnOn);
            pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);
        }
        else if (this->renderStyle == &RenderStyle::LIT_AND_TEXTURED) {
            for (i=0; i<numTextureUnits; i++)
                this->globalStateSet->setTextureMode(i,GL_TEXTURE_2D,turnOn);
            this->globalStateSet->setMode(GL_LIGHTING,turnOn);
            pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);
        }

        if (refreshView) {
            if (!isInitialized())
                EXCEPT(dtDAL::ExceptionEnum::BaseException,"Cannot refresh the viewport. "
                       "It has not been initialized.");
            updateGL();
        }

        emit renderStyleChanged();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::pick(int x, int y)
    {
        unsigned int i;

        if (!this->scene.valid())
            EXCEPT(dtDAL::ExceptionEnum::BaseException,
                   "Scene is invalid.  Cannot pick objects from an invalid scene.");

        osg::ref_ptr<dtDAL::Map> currMap = EditorData::getInstance().getCurrentMap();
        if (!currMap.valid())
            return;

        dtDAL::IntersectionQuery query;
        std::vector<osg::ref_ptr<dtDAL::ActorProxy> > toSelect;
        osg::Vec3 nearPoint,farPoint;
        int yLoc = this->sceneView->getViewport()->height()-y;

        this->sceneView->projectWindowXYIntoObject(x,yLoc,nearPoint,farPoint);
        query.SetStartPos(nearPoint);
        query.SetDirection(farPoint-nearPoint);

        //Next, run a query on all of the root drawables and stop if an
        //intersection has been found.
        for (i=0; (int)i<getScene()->GetNumberOfAddedDrawable(); i++)
        {
            query.SetQueryRoot(getScene()->GetDrawable(i));
            query.Exec();
        }

        //Get the list of intersected objects.  If no intersection was found,
        //this list will be empty.
        std::vector<dtDAL::IntersectionQuery::HitRecord> &hits = query.GetHitList();
        ViewportOverlay *overlay = ViewportManager::getInstance().getViewportOverlay();
        ViewportOverlay::ActorProxyList &selection = overlay->getCurrentActorSelection();
        if (!hits.empty()) {
            dtCore::DeltaDrawable *drawable = hits[0].drawable.get();

            //First see if the selected drawable is an actor.
            dtDAL::ActorProxy *newSelection = currMap->GetProxyById(*drawable->GetUniqueId());

            //If its not an actor then it may be a billboard placeholder for an actor.
            if (newSelection == NULL) {
                const std::map<dtCore::UniqueId, osg::ref_ptr<dtDAL::ActorProxy> > &proxyList =
                    currMap->GetAllProxies();
                std::map<dtCore::UniqueId, osg::ref_ptr<dtDAL::ActorProxy> >::const_iterator proxyItor;

                for (proxyItor=proxyList.begin(); proxyItor!=proxyList.end(); ++proxyItor) {
                    dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(proxyItor->second.get());
                    const dtDAL::ActorProxyIcon *billBoard = proxy->GetBillBoardIcon();
                    if (billBoard && (billBoard->GetDrawable() == drawable)) {
                        newSelection = proxy;
                        break;
                    }
                }
            }

            if (newSelection) {
                if (overlay->isActorSelected(newSelection))
                    overlay->removeActorFromCurrentSelection(newSelection);
                else
                    toSelect.push_back(newSelection);
            }
        }

        //Inform the world what objects were selected and refresh all the viewports
        //affected by the change.  If we are in multi-selection mode (i.e. the control
        //key is pressed) add the current selection to the newly selected proxy.
        if (overlay->getMultiSelectMode()) {
            ViewportOverlay::ActorProxyList::iterator itor;
            for (itor=selection.begin(); itor!=selection.end(); ++itor) {
                toSelect.push_back(const_cast<dtDAL::ActorProxy *>(itor->get()));
            }
        }

        EditorEvents::getInstance().emitActorsSelected(toSelect);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::onGotoActor(osg::ref_ptr<dtDAL::ActorProxy> &proxy)
    {
        dtDAL::TransformableActorProxy *tProxy = dynamic_cast<dtDAL::TransformableActorProxy *>(proxy.get());
        if (tProxy != NULL && getCamera() != NULL) {
            osg::Vec3 viewDir = getCamera()->getViewDir();

            const osg::BoundingSphere &bs = tProxy->GetActor()->GetOSGNode()->getBound();
            float offset = (bs.radius() < 1000.0f) ? bs.radius() : 1.0f;
            if (offset <= 0.0f)
                offset = 10.0f;

            if (this->viewPortType == ViewportManager::ViewportType::PERSPECTIVE) {
                getCamera()->setPosition(bs.center());
                getCamera()->move(viewDir*-offset*1.5f);
            }
            else {
                osg::Vec3 right = getCamera()->getRightDir();
                osg::Vec3 up = getCamera()->getUpDir();
                getCamera()->setPosition(bs.center());
            }

            refresh();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::trapMouseCursor()
    {
        //Get the current cursor so we can restore it later.
        if (cursor().shape() != Qt::BlankCursor)
            this->oldMouseCursor = cursor();
        setCursor(QCursor(Qt::BlankCursor));

        //Cache the old mouse location so the cursor doesn't appear to jump when
        //the camera mode operation is complete.
        if (this->cacheMouseLocation) {
            this->oldMouseLocation = QCursor::pos();
            this->cacheMouseLocation = false;
        }

        //Put the mouse cursor in the center of the viewport.
        QPoint center((x()+width())/2,(y()+height())/2);
        QCursor::setPos(mapToGlobal(center));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::releaseMouseCursor()
    {
        setCursor(this->oldMouseCursor);
        QCursor::setPos(this->oldMouseLocation);
        this->cacheMouseLocation = true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::setCameraMode()
    {
        this->interactionMode = &InteractionMode::CAMERA;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::setActorSelectMode()
    {
        this->interactionMode = &InteractionMode::SELECT_ACTOR;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::setActorTranslateMode()
    {
        this->interactionMode = &InteractionMode::TRANSLATE_ACTOR;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::setActorRotateMode()
    {
        this->interactionMode = &InteractionMode::ROTATE_ACTOR;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::connectInteractionModeSlots()
    {
        //Connect the global actions we want to track.
        EditorActions &ga = EditorActions::getInstance();
        EditorEvents &ge = EditorEvents::getInstance();

        connect(ga.actionSelectionCamera,SIGNAL(triggered()),this,SLOT(setCameraMode()));
        connect(ga.actionSelectionSelectActor,SIGNAL(triggered()),this,SLOT(setActorSelectMode()));
        connect(ga.actionSelectionTranslateActor,SIGNAL(triggered()),this,SLOT(setActorTranslateMode()));
        connect(ga.actionSelectionRotateActor,SIGNAL(triggered()),this,SLOT(setActorRotateMode()));

        connect(&ge,SIGNAL(gotoActor(osg::ref_ptr<dtDAL::ActorProxy> &)),
                this,SLOT(onGotoActor(osg::ref_ptr< dtDAL::ActorProxy >&)));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::disconnectInteractionModeSlots()
    {
        //Disconnect from all our global actions we were previously tracking.
        EditorActions &ga = EditorActions::getInstance();
        EditorEvents &ge = EditorEvents::getInstance();

        disconnect(ga.actionSelectionCamera,SIGNAL(triggered()),this,SLOT(setCameraMode()));
        disconnect(ga.actionSelectionSelectActor,SIGNAL(triggered()),this,SLOT(setActorSelectMode()));
        disconnect(ga.actionSelectionTranslateActor,SIGNAL(triggered()),this,SLOT(setActorTranslateMode()));
        disconnect(ga.actionSelectionRotateActor,SIGNAL(triggered()),this,SLOT(setActorRotateMode()));

        disconnect(&ge,SIGNAL(gotoActor(osg::ref_ptr<dtDAL::ActorProxy> &)),
                  this,SLOT(onGotoActor(osg::ref_ptr< dtDAL::ActorProxy >&)));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::syncWithModeActions()
    {
        if (this->useAutoInteractionMode) {
            QAction *action = EditorActions::getInstance().modeToolsGroup->checkedAction();
            if (action == EditorActions::getInstance().actionSelectionCamera)
                setCameraMode();
            else if (action == EditorActions::getInstance().actionSelectionSelectActor)
                setActorSelectMode();
            else if (action == EditorActions::getInstance().actionSelectionTranslateActor)
                setActorTranslateMode();
            else if (action == EditorActions::getInstance().actionSelectionRotateActor)
                setActorRotateMode();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::saveSelectedActorOrigValues(const std::string &propName)
    {
        ViewportOverlay::ActorProxyList &selection =
            ViewportManager::getInstance().getViewportOverlay()->getCurrentActorSelection();
        ViewportOverlay::ActorProxyList::iterator itor;

        // clear the old list first
        selectedActorOrigValues.clear();

        for (itor=selection.begin(); itor!=selection.end(); ++itor) {
            dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
            dtDAL::ActorProperty *prop = proxy->GetProperty(propName);

            if (prop != NULL) {
                std::string oldValue = prop->GetStringValue();
                selectedActorOrigValues.push_back(oldValue);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::updateActorSelectionProperty(const std::string &propName)
    {
        ViewportOverlay::ActorProxyList &selection =
            ViewportManager::getInstance().getViewportOverlay()->getCurrentActorSelection();
        ViewportOverlay::ActorProxyList::iterator itor;
        int oldValueIndex = 0;

        for (itor=selection.begin(); itor!=selection.end(); ++itor) {
            dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
            dtDAL::ActorProperty *prop = proxy->GetProperty(propName);

            if (prop != NULL) {
                // emit the old value before the change so undo/redo can recover.
                std::string oldValue = selectedActorOrigValues[oldValueIndex];
                std::string newValue = prop->GetStringValue();
                EditorEvents::getInstance().emitActorPropertyAboutToChange(proxy, prop, oldValue, newValue);
                oldValueIndex++;

                EditorEvents::getInstance().emitActorPropertyChanged(proxy,prop);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::setAutoInteractionMode(bool on)
    {
        if (on) {
            if (this->useAutoInteractionMode)
                return; //Already on, so do nothing.
            else {
                this->useAutoInteractionMode = true;
                connectInteractionModeSlots();
            }
        }
        else {
            if (!this->useAutoInteractionMode)
                return; //Already off, so do nothing.
            else {
                this->useAutoInteractionMode = false;
                disconnectInteractionModeSlots();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Viewport::setupInitialRenderState()
    {
        //Some actors in the scene may have a clear node that disables screen clears
        //each frame. (The skybox for example).  Therefore, we add this node to the
        //scene to ensure a clear happens if needed.
        this->clearNode = new osg::ClearNode;
        this->clearNode->setRequiresClear(true);
        this->clearNode->setClearColor(osg::Vec4(0.2f,0.2f,0.4f,1.0f));
        osg::Group *group = getSceneView()->getSceneData()->asGroup();
        if (group != NULL)
            group->addChild(this->clearNode.get());

        //Sets up the global state set used to render the viewport's contents.
        //This also sets up some default modes which are shared amoung
        //all viewports.
        this->globalStateSet = new osg::StateSet();

        osg::AlphaFunc *alphafunc = new osg::AlphaFunc;
        alphafunc->setFunction(osg::AlphaFunc::GREATER,0.0f);
        osg::PolygonMode *pm = new osg::PolygonMode();
        pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);

        osg::CullFace *cf = new osg::CullFace();
        cf->setMode(osg::CullFace::BACK);

        this->globalStateSet->setGlobalDefaults();
        this->globalStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
        this->globalStateSet->setAttributeAndModes(alphafunc, osg::StateAttribute::ON);
        this->globalStateSet->setAttributeAndModes(pm,osg::StateAttribute::ON);
        this->globalStateSet->setAttributeAndModes(cf,osg::StateAttribute::ON);

        this->sceneView->setGlobalStateSet(this->globalStateSet.get());
    }

}
