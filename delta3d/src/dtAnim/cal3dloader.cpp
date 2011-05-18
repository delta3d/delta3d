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
#include <dtAnim/animationwrapper.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/log.h>
#include <dtUtil/threadpool.h>

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
         std::string skelFile = dtUtil::FindFileInPathList(path + handler.mSkeletonFilename);
         if (!skelFile.empty())
         {
            coreModel->loadCoreSkeleton(dtUtil::FindFileInPathList(path + handler.mSkeletonFilename));
#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
            coreModel->getCoreSkeleton()->setName(handler.mSkeletonFilename);
#endif
         }
         else
         {
            LOG_ERROR("Can't find the skeleton file named:'" + path + handler.mSkeletonFilename + "'.");
         }

         // load meshes
         std::vector<CharacterFileHandler::MeshStruct>::iterator meshItr = handler.mMeshes.begin();
         while (meshItr != handler.mMeshes.end())
         {
            std::string filename = dtUtil::FindFileInPathList(path + (*meshItr).mFileName);
            if (!filename.empty())
            {
               // Load the mesh and get its id for further error checking
               int id = coreModel->loadCoreMesh(filename, (*meshItr).mName);

               if (id < 0)
               {
                  LOG_ERROR("Can't load mesh '" + filename +"':" + CalError::getLastErrorDescription());
               }
               else
               {
                  CalCoreMesh* mesh = coreModel->getCoreMesh(id);

                  // Make sure this mesh doesn't reference bones we don't have
                  if (GetMaxBoneID(*mesh) > coreModel->getCoreSkeleton()->getVectorCoreBone().size())
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
               std::string filename = dtUtil::FindFileInPathList(path + (*animItr).mFileName);

               if (!filename.empty())
               {
                  if (coreModel->loadCoreAnimation(filename, (*animItr).mName) < 0)
                  {
                     LOG_ERROR("Can't load animation '" + filename +"':" + CalError::getLastErrorDescription());
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
               std::string filename = dtUtil::FindFileInPathList(path + (*matItr).mFileName);

               if (filename.empty())
               {
                  LOG_ERROR("Can't find material file named:'" + path + (*matItr).mFileName + "'.");
               }
               else
               {
                  int matID = coreModel->loadCoreMaterial(filename, (*matItr).mName);
                  if (matID < 0)
                  {
                     LOG_ERROR("Material file failed to load:'" + path + (*matItr).mFileName + "'." + CalError::getLastErrorDescription());
                  }
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
   bool Cal3DLoader::Load(const std::string& filename, dtCore::RefPtr<Cal3DModelData>& data_in)
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
   void Cal3DLoader::LoadModelData(dtAnim::CharacterFileHandler& handler, CalCoreModel& model, Cal3DModelData& modelData)
   {
      // Redefine some types so that they are easier to read.
      typedef CharacterFileHandler::AnimationChannelStruct ChannelStruct;
      typedef CharacterFileHandler::ChannelStructArray ChannelStructArray;
      typedef CharacterFileHandler::AnimationSequenceStruct SequenceStruct;
      typedef CharacterFileHandler::SequenceStructArray SequenceStructArray;

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
               LOG_ERROR("Unable to find animation '" + pStruct.mAnimationName + "' within the CalCoreModel.");
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

} // namespace dtAnim
