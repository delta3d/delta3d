
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ResourceDialogs.h"
// DELTA3D
#include <dtAnim/basemodeldata.h>
#include <dtAnim/constants.h>
// OSG
#include <osgDB/FileNameUtils>
#include <osgDB/ReadFile>
// Qt
#include <QtCore/QString>
#include <QtGui/QFileDialog>
#include <QtGui/QIcon>
#include <dtCore/project.h>


/////////////////////////////////////////////////////////////////////////////////
// CAL3D RESOURCE FILTERS CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
const QString ModelResourceFilters::FILTER_OSG("OSG (*.osg *.ive *.dae *.fbx *.bvh)");
const QString ModelResourceFilters::FILTER_ANY("Cal3D (*.csf *.xsf *.caf *.xaf *.cmf *.xmf *.crf *.xrf *.cpf *.xpf)");
const QString ModelResourceFilters::FILTER_ALL_NOT_SKEL("Cal3D (*.caf *.xaf *.cmf *.xmf *.crf *.xrf *.cpf *.xpf)");
const QString ModelResourceFilters::FILTER_SKEL("Skeletons (*.csf *.xsf)");
const QString ModelResourceFilters::FILTER_ANIM("Animations (*.caf *.xaf)");
const QString ModelResourceFilters::FILTER_MESH("Meshes (*.cmf *.xmf)");
const QString ModelResourceFilters::FILTER_MAT("Materials (*.crf *.xrf)");
const QString ModelResourceFilters::FILTER_MORPH("Morphs (*.cpf *.xpf)");

/////////////////////////////////////////////////////////////////////////////////
const QString& ModelResourceFilters::GetFilterForFileType(int fileType)
{
   using namespace dtAnim;

   const QString* filter = &ModelResourceFilters::FILTER_ANY;

   dtAnim::ModelResourceType modelFileType = dtAnim::ModelResourceType(fileType);
   switch(modelFileType)
   {
   case dtAnim::SKEL_FILE:
      filter = &ModelResourceFilters::FILTER_SKEL;
      break;
   case dtAnim::ANIM_FILE:
      filter = &ModelResourceFilters::FILTER_ANIM;
      break;
   case dtAnim::MESH_FILE:
      filter = &ModelResourceFilters::FILTER_MESH;
      break;
   case dtAnim::MAT_FILE:
      filter = &ModelResourceFilters::FILTER_MAT;
      break;
   case dtAnim::MORPH_FILE:
      filter = &ModelResourceFilters::FILTER_MORPH;
      break;
   default:
      break;
   }

   return *filter;
}



/////////////////////////////////////////////////////////////////////////////////
// CAL3D RESOURCE ICONS CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
const QString ModelResourceIcons::ICON_NONE("");
const QString ModelResourceIcons::ICON_MIXED(":/images/fileIconSkel.png");
const QString ModelResourceIcons::ICON_SKEL(":/images/fileIconSkel.png");
const QString ModelResourceIcons::ICON_ANIM(":/images/fileIconAnim.png");
const QString ModelResourceIcons::ICON_MESH(":/images/fileIconMesh.png");
const QString ModelResourceIcons::ICON_MAT(":/images/fileIconMat.png");
const QString ModelResourceIcons::ICON_MORPH(":/images/fileIconMorph.png");

/////////////////////////////////////////////////////////////////////////////////
const QIcon ModelResourceIcons::GetIconForFileType(int fileType)
{
   using namespace dtAnim;

   const QString* icon = &ModelResourceIcons::ICON_NONE;

   dtAnim::ModelResourceType modelFileType = dtAnim::ModelResourceType(fileType);
   switch(modelFileType)
   {
   case dtAnim::MIXED_FILE:
      icon = &ModelResourceIcons::ICON_MIXED;
      break;
   case dtAnim::SKEL_FILE:
      icon = &ModelResourceIcons::ICON_SKEL;
      break;
   case dtAnim::ANIM_FILE:
      icon = &ModelResourceIcons::ICON_ANIM;
      break;
   case dtAnim::MESH_FILE:
      icon = &ModelResourceIcons::ICON_MESH;
      break;
   case dtAnim::MAT_FILE:
      icon = &ModelResourceIcons::ICON_MAT;
      break;
   case dtAnim::MORPH_FILE:
      icon = &ModelResourceIcons::ICON_MORPH;
      break;
   default:
      break;
   }

   return QIcon(*icon);
}



