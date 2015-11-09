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
#ifndef DELTA_VIEWPORT_OVERLAY
#define DELTA_VIEWPORT_OVERLAY

#include <QtCore/QObject>
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <dtCore/actorproxy.h>
#include <dtQt/typedefs.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Group;
}
/// @endcond

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
   class ViewportOverlay : public QObject, public osg::Referenced
   {
      Q_OBJECT

   public:
      ///Simplifies the interface to a list of actor proxies.
      typedef dtCore::ActorRefPtrVector ActorProxyList;

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
      void select(dtCore::DeltaDrawable* drawable);

      /**
       * This method removes a Delta3D drawable object from the current selection
       * overlay group.
       * @param drawable
       */
      void unSelect(dtCore::DeltaDrawable* drawable);

      /**
       * Sets whether or not the UI is currently in multi-select mode.  When in
       * multi-select mode, newly selected objects are added to a list of
       * currently selected objects.  When multi-select is not enabled, newly
       * selected objects are made to be the current selection.
       * @param value True if multi-select mode should be enabled.
       */
      void setMultiSelectMode(bool value) { mMultiSelectMode = value; }

      /**
       * Returns whether or not multi-select mode is enabled.
       * @return The multi-selection mode flag.
       */
      bool getMultiSelectMode() const { return mMultiSelectMode; }

      /**
       * Returns a group containing overlay objects.  Usually, objects in this group
       * need to be rendered by the viewports but are not a part of the current scene.
       * @return A group scenegraph node containing overlay objects.
       */
      osg::Group* getOverlayGroup() { return mOverlayGroup.get(); }

      /**
       * Returns the selection group overlay.  This group contains the current selection.
       * @return A group scenegraph node containing the current selection.
       */
      osg::Group* getSelectionDecorator() { return mSelectionDecorator.get(); }

      /**
       * Returns a sorted set of actor proxies corresponding to the current selection.
       * @return ActorProxyList
       */
      ActorProxyList& getCurrentActorSelection() { return mCurrentActorSelection; }

      /**
       * Determines whether or not the specified actor proxy is currently selected.
       * @param proxy The proxy to test.
       * @return True if the proxy is already selected, false otherwise.
       */
      bool isActorSelected(dtCore::BaseActorObject* proxy) const;
      bool isActorSelectedFirst(dtCore::BaseActorObject* proxy) const;

      /**
       * Removes the specified actor proxy from the current selection.
       * @param proxy The proxy to remove.
       * @param clearAll This is useful if you want to remove the selection from the
       *  selection tree but leave the proxy, logically, in a selected state.  Setting
       *  true for this value clears the selection both visually and logically.
       */
      void removeActorFromCurrentSelection(dtCore::BaseActorObject* proxy, bool clearAll = true);

      /**
       * Clears the current selection.
       */
      void clearCurrentSelection();

   public slots:
      /**
       * This method is invoked when the user selects actors in the current scene.
       * @param actors A list of the actors that were selected.
       */
      void onActorsSelected(ActorRefPtrVector& actors);

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
       * Creates or updates the decorator group used to hold the current selection.  The group
       * sets its render state such that all objects contained in it will be
       * rendered using a red wireframe outline.
       */
      void setupSelectionDecorator();

      /**
       * Connects the the UI global event system.
       */
      void listenForEvents();

      ///Do not allow overlays to be copy-constructed.
      ViewportOverlay(const ViewportOverlay& rhs);

      ///Do not allow overlays to be assigned to one another.
      ViewportOverlay& operator=(const ViewportOverlay& rhs);


      osg::ref_ptr<osg::Group> mOverlayGroup;
      osg::ref_ptr<osg::Group> mSelectionDecorator;

      ActorProxyList mCurrentActorSelection;
      bool           mMultiSelectMode;
   };

} // namespace dtEditQt

#endif // DELTA_VIEWPORT_OVERLAY
