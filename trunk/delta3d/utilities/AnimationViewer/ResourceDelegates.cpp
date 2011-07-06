
/////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
/////////////////////////////////////////////////////////////////////////////////
#include "ResourceDelegates.h"
#include "ResourceDialogs.h"
// Qt
#include <QtGui/QFileDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
// OSG
#include <osg/Texture2D>
#include <osgDB/FileNameUtils>
#include <osgDB/ReadFile>
// Cal3D
#include <cal3d/corematerial.h>
#include <cal3d/coremesh.h>
#include <cal3d/coremodel.h>
#include <cal3d/coreskeleton.h>
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
#include <cal3d/coreanimatedmorph.h>
#endif
// Delta3D
#include <dtAnim/animationwrapper.h>
#include <dtUtil/log.h>



/////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
/////////////////////////////////////////////////////////////////////////////////
static const int COLUMN_NAME = 0;
static const int COLUMN_FILE = 1;



/////////////////////////////////////////////////////////////////////////////////
// FILE BUTTONS EDITOR CODE
/////////////////////////////////////////////////////////////////////////////////
FileButtonsEditor::FileButtonsEditor(QWidget* parent)
   : BaseClass(parent)
{
   QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
   setSizePolicy(sizePolicy);

   QHBoxLayout* layout = new QHBoxLayout(this);
   layout->setSizeConstraint(QLayout::SetMinimumSize);
   layout->setSpacing(0);
   layout->setContentsMargins(0, 0, 0, 0);

   mButtonFile = new QPushButton(this);

   QSize qsize(16, 16);
   mButtonRemove = new QPushButton(this);
   mButtonRemove->setMinimumSize(qsize);

   layout->addWidget(mButtonFile);
   layout->addWidget(mButtonRemove);
   setLayout(layout);

   mButtonFile->setSizePolicy(sizePolicy);

   QIcon icon(":/images/remove.png");
   mButtonRemove->setIcon(icon);

   connect(mButtonFile, SIGNAL(clicked(bool)), this, SLOT(OnClickedFile()));
   connect(mButtonRemove, SIGNAL(clicked(bool)), this, SLOT(OnClickedRemove()));
}

////////////////////////////////////////////////////////////////////////////////
void FileButtonsEditor::OnClickedFile()
{
   emit SignalClickedFile();
}

////////////////////////////////////////////////////////////////////////////////
void FileButtonsEditor::OnClickedRemove()
{
   emit SignalClickedRemove();
}



