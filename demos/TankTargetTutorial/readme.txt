Delta3D - Tank Target Tutorial

Note: To run in Visual Studio, you must edit the Project's Properties as follows:
  Debugging->Command : gamestart.exe
  Debugging->Command Arguments : TutorialLibrary
  Debugging->Working Directory : $(DELTA_ROOT)/demos/TankTargetTutorial
  
  These settings are not saved in the VS project file, only in a local settings.
  
This is a simple demo app which unfortunately means there is no in game help for keyboard commands. The following commands can be used in game: 

<space> - Start/stop the tank's engine (so you can move)
<I,J,K,L> - Turn & Move the tank (once engine is started)
<W,A,S,D> - Turn & Move the camera (independent of the tank)
<F> - Damage a blimp (if one is targeted)
<R> - Reset everything
<P> - Reloads and reapplies all the shaders (for artist testing)
<Enter> - Give the tank a little speed boost
<Insert> - Toggle drawing statistics

This application is also capable of doing a full record and playback of all behaviors. As this is a demo application, it's someone tricky to explain. Basically, you have to run the app once as normal and begin a recording by pressing '2'. Once you are finished, return to IDLE with '1' and exit. Then, add '--startPlayback 1 --mapName mapone_playback.xml' to your command line args and restart the app. Alternately, you can just run 'GameStartWithPlayback.bat'. 

The following are record and playback commands:
<1> - Return to 'IDLE' mode from Record or Playback
<2> - Begin 'RECORD'. All messages are recorded to the files, "D3DDefaultMessageLog.dli" and "D3DDefaultMessageLog.dlm".
<3> - Restart 'PLAYBACK'. This can only be done if you used --startPlayback 1 and specified a --mapName. 
<6> - Pause game play. Only available in PLAYBACK.
<7> - Slow down. Only available in PLAYBACK. 
<8> - Speed Up. Only available in PLAYBACK.

