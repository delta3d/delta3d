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
 * William E. Johnson II
 */

#ifndef DELTA_EDITOR_ABOUT_BOX
#define DELTA_EDITOR_ABOUT_BOX

#include <QtGui/QDialog>

class QTextEdit;

namespace dtEditQt
{
   /**
    * @class EditorAboutBox
    * @brief This class generates the Editor's about box.
    */
   class EditorAboutBox : public QDialog
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      EditorAboutBox(QWidget* parent = 0);

      /**
       * Destructor
       */
      virtual ~EditorAboutBox() {}

   private:
      /**
       * Creates a tab displaying copyright information about the level editor.
       * @return Text edit object.
       */
      QTextEdit* createEditorInfoTab();

      /**
       * Creates a tab displaying the GPL license.
       * @return Text edit object.
       */
      QTextEdit* createLicenseTab();
   };

} // namespace dtEditQt

#endif // DELTA_EDITOR_ABOUT_BOX