////////////////////////////////////////////////////////////////////////////////
// ITEM DELEGATE CODE
////////////////////////////////////////////////////////////////////////////////
FileItemDelegate::FileItemDelegate(QObject* parent)
   : BaseClass(parent)
   , mFileType(dtAnim::Cal3DModelData::NO_FILE)
   , mButton(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
void FileItemDelegate::Reset()
{
   mCharWrapper = NULL;
   mButton = NULL;
   mPrevValue = tr("");
   mNewValue = tr("");
   mObjectName = "";
   mFileType = dtAnim::Cal3DModelData::NO_FILE;

   mTextures.clear();
}

/////////////////////////////////////////////////////////////////////////////////
void FileItemDelegate::SetCharModelData(dtAnim::Cal3DModelData* data)
{
   if(mCharData != data)
   {
      Reset();
      mCharData = data;
   }
}

/////////////////////////////////////////////////////////////////////////////////
dtAnim::Cal3DModelData* FileItemDelegate::GetCharModelData()
{
   return mCharData.get();
}

/////////////////////////////////////////////////////////////////////////////////
const dtAnim::Cal3DModelData* FileItemDelegate::GetCharModelData() const
{
   return mCharData.get();
}

////////////////////////////////////////////////////////////////////////////////
bool FileItemDelegate::IsDataValid(const QString& data) const
{
   return mCharData.valid() && mPrevValue != data && !data.isEmpty();
}

////////////////////////////////////////////////////////////////////////////////
void FileItemDelegate::SetCharModelWrapper(dtAnim::Cal3DModelWrapper* wrapper)
{
   mCharWrapper = wrapper;
}

////////////////////////////////////////////////////////////////////////////////
dtAnim::Cal3DModelWrapper* FileItemDelegate::GetCharModelWrapper()
{
   return mCharWrapper.get();
}

////////////////////////////////////////////////////////////////////////////////
const dtAnim::Cal3DModelWrapper* FileItemDelegate::GetCharModelWrapper() const
{
   return mCharWrapper.get();
}

////////////////////////////////////////////////////////////////////////////////
void FileItemDelegate::OnOpenFile() const
{
   using namespace dtAnim;

   std::string file(mPrevValue.toStdString());
   mFileType = mCharData->GetFileType(file);

   // Determine the filter by file type.
   QString filter = Cal3DResourceFilters::GetFilterForFileType(mFileType);

   if (mFileType != Cal3DModelData::NO_FILE) // This case should not be possible.
   {
      // Notify the application that a resource is about to change.
      emit SignalResourceEditStart(mFileType, mObjectName);

      // Get the location of the model file.
      std::string modelContext(osgDB::getFilePath(mCharData->GetFilename()));
      QString dir(modelContext.empty() ? "." : modelContext.c_str());

      mNewValue = QFileDialog::getOpenFileName(NULL, "Open Cal3D File", dir, filter);

      // Update data now if it is valid.
      if (IsDataValid(mNewValue) && ApplyData(mNewValue))
      {
         // Update the button text with the file name registered on the model data.
         QString newText(mCharData->GetFileForObjectName(
            dtAnim::Cal3DModelData::CalFileType(mFileType), mObjectName).c_str());
         mButton->setText(newText);
      }

      // Notify the application that the resource change is complete.
      emit SignalResourceEditEnd(mFileType, mObjectName);
   }
}

////////////////////////////////////////////////////////////////////////////////
void FileItemDelegate::OnRemoveFile() const
{
   using namespace dtAnim;

   std::string file(mPrevValue.toStdString());
   if (mCharData.valid() && IsFileRemovalAllowed(file))
   {
      std::string oldFile(mPrevValue.toStdString());
      mFileType = mCharData->GetFileType(oldFile);

      // Notify the application that a resource is about to change.
      emit SignalResourceEditStart(mFileType, mObjectName);

      // Replace the old file with nothing.
      ReplaceFile(*mCharData, *mCharWrapper, mObjectName, oldFile, "");

      emit SignalResourceRemoved(mFileType, mObjectName);

      // Notify the application that the resource change is complete.
      emit SignalResourceEditEnd(mFileType, mObjectName);
   }
}

////////////////////////////////////////////////////////////////////////////////
bool FileItemDelegate::IsFileRemovalAllowed(const std::string& file) const
{
   using namespace dtAnim;
   bool allowed = true;

   Cal3DModelData::CalFileType fileType = mCharData->GetFileType(file);
   switch (fileType)
   {
   case Cal3DModelData::SKEL_FILE:
      allowed = AskUser("Remove Skeleton?", "The character will not be visible without a skeleton. Are you sure you want to remove it?");
      break;

   case Cal3DModelData::MESH_FILE:
      if (mCharData->GetFileCount(fileType) == 1)
      {
         allowed = AskUser("Remove Mesh?", "The character needs at least one mesh to render. Are you sure you want to remove it?");
      }
      break;

   case Cal3DModelData::MAT_FILE:
      if (mCharData->GetFileCount(fileType) == 1)
      {
         allowed = AskUser("Remove Material?", "The character needs at least one material to render. Are you sure you want to remove it?");
      }
      break;

   default:
      break;
   }

   return allowed;
}

////////////////////////////////////////////////////////////////////////////////
QWidget* FileItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
   FileButtonsEditor* editor = new FileButtonsEditor(parent);
   connect(editor, SIGNAL(SignalClickedFile()), this, SLOT(OnOpenFile()));
   connect(editor, SIGNAL(SignalClickedRemove()), this, SLOT(OnRemoveFile()));
   return editor;
}

////////////////////////////////////////////////////////////////////////////////
void FileItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
   QString value = index.model()->data(index, Qt::EditRole).toString();

   mButton = static_cast<FileButtonsEditor*>(editor)->mButtonFile;
   mButton->setText(value);

   mPrevValue = value;

   // Determine the object name being referenced.
   QModelIndex siblingIndex(index.sibling(index.row(), COLUMN_NAME));
   if (siblingIndex.isValid())
   {
      mObjectName = index.model()->data(siblingIndex, Qt::DisplayRole).toString().toStdString();
   }
}

////////////////////////////////////////////////////////////////////////////////
void FileItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
   const QModelIndex& index) const
{
   QPushButton* button = static_cast<FileButtonsEditor*>(editor)->mButtonFile;
   model->setData(index, button->text(), Qt::EditRole);
}

