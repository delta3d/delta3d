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
#ifndef DELTA_PROPERTY_EDITOR
#define DELTA_PROPERTY_EDITOR

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

namespace dtEditQt
{

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
       * Destructor
       */
      virtual ~PropertyEditor();

      void PropertyAboutToChangeFromControl(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& prop,
               const std::string& oldValue, const std::string& newValue);

      void PropertyChangedFromControl(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& prop);
   public slots:
      void HandleActorsSelected(ActorRefPtrVector&);
      void ActorPropertyChanged(dtCore::ActorPtr proxy, ActorPropertyRefPtr property);
   protected:
      virtual void buildDynamicControls();

      virtual QString GetGroupBoxLabelText(const QString& baseGroupBoxName);

      /// Called on the close event
      virtual void closeEvent(QCloseEvent* e);
   };

} // namespace dtEditQt

#endif // DELTA_PROPERTY_EDITOR
