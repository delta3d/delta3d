// guimgr.cpp: implementation of the GUI class.
//
//////////////////////////////////////////////////////////////////////

#include "guimgr.h"
#include "transformable.h"
#include "camera.h"
#include "environment.h"
#include "skydome.h"
#include "clouddome.h"
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>
#include "system.h"
#include "infiniteterrain.h"

using namespace dtCore;using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

dtCore::Base *UserInterface::GetSelectedInstance( UserInterface *ui)
{
   return ( (dtCore::Base*)ui->InstanceList->data( ui->InstanceList->value() ) );
}

/** Reads the currently selected instance and updates all of its related 
  * widgets.
  */
void UserInterface::SelectInstance (void)
{
   Base *b = GetSelectedInstance(this);
   
   if (IS_A(b, Base*))
   {
      BaseName->value( b->GetName().c_str() );
      std::string name = typeid(b).name();
      InstanceClassName->label( name.c_str() ); 
   }



   /** Transformable **/
   if (IS_A(b, Transformable*))
   {
      Transformable *t = (Transformable*)b;
      InstanceClassName->label( typeid(t).name() ); 
      
      Transform trans;
      if (TransformCSAbsButton->value())
      {
         t->GetTransform( &trans, Transformable::ABS_CS );
      }
      else
      {
         t->GetTransform( &trans, Transformable::REL_CS );
      }

      sgVec3 xyz, hpr;
      trans.Get( xyz, hpr );
      TransformX->value(xyz[0]);
      TransformY->value(xyz[1]);
      TransformZ->value(xyz[2]);
      TransformH->value(hpr[0]);
      TransformP->value(hpr[1]);
      TransformR->value(hpr[2]);

      TransformChildList->clear();
      for (unsigned int childIdx=0; childIdx<t->GetNumChildren(); childIdx++)
      {
         osg::ref_ptr<Transformable> child = t->GetChild(childIdx);
         TransformChildList->add( child->GetName().c_str(), child.get() );
      }

      if (t->GetParent() != NULL)
        TransformParentText->value(  t->GetParent()->GetName().c_str() );
      else
        TransformParentText->value( "NULL" );

      
      TransformGroup->show();
   }
   else TransformGroup->hide();
   
   /** Camera **/
   if (IS_A(b, Camera*))
   {
      Camera *c = (Camera*)b;
      InstanceClassName->label( typeid(c).name() ); 
      sgVec4 color;
      c->GetClearColor(color);
      CameraClearRed->value(color[0]);
      CameraClearGreen->value(color[1]);
      CameraClearBlue->value(color[2]);
      
      Fl_Color fc = fl_color_cube( color[0]*(FL_NUM_RED-1),
         color[1]*(FL_NUM_GREEN-1),
         color[2]*(FL_NUM_BLUE-1) );
      
      CameraClearLoadButton->color(fc);      
      

      //rebuild the CameraSceneChoice menu here in case
      //we have new Scene's or they changed their names
      CameraSceneChoice->clear();

      CameraSceneChoice->add( "None", NULL, NULL);
      for (int i=0; i<Scene::GetInstanceCount(); i++)
      {
         Scene *s = Scene::GetInstance(i);
         CameraSceneChoice->add( s->GetName().c_str(), NULL, NULL, s, 0);
      }

      const Fl_Menu_Item *menu = CameraSceneChoice->menu();
      for ( i=0; i<CameraSceneChoice->size(); i++)
      {
         Scene *menuItemScene = (Scene*)menu[i].user_data();
         
         if (menuItemScene == c->GetScene())
         {
            CameraSceneChoice->value(i);
            break;
         }
      }

      
      //rebuild the CameraWinChoice menu here in case
      //we have new Windows's or they changed their names
      CameraWinChoice->clear();
      CameraWinChoice->add( "None", NULL, NULL);
      for ( i=0; i<Window::GetInstanceCount(); i++)
      {         
         CameraWinChoice->add( Window::GetInstance(i)->GetName().c_str(), NULL, NULL,
                               Window::GetInstance(i), 0);
      }

      menu = CameraWinChoice->menu();
      
      for ( i=0; i<CameraWinChoice->size(); i++)
      {
         Window *menuItemWin = (Window*)menu[i].user_data();
         
         if (menuItemWin == c->GetWindow())
         {
            CameraWinChoice->value(i);
            break;
         }
      }

      
      CameraGroup->show();
   }
   else CameraGroup->hide();
   
   /** Scene **/
   if (IS_A(b, Scene*))
   {
      Scene *s = (Scene*)b;
      InstanceClassName->label( typeid(s).name() ); 
      //SceneGroup->show();
   }
   //else SceneGroup->hide();
   
   /** Object **/
   if (IS_A(b, Object*))
   {
      Object *o = (Object*)b;
      InstanceClassName->label( typeid(o).name() ); 
      std::string filename = o->GetFilename();
      ObjectFilename->value( filename.c_str() );

      ObjectGroup->show();
   }
   else ObjectGroup->hide();
   
   /** Window **/
   if (IS_A(b, Window*))
   {
      Window *w = (Window*)b;
      InstanceClassName->label( typeid(w).name() ); 

      int x,y,width,height;
      w->GetPosition(&x, &y, &width, &height);
      WinPosX->value(x);     WinPosY->value(y);
      WinPosW->value(width); WinPosH->value(height);

      if (w->GetShowCursor()==true) WinCursorToggle->value(true);
      else WinCursorToggle->value(false);

      std::string name = w->GetWindowTitle();
      WinTitle->value(name.c_str());

      if (w->GetFullScreenMode()) WinFullScreenToggle->value(1);
      else                        WinFullScreenToggle->value(0);
      
      WindowGroup->show();
   }
   else WindowGroup->hide();

   /** Environment **/
   if (IS_A(b, Environment*))
   {
      Environment *e = (Environment*)b;
      InstanceClassName->label( typeid(e).name() );
      
      sgVec4 fColor;
      e->GetFogColor(fColor);
      FogRed->value(fColor[0]);
      FogGreen->value(fColor[1]);
      FogBlue->value(fColor[2]);

      Fl_Color fc = fl_color_cube( fColor[0]*(FL_NUM_RED-1),
         fColor[1]*(FL_NUM_GREEN-1),
         fColor[2]*(FL_NUM_BLUE-1) );
      
      FogColorLoadButton->color(fc);

      sgVec4 bc;
      e->GetAdvFogCtrl(bc);
      AdvFogTurbidity->value(bc[0]);
      AdvFogEnergy->value(bc[1]);
      AdvFogMolecules->value(bc[2]);

      sgVec3 sunColor;
      e->GetSunColor(sunColor);
      EnvSunRed->value(sunColor[0]);
      EnvSunGreen->value(sunColor[1]);
      EnvSunBlue->value(sunColor[2]);

      float vis = e->GetVisibility();
      EnvFogVis->value(vis);

      float n = e->GetFogNear();
      EnvFogNear->value(n);

      if (e->GetFogEnable())  EnvFogEnable->value(1);
      else                    EnvFogEnable->value(0);
   
      Environment::FogMode fm = e->GetFogMode();
      EnvFogMode->value(fm);
      if (fm==Environment::ADV)
      {
         EnvAdvFogGroup->show();
         EnvFogColorGroup->hide();
      }
      else
      {
         EnvAdvFogGroup->hide();
         EnvFogColorGroup->show();
      }
      sgVec3 skyColor;
      e->GetSkyColor(skyColor);
      SkyRed->value(skyColor[0]);
      SkyGreen->value(skyColor[1]);
      SkyBlue->value(skyColor[2]);

      Fl_Color sc = fl_color_cube( skyColor[0]*(FL_NUM_RED-1),
         skyColor[1]*(FL_NUM_GREEN-1),
         skyColor[2]*(FL_NUM_BLUE-1) );
      
      SkyColorLoadButton->color(sc);

      int yr, mo, da, hr, mi, sec;
      e->GetDateTime(&yr, &mo, &da, &hr, &mi, &sec);
      EnvYear->value(yr);
      EnvMonth->value(mo);
      EnvDay->value(da);
      EnvTimeOfDay->value(hr+(mi/60.0)+(sec/3600.0));

      sgVec2 refPos;
      e->GetRefLatLong(refPos);
      EnvRefLat->value(refPos[0]);
      EnvRefLong->value(refPos[1]);

      EnvironmentGroup->show();
   }
   else EnvironmentGroup->hide();


   /** SkyDome **/
   if (IS_A(b, SkyDome*))
   {
      SkyDome *s = (SkyDome*)b;
      InstanceClassName->label( typeid(s).name() );

      sgVec3 color;
      s->GetBaseColor(color);
      SkyBoxBaseRed->value(color[0]);
      SkyBoxBaseGreen->value(color[1]);
      SkyBoxBaseBlue->value(color[2]);

      Fl_Color fc = fl_color_cube( color[0]*(FL_NUM_RED-1),
         color[1]*(FL_NUM_GREEN-1),
         color[2]*(FL_NUM_BLUE-1) );
      
      SkyBoxBaseColorLoadButton->color(fc);      

      SkyBoxGroup->show();
   }
   else SkyBoxGroup->hide();


   if (IS_A(b, System*))
   {
      System *s = (System*)b;
      InstanceClassName->label( typeid(s).name() );
   }

   if (IS_A(b, InfiniteTerrain*))
   {
      InfiniteTerrain *t = (InfiniteTerrain*)b;
      float dist = t->GetBuildDistance();
      float hscale = t->GetHorizontalScale();
      float vscale = t->GetVerticalScale();
      float segSize = t->GetSegmentSize();
      int segDiv = t->GetSegmentDivisions();

      InfBuildDistance->value(dist);
      InfSegSize->value(segSize);
      InfSegDivisions->value(segDiv);
      InfVertScale->value(vscale);
      InfHorizScale->value(hscale);

      InfTerrainGroup->show();
   }
   else
   {
      InfTerrainGroup->hide();
   }

   /** CloudDome **/
   if (IS_A(b, CloudDome*))
   {
       CloudDome *cd = (CloudDome*)b;
       InstanceClassName->label( typeid(cd).name() );

       cScale->value(cd->getScale());
       cExponent->value(cd->getExponent());
       cCutoff->value(cd->getCutoff());
       cSpeedX->value(cd->getSpeedX());
       cSpeedY->value(cd->getSpeedY());
       cBias->value(cd->getBias());
       if (cd->getEnable())
           cEnable->value(1);
       else
           cEnable->value(0);

       osg::Vec3 *ccolor;
       ccolor = cd->getCloudColor();
       CloudRed->value(ccolor->x());
       CloudGreen->value(ccolor->y());
       CloudBlue->value(ccolor->z());

       Fl_Color fc = fl_color_cube( ccolor->x()*(FL_NUM_RED-1),
           ccolor->y()*(FL_NUM_GREEN-1),
           ccolor->z()*(FL_NUM_BLUE-1) );

       CloudColorLoadButton->color(fc);      

       CloudEditor->show();
   }
   else CloudEditor->hide();

}

