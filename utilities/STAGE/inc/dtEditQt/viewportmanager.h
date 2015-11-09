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

#ifndef DELTA_VIEWPORTMANAGER
#define DELTA_VIEWPORTMANAGER

#include <QtCore/QObject>
#include <map>
#include <osg/Referenced>
#include <dtCore/scene.h>
#include <dtCore/uniqueid.h>
#include <dtCore/deltadrawable.h>
#include <dtUtil/enumeration.h>
#include <dtCore/actorproxy.h>
#include <dtQt/typedefs.h>
#include <dtABC/application.h>
#include "export.h"

class QWidget;
class QMouseEvent;
class QWheelEvent;

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class GraphicsContext;
}
/// @endcond

namespace dtEditQt
{
   class Viewport;
   class ViewportOverlay;
   class StageCamera;

   /**
    * The viewport manager is responsible for managing all of the viewports in a given scene.
    * The viewport manager can create an orthographic viewport or a perspective (3D) viewport.
    * If an orthographic viewport is created, it can then be set to display either a top,
    * front, or side view.
    *  @note
    *  The viewport manager also stores the "master scene".  That is, the Delta3D scene
    *  that is being edited is stored in the viewport manager.  By default, a newly created
    *  viewport is assigned to this master scene.  If this is not the desired behavior,
    *  merely assign a new scene to the viewport after it is created.
    *  @note
    *  By default, the viewport manager will share the graphics context of the first
    *  viewport that is created with all others.  This is done to optimize memory usage.
    *  If this behavior is not desired, calling enableContextSharing(false) before creating
    *  any viewports will disable context sharing.
    */
   class DT_EDITQT_EXPORT ViewportManager : public QObject, public osg::Referenced
   {
      Q_OBJECT

   public:
      /**
       * Simple enumeration of the types of viewports the manager can create.
       */
      class DT_EDITQT_EXPORT ViewportType : public dtUtil::Enumeration
      {
         DECLARE_ENUM(ViewportType);

      public:
         static ViewportType ORTHOGRAPHIC;
         static ViewportType PERSPECTIVE;

      private:
         ViewportType(const std::string& name)
            : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };

      /**
       * Creates a new viewport of the given type.  The master scene stored in the manager
       * is assigned to it by default.
       * @param name Unique name given to the viewport.
       * @param type Type of viewport to create.
       * @param parent Parent widget in which the newly created viewport will reside.
       * @return The newly created viewport.
       */
      Viewport* createViewport(const std::string& name, ViewportType& type,
         QWidget* parent = NULL);

      /**
       * Gets a handle to the master view.
       */
      Viewport* GetMasterViewport() { return mMasterViewport;}

      /**
       * Gets a handle to the master scene.  Once the handle is retrieved, use it to
       * add/remove objects from the scene.
       * @return A pointer to the master scene.
       */
      dtCore::Scene* getMasterScene() { return mMasterScene.get(); }

      /**
       * Gets a handle to the world view camera.  The 3D perspective view uses
       * the world view camera to render its scene.
       * @return A pointer to the world view camera.
       */
      StageCamera* getWorldViewCamera() { return mWorldCamera.get(); }

      /**
      * Sets a handle to the world view camera.
      *
      * @param A pointer to the perspective camera.
      */
      void setWorldViewCamera(StageCamera* camera);

      void SetApplication(dtABC::Application* app);
      dtABC::Application* GetApplication() const;

      /**
       * Moves an actor such that it is placed in from of the world view camera.
       * @param proxy The proxy to place.
       */
      void placeProxyInFrontOfCamera(dtCore::BaseActorObject* proxy);

      /**
       * Gets the instance of an overlay object that should be shared between viewports.
       * It stores editor specific scene graph components that should be common amoung
       * the viewports. For example, when an object is selected, it should appear selected
       * in all viewports.
       * @return ViewportOverlay
       */
      ViewportOverlay* getViewportOverlay() { return mViewportOverlay.get(); }

      /**
      * Sets the last drawable picked.
      *
      * @param[in]  drawable  The picked drawable.
      */
      void setLastDrawable(dtCore::DeltaDrawable* drawable) { mLastDrawable = drawable; }

      /**
      * Gets the last drawable picked.
      */
      dtCore::DeltaDrawable* getLastDrawable() { return mLastDrawable.get(); }

      /**
      * Sets the last pick position.
      *
      * @param[in]  position  The pick position.
      */
      void setLastPickPosition(const osg::Vec3& position) { mLastPickPosition = position; }

      /**
      * Gets the last pick position.
      */
      osg::Vec3 getLastPickPosition() { return mLastPickPosition; }

