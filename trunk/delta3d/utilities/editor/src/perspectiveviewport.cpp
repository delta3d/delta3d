/*
* Delta3D Open Source Game and Simulation Engine Level Editor
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
#include <QMouseEvent>
#include <QAction>
#include <sstream>
#include <osg/Billboard>
#include <osgDB/WriteFile>
#include "dtEditQt/perspectiveviewport.h"
#include "dtEditQt/viewportoverlay.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editoractions.h"
#include "dtDAL/transformableactorproxy.h"
#include "dtDAL/log.h"
#include "dtDAL/actorproxyicon.h"

namespace dtEditQt
{
    ///////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_ENUM(PerspectiveViewport::InteractionModeExt);
    const PerspectiveViewport::InteractionModeExt
        PerspectiveViewport::InteractionModeExt::CAMERA_TRANSLATE("CAMERA_TRANSLATE");
    const PerspectiveViewport::InteractionModeExt
        PerspectiveViewport::InteractionModeExt::CAMERA_LOOK("CAMERA_LOOK");
    const PerspectiveViewport::InteractionModeExt
        PerspectiveViewport::InteractionModeExt::CAMERA_NAVIGATE("CAMERA_NAVIGATE");
    const PerspectiveViewport::InteractionModeExt
        PerspectiveViewport::InteractionModeExt::ACTOR_AXIS_X("ACTOR_AXIS_X");
    const PerspectiveViewport::InteractionModeExt
        PerspectiveViewport::InteractionModeExt::ACTOR_AXIS_Y("ACTOR_AXIS_Y");
    const PerspectiveViewport::InteractionModeExt
        PerspectiveViewport::InteractionModeExt::ACTOR_AXIS_Z("ACTOR_AXIS_Z");
    const PerspectiveViewport::InteractionModeExt
        PerspectiveViewport::InteractionModeExt::NOTHING("NOTHING");
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    PerspectiveViewport::PerspectiveViewport(const std::string &name, QWidget *parent,
        QGLWidget *shareWith) :
            Viewport(ViewportManager::ViewportType::PERSPECTIVE,name,parent,shareWith)
    {
        this->currentMode = &InteractionModeExt::NOTHING;
        this->camera = ViewportManager::getInstance().getWorldViewCamera();
        this->camera->setFarClipPlane(250000.0f);
        setMoveActorWithCamera(EditorData::getInstance().getRigidCamera());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::resizeGL(int width, int height)
    {
        if (height == 0)
            height = 1;

        getCamera()->setAspectRatio((double)width / (double)height);
        Viewport::resizeGL(width,height);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::initializeGL()
    {
        Viewport::initializeGL();
        setRenderStyle(Viewport::RenderStyle::TEXTURED,false);    
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::mousePressEvent(QMouseEvent *e)
    {
        Qt::KeyboardModifiers shiftAndControl = Qt::ControlModifier | Qt::ShiftModifier;

        if (getInteractionMode() == Viewport::InteractionMode::CAMERA) {
            if (getEnableKeyBindings()) {
                if (e->modifiers() == Qt::ShiftModifier) {
                    if (getMoveActorWithCamera()) {
                        beginCameraMode(e);
                    }
                    else {
                        setActorTranslateMode();
                        beginActorMode(e);
                    }
                }
                else if (e->modifiers() == Qt::AltModifier) {
                    setActorRotateMode();
                    beginActorMode(e);
                }
                else if (e->modifiers() == Qt::ControlModifier) {
                    setActorSelectMode();
                    ViewportManager::getInstance().getViewportOverlay()->setMultiSelectMode(false);
                    if (e->button() == Qt::LeftButton)
                        pick(e->pos().x(),e->pos().y());
                }
                else if (e->modifiers() == shiftAndControl) {
                    setActorSelectMode();
                    ViewportManager::getInstance().getViewportOverlay()->setMultiSelectMode(true);
                    if (e->button() == Qt::LeftButton)
                        pick(e->pos().x(),e->pos().y());
                    ViewportManager::getInstance().getViewportOverlay()->setMultiSelectMode(false);
                }
                else {
                    beginCameraMode(e);
                }
            }
            else {
                beginCameraMode(e);
            }
        }
        else if (getInteractionMode() == Viewport::InteractionMode::SELECT_ACTOR) {
            if (e->modifiers() == Qt::ControlModifier)
                ViewportManager::getInstance().getViewportOverlay()->setMultiSelectMode(true);
            else
                ViewportManager::getInstance().getViewportOverlay()->setMultiSelectMode(false);

            if (e->button() == Qt::LeftButton)
                pick(e->pos().x(),e->pos().y());
        }
        else {
            beginActorMode(e);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::mouseReleaseEvent(QMouseEvent *e)
    {
        if (getInteractionMode() != Viewport::InteractionMode::SELECT_ACTOR) {
            if (getInteractionMode() == Viewport::InteractionMode::CAMERA) {
                endCameraMode(e);
            }
            else {
               endActorMode(e);
            }
        }
        else {
            syncWithModeActions();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::mouseMoveEvent(QMouseEvent *e)
    {
        static bool mouseMoving = false;
        //Moving the mouse back to the center makes the movement recurse
        //so this is a flag to prevent the recursion
        if (mouseMoving)
            return;

        QPoint center((x()+width())/2,(y()+height())/2);
        float dx,dy;

        dx = (float)(e->pos().x() - center.x());
        dy = (float)(e->pos().y() - center.y());

        if (dx != 0 || dy != 0)
        {
            if (getInteractionMode() == Viewport::InteractionMode::SELECT_ACTOR)
            {
                return;
            }
            else if (getInteractionMode() == Viewport::InteractionMode::CAMERA)
            {
                if (*this->currentMode == InteractionModeExt::NOTHING || getCamera() == NULL)
                    return;

                moveCamera(dx,dy);
            }
            else if (getInteractionMode() == Viewport::InteractionMode::TRANSLATE_ACTOR)
            {
               translateCurrentSelection(e,dx,dy);
            }
            else if (getInteractionMode() == Viewport::InteractionMode::ROTATE_ACTOR)
            {
                rotateCurrentSelection(e,dx,dy);
            }

            //Moving the mouse back to the center makes the movement recurse
            //so this is a flag to prevent the recursion
            mouseMoving = true;
            QCursor::setPos(mapToGlobal(center));
            mouseMoving = false;

            updateGL();
        }

    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::beginCameraMode(QMouseEvent *e)
    {
        Qt::MouseButtons bothButtons = Qt::LeftButton | Qt::RightButton;

        if (getMoveActorWithCamera() && getEnableKeyBindings() &&
            e->modifiers() == Qt::ShiftModifier &&
            this->currentMode == &InteractionModeExt::NOTHING)
        {
            attachCurrentSelectionToCamera();
            saveSelectedActorOrigValues("Translation");
            saveSelectedActorOrigValues("Rotation");
        }

        if (e->buttons() == bothButtons || e->buttons() == Qt::MidButton)
            this->currentMode = &InteractionModeExt::CAMERA_TRANSLATE;
        else if (e->button() == Qt::LeftButton)
            this->currentMode = &InteractionModeExt::CAMERA_NAVIGATE;
        else if (e->button() == Qt::RightButton)
            this->currentMode = &InteractionModeExt::CAMERA_LOOK;
        else
        {
            this->currentMode = &InteractionModeExt::NOTHING;
            return;
        }

        setCameraMode();
        trapMouseCursor();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::endCameraMode(QMouseEvent *e)
    {
        if (e->button() == Qt::LeftButton && e->buttons() == Qt::RightButton) {
            this->currentMode = &InteractionModeExt::CAMERA_LOOK;
        }
        else if (e->button() == Qt::RightButton && e->buttons() == Qt::LeftButton) {
            this->currentMode = &InteractionModeExt::CAMERA_NAVIGATE;
        }
        else {
            this->currentMode = &InteractionModeExt::NOTHING;
            releaseMouseCursor();
            syncWithModeActions();
            if (getMoveActorWithCamera() && getCamera() != NULL &&
                getEnableKeyBindings() && getCamera()->getNumActorAttachments() != 0)
            {
                // we could send hundreds of translation and rotation events, so make sure 
                // we surround it in a change transaction
                EditorEvents::getInstance().emitBeginChangeTransaction();

                updateActorSelectionProperty("Translation");
                updateActorSelectionProperty("Rotation");

                EditorEvents::getInstance().emitEndChangeTransaction();

                getCamera()->removeAllActorAttachments();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::beginActorMode(QMouseEvent *e)
    {
        Qt::MouseButtons bothButtons = Qt::LeftButton | Qt::RightButton;

        if (e->buttons() == bothButtons || e->buttons() == Qt::MidButton) {
            this->currentMode = &InteractionModeExt::ACTOR_AXIS_Y;
        }
        else if (e->button() == Qt::LeftButton) {
            this->currentMode = &InteractionModeExt::ACTOR_AXIS_X;
        }
        else if (e->button() == Qt::RightButton) {
            this->currentMode = &InteractionModeExt::ACTOR_AXIS_Z;
        }
        else {
            this->currentMode = &InteractionModeExt::NOTHING;
            return;
        }

        // Save the original values of trans/rotate so undo/redo can track it.
        if (getInteractionMode() == Viewport::InteractionMode::TRANSLATE_ACTOR)
            saveSelectedActorOrigValues("Translation");
        else if (getInteractionMode() == Viewport::InteractionMode::ROTATE_ACTOR)
            saveSelectedActorOrigValues("Rotation");

        trapMouseCursor();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::endActorMode(QMouseEvent *e)
    {
        if (e->button() == Qt::LeftButton && e->buttons() == Qt::RightButton) {
            this->currentMode = &InteractionModeExt::ACTOR_AXIS_Z;
        }
        else if (e->button() == Qt::RightButton && e->buttons() == Qt::LeftButton) {
            this->currentMode = &InteractionModeExt::ACTOR_AXIS_X;
        }
        else {
            this->currentMode = &InteractionModeExt::NOTHING;
            releaseMouseCursor();

            // we could send hundreds of translation and rotation events, so make sure 
            // we surround it in a change transaction
            EditorEvents::getInstance().emitBeginChangeTransaction();

            //Update the selected actor proxies with their new values.
            if (getInteractionMode() == Viewport::InteractionMode::TRANSLATE_ACTOR)
                updateActorSelectionProperty("Translation");
            else if (getInteractionMode() == Viewport::InteractionMode::ROTATE_ACTOR)
                updateActorSelectionProperty("Rotation");

            EditorEvents::getInstance().emitEndChangeTransaction();

            //If a modifier key was pressed the current interaction mode was
            //temporarily overridden, so make sure we restore the previous mode.
            syncWithModeActions();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::moveCamera(float dx, float dy)
    {
        if (*this->currentMode == InteractionModeExt::CAMERA_NAVIGATE) {
            getCamera()->yaw(-dx / 10.0f);

            //Move along the view direction, however, ignore the z-axis.  This way
            //we can look at the ground but move parallel to it.
            osg::Vec3 viewDir = getCamera()->getViewDir() * ((float)-dy/getMouseSensitivity());
            viewDir[2] = 0.0f;
            getCamera()->move(viewDir);
        }
        else if (*this->currentMode == InteractionModeExt::CAMERA_LOOK) {
            getCamera()->pitch(-dy / 10.0f);
            getCamera()->yaw(-dx / 10.0f);
        }
        else if (*this->currentMode == InteractionModeExt::CAMERA_TRANSLATE) {
            getCamera()->move(getCamera()->getUpDir() *
                              (-dy / getMouseSensitivity()));
            getCamera()->move(getCamera()->getRightDir() *
                              (dx / getMouseSensitivity()));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::attachCurrentSelectionToCamera()
    {
        ViewportOverlay::ActorProxyList::iterator itor;
        ViewportOverlay::ActorProxyList &selection =
            ViewportManager::getInstance().getViewportOverlay()->getCurrentActorSelection();

        if (getCamera() == NULL)
            return;

        for (itor=selection.begin(); itor!=selection.end(); ++itor)
        {
            dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
            dtDAL::TransformableActorProxy *tProxy =
                dynamic_cast<dtDAL::TransformableActorProxy *>(proxy);

            if (tProxy != NULL) {
                getCamera()->attachActorProxy(tProxy);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::translateCurrentSelection(QMouseEvent *e, float dx, float dy)
    {
        osg::Vec3 trans;
        ViewportOverlay::ActorProxyList::iterator itor;
        ViewportOverlay::ActorProxyList &selection =
                ViewportManager::getInstance().getViewportOverlay()->getCurrentActorSelection();

        trans.set(0,0,0);
        if (*this->currentMode == InteractionModeExt::ACTOR_AXIS_X)
            trans[0] = -dy / getMouseSensitivity();
        else if (*this->currentMode == InteractionModeExt::ACTOR_AXIS_Y)
            trans[1] = -dy / getMouseSensitivity();
        else if (*this->currentMode == InteractionModeExt::ACTOR_AXIS_Z)
            trans[2] = -dy / getMouseSensitivity();

        osg::Vec3 currTrans;
        for (itor=selection.begin(); itor!=selection.end(); ++itor) {
            dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
            dtDAL::TransformableActorProxy *tProxy =
                dynamic_cast<dtDAL::TransformableActorProxy *>(proxy);

            if (tProxy != NULL) {
                currTrans = tProxy->GetTranslation();
                currTrans += trans;
                tProxy->SetTranslation(currTrans);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::rotateCurrentSelection(QMouseEvent *e, float dx, float dy)
    {
        ViewportOverlay::ActorProxyList &selection =
                ViewportManager::getInstance().getViewportOverlay()->getCurrentActorSelection();
        ViewportOverlay::ActorProxyList::iterator itor;

        for (itor=selection.begin(); itor!=selection.end(); ++itor) {
            dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
            dtDAL::TransformableActorProxy *tProxy =
                    dynamic_cast<dtDAL::TransformableActorProxy *>(proxy);

            if (tProxy != NULL)
            {
                osg::Vec3 hpr = tProxy->GetRotation();
                float delta = dy / getMouseSensitivity();

                if (*this->currentMode == InteractionModeExt::ACTOR_AXIS_X)
                    hpr.x() += delta;
                else if (*this->currentMode == InteractionModeExt::ACTOR_AXIS_Y)
                    hpr.y() += delta;
                else if (*this->currentMode == InteractionModeExt::ACTOR_AXIS_Z)
                    hpr.z() += delta;

                tProxy->SetRotation(hpr);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy> proxy)
    {
        dtCore::Scene *scene = ViewportManager::getInstance().getMasterScene();
        dtDAL::ActorProxyIcon *billBoard = NULL;

        const dtDAL::ActorProxy::RenderMode &renderMode = proxy->GetRenderMode();
        if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
        {
            billBoard = proxy->GetBillBoardIcon();
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

        //Get the current position and direction the camera is facing.
        osg::Vec3 pos = getCamera()->getPosition();
        osg::Vec3 viewDir = getCamera()->getViewDir();

        //If the object is a transformable (can have a position in the scene)
        //add it to the scene in front of the camera.
        dtDAL::TransformableActorProxy *tProxy =
            dynamic_cast<dtDAL::TransformableActorProxy *>(proxy.get());

        if (tProxy != NULL)
        {
            const osg::BoundingSphere &bs = tProxy->GetActor()->GetOSGNode()->getBound();

            //Position it along the camera's view direction.  The distance from
            //the camera is the object's bounding volume so it appears
            //just in front of the camera.  If the object is very large, it is
            //just created at the origin.
            float offset = (bs.radius() < 1000.0f) ? bs.radius() : 1.0f;
            if (offset <= 0.0f)
                offset = 10.0f;
            tProxy->SetTranslation(pos+(viewDir*offset*2));
        }

        // update the viewports unless we're getting lots of changes back to back, in which
        // case our super class handles that.
        if (!inChangeTransaction)
            ViewportManager::getInstance().refreshAllViewports();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::onEditorPreferencesChanged()
    {
        this->attachActorToCamera = EditorData::getInstance().getRigidCamera();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::connectInteractionModeSlots()
    {
        Viewport::connectInteractionModeSlots();

        EditorEvents *editorEvents = &EditorEvents::getInstance();
        connect(editorEvents, SIGNAL(actorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>)),
                this,SLOT(onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>)));

        connect(editorEvents, SIGNAL(editorPreferencesChanged()),
                this,SLOT(onEditorPreferencesChanged()));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::disconnectInteractionModeSlots()
    {
        Viewport::disconnectInteractionModeSlots();

        EditorEvents *editorEvents = &EditorEvents::getInstance();
        disconnect(editorEvents,SIGNAL(actorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>)),
                    this,SLOT(onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>)));

        disconnect(editorEvents, SIGNAL(editorPreferencesChanged()),
                this,SLOT(onEditorPreferencesChanged()));
    }
}
