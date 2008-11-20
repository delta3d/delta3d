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

#ifndef DELTA_PREFERENCES_DIALOG
#define DELTA_PREFERENCES_DIALOG

#include <QtGui/QDialog>

class QCheckBox;
class QPushButton;
class QResizeEvent;
class QSpinBox;

namespace dtEditQt
{
   class PreferencesDialog : public QDialog
   {
      Q_OBJECT

      public:

         /// Constructor
         PreferencesDialog(QWidget *parent = NULL);

         /// Destructor
         virtual ~PreferencesDialog() { }

      public slots:

         /// Called when the last project checkbox is altered
         /// @param state The new checkbox state
         void onLastProjectCheckBox(int state);

         /// Called when the last project checkbox is altered
         /// @param state The new checkbox state
         void onLastMapCheckBox(int state);

         /// Called when the Ok button is pressed
         void onOk();

         /// Called when the color selection button is pressed
         void onColorSelect();

      private:

         QCheckBox   *mProjectCheck;
         QCheckBox   *mMapCheck;
         QCheckBox   *mRigidCamCheck;
         QPushButton *mColor;
         QSpinBox    *mSaveMins;

         /// Helper method to set the new color palette and text
         void setNewPalette();
   };
}
#endif
