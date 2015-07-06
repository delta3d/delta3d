/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
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
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "MotionModelToolbar.h"



////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(MotionModelType);
const MotionModelType MotionModelType::FLY("Fly", MM_FLY);
const MotionModelType MotionModelType::ORBIT("Orbit", MM_ORBIT);
const MotionModelType MotionModelType::UFO("UFO", MM_UFO);

MotionModelType::MotionModelType(const std::string& name, MotionModelTypeE value)
   : BaseClass(name)
   , mValue(value)
{
   AddInstance(this);
}

MotionModelType::~MotionModelType()
{}

MotionModelTypeE MotionModelType::GetValue() const
{
   return mValue;
}



/////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
MotionModelToolbar::MotionModelToolbar(QWidget* parent)
   : BaseClass(parent)
   , mCurrentMotionModel(&MotionModelType::ORBIT)
{
   mUI.setupUi(this);

   // Set initial checked button.
   // TODO:

   CreateConnections();

   UpdateUI();
}

MotionModelToolbar::~MotionModelToolbar()
{}

void MotionModelToolbar::CreateConnections()
{
   // BUTTONS
   connect(mUI.mButtonFly, SIGNAL(pressed()), this, SLOT(OnButtonClick()));
   connect(mUI.mButtonOrbit, SIGNAL(pressed()), this, SLOT(OnButtonClick()));
   connect(mUI.mButtonUFO, SIGNAL(pressed()), this, SLOT(OnButtonClick()));

   // SPINNERS
   connect(mUI.mSpeed, SIGNAL(valueChanged(double)),
      this, SLOT(OnSpeedChanged(double)));
}

void MotionModelToolbar::UpdateUI()
{
   mUI.mMotionModelName->setText(tr(mCurrentMotionModel->GetName().c_str()));
}

const MotionModelType& MotionModelToolbar::GetCurrentMotioModel() const
{
   return *mCurrentMotionModel;
}

void MotionModelToolbar::OnButtonClick()
{
   const MotionModelType* mmType = mCurrentMotionModel;

   QObject* button = sender();
   // Determine which motion model button has been clicked.
   if (button == mUI.mButtonFly)
   {
      mmType = &MotionModelType::FLY;
   }
   else if (button == mUI.mButtonOrbit)
   {
      mmType = &MotionModelType::ORBIT;
   }
   else if (button == mUI.mButtonUFO)
   {
      mmType = &MotionModelType::UFO;
   }

   if (mCurrentMotionModel != mmType)
   {
      mCurrentMotionModel = mmType;

      UpdateUI();

      SignalMotionModelSelected(mCurrentMotionModel->GetValue());
   }
}

void MotionModelToolbar::OnSpeedChanged(double speed)
{
   SignalMotionModelSpeedChanged(mCurrentMotionModel->GetValue(), mUI.mSpeed->value());
}

