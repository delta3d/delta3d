#ifndef  DELTA_LISTENER
#define  DELTA_LISTENER

#include <dtCore/transformable.h>



namespace   dtAudio
{
   /** dtAudio::Listener 
    *
    * dtAudio::Listener is just an interface to the global listener
    * object held within (and protected) by the dtAudio::AudioManager.
    *
    * The listener is not created directly by the user (new/delete).
    * Instead the user requests the listener from the AudioManager:
    *
    *    Listener* global_ear = AudioManager::GetManager()->GetListener();
    *
    * The user can then call any of the Listener interface functions.
    * After the user is finished with the Listener, there is no need to
    * free it up.  The underlying listener object is a global singular
    * which lasts as long as the AudioManager exists.
    *
    * Listener is a transformable, so it can be a child of other
    * transformables (ie. the camera)  When a Listener is child of
    * another object, it automatically gets positioned in scene-space
    * relative to the parent object every frame, so there is no need to
    * update the Listener's position.  The Listener position can be set
    * manually in scene-space without having to make it a child of another
    * object, but any position updates must then be made manually.
    */
   class DT_EXPORT Listener :  public   dtCore::Transformable
   {
      DECLARE_MANAGEMENT_LAYER(Listener)

      protected:
         /**
          * Constructor, user does not create directly
          * instead requests the listener from AudioManager
          */
                        Listener();

         /**
          * Destructor, user does not delete directly
          * AudioManager handles destruction
          */
         virtual        ~Listener();

      public:
         /**
          * Set the velocity of the listener.
          *
          * @param velocity to set
          */
         virtual  void  SetVelocity( const sgVec3& velocity )        = 0L;

         /**
          * Get the velocity of the listener.
          *
          * @param velocity to get
          */
         virtual  void  GetVelocity( sgVec3& velocity )        const = 0L;

         /**
          * Sets the master volume of the listener.
          *
          * @param gain the new gain
          */
         virtual  void  SetGain( float gain )                        = 0L;

         /**
          * Returns the master volume of the listener.
          *
          * @return the current volume
          */
         virtual  float GetGain( void )                        const = 0L;
   };
};



#endif   // DELTA_LISTENER
