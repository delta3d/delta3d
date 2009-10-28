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
 * Curtiss Murphy
 */
#ifndef DELTA_BASE_PROPERTY_EDITOR
#define DELTA_BASE_PROPERTY_EDITOR

#include <QtGui/QDockWidget>
#include <vector>


#include <dtUtil/tree.h>
#include <dtDAL/propertycontainer.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/actortype.h>
#include <dtQt/typedefs.h>

#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/export.h>

class QMainWindow;
class QTreeWidget;
class QLabel;
class QGridLayout;
class QScrollView;
class QGroupBox;

namespace dtDAL
{
   class DataType;
   class ActorProperty;
}

namespace dtQt
{

   class DynamicAbstractControl;
   class DynamicAbstractParentControl;
   class DynamicGroupControl;
   class PropertyEditorTreeView;
   class PropertyEditorModel;

   /**
    * This class is the property editor for displaying and editing properties of selected objects.  It shows the
    * information about the selected actor(s) including type, name, location, rotation, etc...  It is a
    * dockable window.
    */
   class DT_QT_EXPORT BasePropertyEditor : public QDockWidget
   {
      Q_OBJECT
   public:
      typedef std::vector<dtCore::RefPtr<dtDAL::PropertyContainer> > PropertyContainerRefPtrVector;

      /**
       * Constructor
       */
      BasePropertyEditor(QMainWindow* parent);

      /**
       * Destructor
       */
      virtual ~BasePropertyEditor();

      /**
       * Convenience method for making the property editor commit it changes.  There are
       * some cases where an event can fire than needs to current state of the actor properties, but
       * an edit is in process.  Calling this will clear the focus of the property editor to make it
       * commit the changes.
       */
      void CommitCurrentEdits()
      {
         QWidget* lastFocused = focusWidget();
         if (lastFocused != NULL && lastFocused->hasFocus())
         {
            lastFocused->clearFocus();
         }
      }

      ///Fills a vector with pointers to all the currently selected actor proxies.
      void GetSelectedPropertyContainers(std::vector<dtDAL::PropertyContainer*>& toFill)
      {
         toFill.clear();
         toFill.reserve(mSelectedPC.size());
         for (unsigned i = 0; i != mSelectedPC.size(); ++i)
         {
            toFill.push_back(mSelectedPC[i].get());
         }
      }

      /// @return the dynamic control factory.  This is handy for registering new types.
      DynamicControlFactory& GetDynamicControlFactory();

   public slots:
      /**
       * Handles the actor selection changed event message from EditorEvents
       */
      void HandlePropertyContainersSelected(PropertyContainerRefPtrVector& actors);

      void ActorPropertyChanged(dtDAL::PropertyContainer& propCon,
         dtDAL::ActorProperty& property);

      void ProxyNameChanged(dtDAL::ActorProxy& pc, std::string oldName);

      virtual void PropertyAboutToChangeFromControl(dtDAL::PropertyContainer&, dtDAL::ActorProperty&,
               const std::string& oldValue, const std::string& newValue) = 0;
      virtual void PropertyChangedFromControl(dtDAL::PropertyContainer&, dtDAL::ActorProperty&) = 0;

   protected:

      /**
       * Add all the dynamic controls for this proxy object.
       */
      virtual void buildDynamicControls(dtDAL::PropertyContainer& propertyContainer, DynamicGroupControl* parentControl = NULL);

      virtual QString GetGroupBoxLabelText(const QString& baseGroupBoxName);

      DynamicGroupControl* GetRootControl();

      PropertyEditorModel& GetPropertyEditorModel();

   private:
      // list of what the editor thinks is the last known selected actors
      std::vector<dtCore::RefPtr<dtDAL::PropertyContainer> > mSelectedPC;

      dtCore::RefPtr<DynamicControlFactory> mControlFactory;

      //QGroupBox *actorInfoBox;
      QString                 mBaseGroupBoxName;
      QGroupBox*              actorPropBox;
      QGridLayout*            mainGridLayout;
      QWidget*                mainAreaWidget;
      QGridLayout*            dynamicControlLayout;
      PropertyEditorTreeView* propertyTree;
      PropertyEditorModel*    propertyModel;
      DynamicGroupControl*    mRootControl;

      // this is a tree of property group names which were expanded.  It is used
      // when we change selected actors.  We walk the property tree and look for
      // expanded items.  For each one, we add it to the tree.  Then, we walk
      // back through this tree to reexpand items.
      dtUtil::tree<QString> expandedTreeNames;
      int lastScrollBarLocation;
      int lastHeaderPosition;

      /**
       * Creates the main User Interface for the Property Editor.
       * @note The property editor is mostly driven by dynamic controls so the look and
       *    feel can completely change depending on what you have selected.
       */
      void setupUI();

      /**
       * Clear out and rebuild the property editor dialog.  This should be called when
       * you know or suspect that the selected actor has changed.  It will recreate all the
       * appropriate controls.
       */
      void refreshSelectedActors();


      /**
       * Indicate selection information in the group box title
       */
      void resetGroupBoxLabel();

      /**
       * Looks at the current property tree and tries to mark which group controls are
       * currently expanded.  This is then used to re-expand them with restorePreviousExpansion()
       * after the tree is rebuilt.
       */
      void markCurrentExpansion();

      /**
       * recursive method to support markCurrentExpansion().
       */
      void recurseMarkCurrentExpansion(DynamicAbstractControl* parent,
         dtUtil::tree<QString>& currentTree);

      /**
       * Attempts to re-expand previously expanded tree nodes.  This is a nicity for the user
       * for when they switch selected objects.  If they were toggling between 2 different
       * objects, it would be extremely annoying that you had to re-expand the controls each time.
       */
      void restorePreviousExpansion();

      /**
       * Recursive method to support restorePreviousExpansion().
       */
      void recurseRestorePreviousExpansion(DynamicAbstractControl* parent, dtUtil::tree<QString>& currentTree);

   };

} // namespace dtQt

#endif // DELTA_PROPERTY_EDITOR
