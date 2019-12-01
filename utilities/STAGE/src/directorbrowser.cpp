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
#include <QtCore/QDir>
#include <QtGui/QHeaderView>

#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QSplitter>
#include <QtCore/QString>

#include <QtGui/QGroupBox>

#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>

#include <QtGui/QAction>
#include <QtGui/QContextMenuEvent>

#include <dtEditQt/resourcetreewidget.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>

#include <dtEditQt/uiresources.h>

#include <dtEditQt/directorbrowser.h>

#include <dtCore/project.h>

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   DirectorBrowser::DirectorBrowser(dtCore::DataType& type, QWidget* parent)
      : ResourceAbstractBrowser(&type, parent)
   {
      // This sets our resource icon that is visible on leaf nodes
      QIcon resourceIcon;
      resourceIcon.addPixmap(QPixmap(UIResources::ICON_DIRECTOR_RESOURCE.c_str()));
      ResourceAbstractBrowser::mResourceIcon = resourceIcon;

      // setup right mouse click context menu
      createActions();
      createContextMenu();

      connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
         this, SLOT(selectionChanged()));

      QGridLayout* grid = new QGridLayout(this);
      grid->addWidget(listGroup(), 0, 0);
      grid->addWidget(standardButtons(QString("Resource Tools")), 1, 0, Qt::AlignCenter);
   }

   ///////////////////////////////////////////////////////////////////////////////
   DirectorBrowser::~DirectorBrowser(){}

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* DirectorBrowser::listGroup()
   {
      QGroupBox* group = new QGroupBox(this);
      QGridLayout* grid = new QGridLayout(group);

      grid->addWidget(mTree, 0, 0);
      mTree->setResourceName("Director");

      return group;
   }

   ///////////////////////////////////////////////////////////////////////////////
   // Keyboard Event filter
   ///////////////////////////////////////////////////////////////////////////////
   bool DirectorBrowser::eventFilter(QObject* obj, QEvent* e)
   {
      if (obj == mTree)
      {
         // For some reason, KeyPress is getting defined by something...
         // Without this undef, it will not compile under Linux..
         // It would be great if someone could figure out exactly what's
         // going on.
#undef KeyPress
         if (e->type() == QEvent::KeyPress)
         {
            QKeyEvent* keyEvent = (QKeyEvent*)e;
            switch (keyEvent->key())
            {
            case Qt::Key_Return:
               if (mSelection->isResource())
               {
                  selectionChanged();
               }
               break;
            case Qt::Key_Enter:
               if (mSelection->isResource())
               {
                  selectionChanged();
               }
               break;
            default:
               return mTree->eventFilter(obj, e);
            }
         }
         else
         {
            // pass the event on to the parent class
            return mTree->eventFilter(obj, e);
         }
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void DirectorBrowser::selectionChanged()
   {
      ResourceAbstractBrowser::selectionChanged();
   }
   ///////////////////////////////////////////////////////////////////////////////

} // namespace dtEditQt
