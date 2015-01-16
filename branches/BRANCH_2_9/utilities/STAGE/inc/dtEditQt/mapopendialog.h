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

#ifndef DELTA_MAP_OPEN_DIALOG
#define DELTA_MAP_OPEN_DIALOG

#include <QtGui/QDialog>
#include <QtGui/QTreeWidgetItem>

#include <dtCore/project.h>


class QLineEdit;
class QTextEdit;
class QPushButton;
class QTreeWidget;

namespace dtCore
{
   class Map;
}

namespace dtEditQt
{
   class MapTreeItem : public QTreeWidgetItem
   {
   public:
      MapTreeItem(QTreeWidgetItem* parent, const std::string& text, bool isMap = true)
         : QTreeWidgetItem(parent)
      {
         setText(0, text.c_str());
         this->isMap = isMap;
      }

      bool isMap;
   };

   class MapOpenDialog : public QDialog
   {
      Q_OBJECT

   public:
      /// Constructor
      MapOpenDialog(QWidget* parent = NULL);

      /// Destructor
      virtual ~MapOpenDialog() { }

      /// Retrieves the name of the map the user entered
      QString GetMapName();

   public slots:

      /// Event handler when a map is highlighted.
      void MapItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

      /// Event handler when a map has been activated.
      void MapItemActivated(QTreeWidgetItem* item, int column);

   private:

      /// Populates the map tree using a recursive method.
      void PopulateTree(const dtCore::Project::MapTreeData& tree, QTreeWidgetItem* parent = NULL);

      QTreeWidget* mMapTree;
      QTextEdit*   mDescription;
      QPushButton* mOkButton;
      QPushButton* mCancelButton;
   };

} // namespace dtEditQt

#endif // DELTA_MAP_OPEN_DIALOG
