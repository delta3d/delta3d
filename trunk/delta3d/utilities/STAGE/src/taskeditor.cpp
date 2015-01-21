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

#include <prefix/stageprefix.h>
#include <dtEditQt/taskeditor.h>

#include <dtCore/uniqueid.h>

#include <dtCore/actorproxy.h>
#include <dtCore/actortype.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/map.h>
#include <dtCore/namedactorparameter.h>
#include <dtCore/namedgroupparameter.h>

#include <dtEditQt/editordata.h>

#include <QtCore/QString>
#include <QtCore/QVariant>

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   TaskEditor::TaskEditor(QWidget* parent)
      : QDialog(parent)
   {
      //dtUtil::Log::GetInstance("taskeditor.cpp").SetLogLevel(dtUtil::Log::LOG_DEBUG);
      QGroupBox*   group = new QGroupBox(tr("Tasks"));
      QGridLayout* grid  = new QGridLayout(group);

      QVBoxLayout* rightSideLayout = new QVBoxLayout;
      QLabel*      child = new QLabel(tr("Children"));
      grid->addWidget(child, 0, 0);


      mChildrenView = new QTableWidget(NULL);
      mChildrenView->setSelectionMode(QAbstractItemView::SingleSelection);
      mChildrenView->setSelectionBehavior(QAbstractItemView::SelectRows);
      mChildrenView->setAlternatingRowColors(true);
      mChildrenView->setEditTriggers(QAbstractItemView::NoEditTriggers);
      grid->addWidget(mChildrenView, 1, 0);

      mAddExisting = new QPushButton(tr("Add Existing"));

      mComboBox = new QComboBox;
      mComboBox->setEditable(false);

      mShowTasksWithParents = new QCheckBox("Show Tasks With Parents");
      mShowTasksWithParents->setCheckState(Qt::Unchecked);
      mShowTasksWithParents->setTristate(false);

      rightSideLayout->addWidget(mAddExisting);
      rightSideLayout->addWidget(mComboBox);
      rightSideLayout->addWidget(mShowTasksWithParents);
      rightSideLayout->addStretch(1);

      grid->addLayout(rightSideLayout, 0, 1, 2, 1);

      QHBoxLayout* buttonLayout = new QHBoxLayout;
      mMoveUp = new QPushButton(tr("Move Up"));
      mMoveDown = new QPushButton(tr("Move Down"));
      buttonLayout->addWidget(mMoveUp);
      buttonLayout->addStretch(1);
      buttonLayout->addWidget(mMoveDown);
      grid->addLayout(buttonLayout, 2, 0);

      mRemoveChild = new QPushButton(tr("Remove Child"));
      grid->addWidget(mRemoveChild, 3, 0);

      QHBoxLayout* okCancelLayout = new QHBoxLayout;
      QPushButton* ok             = new QPushButton(tr("OK"));
      QPushButton* cancel         = new QPushButton(tr("Cancel"));
      okCancelLayout->addStretch(1);
      okCancelLayout->addWidget(ok);
      okCancelLayout->addStretch(1);
      okCancelLayout->addWidget(cancel);
      okCancelLayout->addStretch(1);

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(group);
      mainLayout->addLayout(okCancelLayout);

      setModal(true);
      setWindowTitle(tr("Task Editor"));
      //setMinimumSize(360, 375);

      connect(mComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboSelectionChanged(int)));
      connect(mAddExisting, SIGNAL(clicked()), this, SLOT(AddSelected()));

      connect(mShowTasksWithParents, SIGNAL(stateChanged(int)), this, SLOT(OnShowTasksWithParentsChanged(int)));

      connect(mMoveUp,       SIGNAL(clicked()), this, SLOT(OnMoveUpClicked()));
      connect(mMoveDown,     SIGNAL(clicked()), this, SLOT(OnMoveDownClicked()));
      connect(mRemoveChild,  SIGNAL(clicked()), this, SLOT(OnRemoveChildClicked()));
      connect(ok,            SIGNAL(clicked()), this, SLOT(OnOkClicked()));
      connect(cancel,        SIGNAL(clicked()), this, SLOT(close()));
      connect(mChildrenView, SIGNAL(itemSelectionChanged()), this, SLOT(EnableEditButtons()));

      RefreshComboBox("");
   }

   ///////////////////////////////////////////////////////////////////////////////
   TaskEditor::~TaskEditor()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::PopulateChildren()
   {
      LOGN_DEBUG("taskeditor.cpp", "Populate Children");
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

      if (mChildrenView->currentItem() == NULL)
      {
         DisableEditButtons();
      }
      else
      {
         mChildrenView->setItemSelected(mChildrenView->currentItem(), true);
      }

      dtCore::Map* m = EditorData::GetInstance().getCurrentMap();
      if (m == NULL)
      {
         LOG_ERROR("Unable read the children of a task actor without a valid current map.");
         return;
      }
      dtCore::Map& currMap = *m;

      if (mChildren.valid())
      {
         std::vector<dtCore::NamedParameter*> toFill;
         mChildren->GetParameters(toFill);

         for (unsigned i = 0; i < toFill.size(); ++i)
         {
            dtCore::NamedParameter* np = toFill[i];
            if (np->GetDataType() == dtCore::DataType::ACTOR)
            {
               dtCore::UniqueId id = static_cast<dtCore::NamedActorParameter*>(np)->GetValue();
               dtCore::BaseActorObject* child = currMap.GetProxyById(id);
               if (child != NULL)
               {
                  AddItemToList(*child);
               }
            }
         }
         BlankRowLabels();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::BlankRowLabels()
   {
      QStringList names;
      QString blank(tr(""));

      for (int i = 0; i < mChildrenView->rowCount(); ++i)
      {
         names.push_back(blank);
      }

      mChildrenView->setVerticalHeaderLabels(names);
   }

   void TaskEditor::SwapRows(int firstRow, int secondRow)
   {
      if (firstRow == secondRow)
      {
         return;
      }
      if (firstRow < 0 || firstRow >= mChildrenView->rowCount())
      {
         return;
      }
      if (secondRow < 0 || secondRow >= mChildrenView->rowCount())
      {
         return;
      }

      //have to copy the items because when you call setItem, the old item is deleted.
      QTableWidgetItem* nm1   = new QTableWidgetItem(*mChildrenView->item(firstRow,  0));
      QTableWidgetItem* type1 = new QTableWidgetItem(*mChildrenView->item(firstRow,  1));
      QTableWidgetItem* nm2   = new QTableWidgetItem(*mChildrenView->item(secondRow, 0));
      QTableWidgetItem* type2 = new QTableWidgetItem(*mChildrenView->item(secondRow, 1));
      mChildrenView->setItem(firstRow,  0, nm2);
      mChildrenView->setItem(firstRow,  1, type2);
      mChildrenView->setItem(secondRow, 0, nm1);
      mChildrenView->setItem(secondRow, 1, type1);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::AddItemToList(dtCore::BaseActorObject& proxy)
   {
      const dtCore::ActorType& at = proxy.GetActorType();
      QTableWidgetItem* nm       = new QTableWidgetItem;
      QTableWidgetItem* type     = new QTableWidgetItem;

      nm->setText(tr(proxy.GetName().c_str()));
      nm->setData(Qt::UserRole, QVariant::fromValue(dtCore::RefPtr<dtCore::BaseActorObject>(&proxy)));
      type->setText(tr((at.GetFullName()).c_str()));

      int row = mChildrenView->rowCount();
      mChildrenView->setRowCount(row + 1);

      mChildrenView->setItem(row, 0, nm);
      mChildrenView->setItem(row, 1, type);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool TaskEditor::HasChild(dtCore::BaseActorObject& proxyToTest)
   {
      for (int i = 0; i < mChildrenView->rowCount(); ++i)
      {
         QTableWidgetItem* item = mChildrenView->item(i, 0);
         if (item != NULL)
         {
            QVariant v = item->data(Qt::UserRole);
            dtCore::RefPtr<dtCore::BaseActorObject> proxy = v.value< dtCore::RefPtr<dtCore::BaseActorObject> >();
            if (proxy->GetId() == proxyToTest.GetId())
            {
               return true;
            }
         }
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::RefreshComboBox(const QString& itemName)
   {
      LOGN_DEBUG("taskeditor.cpp", "Refresh Combo Box");
      mComboBox->clear();
      dtCore::Map* m = EditorData::GetInstance().getCurrentMap();

      if (m == NULL)
      {
         LOG_ERROR("Unable read lookup the task actor types because the current map is NULL.");
         return;
      }

      const std::string topLevelProperty("IsTopLevel");

      dtCore::ActorRefPtrVector toFill;
      dtCore::ActorPtrVector selectedActors;
      m->FindProxies(toFill, "", "dtcore.Tasks", "Task Actor");
      EditorData::GetInstance().GetSelectedActors(selectedActors);

      for (unsigned i = 0; i < toFill.size(); ++i)
      {
         dtCore::BaseActorObject* ap = toFill[i].get();
         bool isRemoved = mRemovedTasks.find(ap) != mRemovedTasks.end();
         // We don't want to see actors with parents unless it has been removed from the current parent actor
         // or the checkbox has been selected by the user to explicitly show them.
         if (!isRemoved && mShowTasksWithParents->checkState() == Qt::Unchecked)
         {
            dtCore::BooleanActorProperty* bap = static_cast<dtCore::BooleanActorProperty*>(ap->GetProperty(topLevelProperty));
            if (bap == NULL)
            {
               LOG_ERROR("A task actor named \"" + ap->GetName() + "\" with type \"" + ap->GetActorType().GetCategory()
                  + "." + ap->GetActorType().GetName() + "\" was found that doesn't have an \""
                  + topLevelProperty + "\" property.  Ignoring.");
               continue;
            }

            //don't add non-top-level tasks to the list.
            if (!bap->GetValue())
            {
               continue;
            }
         }

         bool isSelected = false;
         for (unsigned j = 0; j < selectedActors.size(); ++j)
         {
            if (selectedActors[j] == toFill[i].get())
            {
               isSelected = true;
               break;
            }
         }

         if (!isSelected && !HasChild(*ap))
         {
            //TODO if it's not the currently selected actor
            QVariant v = QVariant::fromValue(dtCore::RefPtr<dtCore::BaseActorObject>(ap));
            mComboBox->addItem(tr(ap->GetName().c_str()), v);
         }

      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::OnMoveUpClicked()
   {
      LOGN_DEBUG("taskeditor.cpp", "Move Up");
      int row = mChildrenView->currentRow();
      if (row > 0)
      {
         SwapRows(row, row - 1);
         mChildrenView->setCurrentCell(row - 1, 0);
         RefreshComboBox(tr(""));
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::OnMoveDownClicked()
   {
      LOGN_DEBUG("taskeditor.cpp", "Move Down");
      int row = mChildrenView->currentRow();
      if (row < (mChildrenView->rowCount() - 1))
      {
         SwapRows(row, row + 1);
         mChildrenView->setCurrentCell(row + 1, 0);
         RefreshComboBox(tr(""));
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::OnRemoveChildClicked()
   {
      LOGN_DEBUG("taskeditor.cpp", "Remove Child");
      int row = mChildrenView->currentRow();
      if (row >= 0)
      {
         QTableWidgetItem* item = mChildrenView->item(row, 0);
         if (item != NULL)
         {
            QVariant v = item->data(Qt::UserRole);
            dtCore::RefPtr<dtCore::BaseActorObject> proxy = v.value<dtCore::RefPtr<dtCore::BaseActorObject> >();
            mRemovedTasks.insert(proxy);
         }

         mChildrenView->removeRow(mChildrenView->currentRow());
         RefreshComboBox(tr(""));
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::OnOkClicked()
   {
      LOGN_DEBUG("taskeditor.cpp", "Okay Clicked");
      accept();
   }

   void TaskEditor::OnShowTasksWithParentsChanged(int state)
   {
      LOGN_DEBUG("taskeditor.cpp", "ShowTasksWithParents changed.");
      RefreshComboBox(tr(""));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::EnableEditButtons()
   {
      LOGN_DEBUG("taskeditor.cpp", "Enable Edit Buttons");
      mMoveUp->setDisabled(false);
      mMoveDown->setDisabled(false);
      mRemoveChild->setDisabled(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::DisableEditButtons()
   {
      LOGN_DEBUG("taskeditor.cpp", "Disable Edit Buttons");
      mMoveUp->setDisabled(true);
      mMoveDown->setDisabled(true);
      mRemoveChild->setDisabled(true);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::AddSelected()
   {
      LOGN_DEBUG("taskeditor.cpp", "Add Selected Clicked");
      int index = mComboBox->currentIndex();
      if (index >= 0)
      {
         QVariant v = mComboBox->itemData(index);
         dtCore::RefPtr<dtCore::BaseActorObject> proxy = v.value<dtCore::RefPtr<dtCore::BaseActorObject> >();
         AddItemToList(*proxy);
         BlankRowLabels();

         //remove the item being added from the removed list, if necessary.
         std::set<dtCore::RefPtr<dtCore::BaseActorObject> >::iterator itor = mRemovedTasks.find(proxy);
         if (itor != mRemovedTasks.end())
         {
            mRemovedTasks.erase(itor);
         }

         RefreshComboBox("");
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::OnComboSelectionChanged(int index)
   {
      LOGN_DEBUG("taskeditor.cpp", "Combo Selection Changed");
      if (index >= 0 && index < mComboBox->count())
      {
         mAddExisting->setDisabled(false);
      }
      else
      {
         mAddExisting->setDisabled(true);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::SetTaskChildren(const dtCore::NamedGroupParameter& children)
   {
      mChildren = new dtCore::NamedGroupParameter(children);
      PopulateChildren();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void TaskEditor::GetTaskChildren(dtCore::NamedGroupParameter& toFill) const
   {
      for (int i = 0; i < mChildrenView->rowCount(); ++i)
      {
         QTableWidgetItem* item = mChildrenView->item(i, 0);
         if (item != NULL)
         {
            QVariant v = item->data(Qt::UserRole);
            dtCore::RefPtr<dtCore::BaseActorObject> proxy = v.value<dtCore::RefPtr<dtCore::BaseActorObject> >();
            std::ostringstream ss;
            ss << i;
            dtCore::RefPtr<dtCore::NamedActorParameter> aParam = new dtCore::NamedActorParameter(ss.str());
            aParam->SetValue(proxy->GetId());
            toFill.AddParameter(*aParam);
         }
      }
   }

} // namespace dtEditQt
