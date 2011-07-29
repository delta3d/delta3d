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
 * William E. Johnson II
 * David Guthrie
 */
#ifndef DELTA_TASK_EDITOR
#define DELTA_TASK_EDITOR

#include <QtCore/QObject>
#include <QtGui/QDialog>
#include <dtEditQt/groupuiplugin.h>
#include <dtCore/refptr.h>
#include <set>

// Forward declarations
class QComboBox;
class QString;
class QTableWidget;
class QCheckBox;

namespace dtCore
{
   class BaseActorObject;
}

namespace dtEditQt
{
   class TaskEditor : public QDialog
   {
      Q_OBJECT;

   public:

      /// Constructor
      TaskEditor(QWidget* parent = NULL);

      /// Destructor
      virtual ~TaskEditor();

      void SetTaskChildren(const dtCore::NamedGroupParameter& children);
      void GetTaskChildren(dtCore::NamedGroupParameter& toFill) const;

      ///@return true if the list being edited contains the given proxy.
      bool HasChild(dtCore::BaseActorObject& proxyToTest);

      /**
       * Swaps the two given rows in the model for the table.
       * It does nothing if the values match or are out of bounds.
       * @param firstRow the index of the first row to swap.
       * @param secondRow the index the second row to swap.
       */
      void SwapRows(int firstRow, int secondRow);

   public slots:

      /**
       * Slot to refresh the combo box
       */
      void RefreshComboBox(const QString& itemName);

   private slots:

      /**
       * Slot called when the move up button is clicked
       */
      void OnMoveUpClicked();

      /**
       * Slot called when the move down button is clicked
       */
      void OnMoveDownClicked();

      /**
       * Slot called when the ok button is clicked
       */
      void OnOkClicked();

      /**
       * Slot called when the remove child button is clicked
       */
      void OnRemoveChildClicked();

      /// Slot called when the checkbox with the same name changes.
      void OnShowTasksWithParentsChanged(int state);

      /// Enables the buttons for manipulating the existing children
      void EnableEditButtons();

      /// Disables the buttons for manipulating the existing children
      void DisableEditButtons();

      /// Adds the selected task actor in the combo box
      void AddSelected();

      /// called when the combo box selection changes.
      void OnComboSelectionChanged(int index);

   private:
      /// builds the list of children.
      void PopulateChildren();
      /// Adds a new proxy to the list of children.
      void AddItemToList(dtCore::BaseActorObject& proxy);
      /// sets all the verical header labels on mChildrenView to ""
      void BlankRowLabels();

      /// The visible list of child tasks
      QTableWidget* mChildrenView;
      QComboBox*    mComboBox;
      QCheckBox*    mShowTasksWithParents;

      QPushButton* mAddExisting;
      QPushButton* mMoveUp;
      QPushButton* mMoveDown;
      QPushButton* mRemoveChild;

      dtCore::RefPtr<dtCore::NamedGroupParameter> mChildren;
      std::set< dtCore::RefPtr<dtCore::BaseActorObject> > mRemovedTasks;
   };

} // namespace dtEditQt

#endif // DELTA_TASK_EDITOR
