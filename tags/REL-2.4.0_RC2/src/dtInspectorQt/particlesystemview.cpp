#include <dtInspectorQt/particlesystemview.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ParticleSystemView::ParticleSystemView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::ParticleSystem");

   connect(mUI->particleSystemEnabledToggle, SIGNAL(stateChanged(int)), this, SLOT(OnEnabled(int)));
   connect(mUI->particleSystemParentRelativeToggle, SIGNAL(stateChanged(int)), this, SLOT(OnParentRelative(int)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ParticleSystemView::~ParticleSystemView()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ParticleSystemView::OperateOn(dtCore::Base* b)
{
   dtCore::ParticleSystem *particlesystem = dynamic_cast<dtCore::ParticleSystem*>(b);

   mOperateOn = particlesystem;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::ParticleSystemView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::ParticleSystem*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ParticleSystemView::OnEnabled(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetEnabled(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ParticleSystemView::OnParentRelative(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetParentRelative(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ParticleSystemView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->particleSystemGroupBox->show();

      mUI->particleSystemEnabledToggle->setChecked(mOperateOn->IsEnabled());
      mUI->particleSystemParentRelativeToggle->setChecked(mOperateOn->IsParentRelative());
   }
   else
   {
      mUI->particleSystemGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
