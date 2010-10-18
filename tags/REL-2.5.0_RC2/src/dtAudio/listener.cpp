#include <dtAudio/listener.h>

#include <assert.h>

#include <dtAudio/dtaudio.h>

#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/collisioncategorydefaults.h>

#include <dtUtil/mathdefines.h>

// namespaces
using namespace   dtAudio;
using namespace   dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Listener)

////////////////////////////////////////////////////////////////////////////////
Listener::Listener()
{
   SetName("Listener");

   Clear();
   
   AddSender(&dtCore::System::GetInstance());

   RegisterInstance(this);

   SetCollisionCategoryBits(COLLISION_CATEGORY_MASK_LISTENER);
}

////////////////////////////////////////////////////////////////////////////////
Listener::~Listener()
{
    DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void Listener::SetVelocity(const osg::Vec3f& velocity)
{
   alListener3f(AL_VELOCITY,
                static_cast<ALfloat>(velocity[0]),
                static_cast<ALfloat>(velocity[1]),
                static_cast<ALfloat>(velocity[2]));
   CheckForError("OpenAL Listener velocity value changing", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void Listener::GetVelocity(osg::Vec3f& velocity) const
{
   alGetListener3f(AL_VELOCITY, static_cast<ALfloat*>(&velocity[0]),
                                static_cast<ALfloat*>(&velocity[1]),
                                static_cast<ALfloat*>(&velocity[2]));
   CheckForError("Getting OpenAL Listener velocity value", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
void Listener::SetGain(float gain)
{
   // force gain to range from zero to one
   dtUtil::Clamp<float>( gain, 0.0f, 1.0f );

   alListenerf(AL_GAIN, gain);
   CheckForError("OpenAL Listener gain value changing", __FUNCTION__, __LINE__);
}

////////////////////////////////////////////////////////////////////////////////
float Listener::GetGain(void) const
{
   float g;
   alGetListenerf(AL_GAIN, &g);
   CheckForError("OpenAL Listener getting gain value", __FUNCTION__, __LINE__);

   return g;
}

////////////////////////////////////////////////////////////////////////////////
void Listener::OnMessage(MessageData* data)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);
   assert(data);

   if(data->message == dtCore::System::MESSAGE_FRAME)
   {
      dtCore::Transform transform;
      osg::Matrix       matrix;
      ALfloat           pos[3];

      union orient
      {
         ALfloat     ort[6];

         struct
         {
            ALfloat  at[3];
            ALfloat  up[3];
         };
      } orient;

      GetTransform(transform);
      osg::Vec3 tmp;
      transform.GetTranslation(tmp);
      pos[0] = tmp[0];
      pos[1] = tmp[1];
      pos[2] = tmp[2];
      transform.Get(matrix);

      //assign at and up vectors directly from the matrix
      orient.at[0] = matrix(1,0);
      orient.at[1] = matrix(1,1);
      orient.at[2] = matrix(1,2);
      orient.up[0] = matrix(2,0);
      orient.up[1] = matrix(2,1);
      orient.up[2] = matrix(2,2);

      alListenerfv(AL_POSITION, pos);
      alListenerfv(AL_ORIENTATION, orient.ort);
      CheckForError("AL Listener value changing", __FUNCTION__, __LINE__);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Listener::Clear(void)
{
   CheckForError(ERROR_CLEARING_STRING, __FUNCTION__, __LINE__);

   union
   {
      ALfloat     ort[6];
      struct
      {
         ALfloat  at[3];
         ALfloat  up[3];
      };
   }  orient;
   orient.ort[0] = 0.0f;
   orient.ort[1] = 1.0f;
   orient.ort[2] = 0.0f;
   orient.ort[3] = 0.0f;
   orient.ort[4] = 0.0f;
   orient.ort[5] = 1.0f;

   ALfloat  pos[3L]  = { 0.0f, 0.0f, 0.0f };

   alListenerfv( AL_POSITION, pos );
   alListenerfv( AL_ORIENTATION, orient.ort );
   CheckForError("AL Listener value changing", __FUNCTION__, __LINE__);

   SetVelocity(osg::Vec3f(0.0f, 0.0f, 0.0f));
   SetGain(1.0);
}
