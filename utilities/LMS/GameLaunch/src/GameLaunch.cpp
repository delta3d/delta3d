/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author Christopher DuBuc
 */
#include <stdio.h>

#include "Export.h"
#include "JavaLaunch.h"
#include "Launcher.h"

/*
 * Entry point for a native executable
 * @param argc The number of command line arguments in the argv array
 * @param argv An array of command line arguments
 */ 
#ifdef __cplusplus
extern "C" {
#endif

GAME_LAUNCH_EXPORT void NativeLaunch(int argc, char** argv)
{
   Launcher* launcher = new Launcher(argc, argv);

   launcher->Launch();

   delete launcher;
}

#ifdef __cplusplus
}
#endif

/*
 * Entry point for a Java executable
 * 
 * @param JNIEnv* Pointer to the Java VM (standard with all JNI calls)
 * @param jobject Java object representing the calling java class
 * @param jobjectArray Java object representing command line parameters
 */
JNIEXPORT void JNICALL Java_org_delta3d_lms_JavaLaunch_NativeLibraryLaunch_JavaLaunch(JNIEnv *env, jobject obj, jobjectArray cmdLineArgs)
{
   //get the size of the array
   jsize argc = env->GetArrayLength(cmdLineArgs);

   //declare a char* array for argv
   char* argv[128];

   //copy java array into our native array
   int i;
   for (i=0; i<argc; i++)
   {
      //obtain current object from java array
      jstring currentObj = (jstring)env->GetObjectArrayElement(cmdLineArgs, i);

      //convert current object into a char*
      const char* constArg = env->GetStringUTFChars(currentObj, 0);
      char* arg = const_cast<char*>(constArg);

      //build the argv array
      argv[i] = arg;
   }

   Launcher* launcher = new Launcher(argc, argv);

   launcher->Launch();

   delete launcher;
}