void UserInterface::BaseNameCB(Fl_Input *o )
{
   GetSelectedInstance(this)->SetName( o->value() );
   InstanceList->text( InstanceList->value(), o->value() );
}

void UserInterface::TransformPosCB(Fl_Value_Input*)
{
   Transformable *t = (Transformable*)GetSelectedInstance(this);
   Transform trans;

   trans.Set(TransformX->value(), 
             TransformY->value(),
             TransformZ->value(),
             TransformH->value(),
             TransformP->value(),
             TransformR->value());

   if (TransformCSAbsButton->value())
   {
      t->SetTransform( &trans, Transformable::ABS_CS );
   }
   else
   {
      t->SetTransform( &trans, Transformable::REL_CS );
   }

}

void UserInterface::TransformCSCB( Fl_Round_Button *)
{
   Transformable *t = (Transformable*)GetSelectedInstance(this);
   Transform trans;
   sgVec3 xyz, hpr;

   if (TransformCSAbsButton->value())
   {
      //fill in widgets with abs position
      t->GetTransform( &trans, Transformable::ABS_CS );
   }
   else
   {
      //fill in widgets with rel position
      t->GetTransform( &trans, Transformable::REL_CS );
   }
   
   trans.Get( xyz, hpr );
   TransformX->value(xyz[0]);
   TransformY->value(xyz[1]);
   TransformZ->value(xyz[2]);
   TransformH->value(hpr[0]);
   TransformP->value(hpr[1]);
   TransformR->value(hpr[2]);
}

