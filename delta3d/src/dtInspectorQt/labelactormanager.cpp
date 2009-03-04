#include <dtInspectorQt/labelactormanager.h>
#include "ui_dtinspectorqt.h"
#include <QtGui/QColorDialog>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::LabelActorManager::LabelActorManager(Ui::InspectorWidget& ui)
:mUI(&ui)
{
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
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::LabelActorManager::~LabelActorManager()
{
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorManager::OperateOn(dtCore::Base* b)
{
   dtABC::LabelActor *labelactor = dynamic_cast<dtABC::LabelActor*>(b);

   mOperateOn = labelactor;
   Update();
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorManager::OnTextChanged()
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetText(mUI->labelActorTextEdit->toPlainText().toStdString());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorManager::OnAlignmentChanged(int newAlignment)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetTextAlignment(static_cast<dtABC::LabelActor::AlignmentType>(newAlignment));
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorManager::OnFontSizeChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      mOperateOn->SetFontSize(newValue);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorManager::OnLightingToggled(int checked)
{
   if (mOperateOn.valid())
   {
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorManager::OnBackdropSizeChanged(double newValue)
{
   if (mOperateOn.valid())
   {
      osg::Vec2 size(mUI->labelActorBackdropWidthEdit->value(),
         mUI->labelActorBackdropHeightEdit->value());
      mOperateOn->SetBackSize(size);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::LabelActorManager::OnTextColorChanged(double newValue)
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
void dtInspectorQt::LabelActorManager::OnTextColorPickerClicked()
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
void dtInspectorQt::LabelActorManager::OnBackdropColorChanged(double newValue)
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
void dtInspectorQt::LabelActorManager::OnBackdropColorPickerClicked()
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
void dtInspectorQt::LabelActorManager::Update()
{
   if (mOperateOn.valid())
   {
      mUI->labelActorGroupBox->show();

      mUI->labelActorTextEdit->setPlainText(QString::fromStdString(mOperateOn->GetText()));
      mUI->labelActorFontSizeEdit->setValue(mOperateOn->GetFontSize());
      mUI->labelActorAlignmentCombo->setCurrentIndex(mOperateOn->GetTextAlignment());
      //mUI->labelActorLightingToggle->setChecked(mOperateOn->GetLightingEnabled());

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

