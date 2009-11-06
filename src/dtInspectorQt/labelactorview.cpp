#include <dtInspectorQt/labelactorview.h>
#include "ui_dtinspectorqt.h"
#include <QtGui/QColorDialog>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::LabelActorView::LabelActorView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtABC::LabelActor");

   connect(mUI->labelActorTextEdit, SIGNAL(textChanged()), this, SLOT(OnTextChanged()));
   connect(mUI->labelActorAlignmentCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnAlignmentChanged(int)));
   connect(mUI->labelActorFontSizeEdit, SIGNAL(valueChanged(double)), this, SLOT(OnFontSizeChanged(double)));
   connect(mUI->labelActorLightingToggle, SIGNAL(stateChanged(int)), this, SLOT(OnLightingToggled(int)));
   connect(mUI->labelActorBackdropWidthEdit, SIGNAL(valueChanged(double)), this, SLOT(OnBackdropSizeChanged(double)));
   connect(mUI->labelActorBackdropHeightEdit, SIGNAL(valueChanged(double)), this, SLOT(OnBackdropSizeChanged(double)));
   connect(mUI->labelActorTextRedEdit, SIGNAL(valueChanged(double)), this, SLOT(OnTextColorChanged(double)));
   connect(mUI->labelActorTextGreenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnTextColorChanged(double)));
   connect(mUI->labelActorTextBlueEdit, SIGNAL(valueChanged(double)), this, SLOT(OnTextColorChanged(double)));
   connect(mUI->labelActorTextAlphaEdit, SIGNAL(valueChanged(double)), this, SLOT(OnTextColorChanged(double)));
   connect(mUI->labelActorTextColorButton, SIGNAL(clicked()), this, SLOT(OnTextColorPickerClicked()));
   connect(mUI->labelActorBackdropRedEdit, SIGNAL(valueChanged(double)), this, SLOT(OnBackdropColorChanged(double)));
   connect(mUI->labelActorBackdropGreenEdit, SIGNAL(valueChanged(double)), this, SLOT(OnBackdropColorChanged(double)));
   connect(mUI->labelActorBackdropBlueEdit, SIGNAL(valueChanged(double)), this, SLOT(OnBackdropColorChanged(double)));
   connect(mUI->labelActorBackdropAlphaEdit, SIGNAL(valueChanged(double)), this, SLOT(OnBackdropColorChanged(double)));
   connect(mUI->labelActorBackdropColorButton, SIGNAL(clicked()), this, SLOT(OnBackdropColorPickerClicked()));
   connect(mUI->labelActorBackToggle, SIGNAL(stateChanged(int)), this, SLOT(OnBackdropToggled(int)));
   connect(mUI->labelActorDepthToggle, SIGNAL(stateChanged(int)), this, SLOT(OnDepthTestingToggled(int)));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::LabelActorView::~LabelActorView()
{
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OperateOn(dtCore::Base* b)
{
   dtABC::LabelActor *labelactor = dynamic_cast<dtABC::LabelActor*>(b);

   mOperateOn = labelactor;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::LabelActorView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtABC::LabelActor*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OnTextChanged()
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetText(mUI->labelActorTextEdit->toPlainText().toStdString());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OnAlignmentChanged(int newAlignment)
{
   if (mOperateOn.valid())
   {
      osgText::TextBase::AlignmentType type = static_cast<osgText::TextBase::AlignmentType>(newAlignment);
      mOperateOn->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::FromOSGType(type));
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OnFontSizeChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetFontSize(newValue);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OnLightingToggled(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetEnableLighting(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OnBackdropSizeChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      osg::Vec2 size(mUI->labelActorBackdropWidthEdit->value(),
         mUI->labelActorBackdropHeightEdit->value());
      mOperateOn->SetBackSize(size);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OnTextColorChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      osg::Vec4 color(mUI->labelActorTextRedEdit->value(),
         mUI->labelActorTextGreenEdit->value(), mUI->labelActorTextBlueEdit->value(),
         mUI->labelActorTextAlphaEdit->value());
      mOperateOn->SetTextColor(color);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OnTextColorPickerClicked()
{
   QColor initialColor(mUI->labelActorTextRedEdit->value() * 255,
      mUI->labelActorTextGreenEdit->value() * 255, mUI->labelActorTextBlueEdit->value() * 255);
   QColor newColor = QColorDialog::getColor(initialColor, mUI->labelActorTextColorButton);
   if (newColor.isValid())
   {
      mUI->labelActorTextRedEdit->setValue(newColor.redF());
      mUI->labelActorTextGreenEdit->setValue(newColor.greenF());
      mUI->labelActorTextBlueEdit->setValue(newColor.blueF());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OnBackdropColorChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      osg::Vec4 color(mUI->labelActorBackdropRedEdit->value(),
         mUI->labelActorBackdropGreenEdit->value(), mUI->labelActorBackdropBlueEdit->value(),
         mUI->labelActorBackdropAlphaEdit->value());
      mOperateOn->SetBackColor(color);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OnBackdropColorPickerClicked()
{
   QColor initialColor(mUI->labelActorBackdropRedEdit->value() * 255,
      mUI->labelActorBackdropGreenEdit->value() * 255, mUI->labelActorBackdropBlueEdit->value() * 255);
   QColor newColor = QColorDialog::getColor(initialColor, mUI->labelActorBackdropColorButton);
   if (newColor.isValid())
   {
      mUI->labelActorBackdropRedEdit->setValue(newColor.redF());
      mUI->labelActorBackdropGreenEdit->setValue(newColor.greenF());
      mUI->labelActorBackdropBlueEdit->setValue(newColor.blueF());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->labelActorGroupBox->show();

      mUI->labelActorTextEdit->setPlainText(QString::fromStdString(mOperateOn->GetText()));
      mUI->labelActorFontSizeEdit->setValue(mOperateOn->GetFontSize());
      mUI->labelActorAlignmentCombo->setCurrentIndex(mOperateOn->GetTextAlignment().ToOSGType());
      mUI->labelActorLightingToggle->setChecked(mOperateOn->GetEnableLighting());
      mUI->labelActorBackToggle->setChecked(mOperateOn->IsBackVisible());
      mUI->labelActorDepthToggle->setChecked(mOperateOn->GetEnableDepthTesting());

      osg::Vec2 size = mOperateOn->GetBackSize();
      mUI->labelActorBackdropWidthEdit->setValue(size[0]);
      mUI->labelActorBackdropHeightEdit->setValue(size[1]);

      osg::Vec4 textColor = mOperateOn->GetTextColor();
      mUI->labelActorTextRedEdit->setValue(textColor[0]);
      mUI->labelActorTextGreenEdit->setValue(textColor[1]);
      mUI->labelActorTextBlueEdit->setValue(textColor[2]);
      mUI->labelActorTextAlphaEdit->setValue(textColor[3]);

      osg::Vec4 backdropColor = mOperateOn->GetBackColor();
      mUI->labelActorBackdropRedEdit->setValue(backdropColor[0]);
      mUI->labelActorBackdropGreenEdit->setValue(backdropColor[1]);
      mUI->labelActorBackdropBlueEdit->setValue(backdropColor[2]);
      mUI->labelActorBackdropAlphaEdit->setValue(backdropColor[3]);


   }
   else
   {
      mUI->labelActorGroupBox->hide();
   }
}
//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OnBackdropToggled(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetBackVisible(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorView::OnDepthTestingToggled(int checked)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetEnableDepthTesting(checked ? true : false);
   }
}

//////////////////////////////////////////////////////////////////////////