void UserInterface::CameraClearColorBrowserCB(Fl_Button*)
{
   double r = CameraClearRed->value();
   double g = CameraClearGreen->value();
   double b = CameraClearBlue->value();

   fl_color_chooser("Clear Color", r, g, b);

   CameraClearRed->value(r);
   CameraClearGreen->value(g);
   CameraClearBlue->value(b);
   
   Fl_Color fc = fl_color_cube( r*(FL_NUM_RED-1),
                                g*(FL_NUM_GREEN-1),
                                b*(FL_NUM_BLUE-1) );

   CameraClearLoadButton->color(fc);
   
   sgVec4 color = {
      CameraClearRed->value(),
      CameraClearGreen->value(),
      CameraClearBlue->value(),
      1.f };

   Camera *c = (Camera*)GetSelectedInstance(this);
   c->SetClearColor( color );
}

void UserInterface::CameraClearColorCB(Fl_Value_Input* )
{
   sgVec4 color = {
      CameraClearRed->value(),
         CameraClearGreen->value(),
         CameraClearBlue->value(),
         1.f };

      Fl_Color fc = fl_color_cube( color[0]*(FL_NUM_RED-1),
         color[1]*(FL_NUM_GREEN-1),
         color[2]*(FL_NUM_BLUE-1) );
      
      CameraClearLoadButton->color(fc);
      CameraClearLoadButton->redraw();
    
   Camera *c = (Camera*)GetSelectedInstance(this);
   c->SetClearColor( color );
}


