cd JavaLMS
jar cvfm JavaLaunch.jar mainclass.mf org/delta3d/lms/JavaLaunch/*.class
jarsigner -keystore ..\netc.keystore JavaLaunch.jar lmskey
move JavaLaunch.jar ..
cd ..
