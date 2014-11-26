/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <dtAnim/cal3dloader.h>
#include <osgDB/FileNameUtils>
#include <osgDB/ReadFile>
#include <osgDB/Registry>
#include <osg/Texture2D>
DT_DISABLE_WARNING_ALL_START
#include <cal3d/coreanimation.h>
#include <cal3d/coremodel.h>
#include <cal3d/error.h>
#include <cal3d/model.h>
#include <cal3d/morphtargetmixer.h>
DT_DISABLE_WARNING_END

#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/characterfilewriter.h>
#include <dtAnim/constants.h>

#include <dtCore/basexmlhandler.h>
#include <dtCore/basexmlreaderwriter.h>
#include <dtCore/hotspotattachment.h>
#include <dtCore/project.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/hotspotdefinition.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/threadpool.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/xerceswriter.h>


namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   static const dtUtil::RefString CAL_PLUGIN_DATA("CalPluginData");

   /////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(Cal3dExtensionEnum);
   Cal3dExtensionEnum Cal3dExtensionEnum::ANIMATION_XML("xaf", ANIM_FILE, "Cal3D Animation File (XML)");
   Cal3dExtensionEnum Cal3dExtensionEnum::ANIMATION_BINARY("caf", ANIM_FILE, "Cal3D Animation File (Binary)");
   Cal3dExtensionEnum Cal3dExtensionEnum::MATERIAL_XML("xrf", MAT_FILE, "Cal3D Material File (XML)");
   Cal3dExtensionEnum Cal3dExtensionEnum::MATERIAL_BINARY("crf", MAT_FILE, "Cal3D Material File (Binary)");
   Cal3dExtensionEnum Cal3dExtensionEnum::MESH_XML("xmf", MESH_FILE, "Cal3D Mesh File (XML)");
   Cal3dExtensionEnum Cal3dExtensionEnum::MESH_BINARY("cmf", MESH_FILE, "Cal3D Mesh File (Binary)");
   Cal3dExtensionEnum Cal3dExtensionEnum::MORPH_XML("xpf", MORPH_FILE, "Cal3D Morph File (XML)");
   Cal3dExtensionEnum Cal3dExtensionEnum::MORPH_BINARY("cpf", MORPH_FILE, "Cal3D Morph File (Binary)");
   Cal3dExtensionEnum Cal3dExtensionEnum::SKELETON_XML("xsf", SKEL_FILE, "Cal3D Skeleton File (XML)");
   Cal3dExtensionEnum Cal3dExtensionEnum::SKELETON_BINARY("csf", SKEL_FILE, "Cal3D Skeleton File (Binary)");
   typedef Cal3dExtensionEnum Cal3dExt;

   Cal3dExtensionEnum::Cal3dExtensionEnum(const std::string& name, dtAnim::ModelResourceType resourceType, const std::string& description)
      : BaseClass(name)
      , mResourceType(resourceType)
      , mDescription(description)
   {
      Cal3dExtensionEnum::AddInstance(this);
   }

   const std::string& Cal3dExtensionEnum::GetDescription() const
   {
      return mDescription;
   }

   dtAnim::ModelResourceType Cal3dExtensionEnum::GetResourceType() const
   {
      return mResourceType;
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class CalOptions : public osg::Object
   {
   public:
      typedef osg::Object BaseClass;

      //////////////////////////////////////////////////////////////////////////
      CalOptions(Cal3DModelData& coreModelData)
         : mCoreModelData(&coreModelData)
      {}

      //////////////////////////////////////////////////////////////////////////
      CalOptions(Cal3DModelData& coreModelData, const std::string objectName)
         : mCoreModelData(&coreModelData)
         , mObjectName(objectName)
      {
      }

      //////////////////////////////////////////////////////////////////////////
      void SetObjectName(const std::string& name)
      {
         mObjectName = name;
      }

      //////////////////////////////////////////////////////////////////////////
      const std::string& GetObjectName()
      {
         return mObjectName;
      }

      //////////////////////////////////////////////////////////////////////////
      void SetFile(const std::string& file)
      {
         mFile = file;
      }

      //////////////////////////////////////////////////////////////////////////
      const std::string& GetFile()
      {
         return mFile;
      }

      //////////////////////////////////////////////////////////////////////////
      Cal3DModelData& GetCoreModelData()
      {
         return *mCoreModelData;
      }

      //////////////////////////////////////////////////////////////////////////
      static dtCore::RefPtr<osgDB::ReaderWriter::Options>
         CreateOSGOptions(CalOptions& optionData)
      {
         dtCore::RefPtr<osgDB::ReaderWriter::Options> newOptions;
         const osgDB::ReaderWriter::Options* globalOptions = osgDB::Registry::instance()->getOptions();

         if (globalOptions != NULL)
         {
            newOptions = static_cast<osgDB::ReaderWriter::Options*>(globalOptions->clone(0));
         }
         else
         {
            newOptions = new osgDB::ReaderWriter::Options;
         }

         newOptions->setPluginData(CAL_PLUGIN_DATA, &optionData);

         return newOptions;
      }

      META_Object("dtAnim", CalOptions);

   private:
      Cal3DModelData* mCoreModelData;
      std::string mObjectName;
      std::string mFile;

      //////////////////////////////////////////////////////////////////////////
      CalOptions()
         : mCoreModelData(NULL)
      {}

      //////////////////////////////////////////////////////////////////////////
      CalOptions(const osg::Object& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
         : BaseClass(obj, copyop)
         , mCoreModelData(NULL)
      {}
   };



   /////////////////////////////////////////////////////////////////////////////
   // PLUGIN CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class CalReaderWriter : public osgDB::ReaderWriter
   {
   public:
      //////////////////////////////////////////////////////////////////////////
      // INTERNAL CLASS CODE
      //////////////////////////////////////////////////////////////////////////
      class MemBuffer : public osg::Referenced
      {
      public:
         //////////////////////////////////////////////////////////////////////////
         MemBuffer(std::istream& fin)
            : mBuffer(NULL)
            , mBufferLength(0)
         {
            if (!fin.fail())
            {
               fin.seekg(0,std::ios_base::end);
               mBufferLength = fin.tellg();
               fin.seekg(0,std::ios_base::beg);

               mBuffer = new (std::nothrow) char [mBufferLength+1];
               mBuffer[mBufferLength] = '\0';
               fin.read(mBuffer, mBufferLength);
            }
         }

         //////////////////////////////////////////////////////////////////////////
         void* GetBufferData() const
         {
            return mBuffer;
         }

         //////////////////////////////////////////////////////////////////////////
         unsigned int GetBufferLength() const
         {
            return mBufferLength;
         }

      protected:

         //////////////////////////////////////////////////////////////////////////
         virtual ~MemBuffer()
         {
            if (mBuffer != NULL)
            {
               delete [] mBuffer;
               mBuffer = NULL;
            }
         }

      private:
         char* mBuffer;
         unsigned int mBufferLength;
      };

      //////////////////////////////////////////////////////////////////////////
      CalReaderWriter()
      {}

      //////////////////////////////////////////////////////////////////////////
      CalOptions* GetCalOptions(const osgDB::ReaderWriter::Options& options) const
      {
         const CalOptions* calOptionsConst
            = static_cast<const CalOptions*>(options.getPluginData(CAL_PLUGIN_DATA));
         return const_cast<CalOptions*>(calOptionsConst);
      }

      //////////////////////////////////////////////////////////////////////////
      osgDB::ReaderWriter::ReadResult readObject(const std::string& fileName, const osgDB::ReaderWriter::Options* options = NULL) const
      {
         std::string ext = osgDB::getLowerCaseFileExtension(fileName);

         if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

         if (!dtUtil::FileUtils::GetInstance().FileExists(fileName))
         {
            return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND);
         }

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
         std::ifstream confStream(fileName.c_str(), std::ios_base::binary);

         if (!confStream.is_open())
         {
            return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
         }

         // Get the Cal3d Options object that is holding onto the Core Model.
         // The Core Model is along for the ride in order to capture the loaded
         // data through its specific interface.
         CalOptions* calOptions = GetCalOptions(*options);
         calOptions->SetFile(fileName);

         osgDB::ReaderWriter::ReadResult result = readObject(confStream, options);
         if (confStream.is_open())
         {
            confStream.close();
         }
         return result;
#else
         // Get the Cal3d Options object that is holding onto the Core Model.
         // The Core Model is along for the ride in order to capture the loaded
         // data through its specific interface.
         CalOptions* calOptions = GetCalOptions(*options);

         if (LoadFile(fileName, *calOptions))
         {
            // Return the CalOptions as the success object. This is so that
            // callers of the ReadObjectFile method will be returned a non-NULL
            // object pointer as a flag that the file loading has succeeded.
            return osgDB::ReaderWriter::ReadResult(calOptions, ReaderWriter::ReadResult::FILE_LOADED);
         }

         return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
#endif
      }

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
      //////////////////////////////////////////////////////////////////////////
      virtual osgDB::ReaderWriter::ReadResult readObject(std::istream& fin, const osgDB::ReaderWriter::Options* options = NULL) const
      {
         // Get the Cal3d Options object that is holding onto the Core Model.
         // The Core Model is along for the ride in order to capture the loaded
         // data through its specific interface.
         CalOptions* calOptions = GetCalOptions(*options);

         // Capture a generic, NULL-terminated memory buffer from the input stream.
         dtCore::RefPtr<MemBuffer> buffer = new MemBuffer(fin);

         if (LoadFile(*buffer, *calOptions))
         {
            // Return the CalOptions as the success object. This is so that
            // callers of the ReadObjectFile method will be returned a non-NULL
            // object pointer as a flag that the file loading has succeeded.
            return osgDB::ReaderWriter::ReadResult(calOptions, ReaderWriter::ReadResult::FILE_LOADED);
         }

         return osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
      }

      //////////////////////////////////////////////////////////////////////////
      virtual bool LoadFile(const MemBuffer& buffer, CalOptions& options) const = 0;
#else
      //////////////////////////////////////////////////////////////////////////
      virtual bool LoadFile(const std::string& file, CalOptions& options) const = 0;
#endif
   };



   ////////////////////////////////////////////////////////////////////////////////
   // PLUGIN CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class CalSkeletonReaderWriter : public CalReaderWriter
   {
   public:

      typedef CalReaderWriter BaseClass;

      //////////////////////////////////////////////////////////////////////////
      CalSkeletonReaderWriter()
      {
         supportsExtension(Cal3dExt::SKELETON_XML.GetName(),
            Cal3dExt::SKELETON_XML.GetDescription());

         supportsExtension(Cal3dExt::SKELETON_BINARY.GetName(),
            Cal3dExt::SKELETON_BINARY.GetDescription());
      }

      //////////////////////////////////////////////////////////////////////////
      const char* className() const
      {
         return "Cal3D Skeleton Reader/Writer";
      }

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
      //////////////////////////////////////////////////////////////////////////
      virtual bool LoadFile(const MemBuffer& buffer, CalOptions& options) const
      {
         return options.GetCoreModelData().LoadCoreSkeletonBuffer(
            buffer.GetBufferData(), options.GetFile(), options.GetObjectName());
      }
#else
      //////////////////////////////////////////////////////////////////////////
      virtual bool LoadFile(const std::string& file, CalOptions& options) const
      {
         options.GetCoreModelData().LoadResource(dtAnim::SKEL_FILE, file, "Skeleton");
         return true;
      }
#endif
   };

   REGISTER_OSGPLUGIN(csf, CalSkeletonReaderWriter)



   ////////////////////////////////////////////////////////////////////////////////
   // PLUGIN CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class CalMaterialReaderWriter : public CalReaderWriter
   {
   public:

      typedef CalReaderWriter BaseClass;

      //////////////////////////////////////////////////////////////////////////
      CalMaterialReaderWriter()
      {
         supportsExtension(Cal3dExt::MATERIAL_XML.GetName(),
            Cal3dExt::MATERIAL_XML.GetDescription());

         supportsExtension(Cal3dExt::MATERIAL_BINARY.GetName(),
            Cal3dExt::MATERIAL_BINARY.GetDescription());
      }

      //////////////////////////////////////////////////////////////////////////
      const char* className() const
      {
         return "Cal3D Material Reader/Writer";
      }

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
      //////////////////////////////////////////////////////////////////////////
      virtual bool LoadFile(const MemBuffer& buffer, CalOptions& options) const
      {
         return 0 <= options.GetCoreModelData().LoadCoreMaterialBuffer(
            buffer.GetBufferData(), options.GetFile(), options.GetObjectName());
      }
#else
      //////////////////////////////////////////////////////////////////////////
      virtual bool LoadFile(const std::string& file, CalOptions& options) const
      {
         return 0 <= options.GetCoreModelData().LoadResource(dtAnim::MAT_FILE,
            file, options.GetObjectName());
      }
#endif
   };

   REGISTER_OSGPLUGIN(crf, CalMaterialReaderWriter)



   ////////////////////////////////////////////////////////////////////////////////
   // PLUGIN CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class CalMeshReaderWriter : public CalReaderWriter
   {
   public:

      typedef CalReaderWriter BaseClass;

      //////////////////////////////////////////////////////////////////////////
      CalMeshReaderWriter()
      {
         supportsExtension(Cal3dExt::MESH_XML.GetName(),
            Cal3dExt::MESH_XML.GetDescription());

         supportsExtension(Cal3dExt::MESH_BINARY.GetName(),
            Cal3dExt::MESH_BINARY.GetDescription());
      }

      //////////////////////////////////////////////////////////////////////////
      const char* className() const
      {
         return "Cal3D Mesh Reader/Writer";
      }

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
      //////////////////////////////////////////////////////////////////////////
      virtual bool LoadFile(const MemBuffer& buffer, CalOptions& options) const
      {
         return 0 <= options.GetCoreModelData().LoadCoreMeshBuffer(
            buffer.GetBufferData(), options.GetFile(), options.GetObjectName());
      }
#else
      //////////////////////////////////////////////////////////////////////////
      virtual bool LoadFile(const std::string& file, CalOptions& options) const
      {
         return 0 <= options.GetCoreModelData().LoadResource(dtAnim::MESH_FILE,
            file, options.GetObjectName());
      }
#endif
   };

   REGISTER_OSGPLUGIN(cmf, CalMeshReaderWriter)



   ////////////////////////////////////////////////////////////////////////////////
   // PLUGIN CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class CalAnimReaderWriter : public CalReaderWriter
   {
   public:

      typedef CalReaderWriter BaseClass;

      //////////////////////////////////////////////////////////////////////////
      CalAnimReaderWriter()
      {
         supportsExtension(Cal3dExt::ANIMATION_XML.GetName(),
            Cal3dExt::ANIMATION_XML.GetDescription());

         supportsExtension(Cal3dExt::ANIMATION_BINARY.GetName(),
            Cal3dExt::ANIMATION_BINARY.GetDescription());
      }

      //////////////////////////////////////////////////////////////////////////
      const char* className() const
      {
         return "Cal3D Animation Reader/Writer";
      }

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
      //////////////////////////////////////////////////////////////////////////
      virtual bool LoadFile(const MemBuffer& buffer, CalOptions& options) const
      {
         return 0 <= options.GetCoreModelData().LoadCoreAnimationBuffer(
            buffer.GetBufferData(), options.GetFile(), options.GetObjectName());
      }
#else
      //////////////////////////////////////////////////////////////////////////
      virtual bool LoadFile(const std::string& file, CalOptions& options) const
      {
         return 0 <= options.GetCoreModelData().LoadResource(dtAnim::ANIM_FILE,
            file, options.GetObjectName());
      }
#endif
   };

   REGISTER_OSGPLUGIN(caf, CalAnimReaderWriter)



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   Cal3dLoader::Cal3dLoader()
      : BaseClass(Constants::CHARACTER_SYSTEM_CAL3D)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3dLoader::~Cal3dLoader()
   {
      Clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   /**
    * @return Could return NULL if the file didn't load.
    * @throw SAXParseException if the file didn't parse correctly
    * @note Relies on the the "animationdefinition.xsd" schema file
    */
   dtCore::RefPtr<dtAnim::BaseModelData> Cal3dLoader::CreateModelData(CharacterFileHandler& handler)
   {
      using namespace dtCore;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      dtCore::RefPtr<Cal3DModelData> modelData = new Cal3DModelData(handler.mName, handler.mResource);
      CalCoreModel* coreModel = modelData->GetCoreModel();

      std::string path;
      try
      {
         path = dtCore::Project::GetInstance().GetResourcePath(handler.mResource);
         path = osgDB::getFilePath(path) + "/";
      }
      catch(const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR, "cal3dloader.cpp");
         path = "./";
      }

      //load skeleton
      std::string skelFile(GetAbsolutePath(path + handler.mSkeletonFilename));
      if (!skelFile.empty() && fileUtils.FileExists(skelFile))
      {
         dtCore::RefPtr<CalOptions> calOptions = new CalOptions(*modelData, "skeleton");
         dtCore::RefPtr<osgDB::ReaderWriter::Options> options = CalOptions::CreateOSGOptions(*calOptions);
         fileUtils.ReadObject(skelFile, options.get());

         modelData->RegisterFile(skelFile, "skeleton");

         size_t boneCount = coreModel->getCoreSkeleton()->getVectorCoreBone().size();

         if (handler.mShaderMaxBones < boneCount)
         {
            LOG_ERROR("Not enough shader bones (" + handler.mResource.GetResourceIdentifier() + ") for the skeleton:'" + skelFile + "'."
               "  Automatically setting shader max bones to " + dtUtil::ToString(boneCount));
            handler.mShaderMaxBones = boneCount;
         }
      }
      else
      {
         LOG_ERROR("Can't find the skeleton file named:'" + skelFile + "'.");
      }

      // load meshes
      std::vector<CharacterFileHandler::MeshStruct>::iterator meshItr = handler.mMeshes.begin();
      while (meshItr != handler.mMeshes.end())
      {
         std::string filename(GetAbsolutePath(path + (*meshItr).mFileName));
         if (!filename.empty())
         {
            // Load the mesh and get its id for further error checking
            dtCore::RefPtr<CalOptions> calOptions = new CalOptions(*modelData, (*meshItr).mName);
            dtCore::RefPtr<osgDB::ReaderWriter::Options> options = CalOptions::CreateOSGOptions(*calOptions);
            if (fileUtils.ReadObject(filename, options.get()) == NULL)
            {
               LOG_ERROR("Can't load mesh '" + filename +"':" + CalError::getLastErrorDescription());
            }
            else
            {
               modelData->RegisterFile(filename, (*meshItr).mName);
               CalCoreMesh* mesh = coreModel->getCoreMesh(coreModel->getCoreMeshId((*meshItr).mName));

               // Make sure this mesh doesn't reference bones we don't have
               if (GetMaxBoneID(*mesh) >= coreModel->getCoreSkeleton()->getVectorCoreBone().size())
               {
                  LOG_ERROR("The bones specified in the cal mesh(" + mesh->getName() +
                     ") do not match the skeleton: (" + handler.mSkeletonFilename + ")");

                  // Attempting to draw this mesh without valid bones will cause a crash so we remove it
                  delete coreModel;
                  coreModel = NULL;
               }
            }
         }
         else
         {
            LOG_ERROR("Can't find mesh file named:'" + path + (*meshItr).mFileName + "'.");
         }
         ++meshItr;
      }

      // If the model is still valid, continue loading
      if (coreModel)
      {
         //load animations
         std::vector<CharacterFileHandler::AnimationStruct>::iterator animItr = handler.mAnimations.begin();
         while (animItr != handler.mAnimations.end())
         {
            std::string filename(GetAbsolutePath(path + (*animItr).mFileName));
            std::string animName = (*animItr).mName.empty() ? filename : (*animItr).mName;

            if (!filename.empty())
            {
               dtCore::RefPtr<CalOptions> calOptions = new CalOptions(*modelData, (*animItr).mName);
               dtCore::RefPtr<osgDB::ReaderWriter::Options> options = CalOptions::CreateOSGOptions(*calOptions);

               AnimationMap::iterator cachedAnimIter = mAnimationCache.find(filename);
               if (cachedAnimIter == mAnimationCache.end())
               {
                  if (fileUtils.ReadObject(filename, options.get()) != NULL)
                  {
                     // Retrieve the animation we just loaded to store in the cache
                     int id = coreModel->getCoreAnimationId(animName);

                     CalCoreAnimation* animToCache = coreModel->getCoreAnimation(id);

                     if (animToCache != NULL)
                     {
                        mAnimationCache[filename] = cal3d::RefPtr<CalCoreAnimation>(animToCache);
                        modelData->RegisterFile(filename, animName);
                     }
                  }
                  else
                  {
                     LOG_ERROR("Can't load animation '" + filename +"':" + CalError::getLastErrorDescription());
                  }
               }
               else
               {
                  int id = coreModel->addCoreAnimation(cachedAnimIter->second.get());
                  coreModel->addAnimationName(animName, id);

                  modelData->RegisterFile(filename, animName);
               }
            }
            else
            {
               LOG_ERROR("Can't find animation file named:'" + path + (*animItr).mFileName + "'.");
            }
            ++animItr;
         }

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
         // load morph animations
         std::vector<CharacterFileHandler::MorphAnimationStruct>::iterator morphAnimItr = handler.mMorphAnimations.begin();
         while (morphAnimItr != handler.mMorphAnimations.end())
         {
            std::string filename = dtUtil::FindFileInPathList(path + (*morphAnimItr).mFileName);

            if (!filename.empty())
            {
               if (coreModel->loadCoreAnimatedMorph(filename) < 0)
               {
                  LOG_ERROR("Can't load animated morph '" + filename +"':" + CalError::getLastErrorDescription());
               }
               else
               {
                  modelData->RegisterFile(filename, filename);
               }
            }
            else
            {
               LOG_ERROR("Can't find morph animation file named:'" + path + (*morphAnimItr).mFileName + "'.");
            }
            ++morphAnimItr;
         }
#endif

         // load materials
         for (std::vector<CharacterFileHandler::MaterialStruct>::iterator matItr = handler.mMaterials.begin();
            matItr != handler.mMaterials.end();
            ++matItr)
         {
            std::string filename(GetAbsolutePath(path + (*matItr).mFileName));

            if (filename.empty())
            {
               LOG_ERROR("Can't find material file named:'" + path + (*matItr).mFileName + "'.");
            }
            else
            {
               dtCore::RefPtr<CalOptions> calOptions = new CalOptions(*modelData, (*matItr).mName);
               dtCore::RefPtr<osgDB::ReaderWriter::Options> options = CalOptions::CreateOSGOptions(*calOptions);
               if (fileUtils.ReadObject(filename, options.get()) == NULL)
               {
                  LOG_ERROR("Material file failed to load:'" + filename + "'." + CalError::getLastErrorDescription());
               }
               else
               {
                  modelData->RegisterFile(filename, (*matItr).mName);
               }
            }
         }
      }


      dtAnim::Cal3DModelData* calModelData = dynamic_cast<dtAnim::Cal3DModelData*>(modelData.get());
      if (calModelData != NULL)
      {
         LoadAllTextures(*calModelData->GetCoreModel(), path); //this should be a user-level process.

         // Store the filename containing IK data if it exists
         if (!handler.mPoseMeshFilename.empty())
         {
            modelData->SetPoseMeshFilename(path + handler.mPoseMeshFilename);
         }
      }
      else
      {
         LOG_ERROR("Unable to load character file: '" + handler.mResource.GetResourceIdentifier() + "'");
         return NULL;
      }

      // Set last few model parameters and attachments.
      BaseClass::SetModelParameters(handler, *modelData);
      CreateAttachments(handler, *modelData);

      // todo remove this if hardware isn't being used
      LoadHardwareData(calModelData);

      return modelData;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtAnim::BaseModelWrapper> Cal3dLoader::CreateModel(dtAnim::BaseModelData& data)
   {
      Cal3DModelData* calModelData = dynamic_cast<Cal3DModelData*>(&data);
      return new Cal3DModelWrapper(*calModelData);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3dLoader::Save(const std::string& file, const dtAnim::BaseModelWrapper& wrapper)
   {
      const dtAnim::Cal3DModelWrapper* calWrapper = dynamic_cast<const dtAnim::Cal3DModelWrapper*>(&wrapper);
      //dtAnim::Cal3DModelData* modelData = calWrapper->GetCalModelData();
      const CalModel* calModel = calWrapper->GetCalModel();

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
      std::string filename = osgDB::convertFileNameToNativeStyle(file);

      dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();
      writer->CreateDocument("character");

      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();

      XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* root = doc->getDocumentElement();

      // Root
      {
         std::string characterName = wrapper.GetModelData()->GetModelName();

         XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("name");
         XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(characterName.c_str());
         root->setAttribute(name, value);
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);
      }

      // Skeleton
      {
         XMLCh* groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("skeleton");
         XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc->createElement(groupName);
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);

         std::string skeletonName = calWrapper->GetCalModel()->getSkeleton()->getCoreSkeleton()->getName();

         XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("fileName");
         XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(skeletonName.c_str());
         groupElement->setAttribute(name, value);
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

         root->appendChild(groupElement);
      }

      // Meshes
      {
         int count = wrapper.GetMeshCount();
         for (int index = 0; index < count; ++index)
         {
            XMLCh* groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("mesh");
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc->createElement(groupName);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);

            dtAnim::Cal3dBaseMesh* mesh = calWrapper->GetMeshByIndex(index);
            std::string meshName = mesh == NULL ? "" : mesh->GetName();

            XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("fileName");
            XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(meshName.c_str());
            groupElement->setAttribute(name, value);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

            root->appendChild(groupElement);
         }
      }

      // Materials
      {
         dtAnim::MaterialInterface* material = NULL;
         dtAnim::MaterialArray materials;
         dtAnim::MaterialArray::iterator curIter = materials.begin();
         dtAnim::MaterialArray::iterator endIter = materials.end();
         wrapper.GetMaterials(materials);
         for (; curIter < endIter; ++curIter)
         {
            material = curIter->get();

            XMLCh* groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("material");
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc->createElement(groupName);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);

            std::string materialName = material->GetName();

            XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("fileName");
            XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(materialName.c_str());
            groupElement->setAttribute(name, value);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

            root->appendChild(groupElement);
         }
      }

      // Bone Animations
      {
         dtAnim::AnimationArray anims;
         wrapper.GetAnimations(anims);

         dtAnim::AnimationInterface* anim = NULL;
         dtAnim::AnimationArray::iterator curIter = anims.begin();
         dtAnim::AnimationArray::iterator endIter = anims.end();
         for (; curIter < endIter; ++curIter)
         {
            anim = curIter->get();

            XMLCh* groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("animation");
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc->createElement(groupName);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);

            std::string animName = anim->GetName();

            XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("fileName");
            XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(animName.c_str());
            groupElement->setAttribute(name, value);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

            root->appendChild(groupElement);
         }
      }

      // Morph Animations
      {
         int count = calModel->getMorphTargetMixer()->getMorphTargetCount();
         for (int index = 0; index < count; ++index)
         {
            XMLCh* groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("morph");
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc->createElement(groupName);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);

            std::string morphName = calModel->getMorphTargetMixer()->getMorphName(index);

            XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("fileName");
            XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(morphName.c_str());
            groupElement->setAttribute(name, value);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

            root->appendChild(groupElement);
         }
      }

      writer->WriteFile(filename);
      return true;
