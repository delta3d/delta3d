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
#include <cal3d/error.h>
#include <cal3d/model.h>
#include <cal3d/coremodel.h>
#include <cal3d/coreanimation.h>
#include <dtAnim/macros.h>
#include <dtAnim/animationwrapper.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/characterfilewriter.h>
#include <dtAnim/hardwaresubmesh.h>

#include <dtCore/basexmlhandler.h>
#include <dtCore/basexmlreaderwriter.h>
#include <dtCore/hotspotattachment.h>

#include <dtUtil/hotspotdefinition.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/threadpool.h>

namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   static const dtUtil::RefString CAL_PLUGIN_DATA("CalPluginData");



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

#ifdef CAL3D_VERSION_DEVELOPMENT
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

#ifdef CAL3D_VERSION_DEVELOPMENT
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
         supportsExtension("xsf","Cal3D Skeleton File (XML)");
         supportsExtension("csf","Cal3D Skeleton File (Binary)");
      }

      //////////////////////////////////////////////////////////////////////////
      const char* className() const
      {
         return "Cal3D Skeleton Reader/Writer";
      }

#ifdef CAL3D_VERSION_DEVELOPMENT
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
         options.GetCoreModelData().LoadCoreSkeleton(file);
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
         supportsExtension("xrf","Cal3D Material File (XML)");
         supportsExtension("crf","Cal3D Material File (Binary)");
      }

      //////////////////////////////////////////////////////////////////////////
      const char* className() const
      {
         return "Cal3D Material Reader/Writer";
      }

#ifdef CAL3D_VERSION_DEVELOPMENT
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
         return 0 <= options.GetCoreModelData().LoadCoreMaterial(file, options.GetObjectName());
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
         supportsExtension("xmf","Cal3D Mesh File (XML)");
         supportsExtension("cmf","Cal3D Mesh File (Binary)");
      }

      //////////////////////////////////////////////////////////////////////////
      const char* className() const
      {
         return "Cal3D Mesh Reader/Writer";
      }

#ifdef CAL3D_VERSION_DEVELOPMENT
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
         return 0 <= options.GetCoreModelData().LoadCoreMesh(file, options.GetObjectName());
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
         supportsExtension("xaf","Cal3D Animation File (XML)");
         supportsExtension("caf","Cal3D Animation File (Binary)");
      }

      //////////////////////////////////////////////////////////////////////////
      const char* className() const
      {
         return "Cal3D Animation Reader/Writer";
      }

#ifdef CAL3D_VERSION_DEVELOPMENT
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
         return 0 <= options.GetCoreModelData().LoadCoreAnimation(file, options.GetObjectName());
      }
#endif
   };

   REGISTER_OSGPLUGIN(caf, CalAnimReaderWriter)



   /////////////////////////////////////////////////////////////////////////////
   // OSG Object for holding sound buffer data loaded from the following
   // OSG plugin. This will allow the Audio Manager to access buffer information
   // supplied from alut, after the file has been loaded from memory but before
   // the buffer is registered with OpenAL; this is to allow the Audio Manager
   // to have veto power over the loaded file, before its buffer is
   // officially registered.
   /////////////////////////////////////////////////////////////////////////////
   class WrapperOSGCharFileObject : public osg::Object
   {
   public:
      typedef osg::Object BaseClass;

      //////////////////////////////////////////////////////////////////////////
      WrapperOSGCharFileObject()
         : BaseClass()
      {}

      //////////////////////////////////////////////////////////////////////////
      explicit WrapperOSGCharFileObject(bool threadSafeRefUnref)
         : BaseClass(threadSafeRefUnref)
      {}

      //////////////////////////////////////////////////////////////////////////
      WrapperOSGCharFileObject(const osg::Object& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
         : BaseClass(obj, copyop)
      {}

      dtCore::RefPtr<CharacterFileHandler> mHandler;

      META_Object("dtAnim", WrapperOSGCharFileObject);
   };



   ////////////////////////////////////////////////////////////////////////////////
   // PLUGIN CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class CharacterXMLReaderWriter : public dtCore::BaseXMLReaderWriter<Cal3DModelData, CharacterFileHandler, CharacterFileWriter>
   {
   public:

      typedef dtCore::BaseXMLReaderWriter<CalCoreModel, CharacterFileHandler, CharacterFileWriter> BaseClass;

      //////////////////////////////////////////////////////////////////////////
      CharacterXMLReaderWriter()
      {
         supportsExtension("dtchar","Delta3D Character File (XML)");

         SetSchemaFile("animationdefinition.xsd");
      }

      //////////////////////////////////////////////////////////////////////////
      const char* className() const
      {
         return "Delta3D Character File Reader/Writer";
      }

      //////////////////////////////////////////////////////////////////////////
      virtual osgDB::ReaderWriter::ReadResult BuildResult(
         const osgDB::ReaderWriter::ReadResult& result, CharacterFileHandler& handler) const
      {
         using namespace osgDB;

         ReaderWriter::ReadResult newResult(result);

         if (result.status() == ReaderWriter::ReadResult::FILE_LOADED)
         {
            // Create the wrapper object that will carry the file
            // handler object out of this plug-in.
            dtCore::RefPtr<WrapperOSGCharFileObject> obj = new WrapperOSGCharFileObject;
            obj->mHandler = dynamic_cast<CharacterFileHandler*>(&handler);

            // Pass the object on the result so that code external
            // to this plug-in can access the data acquired by the
            // contained handler.
            newResult = ReaderWriter::ReadResult(obj.get(), ReaderWriter::ReadResult::FILE_LOADED);
         }

         return newResult;
      }
   };

   REGISTER_OSGPLUGIN(dtchar, CharacterXMLReaderWriter)



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   Cal3DLoader::Cal3DLoader()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3DLoader::~Cal3DLoader()
   {
      PurgeAllCaches();
   }

   /////////////////////////////////////////////////////////////////////////////
   // HELPER FUNCTION
   std::string GetAbsolutePath(const std::string& filePath)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      dtUtil::FileInfo fi = fileUtils.GetFileInfo(filePath, true);

      return fi.fileType == dtUtil::REGULAR_FILE
         ? fi.fileName
         : dtUtil::FindFileInPathList(filePath);
   }

   /////////////////////////////////////////////////////////////////////////////
   /**
    * @return Could return NULL if the file didn't load.
    * @throw SAXParseException if the file didn't parse correctly
    * @note Relies on the the "animationdefinition.xsd" schema file
    */
   dtCore::RefPtr<Cal3DModelData> Cal3DLoader::GetCoreModelData(dtCore::RefPtr<CharacterFileHandler>& handler, const std::string& filename, const std::string& path)
   {
      using namespace dtCore;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      dtCore::RefPtr<Cal3DModelData> coreModelData;

      //gotta parse the file and create/store a new CalCoreModel
      dtUtil::XercesParser parser;

      // Parse the character file, subsequently from the associated OSG plug-in...
      dtCore::RefPtr<WrapperOSGCharFileObject> resultObj;
      osgDB::Registry* osgReg = osgDB::Registry::instance();
      const osgDB::ReaderWriter::Options* globalOptions = osgReg->getOptions();

      // ...if it is the older character format...
      if (osgDB::getLowerCaseFileExtension(filename) == "xml")
      {
         // ...get the plug-in directly...
         CharacterXMLReaderWriter* charPlugin = dynamic_cast<CharacterXMLReaderWriter*>
            (osgReg->getReaderWriterForExtension("dtchar"));

         // Open the file as a stream.
         std::ifstream fstream(filename.c_str(), std::ios_base::binary);
         if (fstream.is_open())
         {
            // Call the input stream overload of "readObject".
            // This will allow the file to load without failing by the
            // file extension (which is not acceptable to the plug-in).
            resultObj = static_cast<WrapperOSGCharFileObject*>
               (charPlugin->readObject(fstream, globalOptions).getObject());
         }
         else
         {
            std::ostringstream oss;
            oss << "Could not open stream for file \"" << filename << "\"." << std::endl;
            LOG_ERROR(oss.str());
         }
      }
      else
      {
         // ...otherwise the file can be opened as normal,
         // by finding the appropriate plug-in automatically.
         resultObj = static_cast<WrapperOSGCharFileObject*>
            (osgDB::readRefObjectFile(filename, globalOptions).get());
      }

      if (resultObj.valid())
      {
         // Acquire the handler that was involved with the parsing.
         handler = resultObj->mHandler.get();
      }

      if (handler.valid())
      {
         coreModelData = new Cal3DModelData(handler->mName, filename);
         CalCoreModel* coreModel = coreModelData->GetCoreModel();

         //load skeleton
         std::string skelFile(GetAbsolutePath(path + handler->mSkeletonFilename));
         if (!skelFile.empty() && fileUtils.FileExists(skelFile))
         {
            dtCore::RefPtr<CalOptions> calOptions = new CalOptions(*coreModelData, "skeleton");
            dtCore::RefPtr<osgDB::ReaderWriter::Options> options = CalOptions::CreateOSGOptions(*calOptions);
            fileUtils.ReadObject(skelFile, options.get());

            size_t boneCount = coreModel->getCoreSkeleton()->getVectorCoreBone().size();

            if (handler->mShaderMaxBones < boneCount)
            {
               LOG_ERROR("Not enough shader bones (" + filename + ") for the skeleton:'" + skelFile + "'."
                  "  Automatically setting shader max bones to " + dtUtil::ToString(boneCount));
               handler->mShaderMaxBones = boneCount;
            }
         }
         else
         {
            LOG_ERROR("Can't find the skeleton file named:'" + skelFile + "'.");
         }

         // load meshes
         std::vector<CharacterFileHandler::MeshStruct>::iterator meshItr = handler->mMeshes.begin();
         while (meshItr != handler->mMeshes.end())
         {
            std::string filename(GetAbsolutePath(path + (*meshItr).mFileName));
            if (!filename.empty())
            {
               // Load the mesh and get its id for further error checking
               dtCore::RefPtr<CalOptions> calOptions = new CalOptions(*coreModelData, (*meshItr).mName);
               dtCore::RefPtr<osgDB::ReaderWriter::Options> options = CalOptions::CreateOSGOptions(*calOptions);
               if (fileUtils.ReadObject(filename, options.get()) == NULL)
               {
                  LOG_ERROR("Can't load mesh '" + filename +"':" + CalError::getLastErrorDescription());
               }
               else
               {
                  CalCoreMesh* mesh = coreModel->getCoreMesh(coreModel->getCoreMeshId((*meshItr).mName));

                  // Make sure this mesh doesn't reference bones we don't have
                  if (GetMaxBoneID(*mesh) >= coreModel->getCoreSkeleton()->getVectorCoreBone().size())
                  {
                     LOG_ERROR("The bones specified in the cal mesh(" + mesh->getName() +
                        ") do not match the skeleton: (" + handler->mSkeletonFilename + ")");

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
            std::vector<CharacterFileHandler::AnimationStruct>::iterator animItr = handler->mAnimations.begin();
            while (animItr != handler->mAnimations.end())
            {
               std::string filename(GetAbsolutePath(path + (*animItr).mFileName));
               std::string animName = (*animItr).mName.empty() ? filename : (*animItr).mName;

               if (!filename.empty())
               {
                  dtCore::RefPtr<CalOptions> calOptions = new CalOptions(*coreModelData, (*animItr).mName);
                  dtCore::RefPtr<osgDB::ReaderWriter::Options> options = CalOptions::CreateOSGOptions(*calOptions);

                  AnimationMap::iterator cachedAnimIter = mAnimationCache.find(filename);
                  if (cachedAnimIter == mAnimationCache.end())
                  {
                     if (fileUtils.ReadObject(filename, options.get()) != NULL)
                     {
                        // Retrieve the animation we just loaded to store in the cache
                        int id = coreModel->getCoreAnimationId(animName);

                        CalCoreAnimation* animToCache = coreModel->getCoreAnimation(id);

                        if (animToCache)
                        {
                           mAnimationCache[filename] = cal3d::RefPtr<CalCoreAnimation>(animToCache);
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

                     coreModelData->RegisterFile(filename, animName);
                  }
               }
               else
               {
                  LOG_ERROR("Can't find animation file named:'" + path + (*animItr).mFileName + "'.");
               }
               ++animItr;
            }

#ifdef CAL3D_VERSION_DEVELOPMENT
            // load morph animations
            std::vector<CharacterFileHandler::MorphAnimationStruct>::iterator morphAnimItr = handler->mMorphAnimations.begin();
            while (morphAnimItr != handler->mMorphAnimations.end())
            {
               std::string filename = dtUtil::FindFileInPathList(path + (*morphAnimItr).mFileName);

               if (!filename.empty())
               {
                  if (coreModel->loadCoreAnimatedMorph(filename) < 0)
                  {
                     LOG_ERROR("Can't load animated morph '" + filename +"':" + CalError::getLastErrorDescription());
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
            for (std::vector<CharacterFileHandler::MaterialStruct>::iterator matItr = handler->mMaterials.begin();
               matItr != handler->mMaterials.end();
               ++matItr)
            {
               std::string filename(GetAbsolutePath(path + (*matItr).mFileName));

               if (filename.empty())
               {
                  LOG_ERROR("Can't find material file named:'" + path + (*matItr).mFileName + "'.");
               }
               else
               {
                  dtCore::RefPtr<CalOptions> calOptions = new CalOptions(*coreModelData, (*matItr).mName);
                  dtCore::RefPtr<osgDB::ReaderWriter::Options> options = CalOptions::CreateOSGOptions(*calOptions);
                  if (fileUtils.ReadObject(filename, options.get()) == NULL)
                  {
                     LOG_ERROR("Material file failed to load:'" + filename + "'." + CalError::getLastErrorDescription());
                  }
               }
            }
         }
      }


      return coreModelData;
   }

   /////////////////////////////////////////////////////////////////////////////
   /**
    * Will use the Delta3D search paths to find the supplied filename.  Will create
    * a new Cal3DModelWrapper, but you're responsible for deleting it.
    * @note The animations are named with their filenames by default, or by an
    *       optional name attribute in the .xml file.
    * @return A fully defined CalModel wrapped by a Cal3DModelWrapper.  RefPtr could
    *         be not valid (wrapper->valid()==false) if the file didn't load correctly.
    * @see SetDataFilePathList()
    * @throw SAXParseException If the file wasn't formatted correctly
    */
   bool Cal3DLoader::Load(const std::string& file, dtCore::RefPtr<Cal3DModelData>& outCoreModelData)
   {
      std::string filename(file);

      std::string path(osgDB::getFilePath(filename));
      if (!path.empty()) { path += '/'; }
      else { path = "./"; }

      dtCore::RefPtr<CharacterFileHandler> handler;
      outCoreModelData = GetCoreModelData(handler, filename, path);
      if (outCoreModelData.valid())
      {
         LoadModelData(*handler, *outCoreModelData);
         LoadAllTextures(*outCoreModelData->GetCoreModel(), path); //this should be a user-level process.

         // Store the filename containing IK data if it exists
         if (!handler->mPoseMeshFilename.empty())
         {
            outCoreModelData->SetPoseMeshFilename(path + handler->mPoseMeshFilename);
         }
      }
      else
      {
         LOG_ERROR("Unable to load character file: '" + filename + "'");
         return false;
      }

      // todo remove this if hardware isn't being used
      LoadHardwareData(outCoreModelData);

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   // Helper Function
   typedef CharacterFileHandler::AnimatableOverrideStruct OverrideStruct;
   typedef CharacterFileHandler::AnimatableOverrideStructArray OverrideStructArray;
   typedef CharacterFileHandler::AnimationSequenceStruct AnimationSequenceStruct;
   void SetAnimatableValues(Animatable& animatable, const OverrideStruct& info)
   {
      if(info.mOverrideFadeIn)
      {
         animatable.SetFadeIn(info.mFadeIn);
      }

      if(info.mOverrideFadeOut)
      {
         animatable.SetFadeOut(info.mFadeOut);
      }

      if(info.mOverrideStartDelay)
      {
         animatable.SetStartDelay(info.mStartDelay);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   // Helper Function
   /*bool IsAnimatableAlreadyParent(Animatable& animatable)
   {
      // TODO:

      return false;
   }*/

   /////////////////////////////////////////////////////////////////////////////
   // Helper Function
   OverrideStruct* GetPreviousOverrideStruct(const std::string& name,
      AnimationSequenceStruct& sequenceStruct, OverrideStructArray::iterator& startIter)
   {
      OverrideStruct* overrideStruct = NULL;

      // Go through and find the last occurrence of the struct
      // that has the specified name.
      OverrideStructArray::iterator curIter = sequenceStruct.GetChildren().begin();
      OverrideStructArray::iterator endIter = startIter;
      for (; curIter != endIter; ++curIter)
      {
         if(name == curIter->mName)
         {
            overrideStruct = &(*curIter);
         }
      }

      // If the struct was not found before the start point, then look after
      // the start point.
      if(overrideStruct == NULL && startIter != sequenceStruct.GetChildren().end())
      {
         curIter = startIter;
         ++curIter;
         OverrideStructArray::iterator endIter = sequenceStruct.GetChildren().end();
         for (; curIter != endIter; ++curIter)
         {
            if(name == curIter->mName)
            {
               overrideStruct = &(*curIter);

               // Found the first occurrence.
               break;
            }
         }
      }

      return overrideStruct;
   }

   /////////////////////////////////////////////////////////////////////////////
   // Helper Function
   Animatable* GetAnimatable(const std::string& animName, Cal3DModelData::AnimatableArray& animArray)
   {
      Animatable* anim = NULL;
      Cal3DModelData::AnimatableArray::iterator curIter = animArray.begin();
      Cal3DModelData::AnimatableArray::iterator endIter = animArray.end();
      for(; curIter != endIter; ++curIter)
      {
         if((*curIter)->GetName() == animName)
         {
            anim = curIter->get();
            break;
         }
      }

      return anim;
   }

   /////////////////////////////////////////////////////////////////////////////
   // Helper Function
   float ResolveCrossFade(OverrideStruct& previous, OverrideStruct& current)
   {
      const float crossFade = current.mCrossFade;

      // Check for cross fade.
      if (crossFade != 0.0f)
      {
         if (previous.mFadeOut < crossFade)
         {
            previous.mOverrideFadeOut = true;
            previous.mFadeOut = crossFade;
         }

         if (current.mFadeIn < crossFade)
         {
            current.mOverrideFadeIn = true;
            current.mFadeIn = crossFade;
         }
      }

      return crossFade;
   }

   /////////////////////////////////////////////////////////////////////////////
   // Helper Function
   void Cal3DLoader::FinalizeSequenceInfo(AnimationSequenceStruct& sequenceStruct,
      Cal3DModelData::AnimatableArray& animArray)
   {
      // Declare variables to be used by the subsequent loop.
      float curTimeOffset = 0.0f;
      Animatable* prevAnim = NULL;
      Animatable* curAnim = NULL;
      OverrideStruct* prevStruct = NULL;
      OverrideStruct* curStruct = NULL;
      OverrideStructArray::iterator curIter = sequenceStruct.GetChildren().begin();
      OverrideStructArray::iterator endIter = sequenceStruct.GetChildren().end();

      // For each animation segment...
      for (; curIter != endIter; ++curIter)
      {
         // Get ready for this loop.
         prevStruct = curStruct;
         curStruct = &(*curIter);

         prevAnim = curAnim;
         curAnim = GetAnimatable(curStruct->mName, animArray);

         // TODO:
         // Prevent recursive nesting of sequences. Avoid situations such as
         // sequence A includes sequence B, and at a lower level of B, sequence A
         // could be included again.
         /*if(IsAnimatableAlreadyParent(curAnim))
         {
            // TODO: How?
            continue;
         }*/

         // Determine if the this loop should be skipped.
         if( ! curStruct->mOverrideStartDelay)
         {
            curStruct->mOverrideStartDelay = !curStruct->mFollowAnimatableName.empty()
               || curStruct->mFollowsPrevious;

            if( ! curStruct->mOverrideStartDelay)
            {
               // Time offset/delay should not be modified
               // for the current animatable.
               continue;
            }
         }

         // Determine if the current animation follows another, that
         // is not the previous one.
         if (!curStruct->mFollowAnimatableName.empty())
         {
            // Get the referenced animation segment info struct.
            OverrideStruct* foundStruct
               = GetPreviousOverrideStruct(curStruct->mFollowAnimatableName, sequenceStruct, curIter);

            // Set it as the new previous animation.
            if(foundStruct != NULL && prevStruct != foundStruct)
            {
               // DEBUG:
               //std::cout << "\n\t\t\t\tFOUND: " << curStruct->mFollowAnimatableName << "\n";

               prevStruct = foundStruct;
               prevAnim = GetAnimatable(foundStruct->mName, animArray);
            }
         }
         // If not following the previously processed struct...
         else if(!curStruct->mFollowsPrevious)
         {
            // ...do not reference the struct and reset the current offset.
            prevStruct = NULL;
            curTimeOffset = 0.0f;
         }


         // Work on things that require information from the previous animatable.
         if (prevStruct != NULL)
         {
            // DEBUG:
            //std::cout << "\n\t\t\t" << curStruct->mName << " follows: " << prevStruct->mName << "\n";

            // Adjust time offset relative to the previous animation segment.
            if(prevAnim != NULL)
            {
               // Adjust the offset only if there is an end to the animation.
               float animLength = prevAnim->CalculateDuration();
               if(animLength != Animatable::INFINITE_TIME)
               {
                  // DEBUG:
                  //std::cout << "\n\t\t\t\tOffset(none) by: " << (prevStruct->mStartDelay + animLength) << "\n";

                  curTimeOffset = prevStruct->mStartDelay + animLength;
               }
            }

            // Deduct cross fade time from the current time offset
            // as the current segment will start withing the time
            // period of the previous segment.
            curTimeOffset -= ResolveCrossFade(*prevStruct, *curStruct);
         }


         // Shift the time offset for the current animatable struct.
         curStruct->mStartDelay += curTimeOffset;

         if(curStruct->mStartDelay < 0.0f)
         {
            curStruct->mStartDelay = 0.0f;
         }

         // DEBUG:
         //std::cout << "\n\t\t\t\tStarts: " << curStruct->mStartDelay << "\n";
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   // Helper Function
   void SetAnimatableEvents(Animatable& anim, const CharacterFileHandler::AnimatableStruct& info)
   {
      typedef CharacterFileHandler::AnimatableStruct::EventTimeMap EventTimeMap;

      // DEBUG:
      //std::cout << "\nEvents for Anim:\t" << anim.GetName();

      float curOffset = 0.0f;
      EventTimeMap::const_iterator curIter = info.mEventTimeMap.begin();
      EventTimeMap::const_iterator endIter = info.mEventTimeMap.end();
      for (; curIter != endIter; ++curIter)
      {
         curOffset = curIter->second;

         // DEBUG:
         //std::cout << "\n\tEvent:\t" << curIter->first;
         //std::cout << "\n\t\tOffset:\t" << curOffset;

         // If the offset is not positive, then the end time (duration) will be used.
         if (curOffset < 0.0f)
         {
            curOffset = anim.CalculateDuration();
         }

         // DEBUG:
         //std::cout << "\n\t\tOffset:\t" << curOffset << "\n\n";

         anim.AddEventOnTime(curIter->first, curOffset);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   static void CreateAttachments(dtAnim::CharacterFileHandler& handler, Cal3DModelData& modelData)
   {
      for (unsigned i = 0; i < handler.mAttachmentPoints.size(); ++i)
      {

         modelData.Add(handler.mAttachmentPoints[i]);

      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DLoader::LoadModelData(dtAnim::CharacterFileHandler& handler, Cal3DModelData& modelData)
   {
      CalCoreModel& model = *modelData.GetCoreModel();

      // Redefine some types so that they are easier to read.
      typedef CharacterFileHandler::AnimationChannelStruct ChannelStruct;
      typedef CharacterFileHandler::ChannelStructArray ChannelStructArray;
      typedef CharacterFileHandler::AnimationSequenceStruct SequenceStruct;
      typedef CharacterFileHandler::SequenceStructArray SequenceStructArray;

      modelData.SetModelName(handler.mName);

      // create animation wrappers
      int numAnims = model.getCoreAnimationCount();
      modelData.GetAnimationWrappers().reserve(numAnims);

      for (int i = 0; i < numAnims; ++i)
      {
         CalCoreAnimation* anim = model.getCoreAnimation(i);
         if (anim)
         {
            AnimationWrapper* pWrapper = new AnimationWrapper(anim->getName(), i);
            pWrapper->SetDuration(anim->getDuration());
            modelData.Add(pWrapper);
         }
         else
         {
            LOG_ERROR("Cannot find CalCoreAnimation for animation '" + anim->getName() + "'");
         }
      }

      modelData.SetShaderGroupName(handler.mShaderGroup);
      modelData.SetShaderName(handler.mShaderName);
      modelData.SetShaderMaxBones(handler.mShaderMaxBones);

      LODOptions& lodOptions = modelData.GetLODOptions();

      //always set default lod values
      //if (handler.mFoundLODOptions)
      {
         lodOptions.SetStartDistance(handler.mLODStartDistance);
         lodOptions.SetEndDistance(handler.mLODEndDistance);
         lodOptions.SetMaxVisibleDistance(handler.mLODMaxVisibleDistance);
      }

      if (handler.mFoundScale && modelData.GetCoreModel() != NULL)
      {
         modelData.SetScale(handler.mScale);
      }

      // register animations
      if (!handler.mAnimationChannels.empty())
      {
         int numAnimatables = handler.mAnimationChannels.size() + handler.mAnimationSequences.size();
         modelData.GetAnimatables().reserve(numAnimatables);

         ChannelStructArray::iterator channelIter = handler.mAnimationChannels.begin();
         ChannelStructArray::iterator channelEnd = handler.mAnimationChannels.end();
         for (;channelIter != channelEnd; ++channelIter)
         {
            ChannelStruct& pStruct = *channelIter;

            // DEBUG:
            //std::cout << "Getting channel:\t" << pStruct.mAnimationName << "\n";

            int id = model.getCoreAnimationId(pStruct.mAnimationName);
            if (id >= 0 && id < numAnims)
            {
               dtCore::RefPtr<AnimationChannel> pChannel = new AnimationChannel();

               pChannel->SetAnimation(modelData.GetAnimationWrappers()[id].get());

               pChannel->SetName(pStruct.mName);
               pChannel->SetLooping(pStruct.mIsLooping);
               pChannel->SetAction(pStruct.mIsAction);
               pChannel->SetMaxDuration(pStruct.mMaxDuration);
               pChannel->SetStartDelay(pStruct.mStartDelay);
               pChannel->SetBaseWeight(pStruct.mBaseWeight);
               pChannel->SetFadeIn(pStruct.mFadeIn);
               pChannel->SetFadeOut(pStruct.mFadeOut);
               pChannel->SetSpeed(pStruct.mSpeed);
               SetAnimatableEvents(*pChannel, pStruct);

               // DEBUG:
               //std::cout << "\tAdding channel:\t" << pChannel->GetName() << "\n";

               modelData.Add(pChannel.get());
            }
            else
            {
               LOG_ERROR("Unable to find animation '" + pStruct.mAnimationName +
                  "' within the CalCoreModel. (" + modelData.GetFilename() + ")");
            }

         }

         SequenceStructArray::iterator sequenceIter = handler.mAnimationSequences.begin();
         SequenceStructArray::iterator sequenceEnd = handler.mAnimationSequences.end();
         for (;sequenceIter != sequenceEnd; ++sequenceIter)
         {
            CharacterFileHandler::AnimationSequenceStruct& pStruct = *sequenceIter;

            // DEBUG:
            //std::cout << "\n\tAdding sequence:\t" << pStruct.mName << "\n";

            // Ensure child animation overrides time offsets are setup in relation to each other.
            FinalizeSequenceInfo(pStruct, modelData.GetAnimatables());

            dtCore::RefPtr<AnimationSequence> pSequence = new AnimationSequence();

            pSequence->SetName(pStruct.mName);
            pSequence->SetStartDelay(pStruct.mStartDelay);
            pSequence->SetFadeIn(pStruct.mFadeIn);
            pSequence->SetFadeOut(pStruct.mFadeOut);
            pSequence->SetSpeed(pStruct.mSpeed);
            pSequence->SetBaseWeight(pStruct.mBaseWeight);

            // find children
            OverrideStruct* curOverrideStruct = NULL;
            OverrideStructArray::iterator i = pStruct.GetChildren().begin();
            OverrideStructArray::iterator end = pStruct.GetChildren().end();
            for (; i != end; ++i)
            {
               curOverrideStruct = &(*i);

               const std::string& nameToFind = curOverrideStruct->mName;
               Cal3DModelData::AnimatableArray::iterator animIter = modelData.GetAnimatables().begin();
               Cal3DModelData::AnimatableArray::iterator animIterEnd = modelData.GetAnimatables().end();

               for (; animIter != animIterEnd; ++animIter)
               {
                  Animatable* animatable = animIter->get();
                  if (animatable->GetName() == nameToFind
                     && pStruct.mName != nameToFind) // Avoid referencing the sequence itself.
                  {
                     // Copy the found animation since it may be overridden
                     // within the scope of a sequence.
                     dtCore::RefPtr<Animatable> newAnim = animatable->Clone(NULL);

                     // Override values as specified in the character file.
                     SetAnimatableValues(*newAnim, *curOverrideStruct);
                     SetAnimatableEvents(*newAnim, *curOverrideStruct);

                     pSequence->GetChildAnimations().push_back(newAnim.get());

                     // DEBUG:
                     //std::cout << "\n\t\tSub Anim:\t" << newAnim->GetName() << "\n";
                  }
               }
            }

            SetAnimatableEvents(*pSequence, pStruct);

            modelData.Add(pSequence.get());

         }
      }

      if (!handler.mAttachmentPoints.empty())
      {
         CreateAttachments(handler, modelData);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DLoader::LoadAllTextures(CalCoreModel& coreModel, const std::string& path)
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
   void Cal3DLoader::PurgeAllCaches()
   {
      // Note: The texture cache has also passed pointers to corematerial userdata.
      // This may need to be cleaned up to avoid potential crashes after this function is called.
      mTextureCache.clear();
      mAnimationCache.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DLoader::LoadHardwareData(Cal3DModelData* modelData)
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
                  subMesh->enableTangents(0, true);
                  numVerts += subMesh->getVertexCount();
                  numIndices += 3 * subMesh->getFaceCount();
               }
            }
         }
      }

      // If verts and indices were already successfully loaded, we can allocate resources
      if (numVerts && numIndices)
      {
         // Allocate data arrays for the hardware model to populate
         modelData->CreateSourceArrays(numVerts, numIndices, HardwareSubmeshDrawable::VBO_STRIDE);

          // need to leave some extra space for duplicate vertices if the mesh needs to be subdivided into
          // submeshes 
         float* tempVertexArray = new float[HardwareSubmeshDrawable::VBO_STRIDE * numVerts * 2];

         // Get the pointer and fill in the data
         osg::FloatArray* vertexArray = modelData->GetSourceVertexArray();
         CalIndex* indexArray = modelData->GetSourceIndexArray();

         hardwareModel->setIndexBuffer(indexArray);

         hardwareModel->setVertexBuffer(reinterpret_cast<char*>(tempVertexArray+ HardwareSubmeshDrawable::VBO_OFFSET_POSITION), HardwareSubmeshDrawable::VBO_STRIDE_BYTES);
         hardwareModel->setNormalBuffer(reinterpret_cast<char*>(tempVertexArray + HardwareSubmeshDrawable::VBO_OFFSET_NORMAL), HardwareSubmeshDrawable::VBO_STRIDE_BYTES);

         hardwareModel->setTextureCoordNum(2);
         hardwareModel->setTextureCoordBuffer(0, reinterpret_cast<char*>(tempVertexArray + HardwareSubmeshDrawable::VBO_OFFSET_TEXCOORD0), HardwareSubmeshDrawable::VBO_STRIDE_BYTES);
         hardwareModel->setTextureCoordBuffer(1, reinterpret_cast<char*>(tempVertexArray + HardwareSubmeshDrawable::VBO_OFFSET_TEXCOORD1), HardwareSubmeshDrawable::VBO_STRIDE_BYTES);

         hardwareModel->setWeightBuffer(reinterpret_cast<char*>(tempVertexArray + HardwareSubmeshDrawable::VBO_OFFSET_WEIGHT), HardwareSubmeshDrawable::VBO_STRIDE_BYTES);
         hardwareModel->setMatrixIndexBuffer(reinterpret_cast<char*>(tempVertexArray + HardwareSubmeshDrawable::VBO_OFFSET_BONE_INDEX), HardwareSubmeshDrawable::VBO_STRIDE_BYTES);
         hardwareModel->setTangentSpaceBuffer(0, reinterpret_cast<char*>(tempVertexArray + HardwareSubmeshDrawable::VBO_OFFSET_TANGENT_SPACE), HardwareSubmeshDrawable::VBO_STRIDE_BYTES);

         // Load the data into our arrays
         if (!hardwareModel->load(0, 0, modelData->GetShaderMaxBones()))
         {
            LOG_ERROR("Unable to create a hardware mesh:" + CalError::getLastErrorDescription());
         }

         InvertTextureCoordinates(hardwareModel, HardwareSubmeshDrawable::VBO_STRIDE, tempVertexArray, modelData, indexArray);

         // Move the data into the osg structure
         vertexArray->assign(tempVertexArray, tempVertexArray + hardwareModel->getTotalVertexCount() * HardwareSubmeshDrawable::VBO_STRIDE);

         delete[] tempVertexArray;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DLoader::InvertTextureCoordinates(CalHardwareModel* hardwareModel, const size_t stride,
      float* vboVertexAttr, Cal3DModelData* modelData, CalIndex*& indexArray)
   {
      const int numVerts = hardwareModel->getTotalVertexCount();
      //invert texture coordinates.
      for (unsigned i = 0; i < numVerts * stride; i += stride)
      {
         for (unsigned j = (HardwareSubmeshDrawable::VBO_OFFSET_BONE_INDEX+1); j < HardwareSubmeshDrawable::VBO_OFFSET_TANGENT_SPACE; ++j)
         {
            if (vboVertexAttr[i + j] > modelData->GetShaderMaxBones())
            {
               vboVertexAttr[i + j] = 0;
            }
         }

         vboVertexAttr[i + (HardwareSubmeshDrawable::VBO_OFFSET_TEXCOORD0 + 1)] = 1.0f - vboVertexAttr[i + (HardwareSubmeshDrawable::VBO_OFFSET_TEXCOORD0 + 1)]; //the odd texture coordinates in cal3d are flipped, not sure why
         vboVertexAttr[i + (HardwareSubmeshDrawable::VBO_OFFSET_TEXCOORD1 + 1)] = 1.0f - vboVertexAttr[i + (HardwareSubmeshDrawable::VBO_OFFSET_TEXCOORD1 + 1)]; //the odd texture coordinates in cal3d are flipped, not sure why
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
   unsigned int Cal3DLoader::GetMaxBoneID(CalCoreMesh& mesh)
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

   void Cal3DLoader::ReleaseGLObjects()
   {
       for (TextureMap::iterator i = mTextureCache.begin(); i != mTextureCache.end(); i++) {
           i->second->releaseGLObjects();
       }
   }

} // namespace dtAnim
