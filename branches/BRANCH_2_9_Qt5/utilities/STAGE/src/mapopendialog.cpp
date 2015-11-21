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
 * Jeff P. Houde
 */
#include <prefix/stageprefix.h>
#include <dtEditQt/mapopendialog.h>

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTreeWidget>

#include <dtCore/map.h>

namespace dtEditQt
{
   ////////////////////////////////////////////////////////////////////////////////
   MapOpenDialog::MapOpenDialog(QWidget* parent)
      : QDialog(parent)
   {
      setWindowTitle(tr("Open Map"));

      QGroupBox*   groupBox = new QGroupBox("Available Maps:", this);
      QGridLayout* gridLayout = new QGridLayout(groupBox);

      // Create our map tree.
      const dtCore::Project::MapTreeData& mapTree = dtCore::Project::GetInstance().GetMapTree();
      mMapTree = new QTreeWidget(groupBox);
      mMapTree->setHeaderHidden(true);
      gridLayout->addWidget(mMapTree, 0, 0, 2, 1);
      PopulateTree(mapTree);

      // Create the description label.
      QLabel* label = new QLabel(tr("Map Description:"), groupBox);
      label->setAlignment(Qt::AlignLeft);
      mDescription = new QTextEdit(groupBox);
      mDescription->setReadOnly(true);
      gridLayout->addWidget(label, 0, 1);
      gridLayout->addWidget(mDescription, 1, 1);

      // Our Ok and Cancel buttons.
      mOkButton = new QPushButton(tr("Ok"), this);
      mCancelButton = new QPushButton(tr("Cancel"), this);
      mOkButton->setEnabled(false);

      QHBoxLayout* buttonLayout = new QHBoxLayout;

      buttonLayout->addStretch(1);
      buttonLayout->addWidget(mOkButton);
      buttonLayout->addWidget(mCancelButton);
      buttonLayout->addStretch(1);

      QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->addWidget(groupBox);
      mainLayout->addLayout(buttonLayout);

      connect(mOkButton,     SIGNAL(clicked()), this, SLOT(accept()));
      connect(mCancelButton, SIGNAL(clicked()), this, SLOT(reject()));

      connect(mMapTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(MapItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
      connect(mMapTree, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(MapItemActivated(QTreeWidgetItem*, int)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   QString MapOpenDialog::GetMapName()
   {
      MapTreeItem* item = static_cast<MapTreeItem*>(mMapTree->currentItem());
      if (item && item->isMap)
      {
         return item->text(0);
      }

      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MapOpenDialog::MapItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
   {
      MapTreeItem* mapItem = static_cast<MapTreeItem*>(current);

      if (mapItem && mapItem->isMap)
      {
         try
         {
            // Find the description of this map.
            dtCore::MapPtr header = dtCore::Project::GetInstance().GetMapHeader(mapItem->text(0).toStdString());

            if (header->GetDescription().empty())
            {
               mDescription->setText("<No Description>");
            }
            else
            {
               mDescription->setText(header->GetDescription().c_str());
            }
         }
         catch(const dtUtil::Exception& ex)
         {
            mDescription->setText(ex.ToString().c_str());
         }

         mOkButton->setEnabled(true);
      }
      else
      {
         mDescription->setText("<No Map Selected>");

         mOkButton->setDisabled(true);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MapOpenDialog::MapItemActivated(QTreeWidgetItem* item, int column)
   {
      MapTreeItem* mapItem = static_cast<MapTreeItem*>(item);

      if (mapItem && mapItem->isMap)
      {
         accept();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MapOpenDialog::PopulateTree(const dtCore::Project::MapTreeData& tree, QTreeWidgetItem* parent)
   {
      int count = (int)tree.subCategories.size();
      for (int index = 0; index < count; ++index)
      {
         const dtCore::Project::MapTreeData& subTree = tree.subCategories[index];

         MapTreeItem* newItem = new MapTreeItem(parent, subTree.categoryName, false);
         if (newItem)
         {
            if (!parent)
            {
               // No parent means top level items.
               mMapTree->addTopLevelItem(newItem);
            }

            PopulateTree(subTree, newItem);
         }
      }

      std::set<std::string>::const_iterator iter;
      for (iter = tree.mapList.begin(); iter != tree.mapList.end(); ++iter)
      {
         std::string mapName = *iter;

         MapTreeItem* newItem = new MapTreeItem(parent, mapName);
         if (!parent && newItem)
         {
            // No parent means top level items.
            mMapTree->addTopLevelItem(newItem);
         }
      }
   }

} // namespace dtEditQt
