
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
void ResAddDialog::SetModelData(dtAnim::BaseModelData* modelData)
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
      if (mModelData->GetFileCount(dtAnim::SKEL_FILE) > 0)
      {
         filters << ModelResourceFilters::FILTER_ALL_NOT_SKEL;
      }
      else
      {
         filters << ModelResourceFilters::FILTER_ANY;
         filters << ModelResourceFilters::FILTER_SKEL;
      }
   }

   // Add common filters.
   filters << ModelResourceFilters::FILTER_ANIM;
   filters << ModelResourceFilters::FILTER_MESH;
   filters << ModelResourceFilters::FILTER_MAT;
   filters << ModelResourceFilters::FILTER_MORPH;

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
   dtAnim::ModelResourceType fileType = dtAnim::NO_FILE;

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

   if (fileType != dtAnim::NO_FILE)
   {
      mUI.mButtonFile->setIcon(ModelResourceIcons::GetIconForFileType(fileType));
   }

   QPushButton* buttonOk = mUI.mButtonBox->button(QDialogButtonBox::Ok);
   buttonOk->setEnabled( ! isObjectNameUsed && ! objName.empty() && ! file.empty());
}
