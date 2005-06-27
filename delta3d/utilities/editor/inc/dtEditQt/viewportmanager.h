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
#ifndef __ViewportManager__h
#define __ViewportManager__h

#include <QObject>
#include <map>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <dtCore/scene.h>
#include <dtCore/uniqueid.h>
#include <dtUtil/enumeration.h>
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"

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
        static ViewportManager &getInstance();


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
            osg::ref_ptr<dtDAL::ActorProxy> > &proxies);

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

    public slots:
        /**
         * Called when an actor property has changed.  This method simply tells the
         * viewports to update themselves.
         * @param proxy
         * @param property
         */
        void onActorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
            osg::ref_ptr<dtDAL::ActorProperty> property);

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
        static osg::ref_ptr<ViewportManager> instance;

        bool shareMasterContext;
        std::map<std::string,Viewport*> viewportList;
        Viewport *masterViewport;
        osg::ref_ptr<ViewportOverlay> viewportOverlay;
        osg::ref_ptr<dtCore::Scene> masterScene;
        osg::ref_ptr<Camera> worldCamera;
        int numTextureUnits;
    };
}

#endif
