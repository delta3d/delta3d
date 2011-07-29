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

#include <prefix/dtqtprefix.h>
#include <QtGui/QIcon>

#include "objecttypetreewidget.h"
#include <QtCore/QStringList>
#include <dtUtil/log.h>

// constant
const std::string ObjectTypeTreeWidget::CATEGORY_SEPARATOR = ".";

///////////////////////////////////////////////////////////////////////////////
ObjectTypeTreeWidget::ObjectTypeTreeWidget(ObjectTypeTreeWidget* parent, dtCore::RefPtr<const dtCore::ObjectType> objectType)
   : QTreeWidgetItem(parent)
{
   LOG_DEBUG("Initializing ObjectTypeTreeWidget - leaf Actor Type Node:" + objectType->GetName());

   mObjectType = objectType;

   // setup data
   if (mObjectType != NULL)
   {
      // This sets our actor icon
//      QIcon actorIcon;
//      actorIcon.addPixmap(QPixmap(UIResources::ICON_ACTOR.c_str()));

      setText(0,    mObjectType->GetName().c_str());
      setToolTip(0, mObjectType->GetDescription().c_str());
//      setIcon(0,    actorIcon);
   }
}

///////////////////////////////////////////////////////////////////////////////
ObjectTypeTreeWidget::ObjectTypeTreeWidget(ObjectTypeTreeWidget* parent, const QString& str)
   : QTreeWidgetItem(parent)
{
   LOG_DEBUG("Initializing ObjectTypeTreeWidget - Internal node:"  + str.toStdString());

   mObjectType = NULL;

   mCategorySegment = str;
   setText(0, mCategorySegment);

   QIcon icon;
   //icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()), QIcon::Normal, QIcon::On);
   //icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()),      QIcon::Normal, QIcon::Off);

   setIcon(0,icon);
}

///////////////////////////////////////////////////////////////////////////////
ObjectTypeTreeWidget::ObjectTypeTreeWidget(QTreeWidget* parent, const QString& str)
   : QTreeWidgetItem(parent)
{
   LOG_DEBUG("Initializing ObjectTypeTreeWidget - as a root node (QTreeWidget parent):" + str.toStdString());

   mObjectType = NULL;

   mCategorySegment = str;
   setText(0, mCategorySegment);

   QIcon icon;
   //icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()), QIcon::Normal, QIcon::On);
   //icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()),      QIcon::Normal, QIcon::Off);

   setIcon(0, icon);
}

///////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<const dtCore::ObjectType> ObjectTypeTreeWidget::GetObjectType()
{
   return mObjectType;
}

///////////////////////////////////////////////////////////////////////////////
bool ObjectTypeTreeWidget::IsLeafNode()
{
   if (mObjectType == NULL)
   {
      return false;
   }
   else
   {
      return true;
   }
}

///////////////////////////////////////////////////////////////////////////////
const QString& ObjectTypeTreeWidget::GetCategorySegment()
{
   return mCategorySegment;
}

///////////////////////////////////////////////////////////////////////////////
QString ObjectTypeTreeWidget::GetCategoryOrName()
{
   if (IsLeafNode())
   {
      return QString(mObjectType->GetName().c_str());
   }
   else
   {
      return mCategorySegment;
   }
}

///////////////////////////////////////////////////////////////////////////////
bool ObjectTypeTreeWidget::RecursivelyAddCategoryAndObjectTypeAsChildren(
   QMutableStringListIterator* listIterator, dtCore::RefPtr<const dtCore::ObjectType> objectType)
{
   bool foundChildMatch = false;
   bool result          = false;

   // error
   if (listIterator == NULL)
   {
      result = false;
   }
   // if we're at the end of the list, then add the actor to this node.
   else if (!listIterator->hasNext())
   {
      new ObjectTypeTreeWidget(this, objectType);
      result = true;
   }

   // if we're not at the end, then pop off the first sub-category and work with it to find
   // our rightful place in the tree
   else
   {
      QString subCategory = listIterator->next();
      listIterator->remove(); // in effect, pop the first subCategory off.

      // if for some reason the next sub category is empty, then just skip it and recurse.
      if (subCategory.isNull() || subCategory.isEmpty())
      {
         result = RecursivelyAddCategoryAndObjectTypeAsChildren(listIterator, objectType);
      }
      else
      {
         // take the category and try to find it as a child of this tree node.
         for (int i = 0; i < childCount(); ++i)
         {
            QTreeWidgetItem*     childWidget = child(i);
            ObjectTypeTreeWidget* objectChild  = dynamic_cast<ObjectTypeTreeWidget*>(childWidget);
            if (objectChild != NULL && subCategory == objectChild->GetCategorySegment())
            {
               result = objectChild->RecursivelyAddCategoryAndObjectTypeAsChildren(listIterator, objectType);
               foundChildMatch = true;
               break;
            }
         }

         // if we didn't find a match, then make an internal node and add the type to that
         if (!foundChildMatch)
         {
            result = true;
            ObjectTypeTreeWidget* innerNode = new ObjectTypeTreeWidget(this, subCategory);
            result = innerNode->RecursivelyAddCategoryAndObjectTypeAsChildren(listIterator, objectType);
         }
      }
   }

   // return whether we did or didn't add the node, regardless of how deep we had to go.
   return result;
}
