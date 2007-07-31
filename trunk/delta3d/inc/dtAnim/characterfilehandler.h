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
#ifndef DELTA_CHARACTER_FILE_HANDLER
#define DELTA_CHARACTER_FILE_HANDLER

#include <dtAnim/export.h>
#include <dtUtil/macros.h>
#include <dtCore/refptr.h>
#include <xercesc/sax2/ContentHandler.hpp>  // for a base class
#include <vector>
#include <string>
#include <stack>

namespace dtUtil
{
   class Log;
}

namespace dtAnim
{
   /** Simple Xerces XML handler that will store the data read from a character
     * definition .xml file.
     * into a series of containers of strings.
     * Usage:
     * @code
     *  dtUtil::XercesParser parse;
     *  dtAnim::CharacterHandler handler;
     *  parser.Parse(filename, handler);
     *
     *  std::string skeletonFilename = handler.mSkeletonFilename;
     *  ...
     * @endcode
     * @code
     *   <character>
     *      <skeleton filename="skel.csf" />
     *      <animation filename="anim1.xaf" />
     *      <animation ...
     *      <mesh filename="mesh1.cmf" />
     *      <mesh ...
     *      <material filename="mat1.crf" />
     *      <material ...
     *	   <animationchannel>
	  *         <name>Run</name>
	  *	      <animationname>Run</animationname>
	  *         <startdelay>0.0</startdelay>
	  *         <fadein>0.0</fadein>
	  *         <fadeout>0.0</fadeout>
	  *         <speed>1.0</speed>
     *         <baseweight>1.0</baseweight>
	  *         <maxduration>0.0</maxduration>
	  *	      <isaction>0</isaction>
	  *         <islooping>1</islooping>
	  *      </animationchannel>
     *   </character>
     * @endcode
     */
   class DT_ANIM_EXPORT CharacterFileHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler
   {
   public:

      //Logger
      static const std::string CHARACTER_XML_LOGGER;

      //parsing constants
      static const std::string CHARACTER_ELEMENT;
      static const std::string ANIMATION_ELEMENT;
      static const std::string SKELETON_ELEMENT;
      static const std::string MESH_ELEMENT;
      static const std::string MATERIAL_ELEMENT;
      static const std::string CHANNEL_ELEMENT;
      static const std::string SEQUENCE_ELEMENT;
      static const std::string NAME_ELEMENT;
      static const std::string FILENAME_ELEMENT;      
      static const std::string ANIMATION_NAME_ELEMENT;
      static const std::string START_DELAY_ELEMENT;
      static const std::string FADE_IN_ELEMENT;
      static const std::string FADE_OUT_ELEMENT;
      static const std::string SPEED_ELEMENT;
      static const std::string BASE_WEIGHT_ELEMENT;
      static const std::string MAX_DURATION_ELEMENT;
      static const std::string IS_LOOPING_ELEMENT;
      static const std::string IS_ACTION_ELEMENT;
      static const std::string CHILD_ELEMENT;
      
      static const std::string SKINNING_SHADER_ELEMENT;
      static const std::string SHADER_GROUP_ELEMENT;
      static const std::string SHADER_NAME_ELEMENT;

   public:
   	CharacterFileHandler();
   	~CharacterFileHandler();

      virtual void characters(const XMLCh* const chars, const unsigned int length);
      virtual void endDocument();
      virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {};
      virtual void processingInstruction(const XMLCh* const target, const XMLCh* const data) {};
      virtual void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator) {};
      virtual void startDocument();

      virtual void startElement(const XMLCh* const uri,const XMLCh* const localname,
                                const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);

      virtual void endElement(const XMLCh* const uri,
                              const XMLCh* const localname,
                              const XMLCh* const qname);

      virtual void startPrefixMapping(const	XMLCh* const prefix,const XMLCh* const uri) {};
      virtual void endPrefixMapping(const XMLCh* const prefix) {};
      virtual void skippedEntity(const XMLCh* const name) {};


#ifdef DELTA_WIN32
      //need these definitions to properly export a std::vector<std::string>
      template class DT_ANIM_EXPORT std::allocator<std::string>;
      template class DT_ANIM_EXPORT std::vector<std::string>;
#endif

      ///structure to contain all info related to an animation
      struct AnimationStruct 
      {
         std::string mFileName; ///<The filename of the cal3D animation
         std::string mName;     ///<The user friendly name of this animation
      };

      ///structure to contain all info related to a mesh
      struct MeshStruct
      {
         std::string mFileName; ///<The filename of the Cal3D mesh
         std::string mName;     ///<The user friendly name of this mesh
      };

      struct MaterialStruct
      {
         std::string mFileName; ///<The filename of the Cal3D material
         std::string mName;     ///<The user friendly name of this material
      };

      struct AnimatableStruct
      {
         AnimatableStruct();

         std::string mName;     ///<The name of this animation channel
         float mStartDelay, mFadeIn, mFadeOut, mSpeed, mBaseWeight;
      };

      struct AnimationChannelStruct : public AnimatableStruct
      {
         AnimationChannelStruct();

         std::string mAnimationName;     ///<The name of the animation this references
         float mMaxDuration;
         bool mIsLooping, mIsAction;
      };

      struct AnimationSequenceStruct : public AnimatableStruct 
      {
         AnimationSequenceStruct();

         std::vector<std::string> mChildNames;
      };

      ///Character Data
      std::string mName;                            ///<The name of this animated entity
      std::vector<AnimationStruct> mAnimations;     ///<Container of animation structs
      std::vector<MaterialStruct> mMaterials;       ///<Container of material structs
      std::vector<MeshStruct> mMeshes;              ///<Container of mesh structs
      std::vector<AnimationChannelStruct> mAnimationChannels; ///<The preconfigured playbable animations
      std::vector<AnimationSequenceStruct> mAnimationSequences; ///<The preconfigured playbable animations
      std::string mSkeletonFilename;                ///<The one skeleton filename      

   private:
      bool AnimatableCharacters(const XMLCh* const chars, AnimatableStruct& animatable);
      void AnimChannelCharacters(const XMLCh* const chars);
      void AnimSequenceCharacters(const XMLCh* const chars);

      typedef std::stack<std::string> ElementStack;
      ElementStack mElements;
      
      bool mInChannel;
      bool mInSequence;
      dtUtil::Log* mLogger;
   };
}
#endif // DELTA_CHARACTER_FILE_HANDLER
