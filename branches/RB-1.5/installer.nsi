

!ifndef VERSION
  !define VERSION 'anonymous-build'
!endif

!ifdef OUTFILE
  OutFile "${OUTFILE}"
!else
  OutFile dt-${VERSION}-setup.exe
!endif

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Delta3D"
!define PRODUCT_VERSION ${VERSION}
!define PRODUCT_PUBLISHER "Delta3D"
!define PRODUCT_WEB_SITE "http://www.delta3d.org"
!define PRODUCT_DIR_REGKEY "Software\Delta3D"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "license.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "Delta3D_${PRODUCT_VERSION}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
;!define MUI_FINISHPAGE_RUN "$INSTDIR\AppMainExe.exe"
!define MUI_FINISHPAGE_LINK "Visit Delta3D.org for the latest news and support"
!define MUI_FINISHPAGE_LINK_LOCATION "http://www.delta3d.org"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
InstallDir "$PROGRAMFILES\Delta3D_${PRODUCT_VERSION}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "changes.txt"
  File "credits.txt"
  File "delta3d.py"
  File "gch.py"
  File "installer.nsi"
  File "license.txt"
  File "macosx.py"
  File "qt4.py"
  File "readme.txt"
  File "scons_template"
  File "SConstruct"

  ;bin
  SetOutPath "$INSTDIR\bin"
  File .\bin\*.dll
  File /x *d.exe .\bin\*.exe
  
  ;data
  SetOutPath "$INSTDIR\data"
  File /r /x .svn .\data\*
  
  ;demos
  SetOutPath "$INSTDIR\demos"
  File /r /x .svn .\demos\*
  
  ;doc
  SetOutPath "$INSTDIR\doc"
  File "doc\doxyfile.cfg"
  File "doc\footer.html"
  File "doc\SConscript"
  File /r /x .svn .\doc\html
  
  ;examples
  SetOutPath "$INSTDIR\examples"
  File /r /x .svn /x *.obj /x *.pch /x *.pdb /x *.idb /x *.ilk /x *.htm /x windows-msvc* .\examples\*
  
  ;ext
  SetOutPath "$INSTDIR\ext"
  File /r .\ext\*

  ;inc
  SetOutPath "$INSTDIR\inc"
  File /r /x .svn .\inc\*
  
  ;lib
  SetOutPath "$INSTDIR\lib"
  File /x *.exp .\lib\*
  
  ;macosx
  SetOutPath "$INSTDIR\macosx"
  File /r /x .svn .\macosx\*
  
  ;src
  SetOutPath "$INSTDIR\src"
  File /r /x .svn /x *.obj /x *.pch /x *.pdb /x *.ilk /x *.idb /x *.htm /x windows-msvc* .\src\*

  ;tests
  SetOutPath "$INSTDIR\tests"
  File /r /x .svn /x *.obj /x *.pch /x *.pdb /x *.ilk /x *.idb /x *.htm /x windows-msvc* .\tests\*

  ;utilities
  SetOutPath "$INSTDIR\utilities"
  File /r /x .svn /x *.obj /x *.pch /x *.pdb /x *.idb /x *.htm /x windows-msvc* .\utilities\*

  ;VisualStudio
  SetOutPath "$INSTDIR\VisualStudio"
  File /r /x .svn /x *.suo /x *.ncb /x Debug /x Release /x Makefile* .\VisualStudio\*

; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  ;CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Delta3D.lnk" "$INSTDIR\AppMainExe.exe"
  ;CreateShortCut "$DESKTOP\Delta3D.lnk" "$INSTDIR\AppMainExe.exe"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\API Documentation.lnk" "$INSTDIR\doc\html\index.html"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Delta3D Directory.lnk" "$INSTDIR"

  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -AdditionalIcons
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Delta3D.org.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" $INSTDIR
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  ;WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\AppMainExe.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  ;root
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\changes.txt"
  Delete "$INSTDIR\credits.txt"
  Delete "$INSTDIR\delta3d.py"
  Delete "$INSTDIR\gch.py"
  Delete "$INSTDIR\installer.nsi"
  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\macosx.py"
  Delete "$INSTDIR\qt4.py"
  Delete "$INSTDIR\readme.txt"
  Delete "$INSTDIR\scons_template"
  Delete "$INSTDIR\SConstruct"

  ;bin
  RMDir /r $INSTDIR\bin
  
  ;data
  RMDIR /r $INSTDIR\data
  
  ;demos
  RMDIR /r $INSTDIR\demos
  
  ;doc
  RMDIR /r $INSTDIR\doc
  
  ;examples
  RMDIR /r $INSTDIR\examples
  
  ;ext
  RMDIR /r $INSTDIR\ext
  
  ;inc
  RMDIR /r $INSTDIR\inc
  
  ;lib
  RMDIR /r $INSTDIR\lib
  
  ;macosx
  RMDIR /r $INSTDIR\macosx
  
  ;src
  RMDIR /r $INSTDIR\src

  ;tests
  RMDIR /r $INSTDIR\tests
  
  ;utilities
  RMDIR /r $INSTDIR\utilities

  ;VisualStudio
  RMDIR /r $INSTDIR\VisualStudio

  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Delta3D.org.lnk"
  ;Delete "$DESKTOP\Delta3D.lnk"
  ;Delete "$SMPROGRAMS\$ICONS_GROUP\Delta3D.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\API Documentation.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Delta3D Directory.lnk"

  RMDir "$SMPROGRAMS\$ICONS_GROUP"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd