
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
#include <dtAnim/cal3dmodeldata.h>
#endif
// Delta3D
#include <dtAnim/animatable.h>
#include <dtUtil/log.h>
#include <dtCore/project.h>



/////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
/////////////////////////////////////////////////////////////////////////////////
static const int COLUMN_NAME = 0;
static const int COLUMN_FILE = 1;



/////////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
CalCoreModel* GetCoreModel(dtAnim::BaseModelData& modelData)
{
   CalCoreModel* coreModel = NULL;

   dtAnim::Cal3DModelData* calData = dynamic_cast<dtAnim::Cal3DModelData*>(&modelData);
   if (calData != NULL)
   {
      coreModel = calData->GetCoreModel();
   }

   return coreModel;
}
#endif



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
   , mFileType(dtAnim::NO_FILE)
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
   mFileType = dtAnim::NO_FILE;

   mTextures.clear();
}

/////////////////////////////////////////////////////////////////////////////////
void FileItemDelegate::SetCharModelData(dtAnim::BaseModelData* data)
{
   if(mCharData != data)
   {
      Reset();
      mCharData = data;
   }
}

/////////////////////////////////////////////////////////////////////////////////
dtAnim::BaseModelData* FileItemDelegate::GetCharModelData()
{
   return mCharData.get();
}

/////////////////////////////////////////////////////////////////////////////////
const dtAnim::BaseModelData* FileItemDelegate::GetCharModelData() const
{
   return mCharData.get();
}

////////////////////////////////////////////////////////////////////////////////
bool FileItemDelegate::IsDataValid(const QString& data) const
{
   return mCharData.valid() && mPrevValue != data && !data.isEmpty();
}

////////////////////////////////////////////////////////////////////////////////
void FileItemDelegate::SetCharModelWrapper(dtAnim::BaseModelWrapper* wrapper)
{
   mCharWrapper = wrapper;
}

////////////////////////////////////////////////////////////////////////////////
dtAnim::BaseModelWrapper* FileItemDelegate::GetCharModelWrapper()
{
   return mCharWrapper.get();
}

