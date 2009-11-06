#include <dtInspectorQt/viewview.h>
#include "ui_dtinspectorqt.h"
#include <dtCore/scene.h>
#include <dtCore/camera.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ViewView::ViewView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::View");

   connect(mUI->viewRenderOrderEdit, SIGNAL(valueChanged(int)), this, SLOT(OnRenderOrderChanged(int)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ViewView::~ViewView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ViewView::OperateOn(dtCore::Base* b)
{
   dtCore::View *view = dynamic_cast<dtCore::View*>(b);

   mOperateOn = view;

   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::ViewView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::View*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ViewView::Update()
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

      mUI->viewRenderOrderEdit->setValue(mOperateOn->GetRenderOrder());
   }
   else
   {
      mUI->viewGroupBox->hide();
   }

}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ViewView::OnRenderOrderChanged(int order)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetRenderOrder(order);
   }
}
