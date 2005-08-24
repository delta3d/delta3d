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
#ifndef __ViewportOverlay__h
#define __ViewportOverlay__h

#include <set>
#include <map>
#include <QObject>
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/Geode>
#include <osg/Geometry>
#include <dtCore/physical.h>
#include "dtDAL/actorproxy.h"
#include "dtEditQt/viewportmanager.h"

namespace osg
{
    class Material;
    class Group;
}

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
                    osg::CopyOp::DEEP_COPY_NODES | osg::CopyOp::DEEP_COPY_DRAWABLES;
        }

        /**
         * Empty destructor.
         */
        virtual ~ExtractSelectionVisitor() { }

        /**
         * Resets the visitor so it can be reused for another traversal.
         */
        virtual void reset() {
            this->selectionCopies.clear();
        }

        /**
         * If this is set to true, the old geodes will be used to replace the copies
         * created in a previous traversal.
         * @param on
         */
        void setRestoreMode(bool on) {
            this->restoreMode = on;
        }

        /**
         * Copies the incoming geode and turns off any color binding that
         * may have been set on the copy.
         * @param geode
         */
        virtual void apply(osg::Geode &geode)
        {
            unsigned int i;
            osg::Geode *copyGeode;

            if (geode.getName() == dtCore::Physical::COLLISION_GEODE_ID)
                return;

            if (restoreMode) { //Restore the geodes we made copies of.
                std::map<osg::ref_ptr<osg::Geode>,osg::ref_ptr<osg::Geode> >::iterator itor =
                        this->selectionCopies.find(&geode);

                if (itor != this->selectionCopies.end()) {
                    replaceGeode(&geode,itor->second.get());
                }
            }
            else {
                copyGeode = new osg::Geode(geode,this->copyOp);

                //Save our original geode along with the copy.
                this->selectionCopies.insert(std::make_pair(copyGeode,&geode));

                //Make sure we turn off color binding so the wireframe color is
                //correct.
                for (i=0; i<copyGeode->getNumDrawables(); i++) {
                    osg::Geometry *geom = copyGeode->getDrawable(i)->asGeometry();
                    if (geom != NULL) {
                        geom->setColorBinding(osg::Geometry::BIND_OFF);
                    }
                }

                //Replace the original geode with the new copy.
                replaceGeode(&geode,copyGeode);
            }
        }

        /**
         * Replaces a geode in the scene graph.
         * @param oldGeode The geode to replace.
         * @param newGeode The new geode.
         */
        void replaceGeode(osg::Geode *oldGeode, osg::Geode *newGeode)
        {
            osg::Node::ParentList parentList = oldGeode->getParents();
            for (unsigned int i=0; i<parentList.size(); i++) {
                parentList[i]->replaceChild(oldGeode,newGeode);
            }
        }

    private:
        osg::CopyOp copyOp;
        bool restoreMode;
        std::map<osg::ref_ptr<osg::Geode>,osg::ref_ptr<osg::Geode> > selectionCopies;
    };
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * This class is mostly an entry point for accessing state data shared between
     * viewports. Since the data is shared between viewports, this class also provides a
     * convienent mechanism for syncronizing elements common to all viewports.
     * @note
     *  As an example, the viewport overlay class manages the current selection of
     *  actor proxies in the scene.
     */
    class ViewportOverlay : public QObject, public osg::Referenced {
        Q_OBJECT
    public:
        ///Simplifies the interface to a list of actor proxies.
        typedef std::set<osg::ref_ptr<dtDAL::ActorProxy> > ActorProxyList;

        /**
         * Constructs the overlay.
         * @return
         */
        ViewportOverlay();

        /**
         * This method "selects" a Delta3D drawable object.
         * @par
         *  A drawable object is selected by adding it to a selection overlay group.
         *  The selection overlay group then instructs then causes the object to
         *  be rendered in a second pass but with wireframe mode enabled on the object.
         *  This results in a wireframe overlay that is applied to a selected object.
         * @param drawable
         */
        void select(dtCore::DeltaDrawable *drawable);

        /**
         * This method removes a Delta3D drawable object from the current selection
         * overlay group.
         * @param drawable
         */
        void unSelect(dtCore::DeltaDrawable *drawable);

        /**
         * Sets whether or not the UI is currently in multi-select mode.  When in
         * multi-select mode, newly selected objects are added to a list of
         * currently selected objects.  When multi-select is not enabled, newly
         * selected objects are made to be the current selection.
         * @param value True if multi-select mode should be enabled.
         */
        void setMultiSelectMode(bool value) {
            this->multiSelectMode = value;
        }

        /**
         * Returns whether or not multi-select mode is enabled.
         * @return The multi-selection mode flag.
         */
        bool getMultiSelectMode() const {
            return this->multiSelectMode;
        }

        /**
         * Returns a group containing overlay objects.  Usually, objects in this group
         * need to be rendered by the viewports but are not a part of the current scene.
         * @return A group scenegraph node containing overlay objects.
         */
        osg::Group *getOverlayGroup() {
            return this->overlayGroup.get();
        }

        /**
         * Returns the selection group overlay.  This group contains the current selection.
         * @return A group scenegraph node containing the current selection.
         */
        osg::Group *getSelectionDecorator() {
            return this->selectionDecorator.get();
        }

        /**
         * Returns a sorted set of actor proxies corresponding to the current selection.
         * @return ActorProxyList
         */
        ActorProxyList &getCurrentActorSelection() {
            return this->currentActorSelection;
        }

        /**
         * Determines whether or not the specified actor proxy is currently selected.
         * @param proxy The proxy to test.
         * @return True if the proxy is already selected, false otherwise.
         */
        bool isActorSelected(dtDAL::ActorProxy *proxy) const;

        /**
         * Removes the specified actor proxy from the current selection.
         * @param proxy The proxy to remove.
         * @param clearAll This is useful if you want to remove the selection from the
         *  selection tree but leave the proxy, logically, in a selected state.  Setting
         *  true for this value clears the selection both visually and logically.
         */
        void removeActorFromCurrentSelection(dtDAL::ActorProxy *proxy, bool clearAll = true);

        /**
         * Clears the current selection.
         */
        void clearCurrentSelection();

    public slots:
        /**
         * This method is invoked when the user selects actors in the current scene.
         * @param actors A list of the actors that were selected.
         */
        void onActorsSelected(proxyRefPtrVector &actors);

        /**
         * Puts the overlay options in sync with the editor preferences.
         */
        void onEditorPreferencesChanged();

    protected:
        /**
         * Destroys the viewport.
         * @return
         */
        virtual ~ViewportOverlay();

    private:
        /**
         * Creates the decorator group used to hold the current selection.  The group
         * sets its render state such that all objects contained in it will be
         * rendered using a red wireframe outline.
         */
        void setupSelectionDecorator();

        /**
         * Connects the the UI global event system.
         */
        void listenForEvents();

        ///Do not allow overlays to be copy-constructed.
        ViewportOverlay(const ViewportOverlay &rhs);

        ///Do not allow overlays to be assigned to one another.
        ViewportOverlay &operator=(const ViewportOverlay &rhs);


        osg::ref_ptr<osg::Group> overlayGroup;
        osg::ref_ptr<osg::Group> selectionDecorator;
        osg::ref_ptr<osg::Material> selectionMaterial;

        ActorProxyList currentActorSelection;
        ExtractSelectionVisitor selectionVisitor;
        bool multiSelectMode;
    };

}

#endif
