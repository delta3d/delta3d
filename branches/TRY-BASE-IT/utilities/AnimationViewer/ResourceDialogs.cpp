
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ResourceDialogs.h"
// OSG
#include <osgDB/FileNameUtils>
#include <osgDB/ReadFile>
// Qt
#include <QtCore/QString>
#include <QtGui/QFileDialog>
#include <QtGui/QIcon>



/////////////////////////////////////////////////////////////////////////////////
// CAL3D RESOURCE FILTERS CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
const QString Cal3DResourceFilters::FILTER_ANY("Cal3D (*.csf *.xsf *.caf *.xaf *.cmf *.xmf *.crf *.xrf *.cpf *.xpf)");
const QString Cal3DResourceFilters::FILTER_ALL_NOT_SKEL("Cal3D (*.caf *.xaf *.cmf *.xmf *.crf *.xrf *.cpf *.xpf)");
const QString Cal3DResourceFilters::FILTER_SKEL("Skeletons (*.csf *.xsf)");
const QString Cal3DResourceFilters::FILTER_ANIM("Animations (*.caf *.xaf)");
const QString Cal3DResourceFilters::FILTER_MESH("Meshes (*.cmf *.xmf)");
const QString Cal3DResourceFilters::FILTER_MAT("Materials (*.crf *.xrf)");
const QString Cal3DResourceFilters::FILTER_MORPH("Morphs (*.cpf *.xpf)");

/////////////////////////////////////////////////////////////////////////////////
const QString& Cal3DResourceFilters::GetFilterForFileType(int fileType)
{
   using namespace dtAnim;

   const QString* filter = &Cal3DResourceFilters::FILTER_ANY;

   dtAnim::Cal3DModelData::CalFileType calFileType = dtAnim::Cal3DModelData::CalFileType(fileType);
   switch(calFileType)
   {
   case Cal3DModelData::SKEL_FILE:
      filter = &Cal3DResourceFilters::FILTER_SKEL;
      break;
   case Cal3DModelData::ANIM_FILE:
      filter = &Cal3DResourceFilters::FILTER_ANIM;
      break;
   case Cal3DModelData::MESH_FILE:
      filter = &Cal3DResourceFilters::FILTER_MESH;
      break;
   case Cal3DModelData::MAT_FILE:
      filter = &Cal3DResourceFilters::FILTER_MAT;
      break;
   case Cal3DModelData::MORPH_FILE:
      filter = &Cal3DResourceFilters::FILTER_MORPH;
      break;
   default:
      break;
   }

   return *filter;
}



/////////////////////////////////////////////////////////////////////////////////
// CAL3D RESOURCE ICONS CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
const QString Cal3DResourceIcons::ICON_NONE("");
const QString Cal3DResourceIcons::ICON_SKEL(":/images/fileIconSkel.png");
const QString Cal3DResourceIcons::ICON_ANIM(":/images/fileIconAnim.png");
const QString Cal3DResourceIcons::ICON_MESH(":/images/fileIconMesh.png");
const QString Cal3DResourceIcons::ICON_MAT(":/images/fileIconMat.png");
const QString Cal3DResourceIcons::ICON_MORPH(":/images/fileIconMorph.png");

/////////////////////////////////////////////////////////////////////////////////
const QIcon Cal3DResourceIcons::GetIconForFileType(int fileType)
{
   using namespace dtAnim;

   const QString* icon = &Cal3DResourceIcons::ICON_NONE;

   dtAnim::Cal3DModelData::CalFileType calFileType = dtAnim::Cal3DModelData::CalFileType(fileType);
   switch(calFileType)
   {
   case Cal3DModelData::SKEL_FILE:
      icon = &Cal3DResourceIcons::ICON_SKEL;
      break;
   case Cal3DModelData::ANIM_FILE:
      icon = &Cal3DResourceIcons::ICON_ANIM;
      break;
   case Cal3DModelData::MESH_FILE:
      icon = &Cal3DResourceIcons::ICON_MESH;
      break;
   case Cal3DModelData::MAT_FILE:
      icon = &Cal3DResourceIcons::ICON_MAT;
      break;
   case Cal3DModelData::MORPH_FILE:
      icon = &Cal3DResourceIcons::ICON_MORPH;
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
void ResAddDialog::SetModelData(dtAnim::Cal3DModelData* modelData)
{
   mModelData = modelData;
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
   if (mModelData.valid())
   {
      modelContext = osgDB::getFilePath(mModelData->GetFilename());

      // Prevent adding more skeleton files, only one is allowed.
      if (mModelData->GetFileCount(dtAnim::Cal3DModelData::SKEL_FILE) > 0)
      {
         filters << Cal3DResourceFilters::FILTER_ALL_NOT_SKEL;
      }
      else
      {
         filters << Cal3DResourceFilters::FILTER_ANY;
         filters << Cal3DResourceFilters::FILTER_SKEL;
      }
   }

   // Add common filters.
   filters << Cal3DResourceFilters::FILTER_ANIM;
   filters << Cal3DResourceFilters::FILTER_MESH;
   filters << Cal3DResourceFilters::FILTER_MAT;
   filters << Cal3DResourceFilters::FILTER_MORPH;

   QString dir(modelContext.empty() ? "." : modelContext.c_str());
   QString filterStr(filters.join(";;"));
   QString file = QFileDialog::getOpenFileName(NULL, "Add Cal3D File", dir, filterStr);
   mUI.mFile->setText(file);

   UpdateUI();
}

/////////////////////////////////////////////////////////////////////////////////
void ResAddDialog::accept()
{
   BaseClass::accept();

   std::string objName(mUI.mObjectName->text().toStdString());
   std::string file(mUI.mFile->text().toStdString());
   if (mModelData.valid() && mModelData->RegisterFile(file, objName))
   {
      mDataChanged = true;
      emit SignalChangedData();
   }
}

/////////////////////////////////////////////////////////////////////////////////
void ResAddDialog::UpdateUI()
{
   std::string objName(mUI.mObjectName->text().toStdString());
   std::string file(mUI.mFile->text().toStdString());
   std::string currentFile;
   dtAnim::Cal3DModelData::CalFileType fileType = dtAnim::Cal3DModelData::NO_FILE;

   if (mModelData.valid())
   {
      fileType = mModelData->GetFileType(file);
      currentFile = mModelData->GetFileForObjectName(fileType, objName);
   }

   bool isObjectNameUsed = ! currentFile.empty();
   if (isObjectNameUsed)
   {
      // TODO: Set some indicator that the there is a name conflict.
   }

   if (fileType != dtAnim::Cal3DModelData::NO_FILE)
   {
      mUI.mButtonFile->setIcon(Cal3DResourceIcons::GetIconForFileType(fileType));
   }

   QPushButton* buttonOk = mUI.mButtonBox->button(QDialogButtonBox::Ok);
   buttonOk->setEnabled( ! isObjectNameUsed && ! objName.empty() && ! file.empty());
}
