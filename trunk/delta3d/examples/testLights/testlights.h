#ifndef DELTA_TESTLIGHTS
#define DELTA_TESTLIGHTS

#include "dtABC/dtabc.h"

class TestLightsApp : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( TestLightsApp )

public:

   /**
   * Default constructor.
   *
   * @param configuration file name
   */
   TestLightsApp( std::string configFilename = "config.xml" );

   /**
   * Destructor.
   */
   virtual ~TestLightsApp();

   virtual void Config();

   /**
   * KeyboardListener override
   * Called when a key is pressed.
   *
   * @param keyboard the source of the event
   * @param key the key pressed
   * @param character the corresponding character
   */
   virtual void KeyPressed(   dtCore::Keyboard*      keyboard, 
                              Producer::KeyboardKey  key,
                              Producer::KeyCharacter character );

   virtual void OnMessage( dtCore::Base::MessageData* data );

};

#endif // DELTA_TESTLIGHTS
