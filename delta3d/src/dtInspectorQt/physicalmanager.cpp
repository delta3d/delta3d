#include <dtInspectorQt/physicalmanager.h>
#include <dtCore/odebodywrap.h>
#include "ui_dtinspectorqt.h"


//////////////////////////////////////////////////////////////////////////
dtInspectorQt::PhysicalManager::PhysicalManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   connect(mUI->physicalDynamicsToggle, SIGNAL(stateChanged(int)), this, SLOT(OnDynamicsToggled(int)));
   connect(mUI->physicalMassEdit, SIGNAL(valueChanged(double)), this, SLOT(OnMassChanged(double)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::PhysicalManager::~PhysicalManager()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::PhysicalManager::OperateOn(dtCore::Base* b)
{
   dtCore::Physical *physical = dynamic_cast<dtCore::Physical*>(b);

   mOperateOn = physical;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::PhysicalManager::Update()
{
   if (mOperateOn.valid())
   {
      mUI->physicalGroupBox->show();

      mUI->physicalDynamicsToggle->setChecked(mOperateOn->GetBodyWrapper()->DynamicsEnabled());
      mUI->physicalMassEdit->setValue(mOperateOn->GetBodyWrapper()->GetMass());
   }
   else
   {
      mUI->physicalGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::PhysicalManager::OnDynamicsToggled(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->GetBodyWrapper()->EnableDynamics(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::PhysicalManager::OnMassChanged(double mass)
{
   if (mOperateOn.valid())
   {
      mOperateOn->GetBodyWrapper()->SetMass(mass);
   }
}
