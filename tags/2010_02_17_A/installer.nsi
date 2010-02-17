; define INSTALL_CHM (no value) to install the index.chm file instead of the html files (defaults to include the html files)
; use VERSION = x to define which version number to use (defaults to "anonymous-build")
; use DELTA_BUILD_DIR = x to define which delta3d subfolder contains the Delta3D binary files (defaults to "build")
; use OUTFILE = x to overwrite the output file name (defaults to "dt_win32_${VERSION}_setup.exe")


!ifndef VERSION
  !define VERSION 'anonymous-build'
!endif

!ifdef OUTFILE
  OutFile "${OUTFILE}"
!else
  OutFile dt_win32_${VERSION}_setup.exe
!endif

!ifndef DELTA_BUILD_DIR
  !define DELTA_BUILD_DIR 'build'
!endif

!ifdef NOCOMPRESS
  SetCompress off
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
!include "MUI2.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "license.txt"

; Components selection page
!insertmacro MUI_PAGE_COMPONENTS
 
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




Section "!Delta3D" Delta3DSection
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "changes.txt"
  ;File "CMakeLists.txt"
  ;File "configure"
  ;File "configure.bat"
  ;File "configure-debug"
  File "credits.txt"
  ;File "installer.nsi"
  File "license.txt"
  File "readme.txt"


  ;bin : both debug/release .dlls, all release .exe's, 3D max plugin Release binaries
  SetOutPath "$INSTDIR\${DELTA_BUILD_DIR}\bin"
  File .\${DELTA_BUILD_DIR}\bin\*.dll
  File /x *d.exe .\${DELTA_BUILD_DIR}\bin\*.exe
  ;erg, special case for testHUD due to the end 'd' in the name
  File .\${DELTA_BUILD_DIR}\bin\testHUD.exe
  File /nonfatal /x *d.dle .\.\${DELTA_BUILD_DIR}\bin\*.dle
  File /nonfatal /x *d.dlo .\.\${DELTA_BUILD_DIR}\bin\*.dlo
  
  ;bin/stplugins   STAGE release plugins
  SetOutPath "$INSTDIR\${DELTA_BUILD_DIR}\bin\stplugins"
  File .\${DELTA_BUILD_DIR}\bin\stplugins\*.dll

  ;bin\release : the Python bindinds
  SetOutPath "$INSTDIR\${DELTA_BUILD_DIR}\bin\release"
  File .\${DELTA_BUILD_DIR}\bin\release\*.pyd
  
  ;CMakeModules
  SetOutPath "$INSTDIR\CMakeModules"
  File /x .svn .\CMakeModules\*
  
  ;data
  SetOutPath "$INSTDIR\data"
  File /r /x .svn .\data\*
  
  ;demos
  SetOutPath "$INSTDIR\demos"
  File /r /x .svn /x CMakeLists.txt .\demos\*
  
  ;doc
  SetOutPath "$INSTDIR\doc"
  File "doc\doxyfile.cfg"
  File "doc\footer.html"
  
  !ifdef INSTALL_CHM
    File ".\doc\html\index.chm"
  !else
    File /r /x .svn .\doc\html
  !endif
    
  ;examples
  SetOutPath "$INSTDIR\examples"
  File /r /x .svn /x CMakeLists.txt .\examples\*
  
  ;examples/buildScripts
  SetOutPath "$INSTDIR\examples\buildScripts"
  File /r /x .svn .\examples\buildScripts\*
  
  ;ext
  SetOutPath "$INSTDIR\ext"
  File /r .\ext\*

  ;inc
  SetOutPath "$INSTDIR\inc"
  File /r /x .svn .\inc\*
  
  ;lib
  SetOutPath "$INSTDIR\${DELTA_BUILD_DIR}\lib"
  File /x *.exp /x *.pdb /x *.idb .\${DELTA_BUILD_DIR}\lib\*
  
  ;src
  SetOutPath "$INSTDIR\src"
  File /r /x .svn /x CMakeLists.txt .\src\*

  ;tests
  SetOutPath "$INSTDIR\tests"
  File /r /x .svn /x *.exp /x *.pdb /x *.idb /x *.ilk /x CMakeLists.txt .\tests\*

  ;utilities
  SetOutPath "$INSTDIR\utilities"
  File /r /x .svn /x CMakeLists.txt .\utilities\*

  ;VisualStudio
  SetOutPath "$INSTDIR\VisualStudio"
  File /r  /x .svn .\VisualStudio\*

; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Delta3D Directory.lnk" "$INSTDIR"
  !ifdef INSTALL_CHM
    CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\API Documentation.lnk" "$INSTDIR\doc\index.chm"
  !else
    CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\API Documentation.lnk" "$INSTDIR\doc\html\index.html"
  !endif
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\STAGE.lnk" "$INSTDIR\${DELTA_BUILD_DIR}\bin\STAGE.exe"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Object Viewer.lnk" "$INSTDIR\${DELTA_BUILD_DIR}\bin\ObjectViewer.exe"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Particle Editor.lnk" "$INSTDIR\${DELTA_BUILD_DIR}\bin\ParticleEditor.exe"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Animation Viewer.lnk" "$INSTDIR\${DELTA_BUILD_DIR}\bin\AnimationViewer.exe"

  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

SectionGroup /e "Addition Installations"
	
	Section /o "Install VCRedist Package" VCRedistSection
	  SetOutPath $INSTDIR
	  File /nonfatal "vcredist_x86.exe"
	  ExecWait "$INSTDIR\vcredist_x86.exe"
	SectionEnd

	; Environment Variables
	Section "Environment Variables" EnvironmentVariableSection
	!define DELTA_ROOT "$INSTDIR"
	!define DELTA_INC "%DELTA_ROOT%\inc;%DELTA_ROOT%\ext\inc;%DELTA_ROOT%\ext\inc\CEGUI"
	!define DELTA_LIB "%DELTA_ROOT%\${DELTA_BUILD_DIR}\lib;%DELTA_ROOT%\ext\lib"
	!define DELTA_DATA "%DELTA_ROOT%\data"
	
	   Push "DELTA_ROOT"
	   Push '${DELTA_ROOT}'
	   Call WriteEnvStr
	   
	   Push "DELTA_INC"
	   Push '${DELTA_INC}'
	   Call WriteEnvExpStr
	   
	   Push "DELTA_LIB"
	   Push '${DELTA_LIB}'
	   Call WriteEnvExpStr
	   
	   Push "DELTA_DATA"
	   Push '${DELTA_DATA}'
	   Call WriteEnvExpStr

	   ;PATH
	   Push "${DELTA_ROOT}\${DELTA_BUILD_DIR}\bin"
	   Call AddToPath
	   Push "${DELTA_ROOT}\ext\bin"
	   Call AddToPath
	SectionEnd
	
SectionGroupEnd ;"Additional Installations"

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

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${Delta3DSection} "The source code, external dependencies, binaries, etc."
  !insertmacro MUI_DESCRIPTION_TEXT ${VCRedistSection} "Install the Visual Studio runtime libraries (install if you don't have Visual Studio already installed)."
  !insertmacro MUI_DESCRIPTION_TEXT ${EnvironmentVariableSection} "Install the Delta3D Environment Variables"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

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
  
  ;environment variables
  MessageBox MB_YESNO "Remove Delta3D Environment Variables (DELTA_ROOT, DELTA_INC, DELTA_LIB, DELTA_DATA)?" IDYES removeEnvVars IDNO next
  removeEnvVars:
    Push "DELTA_ROOT"
    Call un.DeleteEnvStr
    Push "DELTA_INC"
    Call un.DeleteEnvStr
    Push "DELTA_LIB"
    Call un.DeleteEnvStr
    Push "DELTA_DATA"
    Call un.DeleteEnvStr
    Push "${DELTA_ROOT}\${DELTA_BUILD_DIR}\bin"
    Call un.RemoveFromPath
    Push "${DELTA_ROOT}\ext\bin"
    Call un.RemoveFromPath
  next:

  ;root
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\changes.txt"
  Delete "$INSTDIR\CMakeLists.txt"
  Delete "$INSTDIR\configure"
  Delete "$INSTDIR\configure.bat"
  Delete "$INSTDIR\configure-debug"
  Delete "$INSTDIR\credits.txt"
  Delete "$INSTDIR\installer.nsi"
  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\readme.txt"
  Delete "$INSTDIR\vcredist_x86.exe"

  ;bin
  RMDir /r $INSTDIR\${DELTA_BUILD_DIR}
  
  ;CMakeModules
  RMDIR /r $INSTDIR\CMakeModules

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
  RMDIR /r $INSTDIR\${DELTA_BUILD_DIR}\lib
  
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
  Delete "$SMPROGRAMS\$ICONS_GROUP\API Documentation.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Delta3D Directory.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\STAGE.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Object Viewer.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Particle Editor.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Animation Viewer.lnk"
 
  RMDir "$SMPROGRAMS\$ICONS_GROUP"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd







