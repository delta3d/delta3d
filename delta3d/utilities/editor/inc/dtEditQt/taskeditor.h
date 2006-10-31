/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation 
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
 * William E. Johnson II 
 * David Guthrie
 */
#ifndef DELTA_TASK_EDITOR
#define DELTA_TASK_EDITOR

#include <QtCore/QObject>
#include <QtGui/QDialog>
#include <dtEditQt/groupuiplugin.h>
#include <dtCore/refptr.h>

// Forward declarations
class QComboBox;
class QString;
class QTableWidget;

namespace dtEditQt
{
   class TaskEditor : public QDialog
   {
      Q_OBJECT;

      public:

         /// Constructor
         TaskEditor(QWidget *parent = NULL);

         /// Destructor
         virtual ~TaskEditor();

         void SetTaskChildren(const dtDAL::NamedGroupParameter& children);
         void GetTaskChildren(dtDAL::NamedGroupParameter& toFill) const;

      public slots:

         /**
          * Slot to refresh the children table
          */
         void RefreshChildren();

         /**
          * Slot to refresh the combo box
          */
         void RefreshComboBox(const QString &itemName);

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
         
         ///Enables the buttons for manipulating the existing children
         void EnableEditButtons();
   
         ///Disables the buttons for manipulating the existing children
         void DisableEditButtons();
      
      private:

         /// The visible list of child tasks
         QTableWidget* mChildrenView;
         QComboBox*    mComboBox;

         QPushButton* mMoveUp;
         QPushButton* mMoveDown;
         QPushButton* mRemoveChild;
         
         dtCore::RefPtr<dtDAL::NamedGroupParameter> mChildren;
   };
   
}

#endif
