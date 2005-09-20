// dtaudiobindings.cpp: Python bindings for dtAudio library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtCore/dt.h>
#include <dtAudio/dtaudio.h>

void initAudioManagerBindings();
void initListenerBindings();
void initSoundEffectBinderBindings();

BOOST_PYTHON_MODULE(PyDtAudio)
{
   initAudioManagerBindings();
   initListenerBindings();
   initSoundEffectBinderBindings();
}
