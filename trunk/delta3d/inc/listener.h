#ifndef  DELTA_LISTENER
#define  DELTA_LISTENER

#include <transformable.h>



namespace   dtAudio
{
   class Listener :  public   dtCore::Transformable
   {
      protected:
                        Listener();
         virtual        ~Listener();

      public:
         virtual  void  SetTransform( dtCore::Transform* transform ) = 0L;
         virtual  void  GetTransform( dtCore::Transform* transform ) = 0L;

         virtual  void  SetVelocity( const sgVec3& velocity )        = 0L;
         virtual  void  GetVelocity( sgVec3& velocity )        const = 0L;

         virtual  void  SetGain( float gain )                        = 0L;
         virtual  float GetGain( void )                        const = 0L;
   };

};



#endif   // DELTA_LISTENER