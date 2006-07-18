/* 
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
* Copyright (C) 2005, BMH Associates, Inc. 
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
* @author Curtiss Murphy
*/

#include <QtGui/QIcon>

#include "dtEditQt/actortypetreewidget.h"
#include "dtDAL/librarymanager.h"
#include "dtEditQt/uiresources.h"
#include <QtCore/QStringList>
#include <dtUtil/log.h>
namespace dtEditQt 
{

    // constant 
    std::string ActorTypeTreeWidget::CATEGORY_SEPARATOR = ".";

    ///////////////////////////////////////////////////////////////////////////////
    ActorTypeTreeWidget::ActorTypeTreeWidget(ActorTypeTreeWidget* parent, dtCore::RefPtr<dtDAL::ActorType> actorType) 
        : QTreeWidgetItem(parent)
    {
        LOG_INFO("Initializing ActorTypeTreeWidget - leaf Actor Type Node:" + actorType->GetName());

        myActorType = actorType;    
        categorySegment = (const char *) NULL;
        
        // setup data
        if (myActorType != NULL)
        {
            // This sets our actor icon
            QIcon *actorIcon = new QIcon();
            actorIcon->addPixmap(QPixmap(UIResources::ICON_ACTOR.c_str()));

            setText(0, myActorType->GetName().c_str());
            setToolTip(0, myActorType->GetDescription().c_str());
            setIcon(0,*actorIcon);
        }

    }

    ///////////////////////////////////////////////////////////////////////////////
    ActorTypeTreeWidget::ActorTypeTreeWidget(ActorTypeTreeWidget* parent, const QString &str)
        : QTreeWidgetItem(parent)
    {
       LOG_INFO("Initializing ActorTypeTreeWidget - Internal node:"  + str.toStdString() );

        myActorType = NULL;

        categorySegment = str;
        setText(0, categorySegment);
            
        QIcon *icon = new QIcon();
        icon->addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()),QIcon::Normal,QIcon::On);
        icon->addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()),QIcon::Normal,QIcon::Off);

        setIcon(0,*icon);
    }

    ///////////////////////////////////////////////////////////////////////////////
    ActorTypeTreeWidget::ActorTypeTreeWidget(QTreeWidget* parent, const QString &str)
        : QTreeWidgetItem(parent)
    {
       LOG_INFO("Initializing ActorTypeTreeWidget - as a root node (QTreeWidget parent):"+ str.toStdString());

        myActorType = NULL;

        categorySegment = str;
        setText(0, categorySegment);

        QIcon *icon = new QIcon();
        icon->addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER_OPEN.c_str()),QIcon::Normal,QIcon::On);
        icon->addPixmap(QPixmap(UIResources::ICON_TINY_FOLDER.c_str()),QIcon::Normal,QIcon::Off);

        setIcon(0,*icon);
    }

    ///////////////////////////////////////////////////////////////////////////////
    dtCore::RefPtr<dtDAL::ActorType> ActorTypeTreeWidget::getActorType() 
    {
        return myActorType;
    }

    ///////////////////////////////////////////////////////////////////////////////
    bool ActorTypeTreeWidget::isLeafNode() 
    {
        if (myActorType == NULL) 
        {
            return false;
        } 
        else 
        {
            return true;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    const QString &ActorTypeTreeWidget::getCategorySegment()
    {
        return categorySegment;
    }

    ///////////////////////////////////////////////////////////////////////////////
    QString ActorTypeTreeWidget::getCategoryOrName()
    {
        if (isLeafNode()) 
        {
            return QString(myActorType->GetName().c_str());
        } 
        else 
        {
            return categorySegment;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    bool ActorTypeTreeWidget::recursivelyAddCategoryAndActorTypeAsChildren(
        QMutableStringListIterator *listIterator, dtCore::RefPtr<dtDAL::ActorType> actorType) 
    {
        bool foundChildMatch= false;
        bool result = false;

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
                for (int i = 0; i < childCount(); i ++) 
                {
                    QTreeWidgetItem *child = this->child(i);
                    ActorTypeTreeWidget *actorChild = dynamic_cast<ActorTypeTreeWidget *>(child);
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
                    ActorTypeTreeWidget *innerNode = new ActorTypeTreeWidget(this, subCategory);
                    result = innerNode->recursivelyAddCategoryAndActorTypeAsChildren(listIterator, actorType);
                }
            }
        }

        // return whether we did or didn't add the node, regardless of how deep we had to go.
        return result;
    }

}
