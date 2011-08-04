/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2010 MOVES Institute
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/

#ifndef AIUTILITY_PREFERENCES_DIALOG
#define AIUTILITY_PREFERENCES_DIALOG

#include <QtGui/QDialog>

class QCheckBox;

class AIUtilityPreferencesDialog : public QDialog
{
   Q_OBJECT

public:
   /// Constructor
   AIUtilityPreferencesDialog(QWidget* parent = NULL);

   /// Destructor
   virtual ~AIUtilityPreferencesDialog() {}

   void SetRenderSelectionDefaultValue(bool useSelectionRender);
   bool GetRenderSelectionMode() const;

   void SetRenderBackfacesDefaultValue(bool renderBackfaces);
   bool GetRenderBackFaces() const;

private slots:

   /// Called when the Ok button is pressed
   void onOk();

private:
   QCheckBox* mRenderOnSelection;
   QCheckBox* mRenderBackfaces;
};

#endif // AIUTILITY_PREFERENCES_DIALOG
