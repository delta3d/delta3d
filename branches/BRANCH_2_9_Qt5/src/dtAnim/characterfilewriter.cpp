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
 * Chris Rodgers
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/characterfilewriter.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/basemodeldata.h>
#include <dtAnim/characterfileelements.h>
#include <dtUtil/exception.h>
#include <dtUtil/xercesutils.h>
#include <sstream>

//for snprintf
#include <dtUtil/mswinmacros.h>


namespace dtAnim
{
   typedef CharacterFileElements CFE;

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   CharacterFileWriter::CharacterFileWriter()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   CharacterFileWriter::~CharacterFileWriter()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   bool CharacterFileWriter::Write(const dtAnim::BaseModelData& modelData, std::ostream& stream)
   {
      bool success = false;

      // Set the base class' format target with the specified output stream.
      mFormatTarget.SetOutputStream(&stream);

      try
      {
         dtCore::AttributeMap attrs;
         if (!modelData.GetModelName().empty())
         {
            attrs.SetValue(CFE::NAME_ELEMENT, modelData.GetModelName());
         }

         BeginElement(CFE::CHARACTER_ELEMENT, attrs, false);

         WriteSkeleton(modelData);
         WriteAnimations(modelData);
         WriteMorphs(modelData);
         WriteMeshes(modelData);
         WriteMaterials(modelData);
         WriteLOD(modelData);
         WriteScale(modelData);
         WriteShader(modelData);
         WriteChannelsAndSequences(modelData);
         WritePoseMesh(modelData);

         EndElement();

         success = true;
      }
      catch (dtUtil::Exception& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Caught Exception \"%s\" while attempting to save character file \"%s\".",
            ex.What().c_str(), modelData.GetResource().GetResourceIdentifier().c_str());
         mFormatTarget.SetOutputStream(NULL);
      }
      catch (...)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Unknown exception while attempting to save character file \"%s\".",
            modelData.GetResource().GetResourceIdentifier().c_str());
         mFormatTarget.SetOutputStream(NULL);
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteFileTag(const dtAnim::BaseModelData& modelData, int fileType)
   {
      typedef dtAnim::StrArray::const_iterator FileNameIter;

      dtAnim::ModelResourceType calFileType = dtAnim::ModelResourceType(fileType);

      // Get all the object names mapped to the file.
      dtAnim::StrArray nameList;
      modelData.GetObjectNameListForFileTypeSorted(calFileType, nameList);
      FileNameIter curNameIter = nameList.begin();
      FileNameIter endNameIter = nameList.end();

      // For each object name, write a file tag of the specified type.
      for (; curNameIter != endNameIter; ++curNameIter)
      {
         // Set the file and object names as attributes on the tag.
         dtCore::AttributeMap attrs;
         std::string curFile(modelData.GetFileForObjectName(calFileType, *curNameIter));
         attrs.SetValue(CFE::FILENAME_ELEMENT, curFile);
         attrs.SetValue(CFE::NAME_ELEMENT, *curNameIter);

         // Get the appropriate tag name for the specified file type.
         const dtUtil::RefString* tag = NULL;
         switch (fileType)
         {
         case dtAnim::SKEL_FILE:
            tag = &CFE::SKELETON_ELEMENT;
            break;
         case dtAnim::MAT_FILE:
            tag = &CFE::MATERIAL_ELEMENT;
            break;
         case dtAnim::MESH_FILE:
            tag = &CFE::MESH_ELEMENT;
            break;
         case dtAnim::ANIM_FILE:
            tag = &CFE::ANIMATION_ELEMENT;
            break;
         case dtAnim::MORPH_FILE:
            tag = &CFE::MORPH_ANIMATION_ELEMENT;
            break;
         default:
            break;
         }

         if(tag != NULL)
         {
            BeginElement(*tag, attrs, true);
         }
         else
         {
            std::ostringstream oss;
            oss << "No appropriate tag for Cal3D file type [" << fileType
               << "] for file \"" << curFile << "\" mapped to object \""
               << *curNameIter << "\"\n" << std::endl;
            LOG_ERROR(oss.str());
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteSkeleton(const dtAnim::BaseModelData& modelData)
   {
      WriteFileTag(modelData, dtAnim::SKEL_FILE);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteMeshes(const dtAnim::BaseModelData& modelData)
   {
      WriteFileTag(modelData, dtAnim::MESH_FILE);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteMaterials(const dtAnim::BaseModelData& modelData)
   {
      WriteFileTag(modelData, dtAnim::MAT_FILE);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteAnimations(const dtAnim::BaseModelData& modelData)
   {
      WriteFileTag(modelData, dtAnim::ANIM_FILE);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteMorphs(const dtAnim::BaseModelData& modelData)
   {
      WriteFileTag(modelData, dtAnim::MORPH_FILE);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteShader(const dtAnim::BaseModelData& modelData)
   {
      BeginElement(CFE::SKINNING_SHADER_ELEMENT);
      {
         BeginElement(CFE::SHADER_GROUP_ELEMENT);
         Write(modelData.GetShaderGroupName());
         EndElement();

         BeginElement(CFE::SHADER_NAME_ELEMENT);
         Write(modelData.GetShaderName());
         EndElement();
      }
      EndElement();
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteScale(const dtAnim::BaseModelData& modelData)
   {
      BeginElement(CFE::SCALE_ELEMENT);
      {
         BeginElement(CFE::SCALE_FACTOR_ELEMENT);
         Write(modelData.GetScale());
         EndElement();
      }
      EndElement();
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteLOD(const dtAnim::BaseModelData& modelData)
   {
      const LODOptions& lodOptions = modelData.GetLODOptions();

      BeginElement(CFE::LOD_ELEMENT);
      {
         BeginElement(CFE::LOD_START_DISTANCE_ELEMENT);
         Write(lodOptions.GetStartDistance());
         EndElement();

         BeginElement(CFE::LOD_END_DISTANCE_ELEMENT);
         Write(lodOptions.GetEndDistance());
         EndElement();

         BeginElement(CFE::MAX_VISIBLE_DISTANCE_ELEMENT);
         Write(lodOptions.GetMaxVisibleDistance());
         EndElement();
      }
      EndElement();
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WritePoseMesh(const dtAnim::BaseModelData& modelData)
   {
      // TODO:
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteChannelsAndSequences(const dtAnim::BaseModelData& modelData)
   {
      const dtAnim::AnimatableArray& anims = modelData.GetAnimatables();

      typedef dtAnim::AnimatableArray::const_iterator AnimIter;
      const Animatable* curAnim = NULL;
      const AnimationChannel* curChannel = NULL;
      AnimIter curIter = anims.begin();
      AnimIter endIter = anims.end();
      for (; curIter != endIter; ++curIter)
      {
         curAnim = curIter->get();
         curChannel = dynamic_cast<const AnimationChannel*>(curAnim);

         if (curChannel != NULL)
         {
            WriteChannel(*curChannel);
         }
         else
         {
            const AnimationSequence* seq = dynamic_cast<const AnimationSequence*>(curAnim);
            WriteSequence(*seq);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteChannel(const AnimationChannel& channel)
   {
      BeginElement(CFE::CHANNEL_ELEMENT);
      {
         WriteAnimatable(channel, true);
       
         BeginElement(CFE::MAX_DURATION_ELEMENT);
         Write(channel.GetMaxDuration());
         EndElement();

         BeginElement(CFE::IS_ACTION_ELEMENT);
         Write(channel.IsAction());
         EndElement();

         BeginElement(CFE::IS_LOOPING_ELEMENT);
         Write(channel.IsLooping());
         EndElement();

         WriteAnimatableEvents(channel);
      }
      EndElement();
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteSequence(const AnimationSequence& sequence)
   {
      BeginElement(CFE::SEQUENCE_ELEMENT);
      {
         WriteAnimatable(sequence, false);

         const AnimationSequence::AnimationContainer& anims = sequence.GetChildAnimations();

         if( ! anims.empty())
         {
            BeginElement(CFE::CHILDREN_ELEMENT);
            {
               AnimationSequence::AnimationContainer::const_iterator curIter = anims.begin();
               AnimationSequence::AnimationContainer::const_iterator endIter = anims.end();
               for (; curIter != endIter; ++curIter)
               {
                  WriteChildAnimatable(*(curIter->get()));
               }

            }
            EndElement();
         }

         WriteAnimatableEvents(sequence);
      }
      EndElement();
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteChildAnimatable(const Animatable& anim)
   {
      BeginElement(CFE::CHILD_ELEMENT);
      {
         BeginElement(CFE::NAME_ELEMENT);
         Write(anim.GetName());
         EndElement();

         BeginElement(CFE::START_DELAY_ELEMENT);
         Write(anim.GetStartDelay());
         EndElement();

         BeginElement(CFE::FADE_IN_ELEMENT);
         Write(anim.GetFadeIn());
         EndElement();

         BeginElement(CFE::FADE_OUT_ELEMENT);
         Write(anim.GetFadeOut());
         EndElement();

         // TODO: Cross Fade

         // TODO: Follow Previous / Follows

         WriteAnimatableEvents(anim);
      }
      EndElement();
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteAnimatable(const Animatable& anim, bool isChannel)
   {
      BeginElement(CFE::NAME_ELEMENT);
      Write(anim.GetName());
      EndElement();

      if(isChannel)
      {
         const AnimationChannel* channel = static_cast<const AnimationChannel*>(&anim);

         BeginElement(CFE::ANIMATION_NAME_ELEMENT);
         Write(channel->GetName());
         EndElement();
      }

      BeginElement(CFE::START_DELAY_ELEMENT);
      Write(anim.GetStartDelay());
      EndElement();

      BeginElement(CFE::FADE_IN_ELEMENT);
      Write(anim.GetFadeIn());
      EndElement();

      BeginElement(CFE::FADE_OUT_ELEMENT);
      Write(anim.GetFadeOut());
      EndElement();

      BeginElement(CFE::SPEED_ELEMENT);
      Write(anim.GetSpeed());
      EndElement();

      BeginElement(CFE::BASE_WEIGHT_ELEMENT);
      Write(anim.GetBaseWeight());
      EndElement();
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::WriteAnimatableEvents(const Animatable& anim)
   {
      typedef Animatable::TimeEventMap::const_iterator TEIter;

      const float endTimeTolerance = 0.01f;
      float duration = anim.CalculateDuration();
      float curTime = 0.0f;
      const std::string* curEvent = NULL;

      const Animatable::TimeEventMap& timeEventMap = anim.GetTimeEventMap();
      TEIter curIter = timeEventMap.begin();
      TEIter endIter = timeEventMap.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curTime = curIter->second;
         curEvent = &curIter->first;

         dtCore::AttributeMap attrs;
         attrs.SetValue(CFE::EVENT_ATTR, *curEvent);

         // Start Event?
         if(curTime == 0.0f)
         {
            BeginElement(CFE::EVENT_ON_START_ELEMENT, attrs, true);
         }
         // End Event?
         else if(duration > 0.0f && curTime + endTimeTolerance >= duration)
         {
            BeginElement(CFE::EVENT_ON_END_ELEMENT, attrs, true);
         }
         // Somewhere-in-the-middle Event?
         else
         {
            std::ostringstream oss;
            oss << curTime;
            attrs.SetValue(CFE::OFFSET_ATTR, oss.str());
            BeginElement(CFE::EVENT_ON_TIME_ELEMENT, attrs, true);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::Write(bool b)
   {
      char buffer[2];
      snprintf(buffer, 2, "%d", (b?1:0));
      AddCharacters(buffer);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::Write(int n)
   {
      char buffer[80];
      snprintf(buffer, 80, "%d", n);
      AddCharacters(buffer);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::Write(float n)
   {
      char buffer[80];
      snprintf(buffer, 80, "%f", n);
      AddCharacters(buffer);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::Write(double n)
   {
      char buffer[80];
      snprintf(buffer, 80, "%f", n);
      AddCharacters(buffer);
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileWriter::Write(const std::string& s)
   {
      AddCharacters(s);
   }

} // namespace dtAnim