#else
      return false;
#endif
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3dLoader::CreateAttachments(dtAnim::CharacterFileHandler& handler, dtAnim::BaseModelData& modelData)
   {
      for (unsigned i = 0; i < handler.mAttachmentPoints.size(); ++i)
      {
         Cal3DModelData* calModelData = static_cast<Cal3DModelData*>(&modelData);
         calModelData->AddHotspot(handler.mAttachmentPoints[i]);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3dLoader::LoadAllTextures(CalCoreModel& coreModel, const std::string& path)
   {
      int materialId;
      for (materialId = 0; materialId < coreModel.getCoreMaterialCount(); ++materialId)
      {
         // get the core material
         CalCoreMaterial* pCoreMaterial;
         pCoreMaterial = coreModel.getCoreMaterial(materialId);

         // loop through all maps of the core material
         int mapId;
         for (mapId = 0; mapId < pCoreMaterial->getMapCount(); ++mapId)
         {
            // get the filename of the texture
            std::string strFilename;
            strFilename = pCoreMaterial->getMapFilename(mapId);

            TextureMap::iterator textureIterator = mTextureCache.find(strFilename);

            if (textureIterator == mTextureCache.end())
            {
               // load the texture from the file
               osg::Image* img = osgDB::readImageFile(path + strFilename);

               if (!img)
               {
                  LOG_ERROR("Unable to load image file: " + strFilename);
                  continue;
               }

               osg::Texture2D* texture = new osg::Texture2D();
               texture->setImage(img);
               texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
               texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
               mTextureCache[strFilename] = texture;

               // store the opengl texture id in the user data of the map
               pCoreMaterial->setMapUserData(mapId, (Cal::UserData)texture);
            }
            else
            {
               pCoreMaterial->setMapUserData(mapId, (Cal::UserData)((*textureIterator).second.get()));
            }
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
      for (materialId = 0; materialId < coreModel.getCoreMaterialCount(); ++materialId)
      {
         // create the a material thread
         coreModel.createCoreMaterialThread(materialId);

         // initialize the material thread
         coreModel.setCoreMaterialId(materialId, 0, materialId);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   /** Use if you want to completely start over with no history of previous
    * animated entities that have been created.  This will allow you to reload
    * files for a second time.
    * @note: currently this will remove reference to all created osg Textures as well, which
    *        might cause the texture to be deleted.
    */
   void Cal3dLoader::Clear()
   {
      // Note: The texture cache has also passed pointers to corematerial userdata.
      // This may need to be cleaned up to avoid potential crashes after this function is called.
      mTextureCache.clear();
      mAnimationCache.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3dLoader::LoadHardwareData(Cal3DModelData* modelData)
   {
      CalHardwareModel* hardwareModel = modelData->GetOrCreateCalHardwareModel();
      if (!hardwareModel->getVectorHardwareMesh().empty())
      {
         //This could happen if we're re-creating the geometry for a character.  When
         //we call CalHardwareModel::load(), CAL3D doesn't clear this container.
         //CAL3D bug?  Perhaps.  We'll empty it just the same cause it'll double-up
         //all meshes if we don't.
         hardwareModel->getVectorHardwareMesh().clear();
      }

      int numVerts = 0;
      int numIndices = 0;

      {
         CalCoreModel* model = modelData->GetCoreModel();

         const int meshCount = model->getCoreMeshCount();

         for (int meshId = 0; meshId < meshCount; meshId++)
         {
            CalCoreMesh* calMesh = model->getCoreMesh(meshId);

            if (calMesh)
            {
               int submeshCount = calMesh->getCoreSubmeshCount();

               for (int submeshId = 0; submeshId < submeshCount; submeshId++)
               {
                  CalCoreSubmesh* subMesh = calMesh->getCoreSubmesh(submeshId);
                  numVerts += subMesh->getVertexCount();
                  numIndices += 3 * subMesh->getFaceCount();
               }
            }
         }
      }

      // If verts and indices were already successfully loaded, we can allocate resources
      if (numVerts && numIndices)
      {
         const size_t stride = 18;
         const size_t strideBytes = stride * sizeof(float);

         // Allocate data arrays for the hardware model to populate
         modelData->CreateSourceArrays(numVerts, numIndices, stride);

         float* tempVertexArray = new float[stride * numVerts];

         // Get the pointer and fill in the data
         osg::FloatArray* vertexArray = modelData->GetSourceVertexArray();
         osg::IndexArray* indexArray = modelData->GetSourceIndexArray();

         hardwareModel->setIndexBuffer((CalIndex*)indexArray->getDataPointer());

         hardwareModel->setVertexBuffer(reinterpret_cast<char*>(tempVertexArray), strideBytes);
         hardwareModel->setNormalBuffer(reinterpret_cast<char*>(tempVertexArray + 3), strideBytes);

         hardwareModel->setTextureCoordNum(2);
         hardwareModel->setTextureCoordBuffer(0, reinterpret_cast<char*>(tempVertexArray + 6), strideBytes);
         hardwareModel->setTextureCoordBuffer(1, reinterpret_cast<char*>(tempVertexArray + 8), strideBytes);

         hardwareModel->setWeightBuffer(reinterpret_cast<char*>(tempVertexArray + 10), strideBytes);
         hardwareModel->setMatrixIndexBuffer(reinterpret_cast<char*>(tempVertexArray + 14), strideBytes);

         // Load the data into our arrays
         if (!hardwareModel->load(0, 0, modelData->GetShaderMaxBones()))
         {
            LOG_ERROR("Unable to create a hardware mesh:" + CalError::getLastErrorDescription());
         }

         CalIndex* calIndexArray = (CalIndex*)indexArray->getDataPointer();
         InvertTextureCoordinates(hardwareModel, stride, tempVertexArray, modelData, calIndexArray);

         // Move the data into the osg structure
         vertexArray->assign(tempVertexArray, tempVertexArray + numVerts * stride);

         delete[] tempVertexArray;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3dLoader::InvertTextureCoordinates(CalHardwareModel* hardwareModel, const size_t stride,
      float* vboVertexAttr, Cal3DModelData* modelData, CalIndex*& indexArray)
   {
      const int numVerts = hardwareModel->getTotalVertexCount();
      //invert texture coordinates.
      for (unsigned i = 0; i < numVerts * stride; i += stride)
      {
         for (unsigned j = 15; j < 18; ++j)
         {
            if (vboVertexAttr[i + j] > modelData->GetShaderMaxBones())
            {
               vboVertexAttr[i + j] = 0;
            }
         }

         vboVertexAttr[i + 7] = 1.0f - vboVertexAttr[i + 7]; //the odd texture coordinates in cal3d are flipped, not sure why
         vboVertexAttr[i + 9] = 1.0f - vboVertexAttr[i + 9]; //the odd texture coordinates in cal3d are flipped, not sure why
      }

      for (int meshCount = 0; meshCount < hardwareModel->getHardwareMeshCount(); ++meshCount)
      {
         hardwareModel->selectHardwareMesh(meshCount);

         for (int face = 0; face < hardwareModel->getFaceCount(); ++face)
         {
            for (int index = 0; index < 3; ++index)
            {
               indexArray[face * 3 + index + hardwareModel->getStartIndex()] += hardwareModel->getBaseVertexIndex();
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int Cal3dLoader::GetMaxBoneID(CalCoreMesh& mesh)
   {
      int maxBoneID = 0;

      // Go through every vertex and find the highest bone id
      for (int subIndex = 0; subIndex < mesh.getCoreSubmeshCount(); ++subIndex)
      {
         CalCoreSubmesh* subMesh = mesh.getCoreSubmesh(subIndex);
         const std::vector<CalCoreSubmesh::Vertex>& vertList = subMesh->getVectorVertex();

         for (size_t vertIndex = 0; vertIndex < vertList.size(); ++vertIndex)
         {
            std::vector<CalCoreSubmesh::Influence> influenceList = vertList[vertIndex].vectorInfluence;
            for (size_t influenceIndex = 0; influenceIndex < influenceList.size(); ++influenceIndex)
            {
               if (influenceList[influenceIndex].boneId > maxBoneID)
               {
                  maxBoneID = influenceList[influenceIndex].boneId;
               }
            }
         }
      }

      return maxBoneID;
   }

} // namespace dtAnim
