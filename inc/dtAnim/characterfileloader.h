
#ifndef __DELTA_BASEFILELOADER_H__
#define __DELTA_BASEFILELOADER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtAnim/basemodeldata.h>
#include <dtAnim/characterfilehandler.h>
#include <dtCore/refptr.h>
#include <dtUtil/refstring.h>



namespace dtAnim
{
   class CharacterFileHandler;

   ////////////////////////////////////////////////////////////////////////////////
   // INCLUDE DIRECTIVES
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT CharacterFileLoader : public osg::Referenced
   {
   public:
      typedef CharacterFileHandler::AnimatableOverrideStruct OverrideStruct;
      typedef CharacterFileHandler::AnimatableOverrideStructArray OverrideStructArray;
      typedef CharacterFileHandler::AnimationSequenceStruct AnimationSequenceStruct;

      static const dtUtil::RefString CHARACTER_FILE_EXTENSION;
      
      virtual dtCore::RefPtr<CharacterFileHandler> LoadCharacterFile(const dtCore::ResourceDescriptor& resourde);
      virtual dtCore::RefPtr<CharacterFileHandler> LoadCharacterFile(const std::string& file);
   
      virtual void CreateChannelsAndSequences(dtAnim::CharacterFileHandler& handler, dtAnim::BaseModelData& modelData);
   
      static void FinalizeSequenceInfo(AnimationSequenceStruct& sequenceStruct,
         dtAnim::AnimatableArray& animArray);
   protected:
      virtual ~CharacterFileLoader() {}

      void SetAnimatableValues(Animatable& animatable, const OverrideStruct& info);

      static OverrideStruct* GetPreviousOverrideStruct(const std::string& name,
         AnimationSequenceStruct& sequenceStruct, OverrideStructArray::iterator& startIter);

      static Animatable* GetAnimatable(const std::string& animName, dtAnim::AnimatableArray& animArray);

      static float ResolveCrossFade(OverrideStruct& previous, OverrideStruct& current);

      void SetAnimatableEvents(Animatable& anim, const CharacterFileHandler::AnimatableStruct& info);
      
      std::string mCharacterSystem;
   };
}

#endif