      /**
       * Gets a reference to the static singleton instance of the viewport manager.
       * @return
       */
      static ViewportManager& GetInstance();

      /**
       * Disables or enables OpenGL context sharing.  This should be called
       * before any viewports
       * are created.  By default, context sharing is enabled.
       * @param share If true, enable sharing.
       */
      void enableContextSharing(bool share) { mShareMasterContext = share; }

      /**
       * Removes all drawables from the master scene.
       */
      void clearMasterScene(const std::map< dtCore::UniqueId,
         dtCore::RefPtr<dtCore::BaseActorObject> >& proxies);

      /**
       * Called from the first initialized viewport.  This method sets various
       * statistics about the current OpenGL implementation.  For example, the number
       * of texture units available.
       */
      void initializeGL();


      /**
      * Retrieves the nearest snap position to the given position.
      *
      * @param[in]  position         The original position.
      * @param[in]  groundClamp      Ground clamps the translation.
      * @param[in]  ignoredDrawable  Any drawable to ignore ground clamping with.
      *
      * @return                      The snapped position.
      */
      osg::Vec3 GetSnapPosition(osg::Vec3 position, bool groundClamp = false, std::vector<dtCore::DeltaDrawable*> ignoredDrawables = std::vector<dtCore::DeltaDrawable*>());

      /**
      * Accessors for all snap settings.
      */
      bool GetSnapTranslationEnabled() { return mSnapTranslationEnabled; }
      bool GetSnapRotationEnabled() { return mSnapRotationEnabled; }
      bool GetSnapScaleEnabled() { return mSnapScaleEnabled; }

      float GetSnapTranslation() { return mSnapTranslation; }
      float GetSnapRotation() { return mSnapRotation; }
      float GetSnapScale() { return mSnapScale; }

      /**
      * Saves the current camera position into a preset camera.
      *
      * @param[in]  index  The index of the preset to save to.
      */
      void SavePresetCamera(int index);

      /**
      * Loads a previous preset camera position.
      *
      * @param[in]  index  The index of the preset to load.
      */
      void LoadPresetCamera(int index);

      /**
      * Signal used when a viewport has been enabled.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   enabled          Whether the viewport is being enabled or not.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void emitViewportEnabled(Viewport* vp, bool enabled, bool* overrideDefault = NULL);

      /**
      * Signal used when a mouse has been pressed in a viewport.
      *
      * @param[in]  vp  The viewport triggering this event.
      * @param[in]  e   The mouse event.
      */
      void emitMousePressEvent(Viewport* vp, QMouseEvent* e);

      /**
      * Signal used when a mouse has been released in a viewport.
      *
      * @param[in]  vp  The viewport triggering this event.
      * @param[in]  e   The mouse event.
      */
      void emitMouseReleaseEvent(Viewport* vp, QMouseEvent* e);

      /**
      * Signal used when the mouse has been double clicked.
      *
      * @param[in]  vp  The viewport triggering this event.
      * @param[in]  e   The mouse event.
      */
      void emitMouseDoubleClickEvent(Viewport* vp, QMouseEvent* e);

      /**
      * Signal used when a mouse has been moved in a viewport.
      *
      * @param[in]  vp  The viewport triggering this event.
      * @param[in]  e   The mouse event.
      */
      void emitMouseMoveEvent(Viewport* vp, QMouseEvent* e);

      /**
      * Signal used when mouse wheel has been moved in a viewport.
      *
      * @param[in]  vp  The viewport triggering this event.
      * @param[in]  e   The mouse wheel event.
      */
      void emitWheelEvent(Viewport* vp, QWheelEvent* e);

      /**
      * Signal used when testing whether actor mode should be activated.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   position         The position of the mouse click.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      * @param[out]  result           The result of the test if it is overridden.
      */
      void emitShouldBeginActorMode(Viewport* vp, osg::Vec2 position, bool* overrideDefault = NULL, bool* result = NULL);

      /**
      * Signal used when actor mode is begun.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   e                The mouse event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void emitBeginActorMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault = NULL);

      /**
      * Signal used when actor mode is ended.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   e                The mouse event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void emitEndActorMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault = NULL);

      /**
      * Signal used when camera mode is begun.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   e                The mouse event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void emitBeginCameraMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault = NULL);

      /**
      * Signal used when camera mode is ended.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   e                The mouse event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void emitEndCameraMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault = NULL);

      ///**
      //* Signal used when the camera is moved.
      //*
      //* @param[in]   vp               The viewport triggering this event.
      //* @param[in]   dx               Horizontal delta movement.
      //* @param[in]   dy               Vertical delta movement.
      //* @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      //*/
      //void emitMoveCamera(Viewport* vp, float dx, float dy, bool* overrideDefault = NULL);

