#ifndef  DELTA_LISTENER
#define  DELTA_LISTENER

#include <transformable.h>



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
