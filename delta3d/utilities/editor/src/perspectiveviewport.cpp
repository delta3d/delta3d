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
    class PerspBillBoardUpdateVisitor : public osg::NodeVisitor
    {
    public:
        PerspBillBoardUpdateVisitor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) { }

        virtual void apply(osg::Billboard &billBoard) {
            billBoard.setMode(osg::Billboard::POINT_ROT_WORLD);
            billBoard.setAxis(osg::Vec3(0,0,1));
            billBoard.setNormal(osg::Vec3(0,-1,0));
            traverse(billBoard);
        }
    };
    ///////////////////////////////////////////////////////////////////////////////

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
        setMoveActorWithCamera(false);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::resizeGL(int width, int height)
    {
        if (height == 0)
            height = 1;

        getCamera()->setAspectRatio((float)width / (float)height);
        Viewport::resizeGL(width,height);

    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::initializeGL()
    {
        Viewport::initializeGL();
        setRenderStyle(Viewport::RenderStyle::TEXTURED,false);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::renderFrame()
    {
        //Make sure the billboards are oriented properly for this view before
        //rendering.
        PerspBillBoardUpdateVisitor bv;
        getScene()->GetSceneNode()->accept(bv);

        Viewport::renderFrame();
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
        QPoint center((x()+width())/2,(y()+height())/2);
        float dx,dy;

        dx = (float)(e->pos().x() - center.x());
        dy = (float)(e->pos().y() - center.y());

        if (getInteractionMode() == Viewport::InteractionMode::SELECT_ACTOR) {
            return;
        }
        else if (getInteractionMode() == Viewport::InteractionMode::CAMERA)
        {
            if (*this->currentMode == InteractionModeExt::NOTHING || getCamera() == NULL)
                return;

            if (getMoveActorWithCamera() && e->modifiers() == Qt::ShiftModifier) {
                moveCameraAndActor(dx,dy);
            }
            else {
                moveCamera(dx,dy);
            }
        }
        else if (getInteractionMode() == Viewport::InteractionMode::TRANSLATE_ACTOR) {
           translateCurrentSelection(e,dx,dy);
        }
        else if (getInteractionMode() == Viewport::InteractionMode::ROTATE_ACTOR) {
            rotateCurrentSelection(e,dx,dy);
        }

        QCursor::setPos(mapToGlobal(center));
        updateGL();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::beginCameraMode(QMouseEvent *e)
    {
        Qt::MouseButtons bothButtons = Qt::LeftButton | Qt::RightButton;

        if (e->buttons() == bothButtons || e->buttons() == Qt::MidButton) {
            this->currentMode = &InteractionModeExt::CAMERA_TRANSLATE;
        }
        else if (e->button() == Qt::LeftButton) {
            this->currentMode = &InteractionModeExt::CAMERA_NAVIGATE;
        }
        else if (e->button() == Qt::RightButton) {
            this->currentMode = &InteractionModeExt::CAMERA_LOOK;
        }
        else {
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

            //Update the selected actor proxies with their new values.
            if (getInteractionMode() == Viewport::InteractionMode::TRANSLATE_ACTOR)
                updateActorSelectionProperty("Translation");
            else if (getInteractionMode() == Viewport::InteractionMode::ROTATE_ACTOR)
                updateActorSelectionProperty("Rotation");

            //If a modifier key was pressed the current interaction mode was
            //temporarily overridden, so make sure we restore the previous mode.
            syncWithModeActions();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::moveCamera(float dx, float dy)
    {
        if (*this->currentMode == InteractionModeExt::CAMERA_NAVIGATE) {
            getCamera()->yaw(-dx / getMouseSensitivity());

            //Move along the view direction, however, ignore the z-axis.  This way
            //we can look at the ground but move parallel to it.
            osg::Vec3 viewDir = getCamera()->getViewDir() * ((float)-dy/getMouseSensitivity());
            viewDir[2] = 0.0f;
            getCamera()->move(viewDir);
        }
        else if (*this->currentMode == InteractionModeExt::CAMERA_LOOK) {
            getCamera()->pitch(-dy / getMouseSensitivity());
            getCamera()->yaw(-dx / getMouseSensitivity());
        }
        else if (*this->currentMode == InteractionModeExt::CAMERA_TRANSLATE) {
            getCamera()->move(osg::Vec3(0,0,1) *
                              (-dy / getMouseSensitivity()));
            getCamera()->move(getCamera()->getRightDir() *
                              (dx / getMouseSensitivity()));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::moveCameraAndActor(float dx, float dy)
    {
        //Get the old position of the camera so we have a movement delta.
        osg::Vec3 oldCamLook,newCamLook;
        osg::Quat rotDelta;
        osg::Vec3 oldCamPos,transDelta;
        osg::Matrix rotDeltaMat;

        oldCamLook = getCamera()->getViewDir();
        oldCamPos = getCamera()->getPosition();

        moveCamera(dx,dy);

        newCamLook = getCamera()->getViewDir();
        rotDelta.makeRotate(oldCamLook,newCamLook);
        rotDelta.get(rotDeltaMat);
        transDelta = getCamera()->getPosition()-oldCamPos;

        //Now get the current actor selection and move it relative to the
        //camera's position and orientation.
        ViewportOverlay::ActorProxyList::iterator itor;
        ViewportOverlay::ActorProxyList &selection =
                ViewportManager::getInstance().getViewportOverlay()->getCurrentActorSelection();

        for (itor=selection.begin(); itor!=selection.end(); ++itor) 
        {
            dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
            dtDAL::TransformableActorProxy *tProxy = dynamic_cast<dtDAL::TransformableActorProxy *>(proxy);
            dtCore::Transformable *transformable =
                dynamic_cast<dtCore::Transformable *>(proxy->GetActor());

            if (tProxy != NULL && transformable != NULL)
            {
                dtCore::Transform tx;
                osg::Vec3 toObjFromCam = tProxy->GetTranslation() - getCamera()->getPosition();


                osg::Matrix objRotMat;
                osg::Matrix deltaMat;
                osg::Quat objRotQuat,camQuat;
                osg::Vec3 currTrans;
                osg::Matrix txMat;

                transformable->GetTransform(&tx);
                tx.Get(txMat);

//                 osg::Matrix mat;
//                 osg::Matrix offset = osg::Matrix::translate(getCamera()->getPosition());
//                mat.set(getCamera()->getWorldViewMatrix());

//                 mat.postMult(txMat);

                osg::Matrix camMat;

                camMat.set(getCamera()->getOrientation().inverse());
                tx.GetRotation(objRotMat);
                camMat.postMult(objRotMat);

                //tx.Set(mat);

                tx.Set(camMat);
                transformable->SetTransform(&tx);

//                 camMat.set(rotDelta);
//                 tx.GetRotation(objRotMat);
//                 objRotMat *= camMat;


//                 transformable->GetTransform(&tx);
//                 tx.GetRotation(objRotMat);
//                 objRotMat.get(objRotQuat);
//                 objRotQuat *= rotDelta;
//                 objRotMat.set(objRotQuat);
//                 tx.SetRotation(objRotMat);
//                 camQuat = getCamera()->getOrientation();
//
//                 objRotQuat = objRotQuat - camQuat;
//                 objRotMat.set(objRotQuat);

  /*
                currTrans = tProxy->getTranslation();
                currTrans -= getCamera()->getPosition();
                currTrans = rotDelta*currTrans;
                currTrans += getCamera()->getPosition();
                currTrans += transDelta;
                currTrans -= getCamera()->getPosition();*/

                //tx.SetTranslation(currTrans);
                //tx.SetRotation(objRotMat);

//                 tx.GetRotation(currRotation);
//                 currRotation = objRotMat;
//                 tx.SetRotation(currRotation);


                //transformable->SetTransform(&tx);

//                 currTrans = tProxy->getTranslation();
//                 currTrans -= getCamera()->getPosition();
//                 currTrans = rotDelta*currTrans;
//                 currTrans += getCamera()->getPosition();
//                 currTrans += transDelta;
//                 tProxy->setTranslation(currTrans);
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
            dtDAL::TransformableActorProxy *tProxy = dynamic_cast<dtDAL::TransformableActorProxy *>(proxy);

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

        //Current actor rotation mode only supports rotating one actor so if a group of
        //actors is selected, rotation does nothing.
        if (selection.size() != 1) {
            return;
        }

        osg::Matrix rot,currRotation;
        osg::Vec3 axis;
        dtCore::Transform tx;
        ViewportOverlay::ActorProxyList::iterator itor;

        if (*this->currentMode == InteractionModeExt::ACTOR_AXIS_X)
            axis = osg::Vec3(1,0,0);
        else if (*this->currentMode == InteractionModeExt::ACTOR_AXIS_Y)
            axis = osg::Vec3(0,1,0);
        else if (*this->currentMode == InteractionModeExt::ACTOR_AXIS_Z)
            axis = osg::Vec3(0,0,1);

        for (itor=selection.begin(); itor!=selection.end(); ++itor) {
            dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
            dtCore::Transformable *transformable =
                dynamic_cast<dtCore::Transformable *>(proxy->GetActor());

            if (transformable != NULL)
            {
                //Get the current rotation.
                transformable->GetTransform(&tx);
                tx.GetRotation(currRotation);

                //Now multiply our axis of rotation by the current orientation of
                //the actor to put the rotation axis in the actors coordinate space.
                axis = axis*currRotation;
                rot.makeRotate(dy/(getMouseSensitivity()*4),axis);

                //Rotate the actor and set its new rotation.
                currRotation *= rot;
                tx.SetRotation(currRotation);
                transformable->SetTransform(&tx);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy> proxy)
    {
        dtCore::Scene *scene = ViewportManager::getInstance().getMasterScene();
        dtDAL::ActorProxyIcon *billBoard = proxy->GetBillBoardIcon();

        const dtDAL::ActorProxy::RenderMode &renderMode = proxy->GetRenderMode();
        if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
        {
            if (billBoard == NULL) 
            {
                LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
            }
            else 
            {
                scene->AddDrawable(billBoard->GetDrawable());
            }
        }
        else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR) 
        {
            scene->AddDrawable(proxy->GetActor());
        }
        else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON) 
        {
            scene->AddDrawable(proxy->GetActor());

            if (billBoard == NULL) 
            {
                LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
            }
            else 
            {
                scene->AddDrawable(billBoard->GetDrawable());
            }
        }
        else 
        {
            //If we got here, then the proxy wishes the system to determine how to display
            //the proxy.
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

        ViewportManager::getInstance().refreshAllViewports();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::connectInteractionModeSlots()
    {
        Viewport::connectInteractionModeSlots();
        connect(&EditorEvents::getInstance(),
                SIGNAL(actorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>)),
                this,SLOT(onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>)));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::disconnectInteractionModeSlots()
    {
        Viewport::disconnectInteractionModeSlots();

        disconnect(&EditorEvents::getInstance(),
                    SIGNAL(actorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>)),
                    this,SLOT(onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy>)));
    }
}
