/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 MOVES Institute
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
 *
 * Erik Johnson
 * Bradley G Anderegg
 */

#include <dtAnim/characterfilehandler.h>
#include <dtAnim/characterfileelements.h>
#include <dtAnim/animatable.h>
#include <dtAnim/cal3dloader.h> // for Cal3dExtensionEnum
#include <dtAnim/constants.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <osgDB/FileNameUtils> // for getLowerCaseFileExtension



XERCES_CPP_NAMESPACE_USE;


namespace dtAnim
{
   typedef CharacterFileElements CFE;

   ////////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////////////////
   const std::string CharacterFileHandler::CHARACTER_XML_LOGGER("characterfilehandler.cpp");



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////

   CharacterFileHandler::AnimatableStruct::AnimatableStruct()
   : mStartDelay(0.0f)
   , mFadeIn(0.0f)
   , mFadeOut(0.0f)
   , mSpeed(1.0f)
   , mBaseWeight(1.0f)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   CharacterFileHandler::AnimatableOverrideStruct::AnimatableOverrideStruct()
   : mOverrideStartDelay(false)
   , mOverrideFadeIn(false)
   , mOverrideFadeOut(false)
   , mFollowsPrevious(false)
   , mCrossFade(0.0f)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   CharacterFileHandler::AnimationChannelStruct::AnimationChannelStruct()
   : mMaxDuration(0.0f)
   , mIsLooping(false)
   , mIsAction(false)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   CharacterFileHandler::AnimationSequenceStruct::AnimationSequenceStruct()
   {
      mData = new AnimStructContainer;
   }


   ////////////////////////////////////////////////////////////////////////////////
   CharacterFileHandler::AnimatableOverrideStructArray&
   CharacterFileHandler::AnimationSequenceStruct::GetChildren()
   {
      return mData->mChildren;
   }

   ////////////////////////////////////////////////////////////////////////////////
   CharacterFileHandler::CharacterFileHandler()
   : mName()
   , mAnimations()
   , mMorphAnimations()
   , mAttachmentPoints()
   , mMaterials()
   , mMeshes()
   , mShaderMaxBones(72)
   , mLODStartDistance(10.0)
   , mLODEndDistance(100.0)
   , mLODMaxVisibleDistance(500.0)
   , mFoundLODOptions(false)
   , mFoundScale(false)
   , mScale(1.0f)
   , mAnimationChannels()
   , mSkeletonFilename()
   , mInSkinningShader(false)
   , mInLOD(false)
   , mInScale(false)
   , mInChannel(false)
   , mInSequence(false)
   , mInSequenceChild(false)
   , mLogger(NULL)
   {
      mLogger = &dtUtil::Log::GetInstance(CHARACTER_XML_LOGGER);
   }

