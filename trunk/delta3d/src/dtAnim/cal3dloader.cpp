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
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <cal3d/error.h>
#include <cal3d/model.h>
#include <cal3d/coremodel.h>
#include <cal3d/coreanimation.h>
#include <dtAnim/characterfilehandler.h>
#include <dtAnim/animationwrapper.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/log.h>
#include <dtCore/globals.h>

namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   Cal3DLoader::Cal3DLoader()
      : mTextures()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3DLoader::~Cal3DLoader()
   {
      PurgeAllCaches();
   }

   /////////////////////////////////////////////////////////////////////////////
   /**
    * @return Could return NULL if the file didn't load.
    * @throw SAXParseException if the file didn't parse correctly
    * @note Relies on the the "animationdefinition.xsd" schema file
    */
   CalCoreModel* Cal3DLoader::GetCoreModel(CharacterFileHandler& handler, const std::string& filename, const std::string& path)
   {
      using namespace dtCore;

      CalCoreModel* coreModel = NULL;

      //gotta parse the file and create/store a new CalCoreModel
      dtUtil::XercesParser parser;

      if (parser.Parse(filename, handler, "animationdefinition.xsd"))
      {
         coreModel = new CalCoreModel(handler.mName);

         //load skeleton
         std::string skelFile = FindFileInPathList(path + handler.mSkeletonFilename);
         if (!skelFile.empty())
         {
            coreModel->loadCoreSkeleton(FindFileInPathList(path + handler.mSkeletonFilename));
         }
         else
         {
            LOG_ERROR("Can't find the skeleton file named:'" + path + handler.mSkeletonFilename + "'.");
         }

         //load animations
         std::vector<CharacterFileHandler::AnimationStruct>::iterator animItr = handler.mAnimations.begin();
         while (animItr != handler.mAnimations.end())
         {
            std::string filename = FindFileInPathList(path + (*animItr).mFileName);

            if (!filename.empty())
            {
               coreModel->loadCoreAnimation(filename, (*animItr).mName);
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
            std::string filename = FindFileInPathList(path + (*morphAnimItr).mFileName);

            if (!filename.empty())
            {
               coreModel->loadCoreAnimatedMorph(filename);
            }
            else
            {
               LOG_ERROR("Can't find morph animation file named:'" + path + (*morphAnimItr).mFileName + "'.");
            }
            ++morphAnimItr;
         }
#endif

         // load meshes
         std::vector<CharacterFileHandler::MeshStruct>::iterator meshItr = handler.mMeshes.begin();
         while (meshItr != handler.mMeshes.end())
         {
            std::string filename = FindFileInPathList(path + (*meshItr).mFileName);
            if (!filename.empty())
            {
               coreModel->loadCoreMesh(filename, (*meshItr).mName);
            }
            else
            {
               LOG_ERROR("Can't find mesh file named:'" + path + (*meshItr).mFileName + "'.");
            }
            ++meshItr;
         }

         // load materials
         for (std::vector<CharacterFileHandler::MaterialStruct>::iterator matItr = handler.mMaterials.begin();
              matItr != handler.mMaterials.end();
              ++matItr)
         {
            std::string filename = FindFileInPathList(path + (*matItr).mFileName);

            if (filename.empty())
            {
               LOG_ERROR("Can't find material file named:'" + path + (*matItr).mFileName + "'.");
            }
            else
            {
               int matID = coreModel->loadCoreMaterial(filename, (*matItr).mName);
               if (matID < 0)
               {
                  LOG_ERROR("Material file failed to load:'" + path + (*matItr).mFileName + "'.");
               }
            }
         }
      }

      return coreModel;
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
   bool Cal3DLoader::Load(const std::string& filename, Cal3DModelData*& data_in)
   {
      std::string path;
      std::string::size_type stringIndex = filename.find_last_of("\\");
      std::string::size_type lastIndex = filename.find_last_of("/");

      // lets take the bigger of the two that isnt equal to npos
      if (lastIndex != std::string::npos)
      {
         if (stringIndex != std::string::npos) stringIndex = (stringIndex > lastIndex) ? stringIndex : lastIndex;
         else stringIndex = lastIndex;
      }


      if (stringIndex != std::string::npos)
      {
         // The index is the position of the first backslash, so add 1
         path = filename.substr(0, stringIndex + 1);
      }

      CalCoreModel* coreModel = NULL;

      CharacterFileHandler handler;
      coreModel = GetCoreModel(handler, filename, path);
      if (coreModel != NULL)
      {
         data_in = new Cal3DModelData(coreModel, filename);
         LoadModelData(handler, *coreModel, *data_in);
         LoadAllTextures(*coreModel, path); //this should be a user-level process.

         // Store the filename containing IK data if it exists
         if (!handler.mPoseMeshFilename.empty())
         {
            data_in->SetPoseMeshFilename(path + handler.mPoseMeshFilename);
         }
      }
      else
      {
         LOG_ERROR("Unable to load character file: '" + filename + "'");
         return false;
      }

      // todo remove this if hardware isn't being used
      LoadHardwareData(data_in);
      
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DLoader::LoadAsynchronously(const std::string& filename, LoadCompletionCallback loadCallback)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mFileQueueMutex);
      
      mAsynchFilesToLoad.push(filename);
      mAsynchCompletionCallbacks.push(loadCallback);

      // Kick the thread off if it's not already running
      if (!isRunning())
      {
         start();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DLoader::LoadModelData(dtAnim::CharacterFileHandler& handler, CalCoreModel& model, Cal3DModelData& modelData)
   {
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
            pWrapper->SetSpeed(float(anim->getTotalNumberOfKeyframes()) / anim->getDuration());
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

      if (handler.mFoundLODOptions)
      {
         lodOptions.SetStartDistance(handler.mLODStartDistance);
         lodOptions.SetEndDistance(handler.mLODEndDistance);
         lodOptions.SetMaxVisibleDistance(handler.mLODMaxVisibleDistance);
      }

      if (handler.mFoundScale && modelData.GetCoreModel() != NULL)
      {
         modelData.GetCoreModel()->scale(handler.mScale);
      }

      // register animations
      if (!handler.mAnimationChannels.empty())
      {
         int numAnimatables = handler.mAnimationChannels.size() + handler.mAnimationSequences.size();
         modelData.GetAnimatables().reserve(numAnimatables);

         std::vector<CharacterFileHandler::AnimationChannelStruct>::iterator channelIter = handler.mAnimationChannels.begin();
         std::vector<CharacterFileHandler::AnimationChannelStruct>::iterator channelEnd = handler.mAnimationChannels.end();
         for (;channelIter != channelEnd; ++channelIter)
         {
            CharacterFileHandler::AnimationChannelStruct& pStruct = *channelIter;

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

               modelData.Add(pChannel.get());
            }
            else
            {
               LOG_ERROR("Unable to find animation '" + pStruct.mAnimationName + "' within the CalCoreModel.");
            }

         }

         std::vector<CharacterFileHandler::AnimationSequenceStruct>::iterator sequenceIter = handler.mAnimationSequences.begin();
         std::vector<CharacterFileHandler::AnimationSequenceStruct>::iterator sequenceEnd = handler.mAnimationSequences.end();
         for (;sequenceIter != sequenceEnd; ++sequenceIter)
         {
            CharacterFileHandler::AnimationSequenceStruct& pStruct = *sequenceIter;

            dtCore::RefPtr<AnimationSequence> pSequence = new AnimationSequence();

            pSequence->SetName(pStruct.mName);
            pSequence->SetStartDelay(pStruct.mStartDelay);
            pSequence->SetFadeIn(pStruct.mFadeIn);
            pSequence->SetFadeOut(pStruct.mFadeOut);
            pSequence->SetSpeed(pStruct.mSpeed);
            pSequence->SetBaseWeight(pStruct.mBaseWeight);

            // find children
            std::vector<std::string>::const_iterator i = pStruct.mChildNames.begin();
            std::vector<std::string>::const_iterator end = pStruct.mChildNames.end();
            for (; i != end; ++i)
            {
               const std::string& nameToFind = *i;
               Cal3DModelData::AnimatableArray::iterator animIter = modelData.GetAnimatables().begin();
               Cal3DModelData::AnimatableArray::iterator animIterEnd = modelData.GetAnimatables().end();

               for (; animIter != animIterEnd; ++animIter)
               {
                  Animatable* animatable = animIter->get();
                  if (animatable->GetName() == nameToFind)
                  {
                     pSequence->GetChildAnimations().push_back(animatable);
                  }
               }
            }

            modelData.Add(pSequence.get());

         }
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

            TextureMap::iterator textureIterator = mTextures.find(strFilename);

            if (textureIterator == mTextures.end())
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
               mTextures[strFilename] = texture;

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
      mTextures.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DLoader::run()
   {
      std::string currentFile;

      while (1) 
      {
         // Get the next file to load or quit
         {
            OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mFileQueueMutex);
            if (mAsynchFilesToLoad.empty())
            {
               break;
            }

            currentFile = mAsynchFilesToLoad.front();
            mAsynchFilesToLoad.pop();
         }

         Cal3DModelData* loadedData = NULL;

         Load(currentFile, loadedData);

         LoadCompletionCallback completionCallback = mAsynchCompletionCallbacks.front();
         mAsynchCompletionCallbacks.pop();

         // Return the loaded data via a callback
         completionCallback(loadedData);
      } 
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
            int submeshCount = calMesh->getCoreSubmeshCount();

            for (int submeshId = 0; submeshId < submeshCount; submeshId++)
            {
               CalCoreSubmesh* subMesh = calMesh->getCoreSubmesh(submeshId);
               numVerts += subMesh->getVertexCount();
               numIndices += 3 * subMesh->getFaceCount();
            }
         }
      }

      const size_t stride = 18;
      const size_t strideBytes = stride * sizeof(float);

      // Allocate data arrays for the hardware model to populate
      modelData->CreateSourceArrays(numVerts, numIndices, stride);

      float* tempVertexArray = new float[stride * numVerts];

      // Get the pointer and fill in the data
      osg::FloatArray* vertexArray = modelData->GetSourceVertexArray();
      CalIndex* indexArray = modelData->GetSourceIndexArray();

      hardwareModel->setIndexBuffer(indexArray);

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

      InvertTextureCoordinates(hardwareModel, stride, tempVertexArray, modelData, indexArray);

      // Move the data into the osg structure
      vertexArray->assign(tempVertexArray, tempVertexArray + numVerts * stride);

      delete[] tempVertexArray;
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DLoader::InvertTextureCoordinates(CalHardwareModel* hardwareModel, const size_t stride,
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

} // namespace dtAnim