void UserInterface::CameraSceneCB( Fl_Choice *o )
{
   Camera *cam = (Camera*)GetSelectedInstance(this);
   const Fl_Menu *menu = o->menu();   

   Scene *scene = (Scene*)menu[o->value()].user_data();
   cam->SetScene( scene );
}

void UserInterface::CameraWinCB( Fl_Choice *o )
{
   Camera *cam = (Camera*)GetSelectedInstance(this);
   const Fl_Menu *menu = o->menu();   
   
   Window *win = (Window*)menu[o->value()].user_data();
   cam->SetWindow( win );

}

void UserInterface::WinPosCB( Fl_Value_Input *o)
{
   Window *w = (Window*)GetSelectedInstance(this);
   w->SetPosition( WinPosX->value(), WinPosY->value(), 
                   WinPosW->value(), WinPosH->value() );
}

void UserInterface::WinSizeCB(Fl_Menu_Button *o)
{
   Window *win = (Window*)GetSelectedInstance(this);
   const Fl_Menu_Item*menu = o->menu();
   

   int width = 640, height = 480;
   
   std::string size( menu[o->value()].text );

   size_t split = size.find("x");
   
   if (split != string::npos)
   {
      width = atoi(size.substr(0, split).c_str());
      height = atoi(size.substr(split+1).c_str());
   }

   win->SetPosition(0, 0, width, height );

   //also refresh our input widgets
   WinPosW->value( width );
   WinPosH->value( height );
   WinPosX->value(0);
   WinPosY->value(0);
}

void UserInterface::WinCursorCB( Fl_Check_Button *o)
{
   Window *w = (Window*)GetSelectedInstance(this);

   if (o->value()) w->ShowCursor();
   else w->ShowCursor(false);
}

void UserInterface::WinTitleCB( Fl_Input *o)
{
   Window *w = (Window*)GetSelectedInstance(this);
   std::string name(o->value());
   w->SetWindowTitle(name.c_str());
}

void UserInterface::WinFullScreenCB( Fl_Check_Button *o)
{
   Window *w = (Window*)GetSelectedInstance(this);

   if (o->value()) w->SetFullScreenMode(true);
   else            w->SetFullScreenMode(false);    
   
}

void UserInterface::ObjectFileCB( Fl_Input *o)
{
   
}

void UserInterface::ObjectLoadFileCB( Fl_Button *o)
{
   string filename = fl_file_chooser("Load File", NULL, NULL, 1);
   
   Object *obj = (Object*)GetSelectedInstance(this);
   obj->LoadFile(filename);
   
   filename = obj->GetFilename();
   ObjectFilename->value( filename.c_str() );
}


