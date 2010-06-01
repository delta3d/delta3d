#include <dtInspectorQt/infiniteterrainview.h>
#include "ui_dtinspectorqt.h"

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::InfiniteTerrainView::InfiniteTerrainView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::InfiniteTerrain");

   connect(mUI->infTerSegSizeEdit, SIGNAL(editingFinished()), this, SLOT(OnSegmentSizeChanged()));
   connect(mUI->infTerSegDivEdit, SIGNAL(editingFinished()), this, SLOT(OnSegmentDivisionsChanged()));
   connect(mUI->infTerHorizEdit, SIGNAL(editingFinished()), this, SLOT(OnHorizontalScaleChanged()));
   connect(mUI->infTerVertEdit, SIGNAL(editingFinished()), this, SLOT(OnVerticalScaleChanged()));
   connect(mUI->infTerDistanceEdit, SIGNAL(editingFinished()), this, SLOT(OnBuildDistanceChanged()));
   connect(mUI->infTerSmoothToggle, SIGNAL(stateChanged(int)), this, SLOT(OnSmoothCollisionsToggled(int)));
   connect(mUI->infTerRegenButton, SIGNAL(clicked()), this, SLOT(OnRegenerate()));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::InfiniteTerrainView::~InfiniteTerrainView()
{
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InfiniteTerrainView::OperateOn(dtCore::Base* b)
{
   dtCore::InfiniteTerrain *infiniteterrain = dynamic_cast<dtCore::InfiniteTerrain*>(b);

   mOperateOn = infiniteterrain;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::InfiniteTerrainView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::InfiniteTerrain*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InfiniteTerrainView::OnSegmentSizeChanged()
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetSegmentSize(mUI->infTerSegSizeEdit->value());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InfiniteTerrainView::OnSegmentDivisionsChanged()
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetSegmentDivisions(mUI->infTerSegDivEdit->value());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InfiniteTerrainView::OnHorizontalScaleChanged()
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetHorizontalScale(mUI->infTerHorizEdit->value());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InfiniteTerrainView::OnVerticalScaleChanged()
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetVerticalScale(mUI->infTerVertEdit->value());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InfiniteTerrainView::OnBuildDistanceChanged()
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetBuildDistance(mUI->infTerDistanceEdit->value());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InfiniteTerrainView::OnSmoothCollisionsToggled(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->EnableSmoothCollisions(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InfiniteTerrainView::OnRegenerate()
{
   if (mOperateOn.valid())
   {
      mOperateOn->Regenerate();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::InfiniteTerrainView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->infTerGroupBox->show();

      mUI->infTerSegSizeEdit->setValue(mOperateOn->GetSegmentSize());
      mUI->infTerSegDivEdit->setValue(mOperateOn->GetSegmentDivisions());
      mUI->infTerHorizEdit->setValue(mOperateOn->GetHorizontalScale());
      mUI->infTerVertEdit->setValue(mOperateOn->GetVerticalScale());
      mUI->infTerDistanceEdit->setValue(mOperateOn->GetBuildDistance());
      mUI->infTerSmoothToggle->setChecked(mOperateOn->SmoothCollisionsEnabled());
   }
   else
   {
      mUI->infTerGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////

