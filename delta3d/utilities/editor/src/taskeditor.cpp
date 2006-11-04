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
#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/taskeditor.h>

#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>
#include <QtGui/QScrollArea>
#include <QtGui/QListWidget>
#include <QtGui/QLabel>
#include <QtGui/QTableWidget>
#include <QtGui/QHeaderView>
#include <QtCore/QString>

#include <dtEditQt/editordata.h>

#include <dtDAL/map.h>
#include <dtDAL/namedparameter.h>
#include <dtDAL/actortype.h>
#include <dtCore/uniqueid.h>

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   TaskEditor::TaskEditor(QWidget *parent) : QDialog(parent)
   {
      QGroupBox   *group = new QGroupBox(tr("Tasks"));
      QGridLayout *grid  = new QGridLayout(group);

      QVBoxLayout *rightSideLayout = new QVBoxLayout;
      QLabel      *child       = new QLabel(tr("Children")); 
      QPushButton *addExisting = new QPushButton(tr("Add Existing"));
      grid->addWidget(child, 0, 0);
      //grid->addWidget(addExisting, 0, 1);

      mChildrenView = new QTableWidget(NULL);
      mChildrenView->setSelectionMode(QAbstractItemView::SingleSelection);
      mChildrenView->setSelectionBehavior(QAbstractItemView::SelectRows);
      mChildrenView->setAlternatingRowColors(true);
      mChildrenView->setEditTriggers(QAbstractItemView::NoEditTriggers);
      grid->addWidget(mChildrenView, 1, 0);

      mComboBox = new QComboBox;
      rightSideLayout->addWidget(addExisting);
      rightSideLayout->addWidget(mComboBox);
      //grid->addWidget(mComboBox, 1, 1);
      grid->addLayout(rightSideLayout, 0, 1);

      QHBoxLayout *buttonLayout = new QHBoxLayout;
      mMoveUp = new QPushButton(tr("Move Up"));
      mMoveDown = new QPushButton(tr("Move Down"));
      buttonLayout->addWidget(mMoveUp);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(mMoveDown);
      grid->addLayout(buttonLayout, 2, 0);

      mRemoveChild = new QPushButton(tr("Remove Child"));
      grid->addWidget(mRemoveChild, 3, 0);

      QHBoxLayout *okCancelLayout = new QHBoxLayout;
      QPushButton *ok = new QPushButton(tr("OK")), *cancel = new QPushButton(tr("Cancel"));
      okCancelLayout->addWidget(ok);
      okCancelLayout->addWidget(cancel);
      
      QVBoxLayout *mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(group);
      mainLayout->addLayout(okCancelLayout);

      setModal(true);
      setWindowTitle(tr("Task Editor"));
      //setMinimumSize(360, 375);

      connect(mComboBox, SIGNAL(activated(const QString&)), this, SLOT(RefreshComboBox(const QString&)));

      connect(mMoveUp,      SIGNAL(clicked()), this, SLOT(OnMoveUpClicked()));
      connect(mMoveDown,    SIGNAL(clicked()), this, SLOT(OnMoveDownClicked()));
      connect(mRemoveChild, SIGNAL(clicked()), this, SLOT(OnRemoveChildClicked()));
      connect(ok,           SIGNAL(clicked()), this, SLOT(OnOkClicked()));
      connect(cancel,       SIGNAL(clicked()), this, SLOT(close()));      
      connect(mChildrenView,   SIGNAL(itemSelectionChanged()), this, SLOT(EnableEditButtons()));

      RefreshChildren();
      RefreshComboBox(tr(""));
   }

   ///////////////////////////////////////////////////////////////////////////////
   TaskEditor::~TaskEditor()
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::RefreshChildren()
   {
      mChildrenView->clear();
      
      mChildrenView->setColumnCount(2);
      mChildrenView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
      mChildrenView->horizontalHeader()->setFixedHeight(20);
      mChildrenView->verticalHeader()->setHidden(true);

      QStringList names;
      names.push_back(tr("Name"));
      names.push_back(tr("Task Type"));
      mChildrenView->setHorizontalHeaderLabels(names);

      names.clear();
      
      if(mChildrenView->currentItem() == NULL)
         DisableEditButtons();
      else
         mChildrenView->setItemSelected(mChildrenView->currentItem(), true);

      dtDAL::Map* m = EditorData::getInstance().getCurrentMap();
      if (m == NULL)
      {
         LOG_ERROR("Unable read the children of a task actor without a valid current map.");
         return;
      }
      dtDAL::Map& currMap = *m;
      
      if (mChildren.valid())
      {
         std::vector<dtDAL::NamedParameter*> toFill;
         mChildren->GetParameters(toFill);
         
         mChildrenView->setRowCount(toFill.size());

         for (unsigned i = 0; i < toFill.size(); ++i)
         {
            dtDAL::NamedParameter* np = toFill[i];
            if (np->GetDataType() == dtDAL::DataType::ACTOR)
            {
               dtCore::UniqueId id = static_cast<dtDAL::NamedActorParameter*>(np)->GetValue();
               dtDAL::ActorProxy* child = currMap.GetProxyById(id);
               if (child != NULL)
               {
                  dtDAL::ActorType& at = child->GetActorType();
                  QTableWidgetItem *nm = new QTableWidgetItem;
                  QTableWidgetItem *type = new QTableWidgetItem;
               
                  nm->setText(tr(child->GetName().c_str()));
                  type->setText(tr((at.GetCategory() + "." + at.GetName()).c_str()));

                  mChildrenView->setItem(i, 0, nm);
                  mChildrenView->setItem(i, 1, type);
                  names.push_back(tr(""));
               }
            }
         }

         mChildrenView->setVerticalHeaderLabels(names);
         
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::RefreshComboBox(const QString &itemName)
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::OnMoveUpClicked()
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::OnMoveDownClicked()
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::OnRemoveChildClicked()
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::OnOkClicked()
   {
      accept();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::EnableEditButtons()
   {
      mMoveUp->setDisabled(false);
      mMoveDown->setDisabled(false);
      mRemoveChild->setDisabled(false);
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::DisableEditButtons()
   {
      mMoveUp->setDisabled(true);
      mMoveDown->setDisabled(true);
      mRemoveChild->setDisabled(true);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::SetTaskChildren(const dtDAL::NamedGroupParameter& children)
   {
      mChildren = new dtDAL::NamedGroupParameter(children);
      RefreshChildren();
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::GetTaskChildren(dtDAL::NamedGroupParameter& toFill) const
   {
      
   }
   
}
