#include <dtInspectorQt/skydomeview.h>
#include "ui_dtinspectorqt.h"
#include <QtGui/QColorDialog>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SkyDomeView::SkyDomeView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::SkyDome");

   connect(mUI->envSkyRedEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
   connect(mUI->envSkyGreenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
   connect(mUI->envSkyBlueEdit, SIGNAL(valueChanged(double)), this, SLOT(OnColorChanged(double)));
   connect(mUI->skyDomeColorButton, SIGNAL(clicked()), this, SLOT(OnColorPickerClicked()));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::SkyDomeView::~SkyDomeView()
{
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SkyDomeView::OperateOn(dtCore::Base* b)
{
   dtCore::SkyDome *skydome = dynamic_cast<dtCore::SkyDome*>(b);

   mOperateOn = skydome;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::SkyDomeView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::SkyDome*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SkyDomeView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->skyDomeGroupBox->show();

      osg::Vec3 color;
      mOperateOn->GetBaseColor(color);
      mUI->envSkyRedEdit->setValue(color[0]);
      mUI->envSkyGreenEdit->setValue(color[1]);
      mUI->envSkyBlueEdit->setValue(color[2]);
   }
   else
   {
      mUI->skyDomeGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SkyDomeView::OnColorChanged(double val)
{
   if (mOperateOn.valid())
   {
      osg::Vec3 color(mUI->skyDomeRedEdit->value(),
                      mUI->skyDomeGreenEdit->value(),
                      mUI->skyDomeBlueEdit->value());

      mOperateOn->SetBaseColor(color);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::SkyDomeView::OnColorPickerClicked()
{
   QColor initialColor(mUI->skyDomeRedEdit->value() * 255,
      mUI->skyDomeGreenEdit->value() * 255, mUI->skyDomeBlueEdit->value() * 255);
   QColor newColor = QColorDialog::getColor(initialColor, mUI->skyDomeColorButton);
   if (newColor.isValid())
   {
      mUI->skyDomeRedEdit->setValue(newColor.redF());
      mUI->skyDomeGreenEdit->setValue(newColor.greenF());
      mUI->skyDomeBlueEdit->setValue(newColor.blueF());
   }
}

//////////////////////////////////////////////////////////////////////////