////////////////////////////////////////////////////////////////////////////////
void FileItemDelegate::updateEditorGeometry(QWidget* editor,
   const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   FileButtonsEditor* fbe = static_cast<FileButtonsEditor*>(editor);
   fbe->setGeometry(option.rect);

   int height = option.rect.height();
   fbe->mButtonRemove->setMaximumWidth(height);
   fbe->mButtonRemove->setMaximumHeight(height);
   fbe->mButtonFile->setMaximumWidth(option.rect.width() - height);
   fbe->mButtonFile->setMaximumHeight(height);
}

////////////////////////////////////////////////////////////////////////////////
bool FileItemDelegate::ApplyData(const QString& data) const
{
   bool success = false;

   if (mCharData.valid())
   {
      std::string oldFile(mPrevValue.toStdString());
      std::string newFile(data.toStdString());
      success = ReplaceFile(*mCharData, *mCharWrapper, mObjectName, oldFile, newFile);

      if (success)
      {
         // TODO: Signal to the rest of the application that a resource was changed.
      }
   }

   return success;
}

/////////////////////////////////////////////////////////////////////////////////
bool FileItemDelegate::AskUser(const std::string& promptTitle, const std::string& question) const
{
   bool ansewer = false;

   QString qTitle(promptTitle.c_str());
   QString qMessage(question.c_str());
   QMessageBox msgBox;
   msgBox.setText(qTitle);
   msgBox.setInformativeText(qMessage);
   msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
   msgBox.setDefaultButton(QMessageBox::Ok);
   int retCode = msgBox.exec();

   // If OK...
   if (retCode == QMessageBox::Ok)
   {
      ansewer = true;
   }

   return ansewer;
}

/////////////////////////////////////////////////////////////////////////////////
bool FileItemDelegate::ReplaceFile(dtAnim::Cal3DModelData& modelData, dtAnim::Cal3DModelWrapper& modelWrapper,
                 const std::string& objectName, const std::string& oldFile, const std::string& newFile) const
{
   using namespace dtAnim;

   bool success = false;

   CalCoreModel* model = modelData.GetCoreModel();
   Cal3DModelData::CalFileType calFileType = modelData.GetFileType(oldFile);

   modelData.UnregisterObjectName(objectName, oldFile);

   switch(calFileType)
   {
   case Cal3DModelData::SKEL_FILE:
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
      success = !newFile.empty() && modelData.LoadCoreSkeleton(newFile, objectName);
#else
      success = !newFile.empty() && modelData.LoadCoreSkeleton(newFile);
#endif
      break;

   case Cal3DModelData::ANIM_FILE:
      {
         // Stop any instance of the animation on the model instance (wrapper).
         bool wasCycle = false;
         bool wasAction = false;
         int animId = model->getCoreAnimationId(objectName);
         if (modelWrapper.HasAnimation(animId))
         {
            wasCycle = modelWrapper.ClearCycle(animId, 0.0f);
            if (!wasCycle)
            {
               wasAction = modelWrapper.RemoveAction(animId);
            }

            // Force any internal callbacks to complete.
            modelWrapper.UpdateAnimation(0.001f);
         }

         // Remove the animation resource from the central model data.
         if (-1 < model->unloadCoreAnimation(animId))
         {
            if (!newFile.empty())
            {
               animId = modelData.LoadCoreAnimation(newFile, objectName);
               success = -1 < animId;
            }
         }

         if (success)
         {
            AnimationWrapper* wrapper = modelData.GetAnimationWrapperByName(objectName);
            if (wrapper != NULL)
            {
               wrapper->SetID(animId);
            }

            if (wasCycle)
            {
               modelWrapper.BlendCycle(animId, 1.0f, 0.0f);
            }
            else if (wasAction)
            {
               modelWrapper.ExecuteAction(animId, 0.0f, 0.0f);
            }
         }
      }
      break;

   case Cal3DModelData::MESH_FILE:
      if (-1 < model->unloadCoreMesh(model->getCoreMeshId(objectName)))
      {
         success = !newFile.empty() && -1 < modelData.LoadCoreMesh(newFile, objectName);
      }
      break;

   case Cal3DModelData::MAT_FILE:
      if (-1 < model->unloadCoreMaterial(model->getCoreMaterialId(objectName)))
      {
         int id = newFile.empty() ? -1 : modelData.LoadCoreMaterial(newFile, objectName);
         success = -1 < id;
         if (success)
         {
            //LoadMaterial(modelData, id);
         }
      }
      break;

   case Cal3DModelData::MORPH_FILE:
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
      /*if (-1 < model->unloadCoreAnimatedMorph(model->getCoreAnimatedMorphId(objectName)))
      {
         success = !newFile.empty() && -1 < modelData.LoadCoreMorph(newFile, objectName);
      }*/
#endif
      break;

   default:
      break;
   }

   return success;
}

