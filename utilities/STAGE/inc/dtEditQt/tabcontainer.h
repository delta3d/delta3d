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
#ifndef DELTA_TAB_CONTAINER
#define DELTA_TAB_CONTAINER

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QTabWidget>
#include <vector>

class QIcon;

namespace dtEditQt
{

   class TabWrapper;

   /**
    * @class TabContainer
    * @brief Tab Container that holds tabs.
    */
   class TabContainer : public QObject
   {
   public:
      enum Position
      {
         Top,
         Bottom
      };

      /**
       * Constructor
       */
      TabContainer(QWidget* parent = 0);

      /**
       * Destructor
       */
      ~TabContainer();

      /**
       * Adds a new tab
       * @param TabWrapper object
       */
      void addTab(TabWrapper* myTab);

      /**
       * Adds a new tab and uses a tab icon instead of text
       * @param TabWrapper object
       * @param std::string that represents a UIResource which is a image sourcefile path
       */
      void addTab(TabWrapper* myTab, const std::string& tabResource);

      /**
       * Sets the position of the tabs
       * @param Position position
       */
      void setPosition(Position position);

      /**
       * @return Instantiated QTabWidget
       */
      QTabWidget* getWidget();

   private:
      QTabWidget* tabC;
      std::vector<TabWrapper*> tabVector;
      Position position;
   };

} // namespace dtEditQt

#endif // DELTA_TAB_CONTAINER