////////////////////////////////////////////////////////////////////////////////
const dtAnim::BaseModelWrapper* FileItemDelegate::GetCharModelWrapper() const
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
   QString filter = ModelResourceFilters::FILTER_OSG + " "
      + ModelResourceFilters::GetFilterForFileType(mFileType);

   if (mFileType != dtAnim::NO_FILE) // This case should not be possible.
   {
      // Notify the application that a resource is about to change.
      emit SignalResourceEditStart(mFileType, mObjectName);

      QString dir;
      // Get the location of the model file.
      try
      {
         std::string resourcePath = dtCore::Project::GetInstance().GetResourcePath(mCharData->GetResource());
         std::string modelContext(osgDB::getFilePath(resourcePath));
         dir = modelContext.empty() ? "." : modelContext.c_str();
      }
      catch (dtUtil::Exception&)
      {
         dir = ".";
      }

      mNewValue = QFileDialog::getOpenFileName(NULL, "Open File", dir, filter);

      // Update data now if it is valid.
      if (IsDataValid(mNewValue) && ApplyData(mNewValue))
      {
         // Update the button text with the file name registered on the model data.
         QString newText(mCharData->GetFileForObjectName(
            dtAnim::ModelResourceType(mFileType), mObjectName).c_str());
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

   dtAnim::ModelResourceType fileType = mCharData->GetFileType(file);
   switch (fileType)
   {
   case dtAnim::SKEL_FILE:
      allowed = AskUser("Remove Skeleton?", "The character will not be visible without a skeleton. Are you sure you want to remove it?");
      break;

   case dtAnim::MESH_FILE:
      if (mCharData->GetFileCount(fileType) == 1)
      {
         allowed = AskUser("Remove Mesh?", "The character needs at least one mesh to render. Are you sure you want to remove it?");
      }
      break;

   case dtAnim::MAT_FILE:
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
   bool answer = false;

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
      answer = true;
   }

   return answer;
}

/////////////////////////////////////////////////////////////////////////////////
bool FileItemDelegate::ReplaceFile(dtAnim::BaseModelData& modelData, dtAnim::BaseModelWrapper& modelWrapper,
                 const std::string& objectName, const std::string& oldFile, const std::string& newFile) const
{
   using namespace dtAnim;

   bool success = false;

   dtAnim::ModelResourceType fileType = modelData.GetFileType(oldFile);
   
   bool wasCycle = false;
   bool wasAction = false;
   if (fileType == dtAnim::ANIM_FILE)
   {
      // Stop any instance of the animation on the model instance (wrapper).
      dtAnim::AnimationInterface* anim = modelWrapper.GetAnimation(objectName);
      if (anim != NULL)
      {
         wasCycle = anim->ClearCycle(0.0f);
         if (!wasCycle)
         {
            wasAction = anim->ClearAction();
         }

         // Force any internal callbacks to complete.
         modelWrapper.GetAnimator()->Update(0.001f);
      }
   }

   //int result =
   modelData.UnloadResource(fileType, objectName);

   if (!newFile.empty())
   {
      success = 0 <= modelData.LoadResource(fileType, newFile, objectName);
      modelWrapper.HandleModelResourceUpdate(fileType);

      if (success && fileType == dtAnim::ANIM_FILE)
      {
         dtAnim::AnimationInterface* anim = modelWrapper.GetAnimation(objectName);
         if (anim != NULL)
         {
            if (wasCycle)
            {
               anim->PlayCycle(1.0f, 0.0f);
            }
            else if (wasAction)
            {
               anim->PlayAction(0.0f, 0.0f);
            }
         }
      }

   }

   return success;
}

/////////////////////////////////////////////////////////////////////////////////
// Implementation borrowed from dtAnim::Cal3DLoader::LoadAllTextures
void FileItemDelegate::LoadMaterial(dtAnim::BaseModelWrapper& model, const std::string& materialName) const
{
   dtAnim::BaseModelData* modelData = model.GetModelData();

   // get the core material
   dtAnim::MaterialInterface* material = model.GetMaterial(materialName);

   // loop through all maps of the core material
   int numMaps = material->GetTextureCount();
   for (int mapId = 0; mapId < numMaps; ++mapId)
   {
      // get the filename of the texture
      std::string strFilename(material->GetTextureFile(mapId));

      TextureMap::iterator textureIterator = mTextures.find(strFilename);

      if (textureIterator == mTextures.end())
      {
         std::string path;
         try
         {
            std::string resourcePath = dtCore::Project::GetInstance().GetResourcePath(modelData->GetResource());
            path = osgDB::getFilePath(resourcePath);
         }
         catch (dtUtil::Exception&)
         {
            path = ".";
         }
         // TODO replace this with a resource loader.
         // load the texture from the file
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
         material->SetTexture(mapId, texture);
      }
      else
      {
         material->SetTexture(mapId, (*textureIterator).second.get());
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
//   coreModel.createCoreMaterialThread(materialId);

   // initialize the material thread
//   coreModel.setCoreMaterialId(materialId, 0, materialId);
}



////////////////////////////////////////////////////////////////////////////////
// ITEM DELEGATE CODE
////////////////////////////////////////////////////////////////////////////////
ObjectNameItemDelegate::ObjectNameItemDelegate(QObject* parent)
   : BaseClass(parent)
   , mFileType(dtAnim::NO_FILE)
{
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::Reset()
{
   mCharWrapper = NULL;
   mPrevValue = tr("");
   mFileType = dtAnim::NO_FILE;
}

/////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::SetCharModelData(dtAnim::BaseModelData* data)
{
   if(mCharData != data)
   {
      Reset();
      mCharData = data;
   }
}

/////////////////////////////////////////////////////////////////////////////////
dtAnim::BaseModelData* ObjectNameItemDelegate::GetCharModelData()
{
   return mCharData.get();
}

/////////////////////////////////////////////////////////////////////////////////
const dtAnim::BaseModelData* ObjectNameItemDelegate::GetCharModelData() const
{
   return mCharData.get();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::SetCharModelWrapper(dtAnim::BaseModelWrapper* wrapper)
{
   mCharWrapper = wrapper;
}

////////////////////////////////////////////////////////////////////////////////
dtAnim::BaseModelWrapper* ObjectNameItemDelegate::GetCharModelWrapper()
{
   return mCharWrapper.get();
}

////////////////////////////////////////////////////////////////////////////////
const dtAnim::BaseModelWrapper* ObjectNameItemDelegate::GetCharModelWrapper() const
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
      dtAnim::ModelResourceType fileType = dtAnim::ModelResourceType(mFileType);
      if (mCharData->ReplaceObjectName(fileType, oldName, newName))
      {
         switch(fileType)
         {
         case dtAnim::SKEL_FILE:
            RenameSkeleton(oldName, newName);
            break;
         case dtAnim::ANIM_FILE:
            RenameAnimations(oldName, newName);
            break;
         case dtAnim::MESH_FILE:
            RenameMeshes(oldName, newName);
            break;
         case dtAnim::MAT_FILE:
            RenameMaterials(oldName, newName);
            break;
         case dtAnim::MORPH_FILE:
            RenameMorphs(oldName, newName);
            break;
         default:
            break;
         }
         success = fileType != dtAnim::NO_FILE;

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
   CalCoreModel* model = GetCoreModel(*mCharData);
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
   typedef dtAnim::AnimatableArray AnimatableArray;

   // Change the names of the registered Animation Wrappers,
   // the objects that represent the animation files.
   dtAnim::Animatable* animatable = NULL;
   AnimatableArray& animWrappers = mCharData->GetAnimatables();
   AnimatableArray::iterator curIter = animWrappers.begin();
   AnimatableArray::iterator endIter = animWrappers.end();
   for (; curIter != endIter; ++curIter)
   {
      animatable = curIter->get();
      if (animatable->GetName() == oldName)
      {
         animatable->SetName(newName);
      }
   }

   // Rebuild the mapping on animation object names.
   mCharWrapper->HandleModelResourceUpdate(dtAnim::ANIM_FILE);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::RenameMeshes(const std::string& oldName, const std::string& newName) const
{
   dtAnim::MeshArray meshes;
   mCharWrapper->GetMeshes(meshes);

   dtAnim::MeshInterface* mesh = NULL;
   int numMeshes = int(meshes.size());
   for (int i = 0; i < numMeshes; ++i)
   {
      mesh = meshes[i].get();
      if (mesh->GetName() == oldName)
      {
         mesh->SetName(newName);
      }
   }

   // Rebuild the mapping on animation object names.
   mCharWrapper->HandleModelResourceUpdate(dtAnim::MESH_FILE);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::RenameMaterials(const std::string& oldName, const std::string& newName) const
{
   dtAnim::MaterialArray materials;
   mCharWrapper->GetMaterials(materials);

   dtAnim::MaterialInterface* material = NULL;
   int numMaterials = int(materials.size());
   for (int i = 0; i < numMaterials; ++i)
   {
      material = materials[i].get();
      if (material->GetName() == oldName)
      {
         material->SetName(newName);
      }
   }

   mCharWrapper->HandleModelResourceUpdate(dtAnim::MAT_FILE);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectNameItemDelegate::RenameMorphs(const std::string& oldName, const std::string& newName) const
{
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
   CalCoreModel* model = GetCoreModel(*mCharData);
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
