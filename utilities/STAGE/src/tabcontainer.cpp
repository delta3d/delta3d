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
 * Teague Coonan
 */

#include <prefix/stageprefix.h>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

#include <dtEditQt/tabcontainer.h>
#include <dtEditQt/tabwrapper.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   TabContainer::TabContainer(QWidget* parent)
      : QObject(parent)
   {
      tabC = new QTabWidget (parent);
      tabC->setUsesScrollButtons(true);
      tabC->setTabPosition(QTabWidget::West);
   }
   ///////////////////////////////////////////////////////////////////////////////
   TabContainer::~TabContainer()
   {
      for (unsigned int i = 0; i < tabVector.size(); ++i)
      {
         delete tabVector.at(i);
      }

      tabVector.clear();
   }
   ///////////////////////////////////////////////////////////////////////////////
   void TabContainer::addTab(TabWrapper* myTab)
   {
      // add the tab to the container
      tabC->addTab(myTab->getWidget(), myTab->getName());

      // push the tab into a vector
      tabVector.push_back(myTab);
   }
   ///////////////////////////////////////////////////////////////////////////////
   void TabContainer::addTab(TabWrapper* myTab, const std::string& tabResource)
   {
      QIcon tabIcon;
      QString resource = tabResource.c_str();
      tabIcon.addPixmap(QPixmap(resource));

      // add the tab to the container
      tabC->addTab(myTab->getWidget(), tabIcon, myTab->getName());

      // push the tab into a vector
      tabVector.push_back(myTab);
   }
   ///////////////////////////////////////////////////////////////////////////////
   void TabContainer::setPosition(Position position)
   {
      // set the position to either top or bottom
      tabC->setTabPosition((QTabWidget::TabPosition)position);
   }
   ///////////////////////////////////////////////////////////////////////////////
   QTabWidget* TabContainer::getWidget()
   {
      return tabC;
   }
   ///////////////////////////////////////////////////////////////////////////////

} // namespace dtEditQt
