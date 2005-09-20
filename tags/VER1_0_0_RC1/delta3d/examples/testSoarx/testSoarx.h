#include "dtCore/dt.h"
#include "dtABC/dtabc.h"
#include "soarx/dtsoarx.h"


/**
* The terrain test application.
*/
class TestTerrainApp : public dtABC::Application
{

public:

   /**
   * Constructor.
   */
   TestTerrainApp( std::string configFile = "config.xml" );

   ~TestTerrainApp();


   virtual void Config();

   /**
   * Pre-frame callback.
   *
   * @param deltaFrameTime the amount of time elapsed since the last frame
   */
   virtual void PreFrame(const double deltaFrameTime);


   /**
   * KeyboardListener override
   * Called when a key is pressed.
   *
   * @param keyboard the source of the event
   * @param key the key pressed
   * @param character the corresponding character
   */
   virtual void KeyPressed(dtCore::Keyboard* keyboard,
                           Producer::KeyboardKey key,
                           Producer::KeyCharacter character);

   /**
   * KeyboardListener override
   * Called when a key is released.
   *
   * @param keyboard the source of the event
   * @param key the key pressed
   * @param character the corresponding character
   */
   virtual  void  KeyReleased( dtCore::Keyboard*       keyboard, 
                               Producer::KeyboardKey   key,
                               Producer::KeyCharacter  character);

private:
   dtCore::RefPtr<dtSOARX::SOARXTerrain> mSOARXTerrain;
   dtCore::RefPtr<dtCore::Environment> mEnvironment;
   dtCore::RefPtr<dtCore::FlyMotionModel> mMotionModel;

   /**
   * Wireframe flag.
   */
   bool mWireframe;
};