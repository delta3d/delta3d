// dtaudiobindings.cpp: Python bindings for dtAudio library.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"

#include "dtCore/dt.h"
#include "dtAudio/dtaudio.h"


// The individual class bindings

void initAudioManagerBindings();
void initListenerBindings();
void initSoundEffectBinderBindings();


BOOST_PYTHON_MODULE(dtAudio)
{
   initAudioManagerBindings();
   initListenerBindings();
   initSoundEffectBinderBindings();
}
