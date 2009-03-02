#include <dtInspectorQt/particlesystemmanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ParticleSystemManager::ParticleSystemManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   connect(mUI->particleSystemEnabledToggle, SIGNAL(stateChanged(int)), this, SLOT(OnEnabled(int)));
   connect(mUI->particleSystemParentRelativeToggle, SIGNAL(stateChanged(int)), this, SLOT(OnParentRelative(int)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ParticleSystemManager::~ParticleSystemManager()
{

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ParticleSystemManager::OperateOn(dtCore::Base* b)
{
   dtCore::ParticleSystem *particlesystem = dynamic_cast<dtCore::ParticleSystem*>(b);

   mOperateOn = particlesystem;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ParticleSystemManager::OnEnabled(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetEnabled(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ParticleSystemManager::OnParentRelative(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetParentRelative(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ParticleSystemManager::Update()
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
