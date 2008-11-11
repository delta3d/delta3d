/* -*-c++-*-
* Delta3D Simulation Training And Game Editor (STAGE)
* STAGE - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
* 
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* Matthew W. Campbell
*/
#include <prefix/dtstageprefix-src.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QAction>
#include <sstream>
#include <osg/Billboard>
#include <osgDB/WriteFile>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editoractions.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/actorproxyicon.h>

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
        this->camera = ViewportManager::GetInstance().getWorldViewCamera();
        this->camera->setFarClipPlane(250000.0f);
        setMoveActorWithCamera(EditorData::GetInstance().getRigidCamera());
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
        setFocus();
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
                    ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(false);
                    if (e->button() == Qt::LeftButton)
                        pick(e->pos().x(),e->pos().y());
                }
                else if (e->modifiers() == shiftAndControl) {
                    setActorSelectMode();
                    ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(true);
                    if (e->button() == Qt::LeftButton)
                        pick(e->pos().x(),e->pos().y());
                    ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(false);
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
                ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(true);
            else
                ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(false);

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
    void PerspectiveViewport::onMouseMoveEvent(QMouseEvent* e, float dx, float dy)
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
            saveSelectedActorOrigValues(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);
            saveSelectedActorOrigValues(dtDAL::TransformableActorProxy::PROPERTY_ROTATION);
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
                EditorEvents::GetInstance().emitBeginChangeTransaction();

                updateActorSelectionProperty(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);
                updateActorSelectionProperty(dtDAL::TransformableActorProxy::PROPERTY_ROTATION);

                EditorEvents::GetInstance().emitEndChangeTransaction();

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
            saveSelectedActorOrigValues(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);
        else if (getInteractionMode() == Viewport::InteractionMode::ROTATE_ACTOR)
            saveSelectedActorOrigValues(dtDAL::TransformableActorProxy::PROPERTY_ROTATION);

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
            EditorEvents::GetInstance().emitBeginChangeTransaction();

            //Update the selected actor proxies with their new values.
            if (getInteractionMode() == Viewport::InteractionMode::TRANSLATE_ACTOR)
                updateActorSelectionProperty(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);
            else if (getInteractionMode() == Viewport::InteractionMode::ROTATE_ACTOR)
                updateActorSelectionProperty(dtDAL::TransformableActorProxy::PROPERTY_ROTATION);

            EditorEvents::GetInstance().emitEndChangeTransaction();

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
            ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();

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
                ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();

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
                ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
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
    void PerspectiveViewport::onEditorPreferencesChanged()
    {
        this->attachActorToCamera = EditorData::GetInstance().getRigidCamera();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::connectInteractionModeSlots()
    {
        Viewport::connectInteractionModeSlots();
        EditorEvents *editorEvents = &EditorEvents::GetInstance();

        connect(editorEvents, SIGNAL(editorPreferencesChanged()),
                this,SLOT(onEditorPreferencesChanged()));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void PerspectiveViewport::disconnectInteractionModeSlots()
    {
        Viewport::disconnectInteractionModeSlots();
        EditorEvents *editorEvents = &EditorEvents::GetInstance();

        disconnect(editorEvents, SIGNAL(editorPreferencesChanged()),
                this,SLOT(onEditorPreferencesChanged()));
    }
}
