#include <dtInspectorQt/cameramanager.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::CameraManager::CameraManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   connect(mUI->cameraEnabledToggle, SIGNAL(stateChanged(int)), this, SLOT(OnEnabled(int)));
   connect(mUI->cameraVertEdit, SIGNAL(valueChanged(double)), this, SLOT(OnPerspectiveChanged(double)));
   connect(mUI->cameraAspectEdit, SIGNAL(valueChanged(double)), this, SLOT(OnPerspectiveChanged(double)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::CameraManager::~CameraManager()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraManager::OperateOn(dtCore::Base* b)
{
   dtCore::Camera *camera = dynamic_cast<dtCore::Camera*>(b);

   mOperateOn = camera;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraManager::Update()
{
   if (mOperateOn.valid())
   {
      mUI->cameraGroupBox->show();

      mUI->cameraEnabledToggle->setChecked(mOperateOn->GetEnabled());

      double vfov, ar, nearClip, farClip;
      mOperateOn->GetPerspectiveParams(vfov, ar, nearClip, farClip);
      mUI->cameraVertEdit->setValue(vfov);
      mUI->cameraAspectEdit->setValue(ar);
      mUI->cameraNearClipEdit->setValue(nearClip);
      mUI->cameraFarClipEdit->setValue(farClip);
   }
   else
   {
      mUI->cameraGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraManager::OnEnabled(int state)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetEnabled(state ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::CameraManager::OnPerspectiveChanged(double value)
{
   if (mOperateOn.valid())
   {
      double aspectRatio = mUI->cameraAspectEdit->value();
      double vertFOV = mUI->cameraVertEdit->value();
      mOperateOn->SetPerspectiveParams(vertFOV, aspectRatio, 1.0, 10.f); //near/far auto calculated
   }
}