void UserInterface::TransformAddChildCB( Fl_Button *)
{
   Transformable *t = (Transformable*)GetSelectedInstance(this);

   SelectList->clear();
   for (int tIdx=0; tIdx<Transformable::GetInstanceCount(); tIdx++)
   {
      Transformable *xform = Transformable::GetInstance(tIdx);
      //see is xform is a valid child
      if (t->CanBeChild(xform))
      {
         SelectList->add( xform->GetName().c_str());
      }
   }

   SelectWindow->show();

   while (SelectWindow->shown())
   {
      Fl::wait();
      for (;;)
      {
         Fl_Widget *o = Fl::readqueue();

         if (!o) break;

         if (o == SelectWinAddButton)
         {
            //read check boxes 
            for (int i=0; i< SelectList->nitems()+1; i++)
            {
               if (SelectList->checked(i))
               {
                  Transformable *c = Transformable::GetInstance(SelectList->text(i));
                  t->AddChild( c );
               }
            }
            SelectWindow->hide();

            //redraw the list of childer on this Transformable
            TransformChildList->clear();
            for (unsigned int childIdx=0; childIdx<t->GetNumChildren(); childIdx++)
            {
               osg::ref_ptr<Transformable> child = t->GetChild(childIdx);
               TransformChildList->add( child->GetName().c_str(), child.get() );
            }

            break;
         }
         if (o==SelectWindow || o==SelectWinCancelButton)
         {
            SelectWindow->hide();
            break;
         }
      }
   }
}

void UserInterface::TransformRemChildCB( Fl_Button *)
{
   Transformable *t = (Transformable*)GetSelectedInstance(this);

   //loop through selected items in TransformChildList and remove them
   for (int i=0; i<TransformChildList->size()+1; i++)
   {
      if (TransformChildList->selected(i))
      {
         Transformable *child = (Transformable*)TransformChildList->data(i);
         t->RemoveChild( child );
      }
   }

   TransformChildList->clear();
   for (unsigned int childIdx=0; childIdx<t->GetNumChildren(); childIdx++)
   {
      osg::ref_ptr<Transformable> child = t->GetChild(childIdx);
      TransformChildList->add( child->GetName().c_str(), child.get() );
   }
}

void UserInterface::SkyBoxBaseColorCB(Fl_Value_Input* )
{
   sgVec4 color = {
      SkyBoxBaseRed->value(),
         SkyBoxBaseGreen->value(),
         SkyBoxBaseBlue->value(),
         1.f };

      Fl_Color fc = fl_color_cube( color[0]*(FL_NUM_RED-1),
         color[1]*(FL_NUM_GREEN-1),
         color[2]*(FL_NUM_BLUE-1) );
      
      SkyBoxBaseColorLoadButton->color(fc);
      SkyBoxBaseColorLoadButton->redraw();
    
   SkyDome *c = (SkyDome*)GetSelectedInstance(this);
   c->SetBaseColor( color );
}

void UserInterface::SkyBoxBaseColorBrowserCB(Fl_Button*)
{
   double r = SkyBoxBaseRed->value();
   double g = SkyBoxBaseGreen->value();
   double b = SkyBoxBaseBlue->value();

   fl_color_chooser("Base Color", r, g, b);

   SkyBoxBaseRed->value(r);
   SkyBoxBaseGreen->value(g);
   SkyBoxBaseBlue->value(b);
   
   Fl_Color fc = fl_color_cube( r*(FL_NUM_RED-1),
                                g*(FL_NUM_GREEN-1),
                                b*(FL_NUM_BLUE-1) );

   SkyBoxBaseColorLoadButton->color(fc);
   
   sgVec4 color = {
      SkyBoxBaseRed->value(),
      SkyBoxBaseGreen->value(),
      SkyBoxBaseBlue->value(),
      1.f };

   SkyDome *c = (SkyDome*)GetSelectedInstance(this);
   c->SetBaseColor( color );
}

void UserInterface::EnvAdvFogCB(Fl_Value_Input *)
{
   sgVec4 bc;
   bc[0] = AdvFogTurbidity->value();
   bc[1] = AdvFogEnergy->value();
   bc[2] = AdvFogMolecules->value();
   Environment *e = (Environment*)GetSelectedInstance(this);
   e->SetAdvFogCtrl(bc);
}

