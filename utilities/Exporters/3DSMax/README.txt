OSGExp Version 0.9.3 Prerelease 4
=================================

What is it?
-----------

OSGExp is an open source exporter from 3ds max to the OpenSceneGraph. It has
support for Geometry, Material, Texture, Multitexture, Proceduraltexture,
Environmentmaps, Cameras, Animations and has helpers for OSG style Level Of
Detail, Billboards, Switches, Impostors, Occluders, Nodemasks and much more.


The Latest Version
-------------------

Details of the latest version can be found on the OpenSceneGraph website in the
Plugins section under:
http://www.openscenegraph.org/index.php?page=OSGExp.OSGExp


Documentation
-------------

The documentation can be found here:
http://www.openscenegraph.org/index.php?page=OSGExp.OSGExp


Installation
------------

Before installing a new version you must uninstall the old version of OSGExp.

Run the installer. The installer will detect the versions of Max that you have
installed. Currently Max 6 and Max 7 are supported. If you have no Max
installed, the installer will abort. You can select for which Max version you
want to install the exporter. The installer will install the OSG dll's in the
Max root, the plugin's in the OSGExp directory and will configure Max to use the
plugin.


Building
--------

The project is developed in Microsoft Visual Studio C++ 7.1, but should also
compile using Microsoft Visual Studio C++ 6.0. The project files are still for
VS 6.0, but those will be converted without problems to VS 7.1 or later.

1. Extract OSGExp in the same directory as OpenSceneGraph, Producer and
   OpenThreads.

2. Build OpenThreads, Producer and OpenSceneGraph.

3. Install Max6 or Max7 and don't forget to include the maxsdk.

4. Open Microsoft Visual Studio, go to tools -> options and select the
   directories tab.

   Select the Include files from the drop down list and indicate where the
   maxdsk include directory resides on your system:
     3dsmax[6,7]\maxsdk\include
   Move the directory up before the std. directories.

   Select the Library files from the drop down box and indicate where the maxdsk
   lib directory resides on your system:
     3dsmax[6,7]\maxsdk\lib
    Move all the directory up before the std. directories.

5. Open <OSG>\OSGExp\VisualStudio\OSGExp\OSGExp.dsw and do a batch build. Select
   all release versions and do a build.

6. Add the following directories to the path environment:
     <OSG>\OpenThreads\bin\win32
     <OSG>\Producer\bin
     <OSG>\OpenSceneGraph\bin
     <OSG>\OSGExp\bin
   
7. Open 3dsmax[6,7]\plugin.ini and add the following line under the Directories 
   tag:
     OSG export=<OSG>\OSGExp\bin

8. Open 3dsmax5 and export using File->Export or File->Export Selected, choose
   "OpenSceneGraph Exporter (*.ive,*osg)" as file type and off you go!

     
Acknowledgments
---------------

For creating the OSGExp and making it Open Source:
- Rune Schmidt Jensen
- Michael Grønager
- The Danish IT Centre for Education and Research

For making OpenSceneGraph and Producer and giving a spot on their servers for
continueing the development of OSGExp:
- Robert Osfield
- Don Burns

For bug fixes, new features and enhancements:
- Ali Botorabi
- Emmanuel Roche
- Bradley G Anderegg


Contact
-------

Joran Jessurun
Software Engineer
Design Systems Group
Fac. of Architecture, Building, and Planning
Eindhoven University of Technology
The Netherlands
phone: +31 40 2475096
A.J.Jessurun@tue.nl
http://www.ds.arch.tue.nl/joran


