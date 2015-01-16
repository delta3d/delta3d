delta3d 2.8.0 Read me - OSX

This release includes all the dependencies needed to run all the
applications.  Qt and GDAL may require some additional setup.

Qt - The qt libraries and plugins needed to run the applications have been
included, but the tools needed to create or compile a Qt application have
not.    If you wish to build a application, you need to download the Qt
4.8.6 package from the delta3d sourceforge.  The install names of the
libraries in this package are set to make it run within the package using
@executable_path/../Frameworks, but the one on Sourceforge is compiled with
everything pointing to /usr/local/TrollTech/Qt-4.8.6.  If you want to make
an application bundle with them, you'll need to use "install_name_tool
-change" and  "install_name_tool -id" to change the path in the Qt libraries
and in the libraries and executables you build in your application.

GDAL - GDAL is a 3rd party build, and it is configured to run from
/Library/Frameworks.  If you want to run testTerrain or use dtTerrain, or
just run some of the gdal applications, you'll need to copy/move the files in
the GDAL subdirectory to /Library/Frameworks.


Building your own .app bundles.

The App bundle contents show you how app bundles need to be laid out to work
for delta3d.  There are cmake scripts to generate these in the CMakeModules
directory (See delta3d_common.cmake for Build Game Start), but they don't
move all the dependent files and data into the app bundle.  For the release,
we generated symbolic links so we could share all the libraries and data and
not fill up your hard drive unnecessarily, but you can just copy what you
need and replicate the same layout.  See testApp.app for a good example.
Note that we copy the base delta data directory into
Contents/Resources/deltaData.  We also put the config.xml in there.


