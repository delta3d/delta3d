#ifndef DELTA_GUI_FLTK
#define DELTA_GUI_FLTK

// generated by Fast Light User Interface Designer (fluid) version 1.0105

#include <FL/Fl.H>
#include "base.h"
///Do not create directly - use dtCore::GUI instead
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Browser.H>
#include <FL/Fl_Return_Button.H>

class DT_EXPORT UserInterface {
public:
  Fl_Double_Window* make_window();
  Fl_Double_Window *MainWindow;
  Fl_Browser *InstanceList;
private:
  inline void cb_InstanceList_i(Fl_Browser*, void*);
  static void cb_InstanceList(Fl_Browser*, void*);
  Fl_Box *InstanceClassName;
  Fl_Input *BaseName;
  inline void cb_BaseName_i(Fl_Input*, void*);
  static void cb_BaseName(Fl_Input*, void*);
public:
  Fl_Group *TransformGroup;
private:
  Fl_Value_Input *TransformX;
  inline void cb_TransformX_i(Fl_Value_Input*, void*);
  static void cb_TransformX(Fl_Value_Input*, void*);
  Fl_Value_Input *TransformY;
  inline void cb_TransformY_i(Fl_Value_Input*, void*);
  static void cb_TransformY(Fl_Value_Input*, void*);
  Fl_Value_Input *TransformZ;
  inline void cb_TransformZ_i(Fl_Value_Input*, void*);
  static void cb_TransformZ(Fl_Value_Input*, void*);
  Fl_Value_Input *TransformH;
  inline void cb_TransformH_i(Fl_Value_Input*, void*);
  static void cb_TransformH(Fl_Value_Input*, void*);
  Fl_Value_Input *TransformP;
  inline void cb_TransformP_i(Fl_Value_Input*, void*);
  static void cb_TransformP(Fl_Value_Input*, void*);
  Fl_Value_Input *TransformR;
  inline void cb_TransformR_i(Fl_Value_Input*, void*);
  static void cb_TransformR(Fl_Value_Input*, void*);
  Fl_Round_Button *TransformCSAbsButton;
  inline void cb_TransformCSAbsButton_i(Fl_Round_Button*, void*);
  static void cb_TransformCSAbsButton(Fl_Round_Button*, void*);
  Fl_Round_Button *TransformCSRelButton;
  inline void cb_TransformCSRelButton_i(Fl_Round_Button*, void*);
  static void cb_TransformCSRelButton(Fl_Round_Button*, void*);
  Fl_Browser *TransformChildList;
  inline void cb__i(Fl_Button*, void*);
  static void cb_(Fl_Button*, void*);
  inline void cb_1_i(Fl_Button*, void*);
  static void cb_1(Fl_Button*, void*);
  Fl_Output *TransformParentText;
public:
  Fl_Group *ObjectGroup;
private:
  Fl_Input *ObjectFilename;
  inline void cb_ObjectFilename_i(Fl_Input*, void*);
  static void cb_ObjectFilename(Fl_Input*, void*);
  inline void cb_2_i(Fl_Button*, void*);
  static void cb_2(Fl_Button*, void*);
public:
  Fl_Group *CameraGroup;
private:
  Fl_Value_Input *CameraClearRed;
  inline void cb_CameraClearRed_i(Fl_Value_Input*, void*);
  static void cb_CameraClearRed(Fl_Value_Input*, void*);
  Fl_Value_Input *CameraClearGreen;
  inline void cb_CameraClearGreen_i(Fl_Value_Input*, void*);
  static void cb_CameraClearGreen(Fl_Value_Input*, void*);
  Fl_Value_Input *CameraClearBlue;
  inline void cb_CameraClearBlue_i(Fl_Value_Input*, void*);
  static void cb_CameraClearBlue(Fl_Value_Input*, void*);
  Fl_Button *CameraClearLoadButton;
  inline void cb_CameraClearLoadButton_i(Fl_Button*, void*);
  static void cb_CameraClearLoadButton(Fl_Button*, void*);
  Fl_Choice *CameraSceneChoice;
  inline void cb_CameraSceneChoice_i(Fl_Choice*, void*);
  static void cb_CameraSceneChoice(Fl_Choice*, void*);
  Fl_Choice *CameraWinChoice;
  inline void cb_CameraWinChoice_i(Fl_Choice*, void*);
  static void cb_CameraWinChoice(Fl_Choice*, void*);
public:
  Fl_Group *WindowGroup;
private:
  Fl_Value_Input *WinPosW;
  inline void cb_WinPosW_i(Fl_Value_Input*, void*);
  static void cb_WinPosW(Fl_Value_Input*, void*);
  Fl_Value_Input *WinPosH;
  inline void cb_WinPosH_i(Fl_Value_Input*, void*);
  static void cb_WinPosH(Fl_Value_Input*, void*);
  Fl_Menu_Button *WinSizeButton;
  inline void cb_WinSizeButton_i(Fl_Menu_Button*, void*);
  static void cb_WinSizeButton(Fl_Menu_Button*, void*);
  static Fl_Menu_Item menu_WinSizeButton[];
  Fl_Value_Input *WinPosX;
  inline void cb_WinPosX_i(Fl_Value_Input*, void*);
  static void cb_WinPosX(Fl_Value_Input*, void*);
  Fl_Value_Input *WinPosY;
  inline void cb_WinPosY_i(Fl_Value_Input*, void*);
  static void cb_WinPosY(Fl_Value_Input*, void*);
  Fl_Check_Button *WinCursorToggle;
  inline void cb_WinCursorToggle_i(Fl_Check_Button*, void*);
  static void cb_WinCursorToggle(Fl_Check_Button*, void*);
  Fl_Input *WinTitle;
  inline void cb_WinTitle_i(Fl_Input*, void*);
  static void cb_WinTitle(Fl_Input*, void*);
  Fl_Check_Button *WinFullScreenToggle;
  inline void cb_WinFullScreenToggle_i(Fl_Check_Button*, void*);
  static void cb_WinFullScreenToggle(Fl_Check_Button*, void*);
public:
  Fl_Group *SkyBoxGroup;
private:
  Fl_Value_Input *SkyBoxBaseRed;
  inline void cb_SkyBoxBaseRed_i(Fl_Value_Input*, void*);
  static void cb_SkyBoxBaseRed(Fl_Value_Input*, void*);
  Fl_Value_Input *SkyBoxBaseGreen;
  inline void cb_SkyBoxBaseGreen_i(Fl_Value_Input*, void*);
  static void cb_SkyBoxBaseGreen(Fl_Value_Input*, void*);
  Fl_Value_Input *SkyBoxBaseBlue;
  inline void cb_SkyBoxBaseBlue_i(Fl_Value_Input*, void*);
  static void cb_SkyBoxBaseBlue(Fl_Value_Input*, void*);
  Fl_Button *SkyBoxBaseColorLoadButton;
  inline void cb_SkyBoxBaseColorLoadButton_i(Fl_Button*, void*);
  static void cb_SkyBoxBaseColorLoadButton(Fl_Button*, void*);
public:
  Fl_Group *EnvironmentGroup;
  Fl_Group *EnvFogColorGroup;
private:
  Fl_Value_Input *FogRed;
  inline void cb_FogRed_i(Fl_Value_Input*, void*);
  static void cb_FogRed(Fl_Value_Input*, void*);
  Fl_Value_Input *FogGreen;
  inline void cb_FogGreen_i(Fl_Value_Input*, void*);
  static void cb_FogGreen(Fl_Value_Input*, void*);
  Fl_Value_Input *FogBlue;
  inline void cb_FogBlue_i(Fl_Value_Input*, void*);
  static void cb_FogBlue(Fl_Value_Input*, void*);
  Fl_Button *FogColorLoadButton;
  inline void cb_FogColorLoadButton_i(Fl_Button*, void*);
  static void cb_FogColorLoadButton(Fl_Button*, void*);
public:
  Fl_Group *EnvAdvFogGroup;
private:
  Fl_Value_Input *AdvFogTurbidity;
  inline void cb_AdvFogTurbidity_i(Fl_Value_Input*, void*);
  static void cb_AdvFogTurbidity(Fl_Value_Input*, void*);
  Fl_Value_Input *AdvFogEnergy;
  inline void cb_AdvFogEnergy_i(Fl_Value_Input*, void*);
  static void cb_AdvFogEnergy(Fl_Value_Input*, void*);
  Fl_Value_Input *AdvFogMolecules;
  inline void cb_AdvFogMolecules_i(Fl_Value_Input*, void*);
  static void cb_AdvFogMolecules(Fl_Value_Input*, void*);
  Fl_Value_Input *EnvFogVis;
  inline void cb_EnvFogVis_i(Fl_Value_Input*, void*);
  static void cb_EnvFogVis(Fl_Value_Input*, void*);
  Fl_Value_Input *EnvFogNear;
  inline void cb_EnvFogNear_i(Fl_Value_Input*, void*);
  static void cb_EnvFogNear(Fl_Value_Input*, void*);
public:
  Fl_Choice *EnvFogMode;
private:
  inline void cb_EnvFogMode_i(Fl_Choice*, void*);
  static void cb_EnvFogMode(Fl_Choice*, void*);
  static Fl_Menu_Item menu_EnvFogMode[];
  Fl_Check_Button *EnvFogEnable;
  inline void cb_EnvFogEnable_i(Fl_Check_Button*, void*);
  static void cb_EnvFogEnable(Fl_Check_Button*, void*);
  Fl_Value_Input *SkyRed;
  inline void cb_SkyRed_i(Fl_Value_Input*, void*);
  static void cb_SkyRed(Fl_Value_Input*, void*);
  Fl_Value_Input *SkyGreen;
  inline void cb_SkyGreen_i(Fl_Value_Input*, void*);
  static void cb_SkyGreen(Fl_Value_Input*, void*);
  Fl_Value_Input *SkyBlue;
  inline void cb_SkyBlue_i(Fl_Value_Input*, void*);
  static void cb_SkyBlue(Fl_Value_Input*, void*);
  Fl_Button *SkyColorLoadButton;
  inline void cb_SkyColorLoadButton_i(Fl_Button*, void*);
  static void cb_SkyColorLoadButton(Fl_Button*, void*);
  Fl_Value_Input *EnvSunRed;
  Fl_Value_Input *EnvSunGreen;
  Fl_Value_Input *EnvSunBlue;
  Fl_Value_Input *EnvRefLat;
  inline void cb_EnvRefLat_i(Fl_Value_Input*, void*);
  static void cb_EnvRefLat(Fl_Value_Input*, void*);
  Fl_Value_Input *EnvRefLong;
  inline void cb_EnvRefLong_i(Fl_Value_Input*, void*);
  static void cb_EnvRefLong(Fl_Value_Input*, void*);
public:
  Fl_Value_Slider *EnvTimeOfDay;
private:
  inline void cb_EnvTimeOfDay_i(Fl_Value_Slider*, void*);
  static void cb_EnvTimeOfDay(Fl_Value_Slider*, void*);
public:
  Fl_Value_Input *EnvYear;
private:
  inline void cb_EnvYear_i(Fl_Value_Input*, void*);
  static void cb_EnvYear(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *EnvMonth;
private:
  inline void cb_EnvMonth_i(Fl_Value_Input*, void*);
  static void cb_EnvMonth(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *EnvDay;
private:
  inline void cb_EnvDay_i(Fl_Value_Input*, void*);
  static void cb_EnvDay(Fl_Value_Input*, void*);
public:
  Fl_Group *InfTerrainGroup;
  Fl_Value_Input *InfBuildDistance;
  Fl_Value_Input *InfSegSize;
  Fl_Value_Input *InfSegDivisions;
  Fl_Value_Input *InfVertScale;
  Fl_Value_Input *InfHorizScale;
  Fl_Button *InfRegenerateButton;
private:
  inline void cb_InfRegenerateButton_i(Fl_Button*, void*);
  static void cb_InfRegenerateButton(Fl_Button*, void*);
public:
  Fl_Check_Button *InfSmoothCollision;
private:
  inline void cb_InfSmoothCollision_i(Fl_Check_Button*, void*);
  static void cb_InfSmoothCollision(Fl_Check_Button*, void*);
public:
  Fl_Group *CloudEditor;
  Fl_Value_Slider *cScale;
private:
  inline void cb_cScale_i(Fl_Value_Slider*, void*);
  static void cb_cScale(Fl_Value_Slider*, void*);
public:
  Fl_Value_Slider *cCutoff;
private:
  inline void cb_cCutoff_i(Fl_Value_Slider*, void*);
  static void cb_cCutoff(Fl_Value_Slider*, void*);
public:
  Fl_Value_Slider *cExponent;
private:
  inline void cb_cExponent_i(Fl_Value_Slider*, void*);
  static void cb_cExponent(Fl_Value_Slider*, void*);
public:
  Fl_Value_Slider *cBias;
private:
  inline void cb_cBias_i(Fl_Value_Slider*, void*);
  static void cb_cBias(Fl_Value_Slider*, void*);
public:
  Fl_Value_Input *CloudRed;
private:
  inline void cb_CloudRed_i(Fl_Value_Input*, void*);
  static void cb_CloudRed(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *CloudGreen;
private:
  inline void cb_CloudGreen_i(Fl_Value_Input*, void*);
  static void cb_CloudGreen(Fl_Value_Input*, void*);
public:
  Fl_Value_Input *CloudBlue;
private:
  inline void cb_CloudBlue_i(Fl_Value_Input*, void*);
  static void cb_CloudBlue(Fl_Value_Input*, void*);
  Fl_Button *CloudColorLoadButton;
  inline void cb_CloudColorLoadButton_i(Fl_Button*, void*);
  static void cb_CloudColorLoadButton(Fl_Button*, void*);
public:
  Fl_Value_Slider *cSpeedX;
private:
  inline void cb_cSpeedX_i(Fl_Value_Slider*, void*);
  static void cb_cSpeedX(Fl_Value_Slider*, void*);
public:
  Fl_Value_Slider *cSpeedY;
private:
  inline void cb_cSpeedY_i(Fl_Value_Slider*, void*);
  static void cb_cSpeedY(Fl_Value_Slider*, void*);
public:
  Fl_Check_Button *cEnable;
private:
  inline void cb_cEnable_i(Fl_Check_Button*, void*);
  static void cb_cEnable(Fl_Check_Button*, void*);
public:
  Fl_Double_Window *SelectWindow;
private:
  Fl_Check_Browser *SelectList;
  Fl_Return_Button *SelectWinAddButton;
public:
  Fl_Button *SelectWinCancelButton;
  void SelectInstance(void);
private:
  static dtCore::Base *GetSelectedInstance( UserInterface *ui);
  void BaseNameCB(Fl_Input *o);
  void TransformPosCB( Fl_Value_Input *);
  void CameraClearColorBrowserCB(Fl_Button*);
  void CameraClearColorCB(Fl_Value_Input* );
  void CameraWinCB(Fl_Choice *);
  void CameraSceneCB(Fl_Choice *);
  void WinPosCB( Fl_Value_Input * );
  void WinSizeCB( Fl_Menu_Button * );
  void WinCursorCB( Fl_Check_Button * );
  void WinFullScreenCB( Fl_Check_Button * );
  void WinTitleCB( Fl_Input * );
  void ObjectFileCB( Fl_Input * );
  void ObjectLoadFileCB( Fl_Button * );
  void TransformCSCB( Fl_Round_Button *);
  void TransformAddChildCB( Fl_Button *);
  void TransformRemChildCB( Fl_Button *);
  void SkyBoxBaseColorCB(Fl_Value_Input *);
  void SkyBoxBaseColorBrowserCB(Fl_Button*);
  void EnvFogColorCB(Fl_Value_Input *);
  void EnvAdvFogCB(Fl_Value_Input *);
  void EnvFogColorBrowserCB(Fl_Button*);
  void EnvFogVisCB(Fl_Value_Input*);
  void EnvFogNearCB(Fl_Value_Input*);
  void EnvFogEnableCB(Fl_Check_Button*);
  void EnvFogModeCB(Fl_Choice*);
  void EnvSkyColorCB(Fl_Value_Input*);
  void EnvSkyColorBrowserCB(Fl_Button*);
  void EnvDateTimeCB(Fl_Value_Input*);
  void EnvTimeCB(Fl_Value_Slider*);
  void EnvRefPosCB(Fl_Value_Input *);
  void InfRegenerateCB(Fl_Button*);
  void InfSmoothCDCB(Fl_Check_Button*);
  void CloudColorCB(Fl_Value_Input *);
  void CloudColorBrowserCB(Fl_Button*);
  void CloudScaleCB(Fl_Value_Slider*);
  void CloudCutoffCB(Fl_Value_Slider*);
  void CloudExponentCB(Fl_Value_Slider*);
  void CloudWindCB(Fl_Value_Slider*);
  void CloudEnableCB(Fl_Check_Button*);
  void CloudBiasCB(Fl_Value_Slider*);
};

#ifdef _WIN32

#pragma comment(lib, "fltk.lib")
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "comctl32.lib")

#endif // _WIN32

#endif // DELTA_GUI_FLTK