/////////////////////////////////////////////////////////////////////////////////
// Implementation borrowed from dtAnim::Cal3DLoader::LoadAllTextures
void FileItemDelegate::LoadMaterial(dtAnim::Cal3DModelData& modelData, int materialId) const
{
   CalCoreModel& coreModel = *modelData.GetCoreModel();

   // get the core material
   CalCoreMaterial* pCoreMaterial = coreModel.getCoreMaterial(materialId);

   // loop through all maps of the core material
   for (int mapId = 0; mapId < pCoreMaterial->getMapCount(); ++mapId)
   {
      // get the filename of the texture
      std::string strFilename(pCoreMaterial->getMapFilename(mapId));

      TextureMap::iterator textureIterator = mTextures.find(strFilename);

      if (textureIterator == mTextures.end())
      {
         // load the texture from the file
         std::string path(osgDB::getFilePath(modelData.GetFilename()));
         dtCore::RefPtr<osg::Image> img = osgDB::readImageFile(path + "/" + strFilename);

         if (!img.valid())
         {
            LOG_ERROR("Unable to load image file: " + strFilename);
            return;
         }

         dtCore::RefPtr<osg::Texture2D> texture = new osg::Texture2D();
         texture->setImage(img.get());
         texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
         texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
         mTextures[strFilename] = texture.get();

         // store the opengl texture id in the user data of the map
         pCoreMaterial->setMapUserData(mapId, (Cal::UserData)texture);
      }
      else
      {
         pCoreMaterial->setMapUserData(mapId, (Cal::UserData)((*textureIterator).second.get()));
      }
   }

   // make one material thread for each material
   // NOTE: this is not the right way to do it, but this viewer can't do the right
   // mapping without further information on the model etc., so this is the only
   // thing we can do here.

   // Every part of the core model (every submesh to be more exact) has a material
   // thread assigned. You can now very easily change the look of a model instance,
   // by simply select a new current material set for its parts. The Cal3D library
   // is now able to look up the material in the material grid with the given new
   // material set and the material thread stored in the core model parts.

   // create the a material thread
   coreModel.createCoreMaterialThread(materialId);

   // initialize the material thread
   coreModel.setCoreMaterialId(materialId, 0, materialId);
}



