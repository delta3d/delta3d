#include <dtInspectorQt/objectview.h>
#include "ui_dtinspectorqt.h"
#include <QtGui/QFileDialog>

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ObjectView::ObjectView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtCore::Object");

   connect(mUI->objectScaleXEdit, SIGNAL(valueChanged(double)), this, SLOT(OnScaleChanged(double)));
   connect(mUI->objectScaleYEdit, SIGNAL(valueChanged(double)), this, SLOT(OnScaleChanged(double)));
   connect(mUI->objectScaleZEdit, SIGNAL(valueChanged(double)), this, SLOT(OnScaleChanged(double)));

   connect(mUI->objectXEdit, SIGNAL(valueChanged(double)), this, SLOT(OnOffsetChanged(double)));
   connect(mUI->objectYEdit, SIGNAL(valueChanged(double)), this, SLOT(OnOffsetChanged(double)));
   connect(mUI->objectZEdit, SIGNAL(valueChanged(double)), this, SLOT(OnOffsetChanged(double)));
   connect(mUI->objectHEdit, SIGNAL(valueChanged(double)), this, SLOT(OnOffsetChanged(double)));
   connect(mUI->objectPEdit, SIGNAL(valueChanged(double)), this, SLOT(OnOffsetChanged(double)));
   connect(mUI->objectREdit, SIGNAL(valueChanged(double)), this, SLOT(OnOffsetChanged(double)));

   connect(mUI->objectFilenameEdit, SIGNAL(editingFinished()), this, SLOT(OnFilenameChanged()));
   connect(mUI->objectFileButton, SIGNAL(clicked()), this, SLOT(OnFindFile()));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::ObjectView::~ObjectView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ObjectView::OperateOn(dtCore::Base* b)
{
   dtCore::Object *object = dynamic_cast<dtCore::Object*>(b);

   mOperateOn = object;
   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::ObjectView::IsOfType(QString name, dtCore::Base* object)
{
   if (name == mFilterName && dynamic_cast<dtCore::Object*>(object) != NULL)
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ObjectView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->objectGroupBox->show();

      mUI->objectFilenameEdit->setText(QString::fromStdString(mOperateOn->GetFilename()));

      const osg::Vec3 scale = mOperateOn->GetScale();
      mUI->objectScaleXEdit->setValue(scale[0]);
      mUI->objectScaleYEdit->setValue(scale[1]);
      mUI->objectScaleZEdit->setValue(scale[2]);

      const osg::Vec3 xyz = mOperateOn->GetModelTranslation();
      mUI->objectXEdit->setValue(xyz[0]);
      mUI->objectYEdit->setValue(xyz[1]);
      mUI->objectZEdit->setValue(xyz[2]);

      const osg::Vec3 hpr = mOperateOn->GetModelRotation();
      mUI->objectHEdit->setValue(hpr[0]);
      mUI->objectPEdit->setValue(hpr[1]);
      mUI->objectREdit->setValue(hpr[2]);
   }
   else
   {
      mUI->objectGroupBox->hide();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ObjectView::OnScaleChanged(double val)
{
   if (mOperateOn.valid())
   {
      osg::Vec3 scale(mUI->objectScaleXEdit->value(),
                      mUI->objectScaleYEdit->value(),
                      mUI->objectScaleZEdit->value());

      mOperateOn->SetScale(scale);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ObjectView::OnOffsetChanged(double val)
{
   if (mOperateOn.valid())
   {
      osg::Vec3 xyz(mUI->objectXEdit->value(),
                    mUI->objectYEdit->value(),
                    mUI->objectZEdit->value());

      osg::Vec3 hpr(mUI->objectHEdit->value(),
                    mUI->objectPEdit->value(),
                    mUI->objectREdit->value());

      mOperateOn->SetModelTranslation(xyz);
      mOperateOn->SetModelRotation(hpr);
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ObjectView::OnFilenameChanged()
{
   if (mOperateOn.valid())
   {
      mOperateOn->RecenterGeometryUponLoad(mUI->objRecenterToggle->isChecked());

      const QString text = mUI->objectFilenameEdit->text();
      mOperateOn->LoadFile(text.toStdString());
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::ObjectView::OnFindFile()
{
   QString filename = QFileDialog::getOpenFileName();
   if (!filename.isEmpty())
   {
      mUI->objectFilenameEdit->setText(filename);
      OnFilenameChanged();
   }
}
