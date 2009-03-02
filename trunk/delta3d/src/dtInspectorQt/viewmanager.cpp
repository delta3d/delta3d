#include <dtInspectorQt/viewmanager.h>
#include "ui_dtinspectorqt.h"
#include <dtCore/scene.h>
#include <dtCore/camera.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ViewManager::ViewManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ViewManager::~ViewManager()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ViewManager::OperateOn(dtCore::Base* b)
{
   dtCore::View *view = dynamic_cast<dtCore::View*>(b);

   mOperateOn = view;

   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ViewManager::Update()
{
   if (mOperateOn.valid())
   {
      mUI->viewGroupBox->show();

      //camera
      dtCore::Camera* camera = mOperateOn->GetCamera();
      if (camera)
      {
         mUI->viewCameraEdit->setText(QString::fromStdString(camera->GetName()));
      }
      else
      {
         mUI->viewCameraEdit->setText("");
      }

      dtCore::Scene* scene = mOperateOn->GetScene();
      if (scene)
      {
         mUI->viewSceneEdit->setText(QString::fromStdString(scene->GetName()));
      }
   }
   else
   {
      mUI->viewGroupBox->hide();
   }

}
