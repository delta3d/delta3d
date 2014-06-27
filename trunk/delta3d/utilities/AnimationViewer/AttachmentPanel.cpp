
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "AttachmentPanel.h"
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <osgAnimation/Bone>
#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/boneinterface.h>
#include <dtUtil/fileutils.h>



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
AttachmentPanel::AttachmentPanel(QWidget* parent)
   : BaseClass(parent)
   , mSliderScale(100.0f)
{
   mUI.setupUi(this);

   CreateConnections();

   UpdateUI();
}

AttachmentPanel::~AttachmentPanel()
{}

void AttachmentPanel::CreateConnections()
{
   // BUTTONS
   connect(mUI.mButtonLoad, SIGNAL(clicked()),
      this, SLOT(OnLoadAttachmentClicked()));

   // COMBO BOX
   connect(mUI.mComboBones, SIGNAL(currentIndexChanged(int)),
      this, SLOT(OnBoneSelectedFromList(int)));

   // SPINNERS
   connect(mUI.mSliderRotX, SIGNAL(valueChanged(int)),
      this, SLOT(OnRotationSliderChanged()));
   connect(mUI.mSliderRotY, SIGNAL(valueChanged(int)),
      this, SLOT(OnRotationSliderChanged()));
   connect(mUI.mSliderRotZ, SIGNAL(valueChanged(int)),
      this, SLOT(OnRotationSliderChanged()));

   connect(mUI.mSliderOffsetX, SIGNAL(valueChanged(int)),
      this, SLOT(OnOffsetSliderChanged()));
   connect(mUI.mSliderOffsetY, SIGNAL(valueChanged(int)),
      this, SLOT(OnOffsetSliderChanged()));
   connect(mUI.mSliderOffsetZ, SIGNAL(valueChanged(int)),
      this, SLOT(OnOffsetSliderChanged()));

   // SLIDERS
   connect(mUI.mSpinRotX, SIGNAL(valueChanged(double)),
      this, SLOT(OnRotationChanged()));
   connect(mUI.mSpinRotY, SIGNAL(valueChanged(double)),
      this, SLOT(OnRotationChanged()));
   connect(mUI.mSpinRotZ, SIGNAL(valueChanged(double)),
      this, SLOT(OnRotationChanged()));
   
   connect(mUI.mSpinOffsetX, SIGNAL(valueChanged(double)),
      this, SLOT(OnOffsetChanged()));
   connect(mUI.mSpinOffsetY, SIGNAL(valueChanged(double)),
      this, SLOT(OnOffsetChanged()));
   connect(mUI.mSpinOffsetZ, SIGNAL(valueChanged(double)),
      this, SLOT(OnOffsetChanged()));
}

void AttachmentPanel::UpdateUI()
{
   //
}

void AttachmentPanel::UpdateSelectedBone()
{
   mInfo.mBone = NULL;

   if (mCharacter.valid())
   {
      std::string boneName(mUI.mComboBones->currentText().toStdString());

      dtCore::RefPtr<dtAnim::BoneInterface> bone
         = mCharacter->GetModelWrapper()->GetBone(boneName);

      if (mInfo.mBone != bone)
      {
         mInfo.mBone = bone;

         emit SignalAttachmentChanged(mInfo);
      }
   }
}

void AttachmentPanel::OnAttachmentLoaded()
{
   UpdateSelectedBone();
}

void AttachmentPanel::OnBoneSelectedFromList(int index)
{
   UpdateSelectedBone();

   emit SignalBoneSelected(mInfo.mBone);
   emit SignalAttachmentChanged(mInfo);
}

void AttachmentPanel::OnCharacterUpdated(CharacterPtr character)
{
   mUI.mComboBones->clear();

   mCharacter = character.get();

   if (character.valid())
   {
      dtAnim::BaseModelWrapper* model = character->GetModelWrapper();

      dtAnim::BoneArray bones;
      model->GetBones(bones);

      dtAnim::BoneArray::iterator curIter = bones.begin();
      dtAnim::BoneArray::iterator endIter = bones.end();
      for(; curIter != endIter; ++curIter)
      {
         mUI.mComboBones->addItem((*curIter)->GetName().c_str());
      }

      // Now that all bones have been listed, acquire the
      // reference to the bone name in the combo box.
      UpdateSelectedBone();
   }
}

