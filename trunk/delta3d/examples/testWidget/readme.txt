########################################
#                                      #
# Delta3D Examples: testWidget         #
#                                      #
########################################

Uses modules: dtCore, dtABC

Purpose
-------
To demonstrate the integrate of a Delta3D window into a FLTK widget. It
uses the Widget class as an interface to Delta3D and the fl_dt_window
template to stuff it into a FLTK window. Framerate is displayed in the
upper left corner.

Note: The camera movement on Linux is a bit jumpy due to a hack that
      overcomes a bug coming from a conflict between Producer and FLTK
      mouse callbacks. 

Instructions
------------
Win32 Source: 
- Open examples/examples.sln.
- Build testWidget.
- Start testWidget.

Win32 Binary:
- Go to examples/testWidget/Release.
- Double-click testWidget.exe.

Linux Source:
- Run 'cmake .' from examples/ directory.
- To build all examples: run 'make' from examples/
- To build only testWidget: run 'make' from examples/testWidget.
- Enter examples/testWidget directory.
- Run 'testWidget'.

Controls
--------
Left   Mouse Button  -  Rotate plane
Right  Mouse Button  -  Translate plane
Middle Mouse Button  -  Zoom plane