      /**
      * Signal used when to select actors.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   e                The mouse event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void emitSelectActors(Viewport* vp, QMouseEvent* e, bool* overrideDefault = NULL);

      /**
      * Signal used when duplicating the current selection.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void emitDuplicateActors(Viewport* vp, bool* overrideDefault = NULL);

      /**
      * Sets the translation snap increment.
      *
      * @param[in]  increment  The size of the increment.
      */
      void emitSetSnapTranslation(float increment);

      /**
      * Sets the angular snap increment.
      *
      * @param[in]  increment  The size of the increment.
      */
      void emitSetSnapRotation(float increment);

      /**
      * Sets the scalar snap increment.
      *
      * @param[in]  increment  The size of the increment.
      */
      void emitSetSnapScale(float increment);

      /**
      * Sets the enabled status of each snap tool.
      */
      void emitSetSnapEnabled(bool translation, bool rotation, bool scale);

      /**
      * Modifies the list of properties to be displayed in the property editor.
      *
      * @param[in]  propertyContainer  The property container.
      * @param[in]  propList           The list of properties to be displayed.
      */
      void emitModifyPropList(dtCore::PropertyContainer& propertyContainer, std::vector<dtCore::ActorProperty*>& propList);

      /**
      * Enables or Disables the viewport by adding or removing it from the scene.
      *
      * @param[in]  viewport  The viewport.
      * @param[in]  enable    Enable or Disable?
      * @param[in]  disableAfterRender if it should be disabled after 1 render.
      *
      * @return     Returns true if the viewport enabled status was changed.
      */
      bool EnableViewport(Viewport* viewport, bool enable, bool disableAfterRender = false);

   signals:

      /**
      * Signal used when a viewport has been enabled.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   enabled          Whether the viewport is being enabled or not.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void viewportEnabled(Viewport* vp, bool enabled, bool* overrideDefault);

      /**
      * Signal used when a mouse has been pressed in a viewport.
      *
      * @param[in]  vp  The viewport triggering this event.
      * @param[in]  e   The mouse event.
      */
      void mousePressEvent(Viewport* vp, QMouseEvent* e);

      /**
      * Signal used when a mouse has been released in a viewport.
      *
      * @param[in]  vp  The viewport triggering this event.
      * @param[in]  e   The mouse event.
      */
      void mouseReleaseEvent(Viewport* vp, QMouseEvent* e);

      /**
      * Signal used when the mouse has been double clicked.
      *
      * @param[in]  vp  The viewport triggering this event.
      * @param[in]  e   The mouse event.
      */
      void mouseDoubleClickEvent(Viewport* vp, QMouseEvent* e);

      /**
      * Signal used when a mouse has been moved in a viewport.
      *
      * @param[in]  vp  The viewport triggering this event.
      * @param[in]  e   The mouse event.
      */
      void mouseMoveEvent(Viewport* vp, QMouseEvent* e);

      /**
      * Signal used when mouse wheel has been moved in a viewport.
      *
      * @param[in]  vp  The viewport triggering this event.
      * @param[in]  e   The mouse wheel event.
      */
      void wheelEvent(Viewport* vp, QWheelEvent* e);

      /**
      * Signal used when testing whether actor mode should be activated.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   position         The position of the mouse click.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      * @param[out]  result           The result of the test if it is overridden.
      */
      void shouldBeginActorMode(Viewport* vp, osg::Vec2 position, bool* overrideDefault, bool* result);

      /**
      * Signal used when actor mode is begun.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   e                The mouse event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void beginActorMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault);

      /**
      * Signal used when actor mode is ended.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   e                The mouse event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void endActorMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault);

      /**
      * Signal used when camera mode is begun.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   e                The mouse event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void beginCameraMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault);

      /**
      * Signal used when camera mode is ended.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   e                The mouse event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void endCameraMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault);

      ///**
      //* Signal used when the camera is moved.
      //*
      //* @param[in]   vp               The viewport triggering this event.
      //* @param[in]   dx               Horizontal delta movement.
      //* @param[in]   dy               Vertical delta movement.
      //* @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      //*/
      //void moveCamera(Viewport* vp, float dx, float dy, bool* overrideDefault);

      /**
      * Signal used when to select actors.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[in]   e                The mouse event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void selectActors(Viewport* vp, QMouseEvent* e, bool* overrideDefault);

      /**
      * Signal used when duplicating the current selection.
      *
      * @param[in]   vp               The viewport triggering this event.
      * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
      */
      void duplicateActors(Viewport* vp, bool* overrideDefault);