/////////////////////////////////////////////////////////////////////////////////
// RESOURCE ADD DIALOG CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
ResAddDialog::ResAddDialog(QWidget* parent)
   : BaseClass(parent)
   , mDataChanged(false)
{
   mUI.setupUi(this);

   connect(mUI.mObjectName, SIGNAL(textChanged(const QString&)),
      this, SLOT(OnChangedObjectName(const QString&)));
   connect(mUI.mButtonFile, SIGNAL(clicked(bool)),
      this, SLOT(OnClickedFile()));

   UpdateUI();
}

/////////////////////////////////////////////////////////////////////////////////
void ResAddDialog::SetModelWrapper(dtAnim::BaseModelWrapper* wrapper)
{
   mWrapper = wrapper;
   dtAnim::BaseModelData* modelData = mWrapper.valid() ? mWrapper->GetModelData() : NULL;

   if (modelData != NULL)
   {
      // Setup and enable the resource type selection combo box
      // if the character system of the model is not for CAL3D.
      if (modelData->GetCharacterSystemType() != dtAnim::Constants::CHARACTER_SYSTEM_CAL3D)
      {
         SetupResourceTypeList();

         mUI.mComboResourceType->setEnabled(true);
      }
      else // CAL3D character system.
      {
         // The CAL3D resource type will be determined by file
         // extension, so the resource type combo box is irrelevant
         // and should be disabled.
         mUI.mComboResourceType->setEnabled(false);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////
bool ResAddDialog::IsDataChanged() const
{
   return mDataChanged;
}

/////////////////////////////////////////////////////////////////////////////////
void ResAddDialog::OnChangedObjectName(const QString& text)
{
   QString trimmedText(text.trimmed());

   if (trimmedText != text)
   {
      QLineEdit* objNameEdit = mUI.mObjectName;
      objNameEdit->blockSignals(true); // Avoid recalling this method unnecessarily.
      objNameEdit->setText(trimmedText);
      objNameEdit->blockSignals(false);
   }

   UpdateUI();
}

/////////////////////////////////////////////////////////////////////////////////
void ResAddDialog::OnClickedFile()
{
   // Get the location of the model file.
   std::string modelContext;
   QStringList filters;

   // OSG formats should always be available.
   filters << ModelResourceFilters::FILTER_OSG;

   dtAnim::BaseModelData* modelData = mWrapper->GetModelData();

   if (modelData != NULL)
   {
      try
      {
         std::string resFile = dtCore::Project::GetInstance().GetResourcePath(modelData->GetResource());
         modelContext = osgDB::getFilePath(resFile);
      }
      catch (const dtUtil::Exception&)
      {
         modelContext = ".";
      }

      // Prevent adding more skeleton files, only one is allowed.
      if (modelData->GetFileCount(dtAnim::SKEL_FILE) > 0)
      {
         filters << ModelResourceFilters::FILTER_ALL_NOT_SKEL;
      }
      else
      {
         filters << ModelResourceFilters::FILTER_ANY;
         filters << ModelResourceFilters::FILTER_SKEL;
      }
   }
   else // Nothing is loaded so allow loading of skeleton file.
   {
      filters << ModelResourceFilters::FILTER_ANY;
      filters << ModelResourceFilters::FILTER_SKEL;
   }

   // Add common filters.
   filters << ModelResourceFilters::FILTER_ANIM;
   filters << ModelResourceFilters::FILTER_MESH;
   filters << ModelResourceFilters::FILTER_MAT;
   filters << ModelResourceFilters::FILTER_MORPH;

   QString dir(modelContext.empty() ? "." : modelContext.c_str());
   QString filterStr(filters.join(";;"));
   QString file = QFileDialog::getOpenFileName(NULL, "Add File", dir, filterStr);
   mUI.mFile->setText(file);

   // Help the user save time with a default object name.
   if (mUI.mObjectName->text().isEmpty())
   {
      std::string strFile(file.toStdString());
      strFile = osgDB::getSimpleFileName(strFile);
      file = strFile.c_str();
      mUI.mObjectName->setText(file);
   }

   UpdateUI();
}

/////////////////////////////////////////////////////////////////////////////////
void ResAddDialog::accept()
{
   BaseClass::accept();

   dtAnim::BaseModelData* modelData = mWrapper->GetModelData();

   std::string objName(mUI.mObjectName->text().toStdString());
   std::string file(mUI.mFile->text().toStdString());
   dtAnim::ModelResourceType resType = (dtAnim::ModelResourceType)
      (mUI.mComboResourceType->itemData(mUI.mComboResourceType->currentIndex()).toInt());
   if (modelData != NULL)
   {
      if (0 < modelData->LoadResource(resType, file, objName))
      {
         mDataChanged = true;
         emit SignalChangedData();
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////
void ResAddDialog::UpdateUI()
{

   std::string objName(mUI.mObjectName->text().toStdString());
   std::string file(mUI.mFile->text().toStdString());
   std::string currentFile;
   dtAnim::ModelResourceType fileType = dtAnim::NO_FILE;

   if (mWrapper.valid())
   {
      dtAnim::BaseModelData* modelData = mWrapper->GetModelData();
      
      if (modelData != NULL)
      {
         fileType = modelData->GetFileType(file);
         currentFile = modelData->GetFileForObjectName(fileType, objName);
      }
   }

   bool isObjectNameUsed = ! currentFile.empty();
   if (isObjectNameUsed)
   {
      // TODO: Set some indicator that the there is a name conflict.
   }

   if (fileType != dtAnim::NO_FILE)
   {
      mUI.mButtonFile->setIcon(ModelResourceIcons::GetIconForFileType(fileType));
   }

   QPushButton* buttonOk = mUI.mButtonBox->button(QDialogButtonBox::Ok);
   buttonOk->setEnabled( ! isObjectNameUsed && ! objName.empty() && ! file.empty());
}

/////////////////////////////////////////////////////////////////////////////////
void ResAddDialog::SetupResourceTypeList()
{
   if (mUI.mComboResourceType->count() == 0)
   {
      QIcon iconMixed(ModelResourceIcons::ICON_MIXED);
      QIcon iconMesh(ModelResourceIcons::ICON_MESH);
      QIcon iconMorph(ModelResourceIcons::ICON_MORPH);
      QIcon iconAnim(ModelResourceIcons::ICON_ANIM);
      QIcon iconMat(ModelResourceIcons::ICON_MAT);
      QIcon iconSkel(ModelResourceIcons::ICON_SKEL);

      mUI.mComboResourceType->addItem(iconMixed, "Mixed", dtAnim::MIXED_FILE);
      mUI.mComboResourceType->addItem(iconMesh, "Mesh", dtAnim::MESH_FILE);
      mUI.mComboResourceType->addItem(iconMorph, "Morph", dtAnim::MORPH_FILE);
      mUI.mComboResourceType->addItem(iconAnim, "Animation", dtAnim::ANIM_FILE);
      mUI.mComboResourceType->addItem(iconMat, "Material", dtAnim::MAT_FILE);
      mUI.mComboResourceType->addItem(iconSkel, "Skeleton", dtAnim::SKEL_FILE);
   }
}