!ifndef _WriteEnvStr_nsh
!define _WriteEnvStr_nsh

!include WinMessages.nsh

!ifndef WriteEnvStr_RegKey
  !ifdef ALL_USERS
    !define WriteEnvStr_RegKey \
       'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
  !else
    !define WriteEnvStr_RegKey 'HKCU "Environment"'
  !endif
!endif

#
# WriteEnvStr - Writes an environment variable
# Note: Win9x systems requires reboot
#
# Example:
#  Push "HOMEDIR"           # name
#  Push "C:\New Home Dir\"  # value
#  Call WriteEnvStr
#
Function WriteEnvStr
  Exch $1 ; $1 has environment variable value
  Exch
  Exch $0 ; $0 has environment variable name
  Push $2

  Call IsNT
  Pop $2
  StrCmp $2 1 WriteEnvStr_NT
    ; Not on NT
    StrCpy $2 $WINDIR 2 ; Copy drive of windows (c:)
    FileOpen $2 "$2\autoexec.bat" a
    FileSeek $2 0 END
    FileWrite $2 "$\r$\nSET $0=$1$\r$\n"
    FileClose $2
    SetRebootFlag true
    Goto WriteEnvStr_done

  WriteEnvStr_NT:
      WriteRegStr ${WriteEnvStr_RegKey} $0 $1
      SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} \
        0 "STR:Environment" /TIMEOUT=5000

  WriteEnvStr_done:
    Pop $2
    Pop $0
    Pop $1
FunctionEnd

#
# WriteEnvExpStr - Writes an environment variable as an expanding string type
# Note: Win9x systems requires reboot
#
# Example:
#  Push "HOMEDIR"           # name
#  Push "C:\New Home Dir\"  # value
#  Call WriteEnvExpStr
#
Function WriteEnvExpStr
  Exch $1 ; $1 has environment variable value
  Exch
  Exch $0 ; $0 has environment variable name
  Push $2

  Call IsNT
  Pop $2
  StrCmp $2 1 WriteEnvExpStr_NT
    ; Not on NT
    StrCpy $2 $WINDIR 2 ; Copy drive of windows (c:)
    FileOpen $2 "$2\autoexec.bat" a
    FileSeek $2 0 END
    FileWrite $2 "$\r$\nSET $0=$1$\r$\n"
    FileClose $2
    SetRebootFlag true
    Goto WriteEnvExpStr_done

  WriteEnvExpStr_NT:
      WriteRegExpandStr ${WriteEnvStr_RegKey} $0 $1
      SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} \
        0 "STR:Environment" /TIMEOUT=5000

  WriteEnvExpStr_done:
    Pop $2
    Pop $0
    Pop $1
FunctionEnd


#
# un.DeleteEnvStr - Removes an environment variable
# Note: Win9x systems requires reboot
#
# Example:
#  Push "HOMEDIR"           # name
#  Call un.DeleteEnvStr
#
Function un.DeleteEnvStr
  Exch $0 ; $0 now has the name of the variable
  Push $1
  Push $2
  Push $3
  Push $4
  Push $5

  Call un.IsNT
  Pop $1
  StrCmp $1 1 DeleteEnvStr_NT
    ; Not on NT
    StrCpy $1 $WINDIR 2
    FileOpen $1 "$1\autoexec.bat" r
    GetTempFileName $4
    FileOpen $2 $4 w
    StrCpy $0 "SET $0="
    SetRebootFlag true

    DeleteEnvStr_dosLoop:
      FileRead $1 $3
      StrLen $5 $0
      StrCpy $5 $3 $5
      StrCmp $5 $0 DeleteEnvStr_dosLoop
      StrCmp $5 "" DeleteEnvStr_dosLoopEnd
      FileWrite $2 $3
      Goto DeleteEnvStr_dosLoop

    DeleteEnvStr_dosLoopEnd:
      FileClose $2
      FileClose $1
      StrCpy $1 $WINDIR 2
      Delete "$1\autoexec.bat"
      CopyFiles /SILENT $4 "$1\autoexec.bat"
      Delete $4
      Goto DeleteEnvStr_done

  DeleteEnvStr_NT:
    DeleteRegValue ${WriteEnvStr_RegKey} $0
    SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} \
      0 "STR:Environment" /TIMEOUT=5000

  DeleteEnvStr_done:
    Pop $5
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

