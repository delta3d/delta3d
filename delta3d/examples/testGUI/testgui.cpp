// This application demonstrates how to implement a UI using the glGUI
// library.
// This will load a GUI config xml if the filename is passed in on the command
// line or if no filename is present, create one from code.
//
#include "application.h"
#include "dt.h"
#include "globals.h"
#include "uidrawable.h"



CUI_UI *ui = NULL; //global - needed for the callback


///this is the callback function that gets triggered from the widgets
static bool mainHandler( int id, int numparam, void *param )
{
   switch(id) 
   {
   case 1:   dtCore::System::GetSystem()->Stop();  	break;
   case 2:
      {
         if (numparam >0)
         {
            float val = ((float*)param)[1];
            val/=100.f;
            ui->GetRenderer()->SetFade(val);
         }
      }
      break;
   default: break;
   }
   return false;
}


int main(int argc, char* argv[])
{
   std::string filename;
   if (argc>1)
   {
      filename = argv[1];
   }

   dtABC::Application *app = new dtABC::Application();
   dtCore::SetDataFilePathList("../../data");

   ///put something in the background to look at
   dtCore::Object *ground = new dtCore::Object("ground");
   ground->LoadFile("ground.flt");
   app->AddDrawable(ground);

   ///move the camera up
   dtCore::Transform xform(0.f, 0.f, 5.f, 0.f, 0.f,0.f);
   app->GetCamera()->SetTransform(&xform);

   app->Config();

   int w,h,x,y;
   app->GetWindow()->GetPosition(&x, &y, &w, &h);

   //This will contain all our UI elements
   dtCore::UIDrawable *drawable = new dtCore::UIDrawable(w,h);
   drawable->SetWindowResolution(w,h);
   ui = drawable->GetUI();


   if (!filename.empty())
   {
      drawable->LoadGUIFile(filename);
   }
   else
   {
      // make some general purpose shaders
      sgVec4 black_col = { 0.0, 0.0, 0.0, 1.0 };
      sgVec4 dgrey_col = { 0.25, 0.25, 0.25, 1.0 };
      sgVec4 grey_col = { 0.5, 0.5, 0.5, 1.0 };
      drawable->CreateShader( "black", black_col );
      drawable->CreateShader( "dgrey", dgrey_col );
      drawable->CreateShader( "grey", grey_col );
      drawable->CreateShader( "buttonshader", "bBox.rgba" );
      drawable->CreateShader( "buttonhover", "bBox_hi.rgba" );
      drawable->CreateShader( "buttonclick", "bBox_dn.rgba" );
      drawable->CreateShader( "bar", "target.bmp" );
      drawable->CreateShader( "raster8", black_col, "raster8.rgba" );
      drawable->CreateFixedFont("raster8", "raster8");
      drawable->CreateShader( "cursor", "cursor.rgba" );

      ///this main frame will contain all the widgets
      CUI_Frame *mainF = new CUI_Frame();
      mainF->Move(0.2f, 0.20f, 0.8f, 0.8f);
      mainF->SetShader(drawable->GetShader("dgrey"));
      drawable->AddRootFrame("intro", mainF);

      ///display some text
      CUI_TextBox *title = new CUI_TextBox();
      title->Move( 0.2, 0.5, 0.8, 0.7 );
      title->SetSize(4, 1);
      title->SetText( "P-51" );
      title->SetFont(drawable->GetFont("raster8") );
      mainF->AddChild( title );
      drawable->AddFrame( title );

      ///Editable text box
      CUI_EditableTextBox *etb = new CUI_EditableTextBox();
      etb->SetFont( drawable->GetFont("raster8") );
      etb->Move(0.11, 0.2, 0.5, 0.3);
      etb->SetText("EditableTextBox");
      etb->SetShader(drawable->GetShader("defaultLo"));
      etb->SetSize(20, 1);
      etb->SetCursorType(UI_CURSOR_BAR );
      mainF->AddChild(etb);
      drawable->AddFrame(etb);

      ///Slider bar to control the transparency of the interface
      CUI_SliderBar *slide = new CUI_SliderBar ();
      slide->Move(0.1, 0.8, 0.9, 0.9);
      slide->SetRange(0.f, 105.f);
      slide->SetValue(100.f);
      slide->SetFont(drawable->GetFont("raster8") );
      slide->SetShader(drawable->GetShader("defaultLo"));
      slide->SetBarShader(drawable->GetShader("bar"));
      slide->SetFrameID(2);   ///Note the magic ID number - used in the CB
      mainF->AddChild(slide);
      drawable->AddFrame(slide);

      ///An exit button
      CUI_Button *exitB = new CUI_Button();
      exitB->Move(0.7, 0.1, 0.9, 0.3 );
      exitB->SetShader(ui->GetShader("buttonshader"));
      exitB->SetHoverShader(ui->GetShader("buttonhover"));
      exitB->SetClickShader(ui->GetShader("buttonclick"));
      exitB->SetFont( drawable->GetFont("raster8") );
      exitB->SetText("Exit");
      exitB->SetTextScale(0.40f);
      exitB->SetFrameID(1);
      mainF->AddChild(exitB);
      drawable->AddFrame(exitB);      
   }

   //hook up the main callback
   drawable->SetCallbackFunc("intro", mainHandler );
   drawable->SetActiveRootFrame("intro");

   ///Add the drawable to the Scene
   app->GetScene()->AddDrawable( drawable );

   app->Run();

	return 0;
}

