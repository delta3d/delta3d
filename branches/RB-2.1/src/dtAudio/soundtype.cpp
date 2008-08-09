#include <dtAudio/soundtype.h>

using namespace dtAudio;


/////////////////////////////////////////////////////////////////////////////
// SOUND TYPE ENUMERATION CODE
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(SoundType)
SoundType SoundType::SOUND_TYPE_DEFAULT("SOUND_TYPE_DEFAULT");
SoundType SoundType::SOUND_TYPE_MUSIC("SOUND_TYPE_MUSIC");
SoundType SoundType::SOUND_TYPE_UI_EFFECT("SOUND_TYPE_UI_EFFECT");
SoundType SoundType::SOUND_TYPE_VOICE("SOUND_TYPE_VOICE");
SoundType SoundType::SOUND_TYPE_WORLD_EFFECT("SOUND_TYPE_WORLD_EFFECT");

/////////////////////////////////////////////////////////////////////////////
SoundType::SoundType(const std::string &name)
: dtUtil::Enumeration(name)
{
   AddInstance(this);
}

///static
void dtAudio::SoundType::AddNewType( SoundType &soundType )
{
   AddInstance(&soundType);
}
