#include <dtInspectorQt/drawablemanager.h>
#include <dtCore/scene.h>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DrawableManager::DrawableManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   connect(mUI->drawableActiveToggle, SIGNAL(stateChanged(int)), this, SLOT(OnActive(int)));
   connect(mUI->drawableProxyNodeToggle, SIGNAL(stateChanged(int)), this, SLOT(OnRenderProxyNode(int)));

}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DrawableManager::~DrawableManager()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DrawableManager::OperateOn(dtCore::Base* b)
{
   dtCore::DeltaDrawable* drawable = dynamic_cast<dtCore::DeltaDrawable*>(b);

   mOperateOn = drawable;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DrawableManager::Update()
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
   }
   else
   {
      mUI->drawableGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DrawableManager::OnRenderProxyNode(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->RenderProxyNode(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DrawableManager::OnActive(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetActive(checked ? true : false);
   }

}
