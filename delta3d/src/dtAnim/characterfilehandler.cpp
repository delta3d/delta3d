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
#include <dtUtil/xercesutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>


XERCES_CPP_NAMESPACE_USE;


namespace dtAnim
{

const std::string CharacterFileHandler::CHARACTER_XML_LOGGER("characterfilehandler.cpp");

const std::string CharacterFileHandler::CHARACTER_ELEMENT("character");
const std::string CharacterFileHandler::ANIMATION_ELEMENT("animation");
const std::string CharacterFileHandler::SKELETON_ELEMENT("skeleton");
const std::string CharacterFileHandler::MESH_ELEMENT("mesh");
const std::string CharacterFileHandler::MATERIAL_ELEMENT("material");
const std::string CharacterFileHandler::CHANNEL_ELEMENT("animationchannel");
const std::string CharacterFileHandler::SEQUENCE_ELEMENT("animationsequence");
const std::string CharacterFileHandler::NAME_ELEMENT("name");
const std::string CharacterFileHandler::FILENAME_ELEMENT("filename");
const std::string CharacterFileHandler::ANIMATION_NAME_ELEMENT("animationname");

const std::string CharacterFileHandler::START_DELAY_ELEMENT("startdelay");
const std::string CharacterFileHandler::FADE_IN_ELEMENT("fadein");
const std::string CharacterFileHandler::FADE_OUT_ELEMENT("fadeout");
const std::string CharacterFileHandler::SPEED_ELEMENT("speed");
const std::string CharacterFileHandler::BASE_WEIGHT_ELEMENT("baseweight");
const std::string CharacterFileHandler::MAX_DURATION_ELEMENT("maxduration");
const std::string CharacterFileHandler::IS_LOOPING_ELEMENT("islooping");
const std::string CharacterFileHandler::IS_ACTION_ELEMENT("isaction");
const std::string CharacterFileHandler::CHILD_ELEMENT("child");

CharacterFileHandler::AnimatableStruct::AnimatableStruct():
   mStartDelay(0.0f), 
   mFadeIn(0.0f), 
   mFadeOut(0.0f), 
   mSpeed(0.0f), 
   mBaseWeight(0.0f)
{
}

CharacterFileHandler::AnimationChannelStruct::AnimationChannelStruct():
   mMaxDuration(0.0f),
   mIsLooping(false), 
   mIsAction(false)
{
}

CharacterFileHandler::AnimationSequenceStruct::AnimationSequenceStruct()
{
}


CharacterFileHandler::CharacterFileHandler() 
: mName()
, mAnimations()
, mMaterials()
, mMeshes()
, mAnimationChannels()
, mSkeletonFilename()
, mInChannel(false)
, mLogger(0)
{
   mLogger = &dtUtil::Log::GetInstance(CHARACTER_XML_LOGGER);
}

CharacterFileHandler::~CharacterFileHandler()
{
}

void CharacterFileHandler::startDocument()
{   
   if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                           "Begin Parsing File");
   }
   mName.clear();
   mAnimations.clear();
   mMaterials.clear();
   mMeshes.clear();
   mAnimationChannels.clear();
   mAnimationSequences.clear();
   mSkeletonFilename.clear();

   mInChannel = false;
   mInSequence = false;
}

void CharacterFileHandler::endDocument()
{
   if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                           "End Parsing File");
   }
}


