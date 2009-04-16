#include <dtInspectorQt/physicalview.h>
#include <dtCore/odebodywrap.h>
#include "ui_dtinspectorqt.h"


//////////////////////////////////////////////////////////////////////////
dtInspectorQt::PhysicalView::PhysicalView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::Physical");

   connect(mUI->physicalDynamicsToggle, SIGNAL(stateChanged(int)), this, SLOT(OnDynamicsToggled(int)));
   connect(mUI->physicalMassEdit, SIGNAL(valueChanged(double)), this, SLOT(OnMassChanged(double)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::PhysicalView::~PhysicalView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::PhysicalView::OperateOn(dtCore::Base* b)
{
   dtCore::Physical *physical = dynamic_cast<dtCore::Physical*>(b);

   mOperateOn = physical;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::PhysicalView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::Physical*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::PhysicalView::Update()
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
void dtInspectorQt::PhysicalView::OnDynamicsToggled(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->GetBodyWrapper()->EnableDynamics(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::PhysicalView::OnMassChanged(double mass)
{
   if (mOperateOn.valid())
   {
      mOperateOn->GetBodyWrapper()->SetMass(mass);
   }
}