!ifndef IsNT_KiCHiK
!define IsNT_KiCHiK

#
# [un.]IsNT - Pushes 1 if running on NT, 0 if not
#
# Example:
#   Call IsNT
#   Pop $0
#   StrCmp $0 1 +3
#     MessageBox MB_OK "Not running on NT!"
#     Goto +2
#     MessageBox MB_OK "Running on NT!"
#
!macro IsNT UN
Function ${UN}IsNT
  Push $0
  ReadRegStr $0 HKLM \
    "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  StrCmp $0 "" 0 IsNT_yes
  ; we are not NT.
  Pop $0
  Push 0
  Return

  IsNT_yes:
    ; NT!!!
    Pop $0
    Push 1
FunctionEnd
!macroend
!insertmacro IsNT ""
!insertmacro IsNT "un."

!endif ; IsNT_KiCHiK

!endif ; _WriteEnvStr_nsh









!ifndef _AddToPath_nsh
!define _AddToPath_nsh

!verbose 3
!include "WinMessages.NSH"
!verbose 4

!ifndef WriteEnvStr_RegKey
  !ifdef ALL_USERS
    !define WriteEnvStr_RegKey \
       'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
  !else
    !define WriteEnvStr_RegKey 'HKCU "Environment"'
  !endif
!endif

; AddToPath - Adds the given dir to the search path.
;        Input - head of the stack
;        Note - Win9x systems requires reboot

Function AddToPath
  Exch $0
  Push $1
  Push $2
  Push $3

  # don't add if the path doesn't exist
  IfFileExists "$0\*.*" "" AddToPath_done

  ReadEnvStr $1 PATH
  Push "$1;"
  Push "$0;"
  Call StrStr
  Pop $2
  StrCmp $2 "" "" AddToPath_done
  Push "$1;"
  Push "$0\;"
  Call StrStr
  Pop $2
  StrCmp $2 "" "" AddToPath_done
  GetFullPathName /SHORT $3 $0
  Push "$1;"
  Push "$3;"
  Call StrStr
  Pop $2
  StrCmp $2 "" "" AddToPath_done
  Push "$1;"
  Push "$3\;"
  Call StrStr
  Pop $2
  StrCmp $2 "" "" AddToPath_done

  Call IsNT
  Pop $1
  StrCmp $1 1 AddToPath_NT
    ; Not on NT
    StrCpy $1 $WINDIR 2
    FileOpen $1 "$1\autoexec.bat" a
    FileSeek $1 -1 END
    FileReadByte $1 $2
    IntCmp $2 26 0 +2 +2 # DOS EOF
      FileSeek $1 -1 END # write over EOF
    FileWrite $1 "$\r$\nSET PATH=%PATH%;$3$\r$\n"
    FileClose $1
    SetRebootFlag true
    Goto AddToPath_done

  AddToPath_NT:
    ReadRegStr $1 ${WriteEnvStr_RegKey} "PATH"
    StrCmp $1 "" AddToPath_NTdoIt
      Push $1
      Call Trim
      Pop $1
      StrCpy $0 "$1;$0"
    AddToPath_NTdoIt:
      WriteRegExpandStr ${WriteEnvStr_RegKey} "PATH" $0
      SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000

  AddToPath_done:
    Pop $3
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

; RemoveFromPath - Remove a given dir from the path
;     Input: head of the stack

