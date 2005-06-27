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
#ifndef __ViewportOverlay__h
#define __ViewportOverlay__h

#include <set>
#include <QObject>
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <dtCore/transformable.h>
#include "dtDAL/actorproxy.h"
#include "dtEditQt/viewportmanager.h"

namespace osg 
{
	class Group;
}

namespace dtEditQt 
{

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
         */
		void removeActorFromCurrentSelection(dtDAL::ActorProxy *proxy);           
		
	public slots:
        /**
         * This method is invoked when the user selects actors in the current scene.
         * @param actors A list of the actors that were selected.
         */
		void onActorsSelected(std::vector<osg::ref_ptr<dtDAL::ActorProxy> > &actors);

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
		ActorProxyList currentActorSelection;
		bool multiSelectMode;
	};

}

#endif
