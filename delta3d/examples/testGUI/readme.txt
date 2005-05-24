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
- Open examples/examples.sln.
- Build testGUI.
- Start testGUI.
- To load from XML: open a command window next to testGUI.exe (in either the Debug
  or Release folder) and type: testGUI.exe ../gui.xml
  

Win32 Binary:
- Go to examples/testGUI/Release.
- Double-click testGUI.exe.
- To load from XML: open a command window next to testGUI.exe (in either the Debug
  or Release folder) and type: testGUI.exe ../gui.xml

Linux Source:
- Run 'cmake .' from examples/ directory.
- To build all examples: run 'make' from examples/
- To build only testGUI: run 'make' from examples/testGUI.
- Enter examples/testGUI directory.
- Run 'testGUI'.
- To load from XML: testGUI.exe gui.xml

Controls
--------
Esc  -  Exit
 