void UserInterface::EnvFogColorCB(Fl_Value_Input *)
{
      sgVec4 color = {
      FogRed->value(),
         FogGreen->value(),
         FogBlue->value(),
         1.f };

      Fl_Color fc = fl_color_cube( color[0]*(FL_NUM_RED-1),
         color[1]*(FL_NUM_GREEN-1),
         color[2]*(FL_NUM_BLUE-1) );
      
      FogColorLoadButton->color(fc);
      FogColorLoadButton->redraw();
    
   Environment *e = (Environment*)GetSelectedInstance(this);
   e->SetFogColor( color );
}

void UserInterface::EnvFogColorBrowserCB(Fl_Button *)
{
   double r = FogRed->value();
   double g = FogGreen->value();
   double b = FogBlue->value();

   fl_color_chooser("Fog Color", r, g, b);

   FogRed->value(r);
   FogGreen->value(g);
   FogBlue->value(b);
   
   Fl_Color fc = fl_color_cube( r*(FL_NUM_RED-1),
                                g*(FL_NUM_GREEN-1),
                                b*(FL_NUM_BLUE-1) );

   FogColorLoadButton->color(fc);
   
   sgVec4 color = {
      FogRed->value(),
      FogGreen->value(),
      FogBlue->value(),
      1.f };

   Environment *e = (Environment*)GetSelectedInstance(this);
   e->SetFogColor( color );
}

void UserInterface::EnvFogVisCB(Fl_Value_Input *o)
{
   Environment *e = (Environment*)GetSelectedInstance(this);
   e->SetVisibility( o->value() );
}

void UserInterface::EnvFogNearCB(Fl_Value_Input *o)
{
   Environment *e = (Environment*)GetSelectedInstance(this);
   e->SetFogNear( o->value() );
}

void UserInterface::EnvFogEnableCB(Fl_Check_Button *o)
{
   Environment *e = (Environment*)GetSelectedInstance(this);

   if (o->value())   e->SetFogEnable(true);
   else      e->SetFogEnable(false);

}

void UserInterface::EnvFogModeCB(Fl_Choice* o)
{
   if (o->value() == Environment::ADV)
   {
      EnvFogColorGroup->hide();
      EnvAdvFogGroup->show();
   }
   else
   {
      EnvFogColorGroup->show();
      EnvAdvFogGroup->hide();
   }

   Environment *e = (Environment*)GetSelectedInstance(this);
   e->SetFogMode((dtCore::Environment::FogMode)o->value());
}

void UserInterface::EnvSkyColorBrowserCB(Fl_Button *)
{
   double r = SkyRed->value();
   double g = SkyGreen->value();
   double b = SkyBlue->value();

   fl_color_chooser("Sky Color", r, g, b);

   SkyRed->value(r);
   SkyGreen->value(g);
   SkyBlue->value(b);
   
   Fl_Color sc = fl_color_cube( r*(FL_NUM_RED-1),
                                g*(FL_NUM_GREEN-1),
                                b*(FL_NUM_BLUE-1) );

   SkyColorLoadButton->color(sc);
   
   sgVec4 color = {
      SkyRed->value(),
      SkyGreen->value(),
      SkyBlue->value(),
      1.f };

   Environment *e = (Environment*)GetSelectedInstance(this);
   e->SetSkyColor( color );
}

void UserInterface::EnvSkyColorCB(Fl_Value_Input*)
{
      sgVec4 color = {
         SkyRed->value(),
         SkyGreen->value(),
         SkyBlue->value(),
         1.f };

      Fl_Color fc = fl_color_cube( color[0]*(FL_NUM_RED-1),
         color[1]*(FL_NUM_GREEN-1),
         color[2]*(FL_NUM_BLUE-1) );
      
      SkyColorLoadButton->color(fc);
      SkyColorLoadButton->redraw();
    
   Environment *e = (Environment*)GetSelectedInstance(this);
   e->SetSkyColor( color );
}


void UserInterface::EnvTimeCB(Fl_Value_Slider *o)
{
   Environment *e = (Environment*)GetSelectedInstance(this);

   int yr = EnvYear->value();
   int mo = EnvMonth->value();
   int da = EnvDay->value();
   float tod = EnvTimeOfDay->value();
   int hr = floor(tod);
   int min = floor((tod-hr)*60.0);
   
   e->SetDateTime(yr, mo, da, hr, min, 0);
}

void UserInterface::EnvDateTimeCB(Fl_Value_Input *o)
{
   Environment *e = (Environment*)GetSelectedInstance(this);

   int yr = EnvYear->value();
   int mo = EnvMonth->value();
   int da = EnvDay->value();
   float tod = EnvTimeOfDay->value();

   e->SetDateTime(yr, mo, da, (int)tod, 0, 0);
}

