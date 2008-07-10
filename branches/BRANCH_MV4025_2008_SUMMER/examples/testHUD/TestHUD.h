#ifndef TestHUD_h__
#define TestHUD_h__


//delta3D
#include <dtABC/application.h>
#include <dtCore/refptr.h>
#include <dtCore/camera.h>
#include <dtGUI/hud.h>

class TestHUD : public dtABC::Application
{
public:

   TestHUD( const std::string& sConfigFilename = "" );

protected:

   virtual ~TestHUD();

public:

   virtual void Config();

   bool OnButtonClicked(const CEGUI::EventArgs&);

private:

   void _ConfigScene();

   dtCore::RefPtr<dtGUI::HUD>      m_pHUD;
   dtCore::RefPtr<dtCore::Camera>  m_pCamera1;
   dtCore::RefPtr<dtCore::Camera>  m_pCamera2;

};


#endif // TestHUD_h__