      /**
      * Sets the translation snap increment.
      *
      * @param[in]  increment  The size of the increment.
      */
      void setSnapTranslation(float increment);

      /**
      * Sets the angular snap increment.
      *
      * @param[in]  increment  The size of the increment.
      */
      void setSnapRotation(float increment);

      /**
      * Sets the scalar snap increment.
      *
      * @param[in]  increment  The size of the increment.
      */
      void setSnapScale(float increment);

      /**
      * Sets the enabled status of each snap tool.
      */
      void setSnapEnabled(bool translation, bool rotation, bool scale);

      /**
      * Modifies the list of properties to be displayed in the property editor.
      *
      * @param[in]  propertyContainer  The property container.
      * @param[in]  propList           The list of properties to be displayed.
      */
      void modifyPropList(dtCore::PropertyContainer& propertyContainer, std::vector<dtCore::ActorProperty*>& propList);

   public slots:
      /**
       * Called after a system step.
       */
      void onPostTick();

      /**
       * Called when an actor property has changed.  This method simply tells the
       * viewports to update themselves.
       * @param proxy
       * @param property
       */
      void onActorPropertyChanged(dtCore::ActorPtr proxy,
         ActorPropertyRefPtr property);

      /**
       * This method is invoked when the user has created a new actor proxy.  The method
       * then inserts the new actor proxy into the current scene.
       * @param proxy The newly created actor proxy.
       */
      void onActorProxyCreated(dtCore::ActorPtr proxy, bool forceNoAdjustments);

      /**
       * Called when the current map being edited has changed.  This causes the scene
       * to be refreshed, and the viewports to be updated.
       */
      void onCurrentMapChanged();

      /**
       * Refreshes viewports with a new set of selected actors.
       *
       * @param[in]  actors  The list of all actors being selected.
       */
      void refreshActorSelection(const std::vector< dtCore::RefPtr<dtCore::BaseActorObject> >& actors);

      /**
       * Causes a redraw of all viewports that are connected to the viewport manager
       * via the shared Delta3D scene.
       */
      void refreshAllViewports();

      /**
       * Instructs all viewports that have their autoSceneUpdate property set,
       * to update their scene reference to the one stored in this class.
       * @note
       *  For example, when a new map is opened the method is called.
       */
      void refreshScene();

      /**
       * Starts a transaction, so we wont' do refreshes on every change that's
       * about to happen
       */
      void onBeginChangeTransaction();

      /**
       * Ends a transaction  - clears the transaction flag and refreshes the viewports
       */
      void onEndChangeTransaction();

      /**
       * Called when the editor is shutting down.  It cleans up the scene.
       */
      void onEditorShutDown();

   private:
      ///Singletons shouldn't be created at the user's discretion.
      ViewportManager();
      ViewportManager(const ViewportManager& rhs);
      ViewportManager& operator=(const ViewportManager& rhs);

      ///Since the singleton is wrapped in a smart pointer disallow destructor access.
      virtual ~ViewportManager();

      /**
       * Internal method which creates the actual viewport object.
       * @param name Name of the new viewport.
       * @param type Type of the new viewport based on the ViewportType enumeration.
       * @param parent Parent Qt widget or NULL if no parent.
       * @param shareWith OpenGL context to share with or NULL if no sharing.
       * @return The newly created viewport.
       */
      Viewport* createViewportImpl(const std::string& name, ViewportType& type,
         QWidget* parent, osg::GraphicsContext* shareWith);

   private:
      static dtCore::RefPtr<ViewportManager> sInstance;

      bool                            mSnapTranslationEnabled;
      bool                            mSnapRotationEnabled;
      bool                            mSnapScaleEnabled;

      float                           mSnapTranslation;
      float                           mSnapRotation;
      float                           mSnapScale;

      dtCore::RefPtr<dtCore::DeltaDrawable> mLastDrawable;
      osg::Vec3                             mLastPickPosition;

      bool                            mShareMasterContext;
      std::map<std::string,Viewport*> mViewportList;
      std::map<std::string,bool>      mViewportWantsRefresh;
      Viewport*                       mMasterViewport;
      dtCore::RefPtr<ViewportOverlay> mViewportOverlay;
      dtCore::RefPtr<dtCore::Scene>   mMasterScene;
      dtCore::RefPtr<StageCamera>     mWorldCamera;
      bool                            mInChangeTransaction;
      dtCore::RefPtr<dtABC::Application> mApplication;
   };

} // namespace dtEditQt

#endif // DELTA_VIEWPORTMANAGER
