/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#ifndef DIRECTORQT_PROPERTY_EDITOR
#define DIRECTORQT_PROPERTY_EDITOR

#include <dtQt/basepropertyeditor.h>

class QMainWindow;
class QTreeWidget;
class QLabel;
class QGridLayout;
class QScrollView;
class QGroupBox;

namespace dtQt
{
   class DynamicAbstractControl;
   class DynamicAbstractParentControl;
   class DynamicGroupControl;
   class PropertyEditorTreeView;
   class PropertyEditorModel;
}

namespace dtCore
{
   class DataType;
   class ActorProperty;
}

namespace dtDirector
{
   class DirectorEditor;
   class EditorScene;
   class GraphTabs;

   /**
    * This class is the property editor for displaying and editing properties of selected objects.  It shows the
    * information about the selected actor(s) including type, name, location, rotation, etc...  It is a
    * dockable window.
    */
   class PropertyEditor : public dtQt::BasePropertyEditor
   {
      Q_OBJECT
   public:
      typedef dtQt::BasePropertyEditor BaseClass;

      /**
       * Constructor
       */
      PropertyEditor(QMainWindow* parent);

      /**
      * Sets the Property Editor's DirectorEditor
      *
      * @param[in]  editor  The director editor that owns this widget.
      */
      void SetDirectorEditor(DirectorEditor* editor);

      /**
       * Sets the scene.
       */
      EditorScene* GetScene() {return mScene;}
      void SetScene(EditorScene* scene) {mScene = scene;}

      /**
       * Sets the graph tabs widget.
       */
      void SetGraphTabs(GraphTabs* graphTabs) {mGraphTabs = graphTabs;}

      /**
       * Destructor
       */
      virtual ~PropertyEditor();

      virtual void OnContainersSelected(const std::vector<dtCore::RefPtr<dtCore::PropertyContainer> >& selection);

      void PropertyAboutToChangeFromControl(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& prop,
               const std::string& oldValue, const std::string& newValue);

      void PropertyChangedFromControl(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& prop);

   public slots:
   protected:

      virtual QString GetGroupBoxLabelText(const QString& baseGroupBoxName);
      virtual std::string GetContainerGroupName(dtCore::PropertyContainer* propertyContainer);

      /// Called on the close event
      virtual void closeEvent(QCloseEvent* e);

      DirectorEditor* mDirectorEditor;
      EditorScene*    mScene;
      GraphTabs*      mGraphTabs;

      std::string     mOldValue;
      std::string     mNewValue;
   };

} // namespace dtDirector

#endif // DIRECTORQT_PROPERTY_EDITOR