void AttachmentPanel::OnLoadAttachmentClicked()
{
   QString filename = QFileDialog::getOpenFileName(this,
      tr("Load Attachment Mesh"), ".", tr("Meshes (*.osg *.ive *.dae *.3ds *.obj)") );

   if (!filename.isEmpty())
   {
      if (dtUtil::FileUtils::GetInstance().FileExists(filename.toStdString()))
      {
         emit SignalLoadAttachment(filename);
      }
      else
      {
         QString errorString = QString("File not found: %1").arg(filename);
         QMessageBox::warning(this, "Warning", errorString, "&Ok");
      }
   }
}

void AttachmentPanel::OnNodeSelected(OsgNodePtr node)
{
   osgAnimation::Bone* bone
      = dynamic_cast<osgAnimation::Bone*>(node.get());

   if (bone != NULL && mCharacter.valid())
   {
      dtCore::RefPtr<dtAnim::BoneInterface> boneObj
         = mCharacter->GetModelWrapper()->GetBone(bone->getName());

      if (boneObj.valid())
      {
         OnBoneSelected(boneObj);
      }
   }
}

void AttachmentPanel::OnBoneSelected(BonePtr bone)
{
   // Select the bone in the list if it exists.
   QString itemText(bone->GetName().c_str());
   int index = mUI.mComboBones->findText(itemText);

   if (index != mUI.mComboBones->currentIndex())
   {
      mUI.mComboBones->setCurrentIndex(index);
   }
}

void AttachmentPanel::OnOffsetChanged()
{
   bool changed =  mInfo.mOffset.x() != mUI.mSpinOffsetX->value()
      || mInfo.mOffset.y() != mUI.mSpinOffsetY->value()
      || mInfo.mOffset.z() != mUI.mSpinOffsetZ->value();

   if (changed)
   {
      mInfo.mOffset.x() = mUI.mSpinOffsetX->value();
      mInfo.mOffset.y() = mUI.mSpinOffsetY->value();
      mInfo.mOffset.z() = mUI.mSpinOffsetZ->value();

      mUI.mSliderOffsetX->setValue((int)mInfo.mOffset.x() * mSliderScale);
      mUI.mSliderOffsetY->setValue((int)mInfo.mOffset.y() * mSliderScale);
      mUI.mSliderOffsetZ->setValue((int)mInfo.mOffset.z() * mSliderScale);

      emit SignalOffsetChanged(mInfo.mOffset);
      emit SignalAttachmentChanged(mInfo);
   }
}

void AttachmentPanel::OnRotationChanged()
{
   bool changed = mInfo.mRotation.x() != mUI.mSpinRotX->value()
      || mInfo.mRotation.y() != mUI.mSpinRotY->value()
      || mInfo.mRotation.z() != mUI.mSpinRotZ->value();

   if (changed)
   {
      mInfo.mRotation.x() = mUI.mSpinRotX->value();
      mInfo.mRotation.y() = mUI.mSpinRotY->value();
      mInfo.mRotation.z() = mUI.mSpinRotZ->value();

      emit SignalRotationChanged(mInfo.mRotation);
      emit SignalAttachmentChanged(mInfo);
   }
}

void AttachmentPanel::OnOffsetSliderChanged()
{
   mUI.mSpinOffsetX->setValue((double)mUI.mSliderOffsetX->value() / mSliderScale);
   mUI.mSpinOffsetY->setValue((double)mUI.mSliderOffsetY->value() / mSliderScale);
   mUI.mSpinOffsetZ->setValue((double)mUI.mSliderOffsetZ->value() / mSliderScale);
}

void AttachmentPanel::OnRotationSliderChanged()
{
   mUI.mSpinRotX->setValue((double)mUI.mSliderRotX->value());
   mUI.mSpinRotY->setValue((double)mUI.mSliderRotY->value());
   mUI.mSpinRotZ->setValue((double)mUI.mSliderRotZ->value());
}

