
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/animationinterface.h>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(PlayModeEnum);
   PlayModeEnum PlayModeEnum::NONE("NONE");
   PlayModeEnum PlayModeEnum::ONCE("ONCE");
   PlayModeEnum PlayModeEnum::LOOP("LOOP");
   PlayModeEnum PlayModeEnum::SWING("SWING");
   PlayModeEnum PlayModeEnum::POSE("POSE");
   
   /////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(AnimationStateEnum);
   AnimationStateEnum AnimationStateEnum::NONE("NONE");
   AnimationStateEnum AnimationStateEnum::FADE_IN("FADE_IN");
   AnimationStateEnum AnimationStateEnum::STEADY("STEADY");
   AnimationStateEnum AnimationStateEnum::FADE_OUT("FADE_OUT");
   AnimationStateEnum AnimationStateEnum::STOPPED("STOPPED");
}
