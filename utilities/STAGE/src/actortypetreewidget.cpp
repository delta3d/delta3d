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

#include <prefix/stageprefix.h>
#include <QtGui/QIcon>

#include <dtEditQt/actortypetreewidget.h>
#include <dtCore/actorfactory.h>
#include <dtEditQt/uiresources.h>
#include <QtCore/QStringList>
#include <dtUtil/log.h>

namespace dtEditQt
{

   // constant
   const std::string ActorTypeTreeWidget::CATEGORY_SEPARATOR = ".";

   ///////////////////////////////////////////////////////////////////////////////
   ActorTypeTreeWidget::ActorTypeTreeWidget(ActorTypeTreeWidget* parent, dtCore::RefPtr<const dtCore::ActorType> actorType)
      : QTreeWidgetItem(parent)
   {
      LOG_DEBUG("Initializing ActorTypeTreeWidget - leaf Actor Type Node:" + actorType->GetName());

      mActorType = actorType;

      // setup data
      if (mActorType != NULL)
      {
         // This sets our actor icon
         QIcon actorIcon;
         actorIcon.addPixmap(QPixmap(UIResources::ICON_ACTOR.c_str()));

         setText(0,    mActorType->GetName().c_str());
         setToolTip(0, mActorType->GetDescription().c_str());
         setIcon(0,    actorIcon);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorTypeTreeWidget::ActorTypeTreeWidget(ActorTypeTreeWidget* parent, const QString& str)
      : QTreeWidgetItem(parent)
   {
      LOG_DEBUG("Initializing ActorTypeTreeWidget - Internal node:"  + str.toStdString());

      mActorType = NULL;

      mCategorySegment = str;
      setText(0, mCategorySegment);

      QIcon icon;
      icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()), QIcon::Normal, QIcon::On);
      icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()),      QIcon::Normal, QIcon::Off);

      setIcon(0,icon);
   }

   ///////////////////////////////////////////////////////////////////////////////
   ActorTypeTreeWidget::ActorTypeTreeWidget(QTreeWidget* parent, const QString& str)
      : QTreeWidgetItem(parent)
   {
      LOG_DEBUG("Initializing ActorTypeTreeWidget - as a root node (QTreeWidget parent):" + str.toStdString());

      mActorType = NULL;

      mCategorySegment = str;
      setText(0, mCategorySegment);

      QIcon icon;
      icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()), QIcon::Normal, QIcon::On);
      icon.addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()),      QIcon::Normal, QIcon::Off);

      setIcon(0, icon);
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<const dtCore::ActorType> ActorTypeTreeWidget::getActorType()
   {
      return mActorType;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool ActorTypeTreeWidget::isLeafNode()
   {
      if (mActorType == NULL)
      {
         return false;
      }
      else
      {
         return true;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   const QString& ActorTypeTreeWidget::getCategorySegment()
   {
      return mCategorySegment;
   }

   ///////////////////////////////////////////////////////////////////////////////
   QString ActorTypeTreeWidget::getCategoryOrName()
   {
      if (isLeafNode())
      {
         return QString(mActorType->GetName().c_str());
      }
      else
      {
         return mCategorySegment;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool ActorTypeTreeWidget::recursivelyAddCategoryAndActorTypeAsChildren(
      QMutableStringListIterator* listIterator, dtCore::RefPtr<const dtCore::ActorType> actorType)
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
         new ActorTypeTreeWidget(this, actorType);
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
            result = recursivelyAddCategoryAndActorTypeAsChildren(listIterator, actorType);
         }
         else
         {
            // take the category and try to find it as a child of this tree node.
            for (int i = 0; i < childCount(); ++i)
            {
               QTreeWidgetItem*     childWidget = child(i);
               ActorTypeTreeWidget* actorChild  = dynamic_cast<ActorTypeTreeWidget*>(childWidget);
               if (actorChild != NULL && subCategory == actorChild->getCategorySegment())
               {
                  result = actorChild->recursivelyAddCategoryAndActorTypeAsChildren(listIterator, actorType);
                  foundChildMatch = true;
                  break;
               }
            }

            // if we didn't find a match, then make an internal node and add the type to that
            if (!foundChildMatch)
            {
               result = true;
               ActorTypeTreeWidget* innerNode = new ActorTypeTreeWidget(this, subCategory);
               result = innerNode->recursivelyAddCategoryAndActorTypeAsChildren(listIterator, actorType);
            }
         }
      }

      // return whether we did or didn't add the node, regardless of how deep we had to go.
      return result;
   }

} // namespace dtEditQt
