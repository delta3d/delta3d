// generated by Fast Light User Interface Designer (fluid) version 1.0105

#ifndef UserInterface_h
#define UserInterface_h
#include <FL/Fl.H>
#include <string>
#include <FL/Fl_Choice.h>
#include <FL/Fl_File_Chooser.h>
#include "viewwindow.h"
#include "viewstate.h"
#define kDefFltr "Geometry Files (*.{osg,ive,flt,3ds})\tOSG Files (*.osg)\tIVE Files (*.ive)\tFlight Files (*.flt)\t3ds Export Files (*.3ds)"
class ChoicePopUp;   // forward reference
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "viewwindow.h"

class UserInterface {
  int mArgc;
  char** mArgv;
  bool mMenusEnabled;
public:
  UserInterface( int argc = 0L, char** argv = NULL ) ;
private:
  Fl_Double_Window *UIMainWindow;
  Fl_Menu_Bar *UIMenu;
  static Fl_Menu_Item menu_UIMenu[];
  static Fl_Menu_Item *UIMenuFile;
  static Fl_Menu_Item *UIMenuFileOpen;
  inline void cb_UIMenuFileOpen_i(Fl_Menu_*, void*);
  static void cb_UIMenuFileOpen(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuFileSaveAs;
  inline void cb_UIMenuFileSaveAs_i(Fl_Menu_*, void*);
  static void cb_UIMenuFileSaveAs(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuFileQuit;
  inline void cb_UIMenuFileQuit_i(Fl_Menu_*, void*);
  static void cb_UIMenuFileQuit(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuView;
  static Fl_Menu_Item *UIMenuViewDisplay;
  static Fl_Menu_Item *UIMenuViewDisplayCompass;
  inline void cb_UIMenuViewDisplayCompass_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewDisplayCompass(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewDisplayXYPlane;
  inline void cb_UIMenuViewDisplayXYPlane_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewDisplayXYPlane(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewDisplayYZPlane;
  inline void cb_UIMenuViewDisplayYZPlane_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewDisplayYZPlane(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewDisplayZXPlane;
  inline void cb_UIMenuViewDisplayZXPlane_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewDisplayZXPlane(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewScene;
  static Fl_Menu_Item *UIMenuViewScenePolygon;
  inline void cb_UIMenuViewScenePolygon_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewScenePolygon(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewSceneWireframe;
  inline void cb_UIMenuViewSceneWireframe_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewSceneWireframe(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewSceneScribe;
  inline void cb_UIMenuViewSceneScribe_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewSceneScribe(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewSceneTexture;
  inline void cb_UIMenuViewSceneTexture_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewSceneTexture(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewSceneLighting;
  inline void cb_UIMenuViewSceneLighting_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewSceneLighting(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewModel;
  static Fl_Menu_Item *UIMenuViewModelPolygon;
  inline void cb_UIMenuViewModelPolygon_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewModelPolygon(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewModelWireframe;
  inline void cb_UIMenuViewModelWireframe_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewModelWireframe(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewModelScribe;
  inline void cb_UIMenuViewModelScribe_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewModelScribe(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewModelTexture;
  inline void cb_UIMenuViewModelTexture_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewModelTexture(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewModelLighting;
  inline void cb_UIMenuViewModelLighting_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewModelLighting(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuViewResetCam;
  inline void cb_UIMenuViewResetCam_i(Fl_Menu_*, void*);
  static void cb_UIMenuViewResetCam(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuMotion;
  static Fl_Menu_Item *UIMenuMotionFly;
  inline void cb_UIMenuMotionFly_i(Fl_Menu_*, void*);
  static void cb_UIMenuMotionFly(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuMotionOrbit;
  inline void cb_UIMenuMotionOrbit_i(Fl_Menu_*, void*);
  static void cb_UIMenuMotionOrbit(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuMotionUFO;
  inline void cb_UIMenuMotionUFO_i(Fl_Menu_*, void*);
  static void cb_UIMenuMotionUFO(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuMotionWalk;
  inline void cb_UIMenuMotionWalk_i(Fl_Menu_*, void*);
  static void cb_UIMenuMotionWalk(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuMotionJoy1;
  inline void cb_UIMenuMotionJoy1_i(Fl_Menu_*, void*);
  static void cb_UIMenuMotionJoy1(Fl_Menu_*, void*);
  static Fl_Menu_Item *UIMenuMotionJoy2;
  inline void cb_UIMenuMotionJoy2_i(Fl_Menu_*, void*);
  static void cb_UIMenuMotionJoy2(Fl_Menu_*, void*);
  ViewWindow *UIViewWindow;
  ChoicePopUp *UIFileList;
public:
  void Show();
  void Quit();
  void LoadFile( std::string pathfile );
  void SaveFileAs( std::string pathfile );
  void SelectFile( int indx );
  void UpdateSettings( const ViewState* pViewState );
  void ToggleWireframeModel();
  void ToggleWireframeScene();
  void ToggleLightingModel();
  void ToggleLightingScene();
  void ToggleTextureModel();
  void ToggleTextureScene();
  void ResetCam();
  void ToggleCompass();
  void ToggleXYPlane();
  void ToggleYZPlane();
  void ToggleZXPlane();
};

class ChoicePopUp : public Fl_Choice {
public:
  ChoicePopUp( int nX, int nY, int nW, int nH, const char* pL = 0 ) ;
  void InsertFile( std::string szPathFile, ViewWindow* pViewWindow );
  static void SelectSCB( Fl_Widget* pWidget, void* pViewWindow );
  void SelectCB( ViewWindow* pViewWindow );
  void SelectFile( int nIndx );
};
#endif