void UserInterface::EnvRefPosCB(Fl_Value_Input *o)
{
   Environment *e = (Environment*)GetSelectedInstance(this);
   sgVec2 refPos;
   refPos[0] = EnvRefLat->value();
   refPos[1] = EnvRefLong->value();
   e->SetRefLatLong(refPos);
}

void UserInterface::InfRegenerateCB(Fl_Button *o)
{
   InfiniteTerrain *t = (InfiniteTerrain*)GetSelectedInstance(this);

   float dist = InfBuildDistance->value();
   float segSize = InfSegSize->value();
   int segDiv = InfSegDivisions->value();
   float vScale = InfVertScale->value();
   float hScale = InfHorizScale->value();

   t->SetBuildDistance(dist);
   t->SetSegmentSize(segSize);
   t->SetSegmentDivisions(segDiv);
   t->SetVerticalScale(vScale);
   t->SetHorizontalScale(hScale);

   t->Regenerate();
}

void UserInterface::InfSmoothCDCB(Fl_Check_Button *o)
{
   InfiniteTerrain *t = (InfiniteTerrain*)GetSelectedInstance(this);

   if (o->value()>0)
   {
      t->EnableSmoothCollisions(true);
   }
   else
   {
      t->EnableSmoothCollisions(false);
   }
}

// -- Additional GUI stuff for CloudDome. --> George
void UserInterface::CloudScaleCB(Fl_Value_Slider *o)
{
   CloudDome *cd = (CloudDome*)GetSelectedInstance(this);
   cd->setScale( cScale->value());
}

void UserInterface::CloudCutoffCB(Fl_Value_Slider *o)
{
   CloudDome *cd = (CloudDome*)GetSelectedInstance(this);
   cd->setCutoff( cCutoff->value());
}

void UserInterface::CloudExponentCB(Fl_Value_Slider *o)
{
   CloudDome *cd = (CloudDome*)GetSelectedInstance(this);
   cd->setExponent( cExponent->value());
}

void UserInterface::CloudWindCB(Fl_Value_Slider *)
{
    CloudDome *cd = (CloudDome*)GetSelectedInstance(this);
    cd->setSpeedX( cSpeedX->value());
    cd->setSpeedY( cSpeedY->value());
       
}

void UserInterface::CloudEnableCB(Fl_Check_Button *)
{
    CloudDome *cd = (CloudDome*)GetSelectedInstance(this);
    cd->setShaderEnable( cEnable->value());
       
}

void UserInterface::CloudBiasCB(Fl_Value_Slider *)
{
    CloudDome *cd = (CloudDome*)GetSelectedInstance(this);
    cd->setBias( cBias->value());
       
}

void UserInterface::CloudColorBrowserCB(Fl_Button *)
{
    CloudDome *cd = (CloudDome*)GetSelectedInstance(this);

    double r = CloudRed->value();
    double g = CloudGreen->value();
    double b = CloudBlue->value();

    fl_color_chooser("Cloud Color", r, g, b);

    CloudRed->value(r);
    CloudGreen->value(g);
    CloudBlue->value(b);

    Fl_Color cc = fl_color_cube( r*(FL_NUM_RED-1),
        g*(FL_NUM_GREEN-1),
        b*(FL_NUM_BLUE-1) );

    CloudColorLoadButton->color(cc);

    cd->setCloudColor(new osg::Vec3(CloudRed->value(),
                        CloudGreen->value(),
                        CloudBlue->value()));
}

void UserInterface::CloudColorCB(Fl_Value_Input*)
{
    CloudDome *cd = (CloudDome*)GetSelectedInstance(this);

    osg::Vec3 *ccolor = new osg::Vec3(
         CloudRed->value(),
         CloudGreen->value(),
         CloudBlue->value());

      Fl_Color fc = fl_color_cube( ccolor->x()*(FL_NUM_RED-1),
         ccolor->y()*(FL_NUM_GREEN-1),
         ccolor->z()*(FL_NUM_BLUE-1) );
      
      CloudColorLoadButton->color(fc);
      CloudColorLoadButton->redraw();

   cd->setCloudColor( ccolor );
}