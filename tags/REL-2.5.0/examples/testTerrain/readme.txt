########################################
#                                      #
# Delta3D Examples: testTerrain        #
#                                      #
########################################

Uses modules: dtCore, dtABC, dtUtil, dtTerrain

Purpose
-------
To demonstrate the dtTerrain terrain engine. This application uses
source Digital Terrain Eleveation Data (DTED) at runtime to procedurally 
create triangles represting a given latitude and longitude. The SOARX
rendering algorithm is used to minimize the numbers of triangles drawn
per frame.


Command Line Options
--------------------
--dted <level> <latitude> <longitude>   Sets the DTED level and starting 
                                            latitude and longitude
--enable-vegetation                     Enables the placement of LCC 
                                            vegetation
--geo <geopath>                         Sets file for geospecific data
--geodrape <path>                       Sets file for geospecific drape texture across the terrain
-c <cachepath>                          Sets the cache path
-h or --help                            Displays command line options
-r <resourcepath>                       Sets the resource path

With no options, the following values are set by default:
--dted 0 36.96 -121.96
-c cache
-r dted/level0

Note: If you enable-vegetation by default this program uses models from our 
3D Asset Library: Vegetation. You can find the models here:

http://sourceforge.net/projects/delta3d

Controls
--------
+ / -  -  Increase/decrease threshold
[ / ]  -  Increase/decrease detail
Left   -  Turn left
Right  -  Turn right
Up     -  Look up 
Down   -  Look down 
ShftUp -  Move up
ShftDn -  Move down
Enter  -  Display next statistic
W      -  Move forward.
A      -  Move backward.
Space  -  Toggle wireframe view.
