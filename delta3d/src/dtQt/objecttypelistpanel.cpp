/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_objecttypelistpanel.h"
#include <dtQt/objecttypelistpanel.h>
#include <dtCore/actorfactory.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   ObjectTypeListPanel::ObjectTypeListPanel(QWidget* parent)
      : BaseClass(parent)
      , mUI(new Ui::ObjectTypeListPanel)
   {
      mUI->setupUi(this);

      SetSingleSelectMode(false);
   }

   ObjectTypeListPanel::~ObjectTypeListPanel()
   {
      delete mUI;
      mUI = NULL;
   }

   void ObjectTypeListPanel::CreateConnections()
   {
      connect(mUI->mList, SIGNAL(itemSelectionChanged()),
         this, SLOT(OnSelectionChanged()));
   }

   void ObjectTypeListPanel::SetFilterFunc(FilterFunc filterFunc)
   {
      mFilterFunc = filterFunc;
   }

   void ObjectTypeListPanel::SetItemCreateFunc(ItemCreateFunc createFunc)
   {
      mItemCreateFunc = createFunc;
   }

   void ObjectTypeListPanel::SetList(const ObjectTypeList& objTypeList)
   {
      // The control's item list is about to change.
      // Prevent signals firing during the insert, which could
      // potentially cause problems.
      mUI->mList->blockSignals(true);

      // Remove old items to make way for newer current items.
      mUI->mList->clear();

      const dtCore::ObjectType* curType = NULL;
      ObjectTypeList::const_iterator curIter = objTypeList.begin();
      ObjectTypeList::const_iterator endIter = objTypeList.end();
      for (; curIter != endIter; ++curIter)
      {
         curType = *curIter;

         // Add the item if the filter allows it or if there is no filter.
         if ( ! mFilterFunc.valid() || mFilterFunc(*curType))
         {
            QString qstr(curType->GetName().c_str());

            QListWidgetItem* item = NULL;

            if (mItemCreateFunc.valid())
            {
               mItemCreateFunc(*curType);
            }
            else
            {
               item = new QListWidgetItem(qstr);
            }

            // Ensure the item refers to the object type by its name.
            // The text cannot be assumed to match since user
            // code may choose to change the display text to
            // show something other than the object's raw name.
            item->setData(Qt::EditRole, qstr);

            mUI->mList->addItem(item);
         }
      }

      mObjectTypeList = objTypeList;

      // Restore signal functionality since the list has finished being updated.
      mUI->mList->blockSignals(false);

      emit SignalUpdated();
   }

   int ObjectTypeListPanel::GetList(ObjectTypeList& outList) const
   {
      return GetObjectTypesFromList(outList, false);
   }

   int ObjectTypeListPanel::GetSelection(ObjectTypeList& outSelection) const
   {
      return GetObjectTypesFromList(outSelection, true);
   }

   int ObjectTypeListPanel::GetObjectTypesFromList(ObjectTypeList& outList, bool selectedOnly) const
   {
      int results = 0;

      const dtCore::ObjectType* curType = NULL;
      int limit = mUI->mList->count();
      for (int i = 0; i < limit; ++i)
      {
         QListWidgetItem* item = mUI->mList->item(i);

         bool allowItem = ! selectedOnly || item->isSelected();

         if (allowItem)
         {
            QString qstr = item->data(Qt::EditRole).toString();
            curType = FindObjectType(qstr.toStdString());

            if (curType != NULL)
            {
               outList.push_back(curType);
               
               ++results;
            }
         }
      }

      return results;
   }

   const dtCore::ObjectType* ObjectTypeListPanel::FindObjectType(const std::string& name) const
   {
      const dtCore::ObjectType* result = NULL;

      const dtCore::ObjectType* curType = NULL;
      ObjectTypeList::const_iterator curIter = mObjectTypeList.begin();
      ObjectTypeList::const_iterator endIter = mObjectTypeList.end();
      for (; curIter != endIter; ++curIter)
      {
         curType = *curIter;

         if (curType->GetName() == name)
         {
            result = curType;
            break;
         }
      }

      return result;
   }

   void ObjectTypeListPanel::OnSelectionChanged()
   {
      ObjectTypeList objTypeList;
      if (0 < GetSelection(objTypeList))
      {
         emit SignalSelectionChanged(objTypeList);
      }
   }

   int ObjectTypeListPanel::GetItemCount() const
   {
      return mUI->mList->count();
   }

   void ObjectTypeListPanel::SetSingleSelectMode(bool singleSelect)
   {
      mUI->mList->setSelectionMode(singleSelect
         ? QAbstractItemView::SingleSelection
         : QAbstractItemView::MultiSelection);
   }

   bool ObjectTypeListPanel::IsSingleSelectMode() const
   {
      return mUI->mList->selectionMode() == QAbstractItemView::SingleSelection;
   }

   void ObjectTypeListPanel::UpdateUI()
   {
      dtCore::ActorTypeVec actorTypesList;
      ObjectTypeList typesList;

      dtCore::ActorFactory& libManager = dtCore::ActorFactory::GetInstance();
      libManager.GetActorTypes(actorTypesList);

      typesList.insert(typesList.end(), actorTypesList.begin(), actorTypesList.end());

      SetList(typesList);
   }

}
