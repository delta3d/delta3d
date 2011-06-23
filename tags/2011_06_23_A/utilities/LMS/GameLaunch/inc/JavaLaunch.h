/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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

#ifndef DELTA_GAME_LAUNCH_JAVA
#define DELTA_GAME_LAUNCH_JAVA

#include "jni.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Entry point for a Java calling application.
 * 
 * @param JNIEnv* Pointer to the Java VM (standard with all JNI calls)
 * @param jobject Java object representing the calling java class
 * @param jobjectArray Java object representing command line parameters
 */
JNIEXPORT void JNICALL Java_org_delta3d_lms_JavaLaunch_NativeLibraryLaunch_JavaLaunch
  (JNIEnv *, jobject, jobjectArray);

#ifdef __cplusplus
}
#endif
#endif
