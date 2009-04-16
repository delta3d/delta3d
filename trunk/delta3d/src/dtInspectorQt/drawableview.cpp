#include "ui_dtinspectorqt.h"
#include <dtInspectorQt/drawableview.h>
#include <dtCore/scene.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DrawableView::DrawableView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::DeltaDrawable");

   connect(mUI->drawableActiveToggle, SIGNAL(stateChanged(int)), this, SLOT(OnActive(int)));
   connect(mUI->drawableProxyNodeToggle, SIGNAL(stateChanged(int)), this, SLOT(OnRenderProxyNode(int)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DrawableView::~DrawableView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DrawableView::OperateOn(dtCore::Base* b)
{
   dtCore::DeltaDrawable* drawable = dynamic_cast<dtCore::DeltaDrawable*>(b);

   mOperateOn = drawable;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::DrawableView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::DeltaDrawable*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DrawableView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->drawableGroupBox->show();

      //children
      mUI->drawableChildList->clear();
      for (unsigned int childIdx=0; childIdx<mOperateOn->GetNumChildren(); childIdx++)
      {
         dtCore::RefPtr<dtCore::DeltaDrawable> child = mOperateOn->GetChild(childIdx);
         mUI->drawableChildList->addItem(QString::fromStdString(child->GetName()));
      }

      //parent
      std::string parentName;
      if (mOperateOn->GetParent() != NULL)
      {
         parentName = mOperateOn->GetParent()->GetName();
      }
      mUI->drawableParentLabel->setText(QString::fromStdString(parentName));


      //parent scene
      std::string parentSceneName;
      if (mOperateOn->GetSceneParent() != NULL)
      {
         parentSceneName = mOperateOn->GetSceneParent()->GetName();
      }
      mUI->drawableParentSceneLabel->setText(QString::fromStdString(parentSceneName));

      //Active
      mUI->drawableActiveToggle->setChecked(mOperateOn->GetActive());

      //proxy node
      mUI->drawableProxyNodeToggle->setChecked(mOperateOn->GetIsRenderingProxyNode());
   }
   else
   {
      mUI->drawableGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DrawableView::OnRenderProxyNode(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->RenderProxyNode(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DrawableView::OnActive(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetActive(checked ? true : false);
   }

}
