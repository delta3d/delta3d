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
 * Eric R. Heine
 */
#include <prefix/dtqtprefix.h>
#include <dtQt/resourceselectorwidget.h>

#include <dtCore/project.h>

#include <QtGui/QMenu>

namespace dtQt
{
   ///////////////////////////////////////////////////////////////////////////////
   ResourceSelectorWidget::ResourceSelectorWidget(QWidget* parent /*= NULL*/)
   : QToolButton(parent)
   {
      setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
      setIcon(QIcon(QPixmap(1, 1)));
      setPopupMode(QToolButton::InstantPopup);
   }

   /////////////////////////////////////////////////////////////////////////////////
   ResourceSelectorWidget::~ResourceSelectorWidget()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceSelectorWidget::SetResourceType(QStringList prependedActions, const dtCore::DataType& dataType)
   {
      dtUtil::tree<dtCore::ResourceTreeNode> tree;
      dtCore::Project::GetInstance().GetResourcesOfType(dataType, tree);
      QMenu* menu = new QMenu("Resources");
      connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(ItemSelected(QAction*)));

      // Add prepended actions
      for (int actionIndex = 0; actionIndex < prependedActions.size(); ++actionIndex)
      {
         menu->addAction(prependedActions[actionIndex]);
      }

      SetupMenu(tree, menu);
      setMenu(menu);

      // Set our default text to the first action item in the menu
      if (menu->actions().size() > 0)
      {
         setText(menu->actions()[0]->text());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceSelectorWidget::SetResource(const QString& value)
   {
      setText(value);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ResourceSelectorWidget::ItemSelected(QAction* action)
   {
      SetResource(action->text());
      emit ResourceSelected(action);
      emit ResourceSelected(action->data().toString());
   }

   //////////////////////////////////////////////////////////////////////////
   void ResourceSelectorWidget::SetupMenu(const dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator& iter, QMenu* menu)
   {
      for (dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator i = iter.tree_ref().in();
         i != iter.tree_ref().end();
         ++i)
      {
         if (i->isCategory())
         {
            QMenu* subMenu = menu->addMenu(i->getNodeText().c_str());

            SetupMenu(i, subMenu);
         }
         else
         {
            QAction* action = menu->addAction(i->getResource().GetDisplayName().c_str());
            if (action)
            {
               action->setData(QVariant(i->getResource().GetResourceIdentifier().c_str()));
            }
         }
      }
   }
} // namespace dtEditQt
