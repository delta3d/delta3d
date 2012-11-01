########################################
#                                      #
# Delta3D Examples: testNetwork        #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil, dtNet

Purpose
-------
Provides a simple demonstration of how to use the dtNet module in an application.
This example creates a host or a client and passes position information to all
connected players.  The remote players are all represented by the same 3D model.

Instructions
------------
Running testNetwork with no command line arguments creates a server.  Supplying
a hostname or IP address on the command line will create a client and try to 
connect to the host at that address.


Controls
--------
Esc    -  Quit application
P      -  Send a ping packet to measure network lag

Fly Motion Model:
Up     -  Look up
Down   -  Look down
Left   -  Turn left
Right  -  Turn right
S      -  Move forward
W      -  Move backward
Left  Mouse Button - Look up/down, turn left/right
Right Mouse Button - Move forward/backward
