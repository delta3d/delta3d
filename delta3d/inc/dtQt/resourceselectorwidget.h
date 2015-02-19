/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamicresourcecontrol (.h & .cpp) - Using 'The MIT License'
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
 * Eric R. Heine
 */
#ifndef DELTA_RESOURCESELECTORWIDGET
#define DELTA_RESOURCESELECTORWIDGET

#include <dtQt/export.h>

#include <dtCore/resourcetreenode.h>
#include <dtUtil/utiltree.h>

#include <QtGui/QToolButton>

namespace dtCore
{
   class DataType;
}

namespace dtQt
{
   /**
    * @class ResourceSelectorWidget
    * @brief This is a widget that can be used to select a resource of a given
    * type.  It looks like a combo box with submenus.
    */
   class DT_QT_EXPORT ResourceSelectorWidget : public QToolButton
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      ResourceSelectorWidget(QWidget* parent = NULL);

      /**
       * Destructor
       */
      virtual ~ResourceSelectorWidget();

      void SetResourceType(QStringList prependedActions, const dtCore::DataType& dataType);

      void SetResource(const QString& value);

   signals:
      void ResourceSelected(QAction* action);
      void ResourceSelected(const QString& identifier);

   public slots:
      void ItemSelected(QAction* action);

   private:
      void SetupMenu(const dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator& iter, QMenu* menu);
   };

} // namespace dtQt

#endif // DELTA_RESOURCESELECTORWIDGET
