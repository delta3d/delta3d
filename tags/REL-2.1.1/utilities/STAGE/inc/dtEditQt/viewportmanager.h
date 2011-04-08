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
#include <dtCore/view.h>
#include <dtCore/uniqueid.h>
#include <dtUtil/enumeration.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtCore/timer.h>
#include "dtEditQt/typedefs.h"

class QGLWidget;
class QWidget;

namespace dtEditQt
{

    class Viewport;
    class ViewportOverlay;
    class Camera;

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
    class ViewportManager : public QObject, public osg::Referenced
    {
        Q_OBJECT
    public:
        /**
         * Simple enumeration of the types of viewports the manager can create.
         */
        class ViewportType : public dtUtil::Enumeration {
            DECLARE_ENUM(ViewportType);
        public:
            static ViewportType ORTHOGRAPHIC;
            static ViewportType PERSPECTIVE;
        private:
            ViewportType(const std::string &name) : dtUtil::Enumeration(name)
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
        Viewport *createViewport(const std::string &name, ViewportType &type,
            QWidget *parent = NULL);

        /**
         * Gets a handle to the master scene.  Once the handle is retrieved, use it to
         * add/remove objects from the scene.
         * @return A pointer to the master scene.
         */
        dtCore::Scene *getMasterScene() {
            return this->masterScene.get();
        }

        /**
         * Gets a handle to the world view camera.  The 3D perspective view uses
         * the world view camera to render its scene.
         * @return A pointer to the world view camera.
         */
        Camera *getWorldViewCamera() {
            return this->worldCamera.get();
        }

        /**
         * Moves an actor such that it is placed in from of the world view camera.
         * @param proxy The proxy to place.
         */
        void placeProxyInFrontOfCamera(dtDAL::ActorProxy *proxy);

        /**
         * Gets the instance of an overlay object that should be shared between viewports.
         * It stores editor specific scene graph components that should be common amoung
         * the viewports. For example, when an object is selected, it should appear selected
         * in all viewports.
         * @return ViewportOverlay
         */
        ViewportOverlay *getViewportOverlay() {
            return this->viewportOverlay.get();
        }

        /**
         * Gets a reference to the static singleton instance of the viewport manager.
         * @return
         */
        static ViewportManager &GetInstance();

        /**
         * Disables or enables OpenGL context sharing.  This should be called
         * before any viewports
         * are created.  By default, context sharing is enabled.
         * @param share If true, enable sharing.
         */
        void enableContextSharing(bool share) { this->shareMasterContext = share; }

        /**
         * Removes all drawables from the master scene.
         */
        void clearMasterScene(const std::map<dtCore::UniqueId,
            dtCore::RefPtr<dtDAL::ActorProxy> > &proxies);

        /**
         * Called from the first initialized viewport.  This method sets various
         * statistics about the current OpenGL implementation.  For example, the number
         * of texture units available.
         */
        void initializeGL();

        /**
         * Gets the number of available texture units.
         * @return The number of texture units.
         */
        int getNumTextureUnits() const {
            return this->numTextureUnits;
        }

        /// Returns if database paging is enabled
        bool IsPagingEnabled() const { return isPagingEnabled; }

        /// Sets is database paging is enabled
        void EnablePaging(bool enable);

        /// Returns if database paging is enabled
        dtCore::DatabasePager* GetDatabasePager();

        dtCore::Timer_t GetStartTick() { return startTick; }

        void SetStartTick(unsigned int time) { startTick = time; }

    public slots:
        /**
         * Called when an actor property has changed.  This method simply tells the
         * viewports to update themselves.
         * @param proxy
         * @param property
         */
        void onActorPropertyChanged(ActorProxyRefPtr proxy,
            ActorPropertyRefPtr property);

        /**
         * This method is invoked when the user has created a new actor proxy.  The method
         * then inserts the new actor proxy into the current scene.
         * @param proxy The newly created actor proxy.
         */
        void onActorProxyCreated(ActorProxyRefPtr proxy, bool forceNoAdjustments);

        /**
         * Called when the current map being edited has changed.  This causes the scene
         * to be refreshed, and the viewports to be updated.
         */
        void onCurrentMapChanged();

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
        ViewportManager(const ViewportManager &rhs);
        ViewportManager &operator=(const ViewportManager &rhs);

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
        Viewport *createViewportImpl(const std::string &name, ViewportType &type,
            QWidget *parent, QGLWidget *shareWith);

    private:
        static dtCore::RefPtr<ViewportManager> instance;

        bool shareMasterContext;
        std::map<std::string,Viewport*> viewportList;
        Viewport *masterViewport;
        dtCore::RefPtr<ViewportOverlay> viewportOverlay;
        dtCore::RefPtr<dtCore::Scene> masterScene;
        dtCore::RefPtr<dtCore::View> mMasterView;
        dtCore::RefPtr<Camera> worldCamera;
        int numTextureUnits;
        bool inChangeTransaction, isPagingEnabled;
        dtCore::Timer_t startTick;
    };
}

#endif