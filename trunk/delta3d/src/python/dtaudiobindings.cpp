// dtaudiobindings.cpp: Python bindings for dtAudio library.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"

#include "dt.h"
#include "dtaudio.h"


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
