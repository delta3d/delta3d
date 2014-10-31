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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/characterfileloader.h>
#include <dtAnim/characterfilewriter.h>
#include <dtAnim/animatable.h>
#include <dtAnim/basemodeldata.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationsequence.h>
#include <dtCore/basexmlhandler.h>
#include <dtCore/basexmlreaderwriter.h>
#include <dtCore/project.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/xercesparser.h>



namespace dtAnim
{
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
   class CharacterXMLReaderWriter : public dtCore::BaseXMLReaderWriter<BaseModelData, CharacterFileHandler, CharacterFileWriter>
   {
   public:

      typedef dtCore::BaseXMLReaderWriter<BaseModelData, CharacterFileHandler, CharacterFileWriter> BaseClass;

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

   REGISTER_OSGPLUGIN(dtchar, CharacterXMLReaderWriter);


   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<CharacterFileHandler> CharacterFileLoader::LoadCharacterFile(const dtCore::ResourceDescriptor& resource)
   {
      dtCore::RefPtr<CharacterFileHandler> result;
      try
      {
          std::string file = dtCore::Project::GetInstance().GetResourcePath(resource);
          result = LoadCharacterFile(file);
          if (result.valid())
          {
             result->mResource = resource;
          }
      }
      catch(const dtUtil::Exception& ex)
      {
         ex.LogException();
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString CharacterFileLoader::CHARACTER_FILE_EXTENSION("dtchar");

   dtCore::RefPtr<CharacterFileHandler> CharacterFileLoader::LoadCharacterFile(const std::string& file)
   {
      using namespace dtCore;

      std::string filename(file);

      std::string path(osgDB::getFilePath(filename));
      if (!path.empty()) { path += '/'; }
      else { path = "./"; }

      dtCore::RefPtr<CharacterFileHandler> handler;

      //gotta parse the file and create/store a new CalCoreModel
      dtUtil::XercesParser parser;

      // Parse the character file, subsequently from the associated OSG plug-in...
      dtCore::RefPtr<WrapperOSGCharFileObject> resultObj;
      osgDB::Registry* osgReg = osgDB::Registry::instance();
      const osgDB::ReaderWriter::Options* globalOptions = osgReg->getOptions();

      // ...if it is the older character format...
      std::string extension(osgDB::getLowerCaseFileExtension(filename));
      if (extension == "xml")
      {
         // ...get the plug-in directly...
         CharacterXMLReaderWriter* charPlugin = dynamic_cast<CharacterXMLReaderWriter*>
            (osgReg->getReaderWriterForExtension(CHARACTER_FILE_EXTENSION.Get()));

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

      return handler;
   }

   /////////////////////////////////////////////////////////////////////////////
   void CharacterFileLoader::CreateChannelsAndSequences(dtAnim::CharacterFileHandler& handler, dtAnim::BaseModelData& modelData)
   {
      // Redefine some types so that they are easier to read.
      typedef CharacterFileHandler::AnimationChannelStruct ChannelStruct;
      typedef CharacterFileHandler::ChannelStructArray ChannelStructArray;
      typedef CharacterFileHandler::AnimationSequenceStruct SequenceStruct;
      typedef CharacterFileHandler::SequenceStructArray SequenceStructArray;

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

            int id = modelData.GetIndexForObjectName(dtAnim::ANIM_FILE, pStruct.mAnimationName);
            if (id >= 0)
            {
               dtCore::RefPtr<AnimationChannel> pChannel = new AnimationChannel();
               
               pChannel->SetID(id);
               pChannel->SetDuration(modelData.GetAnimationDuration(pStruct.mAnimationName));

               pChannel->SetName(pStruct.mName);
               pChannel->SetAnimationName(pStruct.mAnimationName);
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

               modelData.Add(*pChannel);
            }
            else
            {
               LOG_ERROR("Unable to find animation '" + pStruct.mAnimationName +
                  "' within the CalCoreModel. (" + modelData.GetResource().GetResourceIdentifier() + ")");
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
            CharacterFileLoader::FinalizeSequenceInfo(pStruct, modelData.GetAnimatables());

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
               dtAnim::AnimatableArray::iterator animIter = modelData.GetAnimatables().begin();
               dtAnim::AnimatableArray::iterator animIterEnd = modelData.GetAnimatables().end();

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

            modelData.Add(*pSequence);

         }
      }
   }


      

   /////////////////////////////////////////////////////////////////////////////
   typedef CharacterFileHandler::AnimatableOverrideStruct OverrideStruct;
   typedef CharacterFileHandler::AnimatableOverrideStructArray OverrideStructArray;
   typedef CharacterFileHandler::AnimationSequenceStruct AnimationSequenceStruct;
   void CharacterFileLoader::SetAnimatableValues(Animatable& animatable, const OverrideStruct& info)
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
   OverrideStruct* CharacterFileLoader::GetPreviousOverrideStruct(const std::string& name,
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
   Animatable* CharacterFileLoader::GetAnimatable(const std::string& animName, dtAnim::AnimatableArray& animArray)
   {
      Animatable* anim = NULL;
      dtAnim::AnimatableArray::iterator curIter = animArray.begin();
      dtAnim::AnimatableArray::iterator endIter = animArray.end();
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
   float CharacterFileLoader::ResolveCrossFade(OverrideStruct& previous, OverrideStruct& current)
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
   void CharacterFileLoader::FinalizeSequenceInfo(AnimationSequenceStruct& sequenceStruct,
      dtAnim::AnimatableArray& animArray)
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
   void CharacterFileLoader::SetAnimatableEvents(Animatable& anim, const CharacterFileHandler::AnimatableStruct& info)
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

} // namespace dtAnim
