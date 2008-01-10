/*
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
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
