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
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>

#include "aiutilitypreferencesdialog.h"


////////////////////////////////////////////////////////////////////////////////
AIUtilityPreferencesDialog::AIUtilityPreferencesDialog(QWidget* parent)
: QDialog(parent)
, mRenderOnSelection(NULL)
{
   QVBoxLayout* vLay   = new QVBoxLayout(this);
   QGroupBox*   group  = new QGroupBox(tr("Preferences"));
   QGridLayout* grid   = new QGridLayout(group);
   QHBoxLayout* hLay   = new QHBoxLayout;
   QPushButton* ok     = new QPushButton(tr("OK"));
   QPushButton* cancel = new QPushButton(tr("Cancel"));
   QLabel*      label  = new QLabel(tr("Selection based rendering."));
   QLabel* backfaceLabel = new QLabel(tr("Render back faces."));

   mRenderOnSelection = new QCheckBox;
   mRenderOnSelection->setChecked(false);
   mRenderOnSelection->setToolTip(tr("Allows visualization of waypoint edges and text by selection."));

   mRenderBackfaces = new QCheckBox;
   mRenderBackfaces->setChecked(false);
   mRenderBackfaces->setToolTip(tr("Cause geometry not facing the camera to be rendered (typically culled)."));

   label->setToolTip(tr("Allows visualization of waypoint edges and text by selection."));

   vLay->addWidget(group);

   grid->addWidget(label, 0, 0);
   grid->addWidget(mRenderOnSelection, 0, 2);
   grid->addWidget(backfaceLabel, 1, 0);
   grid->addWidget(mRenderBackfaces, 1, 2);

   hLay->addStretch(1);
   hLay->addWidget(ok);
   hLay->addWidget(cancel);
   hLay->addStretch(1);

   vLay->addLayout(hLay);

   connect(ok, SIGNAL(clicked()), this, SLOT(onOk()));
   connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

   setWindowTitle(tr("Preference Editor"));

   setModal(true);
   setFixedSize(325, 150);
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityPreferencesDialog::SetRenderSelectionDefaultValue(bool useSelectionRender)
{
   mRenderOnSelection->setChecked(useSelectionRender);
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityPreferencesDialog::onOk()
{
   accept();
}

////////////////////////////////////////////////////////////////////////////////
bool AIUtilityPreferencesDialog::GetRenderSelectionMode() const
{
   return mRenderOnSelection->isChecked();
}

////////////////////////////////////////////////////////////////////////////////
void AIUtilityPreferencesDialog::SetRenderBackfacesDefaultValue(bool renderBackfaces)
{
   mRenderBackfaces->setChecked(renderBackfaces);
}

////////////////////////////////////////////////////////////////////////////////
bool AIUtilityPreferencesDialog::GetRenderBackFaces() const
{
   return mRenderBackfaces->isChecked();
}
