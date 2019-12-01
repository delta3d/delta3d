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

#ifndef DELTA_PROJECT_BROWSER
#define DELTA_PROJECT_BROWSER

#include <QtGui/QWidget>

class QGroupBox;
class QPushButton;

namespace dtEditQt
{

   /**
    * @class ProjectBrowser
    * @brief This class provides the user with a mechanism to browse and manipulate
    *        their current project
    */
   class ProjectBrowser : public QWidget
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      ProjectBrowser(QWidget* parent = 0);

      /**
       * Destructor
       */
      ~ProjectBrowser();

   private slots:
      /**
       * Slot - Used as a mechanism to trigger a refresh if the user believes
       *        something has changed or been added to the project.
       */
      void refreshButtonClicked();

   private:
      /**
       * This defines the layout for projects
       * @return a QGroupBox layout widget
       */
      QGroupBox*   projectGroup();
      QPushButton* refreshButton;
   };

} // namespace dtEditQt

#endif // DELTA_PROJECT_BROWSER
