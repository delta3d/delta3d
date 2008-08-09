/* -*-c++-*-
* testHUD - testHUD(.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
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
