cd JavaLMS
jar cvf MessageApplet.jar org/delta3d/lms/MessageApplet/*.class
jarsigner -keystore ..\netc.keystore MessageApplet.jar lmskey
move MessageApplet.jar ..
cd ..