////////////////////////////////////////////////////////////////////////////////
// ITEM DELEGATE CODE
////////////////////////////////////////////////////////////////////////////////
ObjectNameItemDelegate::ObjectNameItemDelegate(QObject* parent)
   : BaseClass(parent)
   , mFileType(dtAnim::Cal3DModelData::NO_FILE)
{
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::Reset()
{
   mCharWrapper = NULL;
   mPrevValue = tr("");
   mFileType = dtAnim::Cal3DModelData::NO_FILE;
}

/////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::SetCharModelData(dtAnim::Cal3DModelData* data)
{
   if(mCharData != data)
   {
      Reset();
      mCharData = data;
   }
}

/////////////////////////////////////////////////////////////////////////////////
dtAnim::Cal3DModelData* ObjectNameItemDelegate::GetCharModelData()
{
   return mCharData.get();
}

/////////////////////////////////////////////////////////////////////////////////
const dtAnim::Cal3DModelData* ObjectNameItemDelegate::GetCharModelData() const
{
   return mCharData.get();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::SetCharModelWrapper(dtAnim::Cal3DModelWrapper* wrapper)
{
   mCharWrapper = wrapper;
}

////////////////////////////////////////////////////////////////////////////////
dtAnim::Cal3DModelWrapper* ObjectNameItemDelegate::GetCharModelWrapper()
{
   return mCharWrapper.get();
}

////////////////////////////////////////////////////////////////////////////////
const dtAnim::Cal3DModelWrapper* ObjectNameItemDelegate::GetCharModelWrapper() const
{
   return mCharWrapper.get();
}

////////////////////////////////////////////////////////////////////////////////
bool ObjectNameItemDelegate::IsDataValid(const QString& data) const
{
   return mPrevValue != data && !data.isEmpty();
}

////////////////////////////////////////////////////////////////////////////////
QWidget* ObjectNameItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
   const QModelIndex &index) const
{
   return new QLineEdit(parent);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
   QString value = index.model()->data(index, Qt::EditRole).toString();

   QLineEdit* textEdit = static_cast<QLineEdit*>(editor);
   textEdit->setText(value);

   mPrevValue = value;

   // Determine the type of object being referenced.
   QModelIndex siblingIndex(index.sibling(index.row(), COLUMN_FILE));
   if (siblingIndex.isValid())
   {
      std::string file = index.model()->data(siblingIndex, Qt::DisplayRole).toString().toStdString();
      mFileType = mCharData->GetFileType(file);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
   const QModelIndex &index) const
{
   QLineEdit* textEdit = static_cast<QLineEdit*>(editor);

   QString value = textEdit->text();

   if (IsDataValid(value) && ApplyData(value))
   {
      model->setData(index, value, Qt::EditRole);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::updateEditorGeometry(QWidget* editor,
   const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   editor->setGeometry(option.rect);
}

////////////////////////////////////////////////////////////////////////////////
bool ObjectNameItemDelegate::ApplyData(const QString& data) const
{
   bool success = false;

   using namespace dtAnim;

   if (mCharData.valid())
   {
      // Convert the new and old values.
      std::string oldName(mPrevValue.trimmed().toStdString());
      std::string newName(data.trimmed().toStdString());

      // Update the object name.
      Cal3DModelData::CalFileType calFileType = Cal3DModelData::CalFileType(mFileType);
      if (mCharData->ReplaceObjectName(calFileType, oldName, newName))
      {
         switch(calFileType)
         {
         case Cal3DModelData::SKEL_FILE:
            RenameSkeleton(oldName, newName);
            break;
         case Cal3DModelData::ANIM_FILE:
            RenameAnimations(oldName, newName);
            break;
         case Cal3DModelData::MESH_FILE:
            RenameMeshes(oldName, newName);
            break;
         case Cal3DModelData::MAT_FILE:
            RenameMaterials(oldName, newName);
            break;
         case Cal3DModelData::MORPH_FILE:
            RenameMorphs(oldName, newName);
            break;
         default:
            break;
         }
         success = calFileType != Cal3DModelData::NO_FILE;

         if (success)
         {
            emit SignalResourceNameChanged(mFileType, oldName, newName);
         }
      }
      else
      {
         // TODO: Prompt error.
      }
   }
   else
   {
      // TODO: Prompt error.
   }

   return success;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::RenameSkeleton(const std::string& oldName, const std::string& newName) const
{
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
   CalCoreModel* model = mCharData->GetCoreModel();
   CalCoreSkeleton* skel = model->getCoreSkeleton();
   if (skel != NULL)
   {
      skel->setName(newName);
   }
#endif
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::RenameAnimations(const std::string& oldName, const std::string& newName) const
{
   typedef dtAnim::Cal3DModelData::AnimationWrapperArray AnimWrapperArray;

   // Change the names of the registered Animation Wrappers,
   // the objects that represent the animation files.
   dtAnim::AnimationWrapper* animWrapper = NULL;
   AnimWrapperArray& animWrappers = mCharData->GetAnimationWrappers();
   AnimWrapperArray::iterator curIter = animWrappers.begin();
   AnimWrapperArray::iterator endIter = animWrappers.end();
   for (; curIter != endIter; ++curIter)
   {
      animWrapper = curIter->get();
      if (animWrapper->GetName() == oldName)
      {
         animWrapper->SetName(newName);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::RenameMeshes(const std::string& oldName, const std::string& newName) const
{
   CalCoreModel* model = mCharData->GetCoreModel();
   CalCoreMesh* mesh = NULL;
   int numMeshes = model->getCoreMeshCount();
   for (int i = 0; i < numMeshes; ++i)
   {
      mesh = model->getCoreMesh(i);
      if (mesh->getName() == oldName)
      {
         mesh->setName(newName);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::RenameMaterials(const std::string& oldName, const std::string& newName) const
{
   CalCoreModel* model = mCharData->GetCoreModel();
   CalCoreMaterial* material = NULL;
   int numMaterials = model->getCoreMaterialCount();
   for (int i = 0; i < numMaterials; ++i)
   {
      material = model->getCoreMaterial(i);
      if (material->getName() == oldName)
      {
         material->setName(newName);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::RenameMorphs(const std::string& oldName, const std::string& newName) const
{
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
   CalCoreModel* model = mCharData->GetCoreModel();
   CalCoreAnimatedMorph* morph = NULL;
   int numMorphs = model->getNumCoreAnimatedMorphs();
   for (int i = 0; i < numMorphs; ++i)
   {
      morph = model->getCoreAnimatedMorph(i);
      if (morph->getName() == oldName)
      {
         morph->setName(newName);
      }
   }
#endif
}