Function un.RemoveFromPath
  Exch $0
  Push $1
  Push $2
  Push $3
  Push $4
  Push $5
  Push $6

  IntFmt $6 "%c" 26 # DOS EOF

  Call un.IsNT
  Pop $1
  StrCmp $1 1 unRemoveFromPath_NT
    ; Not on NT
    StrCpy $1 $WINDIR 2
    FileOpen $1 "$1\autoexec.bat" r
    GetTempFileName $4
    FileOpen $2 $4 w
    GetFullPathName /SHORT $0 $0
    StrCpy $0 "SET PATH=%PATH%;$0"
    Goto unRemoveFromPath_dosLoop

    unRemoveFromPath_dosLoop:
      FileRead $1 $3
      StrCpy $5 $3 1 -1 # read last char
      StrCmp $5 $6 0 +2 # if DOS EOF
        StrCpy $3 $3 -1 # remove DOS EOF so we can compare
      StrCmp $3 "$0$\r$\n" unRemoveFromPath_dosLoopRemoveLine
      StrCmp $3 "$0$\n" unRemoveFromPath_dosLoopRemoveLine
      StrCmp $3 "$0" unRemoveFromPath_dosLoopRemoveLine
      StrCmp $3 "" unRemoveFromPath_dosLoopEnd
      FileWrite $2 $3
      Goto unRemoveFromPath_dosLoop
      unRemoveFromPath_dosLoopRemoveLine:
        SetRebootFlag true
        Goto unRemoveFromPath_dosLoop

    unRemoveFromPath_dosLoopEnd:
      FileClose $2
      FileClose $1
      StrCpy $1 $WINDIR 2
      Delete "$1\autoexec.bat"
      CopyFiles /SILENT $4 "$1\autoexec.bat"
      Delete $4
      Goto unRemoveFromPath_done

  unRemoveFromPath_NT:
    ReadRegStr $1 ${WriteEnvStr_RegKey} "PATH"
    StrCpy $5 $1 1 -1 # copy last char
    StrCmp $5 ";" +2 # if last char != ;
      StrCpy $1 "$1;" # append ;
    Push $1
    Push "$0;"
    Call un.StrStr ; Find `$0;` in $1
    Pop $2 ; pos of our dir
    StrCmp $2 "" unRemoveFromPath_done
      ; else, it is in path
      # $0 - path to add
      # $1 - path var
      StrLen $3 "$0;"
      StrLen $4 $2
      StrCpy $5 $1 -$4 # $5 is now the part before the path to remove
      StrCpy $6 $2 "" $3 # $6 is now the part after the path to remove
      StrCpy $3 $5$6

      StrCpy $5 $3 1 -1 # copy last char
      StrCmp $5 ";" 0 +2 # if last char == ;
        StrCpy $3 $3 -1 # remove last char

      WriteRegExpandStr ${WriteEnvStr_RegKey} "PATH" $3
      SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000

  unRemoveFromPath_done:
    Pop $6
    Pop $5
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
FunctionEnd



!ifndef IsNT_KiCHiK
!define IsNT_KiCHiK

###########################################
#            Utility Functions            #
###########################################

; IsNT
; no input
; output, top of the stack = 1 if NT or 0 if not
;
; Usage:
;   Call IsNT
;   Pop $R0
;  ($R0 at this point is 1 or 0)

!macro IsNT un
Function ${un}IsNT
  Push $0
  ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  StrCmp $0 "" 0 IsNT_yes
  ; we are not NT.
  Pop $0
  Push 0
  Return

  IsNT_yes:
    ; NT!!!
    Pop $0
    Push 1
FunctionEnd
!macroend
!insertmacro IsNT ""
!insertmacro IsNT "un."

!endif ; IsNT_KiCHiK

; StrStr
; input, top of stack = string to search for
;        top of stack-1 = string to search in
; output, top of stack (replaces with the portion of the string remaining)
; modifies no other variables.
;
; Usage:
;   Push "this is a long ass string"
;   Push "ass"
;   Call StrStr
;   Pop $R0
;  ($R0 at this point is "ass string")

!macro StrStr un
Function ${un}StrStr
Exch $R1 ; st=haystack,old$R1, $R1=needle
  Exch    ; st=old$R1,haystack
  Exch $R2 ; st=old$R1,old$R2, $R2=haystack
  Push $R3
  Push $R4
  Push $R5
  StrLen $R3 $R1
  StrCpy $R4 0
  ; $R1=needle
  ; $R2=haystack
  ; $R3=len(needle)
  ; $R4=cnt
  ; $R5=tmp
  loop:
    StrCpy $R5 $R2 $R3 $R4
    StrCmp $R5 $R1 done
    StrCmp $R5 "" done
    IntOp $R4 $R4 + 1
    Goto loop
done:
  StrCpy $R1 $R2 "" $R4
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Exch $R1
FunctionEnd
!macroend
!insertmacro StrStr ""
!insertmacro StrStr "un."

!endif ; _AddToPath_nsh

Function Trim ; Added by Pelaca
	Exch $R1
	Push $R2
Loop:
	StrCpy $R2 "$R1" 1 -1
	StrCmp "$R2" " " RTrim
	StrCmp "$R2" "$\n" RTrim
	StrCmp "$R2" "$\r" RTrim
	StrCmp "$R2" ";" RTrim
	GoTo Done
RTrim:
	StrCpy $R1 "$R1" -1
	Goto Loop
Done:
	Pop $R2
	Exch $R1
FunctionEnd