void CharacterFileHandler::startElement( const XMLCh* const uri,const XMLCh* const localname,
                                                 const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs )
{
   dtUtil::XMLStringConverter elementName(localname);
   std::string elementStr = elementName.ToString();

   dtUtil::AttributeSearch search;
   dtUtil::AttributeSearch::ResultMap results;
   dtUtil::AttributeSearch::ResultMap::iterator resultIter;
   results = search(attrs);

   std::string errorString;

   if (elementStr == CHARACTER_ELEMENT)
   {
      resultIter = results.find(NAME_ELEMENT);

      if (resultIter != results.end())
      {
          mName = resultIter->second;
      }
      else
      {
         errorString = std::string("Invalid XML format: <character> missing <name> child");
      }     
   }
   else if (elementStr == SKELETON_ELEMENT)
   {     
      resultIter = results.find(FILENAME_ELEMENT);

      if (resultIter != results.end())
      {
         mSkeletonFilename = resultIter->second;
      }
      else
      {
         errorString = std::string("Invalid XML format: <character> missing <name> child");
      }     
   }
   else if (elementStr == ANIMATION_ELEMENT)
   {      
      resultIter = results.find(FILENAME_ELEMENT);

      if (resultIter != results.end())
      {
         std::string filename = resultIter->second;
         
         //default the name of the animation to be the filename
         std::string name = resultIter->second; 

         resultIter = results.find(NAME_ELEMENT);
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
   else if (elementStr == MESH_ELEMENT)
   {     
      resultIter = results.find(FILENAME_ELEMENT);

      if (resultIter != results.end())
      {
         std::string filename = resultIter->second;

         //default the mesh name to be the same as the filename
         std::string meshName = filename;

         resultIter = results.find(NAME_ELEMENT);
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
   else if (elementStr == MATERIAL_ELEMENT)
   {      
      resultIter = results.find(FILENAME_ELEMENT);

      if (resultIter != results.end())
      {
         std::string filename = resultIter->second;

         //default the material name to be the same as the filename
         std::string matName = filename;

         resultIter = results.find(NAME_ELEMENT);
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
   else if (elementStr == CHANNEL_ELEMENT)
   {      
      mInChannel = true;
      mAnimationChannels.push_back(AnimationChannelStruct());
   }
   else if (elementStr == SEQUENCE_ELEMENT)
   {      
      mInSequence = true;
      mAnimationSequences.push_back(AnimationSequenceStruct());
   }

   if (!errorString.empty())
   {      
      mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__, errorString);
   }

   //push elemet
   mElements.push(elementStr);
}

void CharacterFileHandler::endElement(
   const XMLCh* const uri,
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

   const char* lname = mElements.top().c_str();

   if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                           "Ending element: \"%s\"", lname);
   }
 
   if(dtUtil::XMLStringConverter(localname).ToString() == CHANNEL_ELEMENT)
   { 
      mInChannel = false;
   }
   else if(dtUtil::XMLStringConverter(localname).ToString() == SEQUENCE_ELEMENT)
   { 
      mInSequence = false;
   }

   mElements.pop();
}



void CharacterFileHandler::characters(const XMLCh* const chars,
                                      const unsigned int length)
{
   if (mElements.empty())
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                           "Attempting to pop elements off of stack and the stack is empty."
                           "it should at least contain element %s.",
                           dtUtil::XMLStringConverter(chars).c_str());
      return;
   }

   if (mInChannel)
   {
      AnimChannelCharacters(chars);
   }
   else if (mInSequence)
   {
      AnimSequenceCharacters(chars);
   }
}

bool CharacterFileHandler::AnimatableCharacters(const XMLCh* const chars, AnimatableStruct& animatable)
{
   bool result = true;

   std::string& topEl = mElements.top();
   
   if(topEl == NAME_ELEMENT)
   {
      animatable.mName = dtUtil::XMLStringConverter(chars).ToString();              
   }
   else if(topEl == START_DELAY_ELEMENT)
   {
      std::string delay = dtUtil::XMLStringConverter(chars).ToString();                    
      animatable.mStartDelay = dtUtil::ToType<float>(delay);
   }
   else if(topEl == FADE_IN_ELEMENT)
   {
      std::string fade_in = dtUtil::XMLStringConverter(chars).ToString();                    
      animatable.mFadeIn = dtUtil::ToType<float>(fade_in);
   }
   else if(topEl == FADE_OUT_ELEMENT)
   {
      std::string fade_out = dtUtil::XMLStringConverter(chars).ToString();                    
      animatable.mFadeOut = dtUtil::ToType<float>(fade_out);
   }
   else if(topEl == SPEED_ELEMENT)
   {
      std::string speed = dtUtil::XMLStringConverter(chars).ToString();                    
      animatable.mSpeed = dtUtil::ToType<float>(speed);
   }
   else if(topEl == BASE_WEIGHT_ELEMENT)
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

void CharacterFileHandler::AnimChannelCharacters(const XMLCh* const chars)
{
   std::string& topEl = mElements.top();
   AnimationChannelStruct& pChannel = mAnimationChannels.back();

   if (mElements.empty())
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                           "Attempting to pop elements off of stack and the stack is empty."
                           "it should at least contain element %s.",
                           dtUtil::XMLStringConverter(chars).c_str());
      return;
   }

   if (AnimatableCharacters(chars, pChannel))
   {
   }
   else if(topEl == ANIMATION_NAME_ELEMENT)
   {
      pChannel.mAnimationName = dtUtil::XMLStringConverter(chars).ToString();              
   }
   else if(topEl == MAX_DURATION_ELEMENT)
   {
      std::string max_duration = dtUtil::XMLStringConverter(chars).ToString();                    
      pChannel.mMaxDuration = dtUtil::ToType<float>(max_duration);
   }
   else if(topEl == IS_LOOPING_ELEMENT)
   {
      std::string is_looping = dtUtil::XMLStringConverter(chars).ToString();                    
      pChannel.mIsLooping = dtUtil::ToType<bool>(is_looping);
   }
   else if(topEl == IS_ACTION_ELEMENT)
   {
      std::string is_action = dtUtil::XMLStringConverter(chars).ToString();                    
      pChannel.mIsAction = dtUtil::ToType<bool>(is_action);
   }
   else
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                           "Found characters for unknown element \"%s\" \"%s\"", 
                           topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());      
   }


   if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                           "Found characters for element \"%s\" \"%s\"", topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());
   }
}

void CharacterFileHandler::AnimSequenceCharacters(const XMLCh* const chars)
{
   std::string& topEl = mElements.top();
   AnimationSequenceStruct& pSequence = mAnimationSequences.back();

   if (mElements.empty())
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                           "Attempting to pop elements off of stack and the stack is empty."
                           "it should at least contain element %s.",
                           dtUtil::XMLStringConverter(chars).c_str());
      return;
   }

   if (AnimatableCharacters(chars, pSequence))
   {
   }
   else if(topEl == CHILD_ELEMENT)
   {
      std::string childName = dtUtil::XMLStringConverter(chars).ToString();                    
      pSequence.mChildNames.push_back(childName);
   }
   else
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                           "Found characters for unknown element \"%s\" \"%s\"", 
                           topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());      
   }


   if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                           "Found characters for element \"%s\" \"%s\"", 
                           topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());
   }
}

}//namespace dtAnim