   ////////////////////////////////////////////////////////////////////////////////
   CharacterFileHandler::~CharacterFileHandler()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::startDocument()
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                  "Begin Parsing File");
      }
      mName.clear();
      mAnimations.clear();
      mMorphAnimations.clear();
      mAttachmentPoints.clear();
      mMaterials.clear();
      mMeshes.clear();
      mShaderGroup.clear();
      mShaderName.clear();
      mAnimationChannels.clear();
      mAnimationSequences.clear();
      mSkeletonFilename.clear();

      mFoundLODOptions = false;
      mFoundScale = false;

      mInSkinningShader = false;
      mInLOD = false;
      mInScale = false;
      mInChannel = false;
      mInSequence = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& CharacterFileHandler::GetCharacterSystemType() const
   {
      return mCharacterSystemType;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::endDocument()
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                  "End Parsing File");
      }

      // Determine the character system to be used for the loaded files.
      UpdateCharacterSystemType();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::HandleEventAttributes(const std::string& elementName,
            dtUtil::AttributeSearch::ResultMap& attrs)
   {
      AnimatableStruct* animatable = NULL;

      // Get the current animatable
      if (mInChannel)
      {
         if ( ! mAnimationChannels.empty())
         {
            animatable = &mAnimationChannels.back();
         }
      }
      else if (mInSequenceChild)
      {
         AnimationSequenceStruct* sequence
         = mAnimationSequences.empty() ? NULL : &mAnimationSequences.back();

         if (sequence != NULL && ! sequence->GetChildren().empty())
         {
            animatable = &sequence->GetChildren().back();
         }
      }
      else if (mInSequence)
      {
         if ( ! mAnimationSequences.empty())
         {
            animatable = &mAnimationSequences.back();
         }
      }

      // Handle the attributes.
      if (animatable != NULL)
      {
         dtUtil::AttributeSearch::ResultMap::iterator iter = attrs.find(CFE::EVENT_ATTR);

         if (iter != attrs.end())
         {
            std::string eventName = iter->second;

            if (elementName == CFE::EVENT_ON_START_ELEMENT)
            {
               animatable->mEventTimeMap.insert(std::make_pair(eventName, 0.0f));
            }
            else if (elementName == CFE::EVENT_ON_END_ELEMENT)
            {
               animatable->mEventTimeMap.insert(std::make_pair(eventName, Animatable::INFINITE_TIME));
            }
            else if (elementName == CFE::EVENT_ON_TIME_ELEMENT)
            {
               iter = attrs.find(CFE::OFFSET_ATTR);
               float offset = iter == attrs.end() ? 0.0f : dtUtil::ToFloat(iter->second);
               animatable->mEventTimeMap.insert(std::make_pair(eventName, offset));
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::startElement( const XMLCh* const uri,const XMLCh* const localname,
            const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs )
   {
      dtCore::BaseXMLHandler::startElement(uri, localname, qname, attrs);

      dtUtil::XMLStringConverter elementName(localname);
      std::string elementStr = elementName.ToString();

      dtUtil::AttributeSearch search;
      dtUtil::AttributeSearch::ResultMap results;
      dtUtil::AttributeSearch::ResultMap::iterator resultIter;
      results = search(attrs);

      std::string errorString;

      if (elementStr == CFE::CHARACTER_ELEMENT)
      {
         resultIter = results.find(CFE::NAME_ELEMENT);

         if (resultIter != results.end())
         {
            mName = resultIter->second;
         }
         else
         {
            errorString = std::string("Invalid XML format: <character> missing <name> child");
         }
      }
      else if (elementStr == CFE::MIXED_RESOURCE_ELEMENT)
      {
         resultIter = results.find(CFE::FILENAME_ELEMENT);

         if (resultIter != results.end())
         {
            std::string filename = resultIter->second;

            //default the name of the animation to be the filename
            std::string name = resultIter->second;

            resultIter = results.find(CFE::NAME_ELEMENT);
            if (resultIter != results.end() )
            {
               name = resultIter->second;
            }
            MixedResourceStruct mixedRes;
            mixedRes.mFileName = filename;
            mixedRes.mName = name;
            mMixedResources.push_back(mixedRes);
         }
         else
         {
            errorString = std::string("Invalid XML format: <mixedresource> missing <filename> child");
         }
      }
      else if (elementStr == CFE::SKELETON_ELEMENT)
      {
         resultIter = results.find(CFE::FILENAME_ELEMENT);

         if (resultIter != results.end())
         {
            mSkeletonFilename = resultIter->second;
         }
         else
         {
            errorString = std::string("Invalid XML format: <character> missing <name> child");
         }
      }
      else if (elementStr == CFE::POSEMESH_ELEMENT)
      {
         resultIter = results.find(CFE::FILENAME_ELEMENT);

         if (resultIter != results.end())
         {
            mPoseMeshFilename = resultIter->second;
         }
         else
         {
            errorString = std::string("Invalid XML format: <poseMesh> missing 'fileName' attribute");
         }
      }
      else if (elementStr == CFE::ANIMATION_ELEMENT)
      {
         resultIter = results.find(CFE::FILENAME_ELEMENT);

         if (resultIter != results.end())
         {
            std::string filename = resultIter->second;

            //default the name of the animation to be the filename
            std::string name = resultIter->second;

            resultIter = results.find(CFE::NAME_ELEMENT);
            if (resultIter != results.end() )
            {
               name = resultIter->second;
            }
            AnimationStruct anim;
            anim.mFileName = filename;
            anim.mName = name;
            mAnimations.push_back(anim);
         }
         else
         {
            errorString = std::string("Invalid XML format: <animation> missing <filename> child");
         }
      }
      else if (elementStr == CFE::MORPH_ANIMATION_ELEMENT)
      {
         resultIter = results.find(CFE::FILENAME_ELEMENT);

         if (resultIter != results.end())
         {
            std::string filename = resultIter->second;

            //default the name of the animation to be the filename
            std::string name = resultIter->second;
            resultIter = results.find(CFE::NAME_ELEMENT);
            if (resultIter != results.end() )
            {
               name = resultIter->second;
            }
            MorphAnimationStruct morph;
            morph.mFileName = filename;
            morph.mName = name;
            mMorphAnimations.push_back(morph);
         }
         else
         {
            errorString = std::string("Invalid XML format: <morph> missing <filename> child");
         }
      }
      else if (elementStr == CFE::ATTACHMENT_POINT_ELEMENT)
      {
         dtUtil::HotSpotDefinition hotspot;
         std::string resource;

         resultIter = results.find(CFE::RESOURCE_ATTR);

         if (resultIter != results.end())
         {
            resource = resultIter->second;
         }

         resultIter = results.find(CFE::OFFSET_X_ATTR);

         if (resultIter != results.end())
         {
            hotspot.mLocalTranslation.x() = dtUtil::ToType<float>(resultIter->second);
         }

         resultIter = results.find(CFE::OFFSET_Y_ATTR);

         if (resultIter != results.end())
         {
            hotspot.mLocalTranslation.y() = dtUtil::ToType<float>(resultIter->second);
         }

         resultIter = results.find(CFE::OFFSET_Z_ATTR);

         if (resultIter != results.end())
         {
            hotspot.mLocalTranslation.z() = dtUtil::ToType<float>(resultIter->second);
         }

         float rx = 0.0, ry = 0.0, rz = 0.0;

         resultIter = results.find(CFE::OFFSET_RX_ATTR);
         if (resultIter != results.end())
         {
            rx = dtUtil::ToType<float>(resultIter->second);
         }

         resultIter = results.find(CFE::OFFSET_RY_ATTR);
         if (resultIter != results.end())
         {
            ry = dtUtil::ToType<float>(resultIter->second);
         }

         resultIter = results.find(CFE::OFFSET_RZ_ATTR);
         if (resultIter != results.end())
         {
            rz = dtUtil::ToType<float>(resultIter->second);
         }

         hotspot.mLocalRotation.makeRotate(osg::DegreesToRadians(rx), osg::Vec3(1.0, 0.0, 0.0),
                              osg::DegreesToRadians(ry), osg::Vec3(0.0, 1.0, 0.0),
                              osg::DegreesToRadians(rz), osg::Vec3(0.0, 0.0, 1.0));

         resultIter = results.find(CFE::BONE_NAME_ATTR);

         if (resultIter != results.end())
         {
            hotspot.mParentName = resultIter->second;

            // reuse this element name for the attribute to keep the strings consistent.
            resultIter = results.find(CFE::NAME_ELEMENT);
            if (resultIter != results.end())
            {
               hotspot.mName = resultIter->second;
            }
            else
            {
               hotspot.mName = hotspot.mParentName;
            }


            mAttachmentPoints.push_back(std::make_pair(hotspot, resource));
         }
         else
         {
            errorString = std::string("Invalid XML format: <")
            + CFE::ATTACHMENT_POINT_ELEMENT +  "> missing <" + CFE::BONE_NAME_ATTR + "> attribute";
         }

      }
      else if (elementStr == CFE::MESH_ELEMENT)
      {
         resultIter = results.find(CFE::FILENAME_ELEMENT);

         if (resultIter != results.end())
         {
            std::string filename = resultIter->second;

            //default the mesh name to be the same as the filename
            std::string meshName = filename;

            resultIter = results.find(CFE::NAME_ELEMENT);
            if (resultIter != results.end() )
            {
               meshName = resultIter->second;
            }

            MeshStruct mesh;
            mesh.mFileName = filename;
            mesh.mName = meshName;

            mMeshes.push_back(mesh);
         }
         else
         {
            errorString = std::string("Invalid XML format: <mesh> missing <filename> child");
         }
      }
      else if (elementStr == CFE::MATERIAL_ELEMENT)
      {
         resultIter = results.find(CFE::FILENAME_ELEMENT);

         if (resultIter != results.end())
         {
            std::string filename = resultIter->second;

            //default the material name to be the same as the filename
            std::string matName = filename;

            resultIter = results.find(CFE::NAME_ELEMENT);
            if (resultIter != results.end() )
            {
               matName = resultIter->second;
            }

            MaterialStruct mat;
            mat.mFileName = filename;
            mat.mName = matName;

            mMaterials.push_back(mat);
         }
         else
         {
            errorString = std::string("Invalid XML format: <material> missing <filename> child");
         }
      }
      else if (elementStr == CFE::SKINNING_SHADER_ELEMENT)
      {
         mInSkinningShader = true;
      }
      else if (elementStr == CFE::LOD_ELEMENT)
      {
         mInLOD = true;
         mFoundLODOptions = true;
      }
      else if (elementStr == CFE::SCALE_ELEMENT)
      {
         mInScale = true;
         mFoundScale = true;
      }
      else if (elementStr == CFE::CHANNEL_ELEMENT)
      {
         mInChannel = true;
         mAnimationChannels.push_back(AnimationChannelStruct());
      }
      else if (elementStr == CFE::SEQUENCE_ELEMENT)
      {
         mInSequence = true;
         mAnimationSequences.push_back(AnimationSequenceStruct());
      }
      else if(elementStr == CFE::CHILD_ELEMENT)
      {
         mInSequenceChild = true;

         if (!mAnimationSequences.empty())
         {
            AnimatableOverrideStruct aos;
            mAnimationSequences.back().GetChildren().push_back(aos);
         }
      }
      else if (elementStr == CFE::EVENT_ON_START_ELEMENT
               || elementStr == CFE::EVENT_ON_TIME_ELEMENT
               || elementStr == CFE::EVENT_ON_END_ELEMENT)
      {
         HandleEventAttributes(elementStr, results);
      }

      if (!errorString.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__, errorString);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::endElement(const XMLCh* const uri,
            const XMLCh* const localname,
            const XMLCh* const qname)
   {
      if (mElements.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Attempting to pop elements off of stack and the stack is empty."
                  "it should at least contain element %s.",
                  dtUtil::XMLStringConverter(localname).c_str());
         return;
      }

      const char* lname = dtUtil::XMLStringConverter(mElements.top().c_str()).ToString().c_str();

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                  "Ending element: \"%s\"", lname);
      }

      dtCore::BaseXMLHandler::endElement(uri, localname, qname);

      std::string elementStr = dtUtil::XMLStringConverter(localname).ToString();

      if (elementStr == CFE::SKINNING_SHADER_ELEMENT)
      {
         mInSkinningShader = false;
      }
      else if (elementStr == CFE::LOD_ELEMENT)
      {
         mInLOD = false;
      }
      else if (elementStr == CFE::SCALE_ELEMENT)
      {
         mInScale = false;
      }
      else if (elementStr == CFE::CHANNEL_ELEMENT)
      {
         mInChannel = false;
      }
      else if (elementStr == CFE::SEQUENCE_ELEMENT)
      {
         mInSequence = false;
      }
      else if(elementStr == CFE::CHILD_ELEMENT)
      {
         mInSequenceChild = false;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::CombinedCharacters(const XMLCh* const chars, size_t length)
   {
      if (mElements.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Characters should not be found outside of an element: \"%s\"",
                  dtUtil::XMLStringConverter(chars).c_str());
         return;
      }

      if (mInSkinningShader)
      {
         SkinningShaderCharacters(chars);
      }
      else if (mInLOD)
      {
         LODCharacters(chars);
      }
      else if (mInScale)
      {
         ScaleCharacters(chars);
      }
      else if (mInChannel)
      {
         AnimChannelCharacters(chars);
      }
      else if (mInSequenceChild)
      {
         AnimSequenceChildCharacters(chars);
      }
      else if (mInSequence)
      {
         AnimSequenceCharacters(chars);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::SkinningShaderCharacters(const XMLCh* const chars)
   {
      const std::string& topEl = dtUtil::XMLStringConverter(mElements.top().c_str()).ToString();
      if (topEl == CFE::SHADER_GROUP_ELEMENT)
      {
         mShaderGroup = dtUtil::XMLStringConverter(chars).ToString();
      }
      else if (topEl == CFE::SHADER_NAME_ELEMENT)
      {
         mShaderName = dtUtil::XMLStringConverter(chars).ToString();
      }
      else if (topEl == CFE::SHADER_MAX_BONES_ELEMENT)
      {
         mShaderMaxBones = dtUtil::ToUnsignedInt(dtUtil::XMLStringConverter(chars).ToString());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::LODCharacters(const XMLCh* const chars)
   {
      const std::string& topEl = dtUtil::XMLStringConverter(mElements.top().c_str()).ToString();

      double value = dtUtil::ToType<double>(dtUtil::XMLStringConverter(chars).ToString());

      if (topEl == CFE::LOD_START_DISTANCE_ELEMENT)
      {
         mLODStartDistance = value;
      }
      else if (topEl == CFE::LOD_END_DISTANCE_ELEMENT)
      {
         mLODEndDistance = value;
      }
      else if (topEl == CFE::MAX_VISIBLE_DISTANCE_ELEMENT)
      {
         mLODMaxVisibleDistance = value;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::ScaleCharacters(const XMLCh* const chars)
   {
      const std::string& topEl = dtUtil::XMLStringConverter(mElements.top().c_str()).ToString();
      if (topEl == CFE::SCALE_FACTOR_ELEMENT)
      {
         mScale = dtUtil::ToType<float>(dtUtil::XMLStringConverter(chars).ToString());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool CharacterFileHandler::AnimatableCharacters(const XMLCh* const chars, AnimatableStruct& animatable)
   {
      bool result = true;

      const std::string& topEl = dtUtil::XMLStringConverter(mElements.top().c_str()).ToString();

      if (topEl == CFE::NAME_ELEMENT)
      {
         animatable.mName = dtUtil::XMLStringConverter(chars).ToString();
      }
      else if (topEl == CFE::START_DELAY_ELEMENT)
      {
         std::string delay = dtUtil::XMLStringConverter(chars).ToString();
         animatable.mStartDelay = dtUtil::ToType<float>(delay);
      }
      else if (topEl == CFE::FADE_IN_ELEMENT)
      {
         std::string fade_in = dtUtil::XMLStringConverter(chars).ToString();
         animatable.mFadeIn = dtUtil::ToType<float>(fade_in);
      }
      else if (topEl == CFE::FADE_OUT_ELEMENT)
      {
         std::string fade_out = dtUtil::XMLStringConverter(chars).ToString();
         animatable.mFadeOut = dtUtil::ToType<float>(fade_out);
      }
      else if (topEl == CFE::SPEED_ELEMENT)
      {
         std::string speed = dtUtil::XMLStringConverter(chars).ToString();
         animatable.mSpeed = dtUtil::ToType<float>(speed);
      }
      else if (topEl == CFE::BASE_WEIGHT_ELEMENT)
      {
         std::string base_weight = dtUtil::XMLStringConverter(chars).ToString();
         animatable.mBaseWeight = dtUtil::ToType<float>(base_weight);
      }
      else
      {
         result = false;
      }

      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::AnimChannelCharacters(const XMLCh* const chars)
   {
      const std::string& topEl = dtUtil::XMLStringConverter(mElements.top().c_str()).ToString();
      AnimationChannelStruct& pChannel = mAnimationChannels.back();

      if (!AnimatableCharacters(chars, pChannel))
      {
         if (topEl == CFE::ANIMATION_NAME_ELEMENT)
         {
            pChannel.mAnimationName = dtUtil::XMLStringConverter(chars).ToString();
         }
         else if (topEl == CFE::MAX_DURATION_ELEMENT)
         {
            std::string max_duration = dtUtil::XMLStringConverter(chars).ToString();
            pChannel.mMaxDuration = dtUtil::ToType<float>(max_duration);
         }
         else if (topEl == CFE::IS_LOOPING_ELEMENT)
         {
            std::string is_looping = dtUtil::XMLStringConverter(chars).ToString();
            pChannel.mIsLooping = dtUtil::ToType<bool>(is_looping);
         }
         else if (topEl == CFE::IS_ACTION_ELEMENT)
         {
            std::string is_action = dtUtil::XMLStringConverter(chars).ToString();
            pChannel.mIsAction = dtUtil::ToType<bool>(is_action);
         }
         else if (topEl != CFE::CHANNEL_ELEMENT && topEl != CFE::CHILDREN_ELEMENT &&
                  topEl != CFE::EVENT_ON_START_ELEMENT && topEl != CFE::EVENT_ON_TIME_ELEMENT &&
                  topEl != CFE::EVENT_ON_END_ELEMENT)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                     "Found characters for unknown element \"%s\" \"%s\"",
                     topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());
         }
      }

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                  "Found characters for element \"%s\" \"%s\"", topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::AnimSequenceCharacters(const XMLCh* const chars)
   {
      const std::string& topEl = dtUtil::XMLStringConverter(mElements.top().c_str()).ToString();
      AnimationSequenceStruct& pSequence = mAnimationSequences.back();

      if (mElements.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Attempting to pop elements off of stack and the stack is empty."
                  "it should at least contain element %s.",
                  dtUtil::XMLStringConverter(chars).c_str());
         return;
      }

      if (!AnimatableCharacters(chars, pSequence))
      {
         if (topEl != CFE::SEQUENCE_ELEMENT && topEl != CFE::CHILDREN_ELEMENT &&
                  topEl != CFE::EVENT_ON_START_ELEMENT && topEl != CFE::EVENT_ON_TIME_ELEMENT &&
                  topEl != CFE::EVENT_ON_END_ELEMENT)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                     "Found characters for unknown element \"%s\" \"%s\"",
                     topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());
         }
      }

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                  "Found characters for element \"%s\" \"%s\"",
                  topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());
      }
   }


   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::AnimSequenceChildCharacters(const XMLCh* const chars)
   {
      const std::string& topEl = dtUtil::XMLStringConverter(mElements.top().c_str()).ToString();
      AnimationSequenceStruct& sequence = mAnimationSequences.back();
      AnimatableOverrideStruct* curStruct = NULL;

      if ( ! sequence.GetChildren().empty())
      {
         curStruct = &sequence.GetChildren().back();
      }

      if(curStruct != NULL)
      {
         std::string value(dtUtil::XMLStringConverter(chars).ToString());
         dtUtil::Trim(value);

         if (topEl == CFE::CHILD_ELEMENT)
         {
            // If valid text was entered, then this must be an older version
            // of the character XML.
            if (curStruct->mName.empty() && !value.empty())
            {
               curStruct->mName = value;
            }
         }
         else if (topEl == CFE::NAME_ELEMENT)
         {
            curStruct->mName = value;
         }
         else if (topEl == CFE::FOLLOWS_ELEMENT)
         {
            curStruct->mFollowAnimatableName = value;
         }
         else if (topEl == CFE::FOLLOW_PREV_ELEMENT)
         {
            curStruct->mFollowsPrevious = dtUtil::ToType<int>(value) > 0;
         }
         else if (topEl == CFE::START_DELAY_ELEMENT)
         {
            curStruct->mOverrideStartDelay = true;
            curStruct->mStartDelay = dtUtil::ToType<float>(value);
         }
         else if (topEl == CFE::FADE_IN_ELEMENT)
         {
            curStruct->mOverrideFadeIn = true;
            curStruct->mFadeIn = dtUtil::ToType<float>(value);
         }
         else if (topEl == CFE::FADE_OUT_ELEMENT)
         {
            curStruct->mOverrideFadeOut = true;
            curStruct->mFadeOut = dtUtil::ToType<float>(value);
         }
         else if (topEl == CFE::CROSS_FADE_ELEMENT)
         {
            curStruct->mCrossFade = dtUtil::ToType<float>(value);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharacterFileHandler::UpdateCharacterSystemType()
   {
      std::string ext = osgDB::getLowerCaseFileExtension(mSkeletonFilename);
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

      if (Cal3dExtensionEnum::GetValueForName(ext) != NULL)
      {
         mCharacterSystemType = Constants::CHARACTER_SYSTEM_CAL3D;
      }
      else // Assume OSG has plugins to load files.
      {
         mCharacterSystemType = Constants::CHARACTER_SYSTEM_OSG;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////

} // namespace dtAnim
