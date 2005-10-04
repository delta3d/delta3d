########################################
#                                      #
# Delta3D Examples: testGUI            #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtGUI

Purpose
-------
To demonstrate basic functionality of the dtGUI::CEUIDrawable class. A SliderBar
controls the opacity of the box. "Delta3D" is displayed inside a TextBox.
An EditableTextBox can be... surprise surprise... edited. The Button exits 
the application. The cursor also changes to a different icon within the window.

Instructions
------------
Win32 Source: 
- Open VisualStudio\examples\testGUI\testGUI.sln.
- Build testGUI.
- Start testGUI.
- To load from XML: open a command window next to testGUI.exe (in either the Debug
  or Release folder) and type: testGUI.exe ..\examples\testGUI\gui.xml
  
Win32 Binary:
- Go to bin.
- Double-click testGUI.exe.
- To load from XML: open a command window next to testGUI.exe (in either the Debug
  or Release folder) and type: testGUI.exe ..\examples\tesGUI\gui.xml

Linux Source:
- To build only testGUI, from the root delta3d folder:
  scons bin/testGUI

Linux Binary:
- cd bin
- ./testGUI
  ./testGUI ../examples/testGUI/gui.xml

Controls
--------
Esc  -  Exit
 

