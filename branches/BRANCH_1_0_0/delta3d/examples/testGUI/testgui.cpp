#include "dt.h"
#include "dtabc.h"

CUI_UI *ui = NULL; //global - needed for the callback

using namespace dtCore;
using namespace dtABC;
using namespace std;

///this is the callback function that gets triggered from the widgets
static bool mainHandler( int id, int numparam, void *value )
{
   switch(id) 
   {
   case 1:   dtCore::System::GetSystem()->Stop();  	break;
   case 2:
      {
         CUI_SliderBar* bar = (CUI_SliderBar*)ui->GetFrame(2);

         ui->GetRenderer()->SetFade(bar->GetValue()/100);
      }
      break;
   default: break;
   }
   return false;
}

class TestGUIApp : public Application
{

DECLARE_MANAGEMENT_LAYER( TestGUIApp )

public:
   TestGUIApp( string configFilename = "config.xml" )
      : 
   Application( configFilename ),
      mFilename("")
   {
   }

   void Config()
   {
      Application::Config();

      ///put something in the background to look at
      Object *helo = new Object( "Helo" );
      helo->LoadFile( "UH-1N/UH-1N.ive" );
      AddDrawable( helo );

      ///move the camera up
      Transform xform(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f );
      GetCamera()->SetTransform( &xform );

      //position the helo behind the UI
      xform.Set( -2.5f, 30.0f, 2.5f, 90.0f, 0.0f, 0.0f );
      helo->SetTransform( &xform );

      int w,h,x,y;
      GetWindow()->GetPosition(&x, &y, &w, &h);

      //This will contain all our UI elements
      UIDrawable *drawable = new UIDrawable(w,h);
      drawable->SetWindowResolution(w,h);
      ui = drawable->GetUI();

      if (!mFilename.empty()) //create the GUI w/ XML
      {
         if(!drawable->LoadGUIFile(mFilename))
            return;
      }
      else //create the GUI w/ C++
      {
         // make some general purpose shaders
         sgVec4 black_col = { 0.0, 0.0, 0.0, 1.0 };
         sgVec4 white_col = { 1.0, 1.0, 1.0, 1.0 };
         sgVec4 trans_col = { 0.0, 0.0, 0.0, 0.0 };
         drawable->CreateShader( "transparent", trans_col );

         // fonts
         drawable->CreateShader( "raster8", white_col, "gui/raster8.rgba" );
         drawable->CreateShader( "digital_font", black_col, "gui/digital_font.rgba" );
         drawable->CreateFixedFont("digital_font", "digital_font");
         drawable->CreateFixedFont("raster8", "raster8");

         // textures
         drawable->CreateShader( "button", "gui/button.tga" );
         drawable->CreateShader( "button_hover", "gui/button_hover.tga" );
         drawable->CreateShader( "button_click", "gui/button_click.tga" );
         drawable->CreateShader( "cursor", "gui/cursor.rgba" );
         drawable->CreateShader( "panel", "gui/panel.rgba");
         drawable->CreateShader( "edit_box", "gui/edit_box.rgb");
         drawable->CreateShader( "slider_bar", "gui/slider_bar.tga");
         drawable->CreateShader( "slider", "gui/slider.rgba");

         ///this main frame will contain all the widgets
         CUI_Frame *mainFrame = new CUI_Frame();
         mainFrame->SetShader(drawable->GetShader("panel"));
         drawable->AddRootFrame("main", mainFrame);

         ///display some text
         CUI_TextBox *title = new CUI_TextBox();
         title->Move( 0.2, 0.5, 0.8, 0.7 );
         title->SetSize(8, 1);
         title->SetTextScale(1.0f);
         title->SetText( "Delta_3D" );
         title->SetFont(drawable->GetFont("digital_font") );
         title->SetShader(drawable->GetShader("transparent"));
         mainFrame->AddChild( title );
         drawable->AddFrame( title );

         ///Editable text box
         CUI_EditableTextBox *etb = new CUI_EditableTextBox();
         etb->SetFont( drawable->GetFont("raster8") );
         etb->Move(0.3, 0.35, 0.7, 0.45);
         etb->SetSize(20,1);
         etb->SetText("EditableTextBox");
         etb->SetShader(drawable->GetShader("edit_box"));
         etb->SetCursorType(UI_CURSOR_BAR);
         etb->MoveCursor( 15 );
         mainFrame->AddChild(etb);
         drawable->AddFrame(etb);

         ///Slider bar to control the transparency of the interface
         CUI_SliderBar *slide = new CUI_SliderBar ();
         slide->Move(0.12, 0.8, 0.88, 0.9);
         slide->SetRange(0.f, 105.f);
         slide->SetValue(100.f);
         slide->SetShader(drawable->GetShader("slider"));
         slide->SetBarShader(drawable->GetShader("slider_bar"));
         slide->SetFrameID(2);   ///Note the magic ID number - used in the CB
         mainFrame->AddChild(slide);
         drawable->AddFrame(slide);

         ///An exit button
         CUI_Button *exitB = new CUI_Button();
         exitB->Move(0.4, 0.2, 0.6, 0.3 );
         exitB->SetShader(ui->GetShader("button"));
         exitB->SetHoverShader(ui->GetShader("button_hover"));
         exitB->SetClickShader(ui->GetShader("button_click"));
         exitB->SetFont( drawable->GetFont("raster8") );
         exitB->SetText("Exit");
         exitB->SetTextScale(0.40f);
         exitB->SetFrameID(1);
         mainFrame->AddChild(exitB);
         drawable->AddFrame(exitB);      
      }

      //hook up the main callback
      drawable->SetCallbackFunc("main", (CUI_UI::callbackfunc)mainHandler );
      drawable->SetActiveRootFrame("main");

      ///Add the drawable to the Scene
      AddDrawable( drawable );


   }

   void SetFilename( string file )
   {
      mFilename = file;
   }

   string mFilename;
};

IMPLEMENT_MANAGEMENT_LAYER( TestGUIApp )


int main( int argc, const char* argv[] )
{
   string filename = "";
   if (argc > 1)
   {
      Notify(ALWAYS,"Using GUI file %s...",argv[1]);
      filename = argv[1];
   }
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );

   TestGUIApp *app = new TestGUIApp( "config.xml" );
   app->SetFilename( filename );

   app->Config();
   app->Run();

   delete app;

   return 0;
}


