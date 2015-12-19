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
const MotionModelType MotionModelType::FLY("Fly", MM_FLY, ":dtQt/icons/common/motion_fly.png");
const MotionModelType MotionModelType::ORBIT("Orbit", MM_ORBIT, ":dtQt/icons/common/motion_orbit.png");
const MotionModelType MotionModelType::UFO("UFO", MM_UFO, ":dtQt/icons/common/motion_ufo.png");



MotionModelType::MotionModelType(const std::string& name, MotionModelTypeE value, const std::string& iconPath)
   : BaseClass(name)
   , mValue(value)
   , mIconPath(iconPath)
{
   AddInstance(this);
}

MotionModelType::~MotionModelType()
{}

MotionModelTypeE MotionModelType::GetValue() const
{
   return mValue;
}

const std::string& MotionModelType::GetIconPath() const
{
   return mIconPath;
}



/////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
MotionModelPropertiesWidget::MotionModelPropertiesWidget(QWidget* parent)
   : BaseClass(parent)
{
   mUI.setupUi(this);
}



/////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
MotionModelToolbar::MotionModelToolbar(QWidget* parent)
   : BaseClass(parent)
   , mCurrentMotionModel(&MotionModelType::ORBIT)
{
   mPropWidget = new MotionModelPropertiesWidget;

   QIcon iconFly(MotionModelType::FLY.GetIconPath().c_str());
   QIcon iconOrbit(MotionModelType::ORBIT.GetIconPath().c_str());
   QIcon iconUFO(MotionModelType::UFO.GetIconPath().c_str());

   QAction* actionFly = new QAction(iconFly, "Fly", this);
   QAction* actionOrbit = new QAction(iconOrbit, "Orbit", this);
   QAction* actionUFO = new QAction(iconUFO, "UFO", this);

   addAction(actionFly);
   addAction(actionOrbit);
   addAction(actionUFO);
   addWidget(mPropWidget);

   mActionMap[actionFly] = &MotionModelType::FLY;
   mActionMap[actionOrbit] = &MotionModelType::ORBIT;
   mActionMap[actionUFO] = &MotionModelType::UFO;

   CreateConnections();

   UpdateUI();
}

MotionModelToolbar::~MotionModelToolbar()
{
   delete mPropWidget;
   mPropWidget = nullptr;
}

void MotionModelToolbar::CreateConnections()
{
   // BUTTONS
   connect(this, SIGNAL(actionTriggered(QAction*)),
      this, SLOT(OnButtonClick(QAction*)));

   // SPINNERS
   connect(mPropWidget->mUI.mSpeed, SIGNAL(valueChanged(double)),
      this, SLOT(OnSpeedChanged(double)));
}

void MotionModelToolbar::UpdateUI()
{
   mPropWidget->mUI.mMotionModelName->setText(tr(mCurrentMotionModel->GetName().c_str()));
}

const MotionModelType& MotionModelToolbar::GetCurrentMotioModel() const
{
   return *mCurrentMotionModel;
}

void MotionModelToolbar::OnButtonClick(QAction* action)
{
   const MotionModelType* mmType = mCurrentMotionModel;

   ActionMotionModelTypeMap::iterator foundIter = mActionMap.find(action);

   if (foundIter != mActionMap.end())
   {
      mmType = foundIter->second;
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
   SignalMotionModelSpeedChanged(mCurrentMotionModel->GetValue(), mPropWidget->mUI.mSpeed->value());
}

