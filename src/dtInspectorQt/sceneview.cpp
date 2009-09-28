#include <dtInspectorQt/sceneview.h>
#include <dtCore/refptr.h>
#include <dtCore/deltadrawable.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SceneView::SceneView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::Scene");

   connect(mUI->sceneLightToggle, SIGNAL(stateChanged(int)), this, SLOT(OnUseSceneLight(int)));
   connect(mUI->sceneFaceCombo, SIGNAL(activated(int)), this, SLOT(OnRenderStateChanged(int)));
   connect(mUI->sceneModeCombo, SIGNAL(activated(int)), this, SLOT(OnRenderStateChanged(int)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SceneView::~SceneView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SceneView::OperateOn(dtCore::Base* b)
{
   dtCore::Scene* drawable = dynamic_cast<dtCore::Scene*>(b);

   mOperateOn = drawable;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::SceneView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::Scene*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SceneView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->sceneGroupBox->show();

      //children
      mUI->sceneChildList->clear();
      for (unsigned int childIdx=0; childIdx<mOperateOn->GetNumberOfAddedDrawable(); childIdx++)
      {
         dtCore::RefPtr<dtCore::DeltaDrawable> child = mOperateOn->GetDrawable(childIdx);
         mUI->sceneChildList->addItem(QString::fromStdString(child->GetName()));
      }

      //render state
      const std::pair<dtCore::Scene::Face, dtCore::Scene::Mode> state = mOperateOn->GetRenderState();
      switch(state.first)
      {
      case dtCore::Scene::FRONT:mUI->sceneFaceCombo->setCurrentIndex(0);     	break;
      case dtCore::Scene::BACK:mUI->sceneFaceCombo->setCurrentIndex(1);     	break;
      case dtCore::Scene::FRONT_AND_BACK:mUI->sceneFaceCombo->setCurrentIndex(2);     	break;
      default:  break;
      }

      switch(state.second)
      {
      case dtCore::Scene::POINT:mUI->sceneModeCombo->setCurrentIndex(0);     	break;
      case dtCore::Scene::LINE:mUI->sceneModeCombo->setCurrentIndex(1);     	break;
      case dtCore::Scene::FILL:mUI->sceneModeCombo->setCurrentIndex(2);     	break;
      default:  break;
      }
   }
   else
   {
      mUI->sceneGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SceneView::OnUseSceneLight(int state)
{
   if (mOperateOn.valid())
   {
      mOperateOn->UseSceneLight( state ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SceneView::OnRenderStateChanged(int mode)
{
   if (mOperateOn.valid())
   {
      dtCore::Scene::Face faceEnum;
      switch(mUI->sceneFaceCombo->currentIndex())
      {
      case 0: faceEnum = dtCore::Scene::FRONT; 	break;
      case 1: faceEnum = dtCore::Scene::BACK; 	break;
      case 2: faceEnum = dtCore::Scene::FRONT_AND_BACK; 	break;
      default: break;
      }

      dtCore::Scene::Mode modeEnum;
      switch(mUI->sceneModeCombo->currentIndex())
      {
      case 0: modeEnum = dtCore::Scene::POINT; 	break;
      case 1: modeEnum = dtCore::Scene::LINE; 	break;
      case 2: modeEnum = dtCore::Scene::FILL; 	break;
      default: break;
      }

      mOperateOn->SetRenderState(faceEnum, modeEnum);
   }
}
