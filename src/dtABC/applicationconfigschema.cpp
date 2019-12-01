/* -*-c++-*- 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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
 * John K. Grant
 * David Guthrie
 */
#include <dtABC/applicationconfigschema.h>

namespace dtABC
{
   // --- config schema model implementation --- //
   const std::string ApplicationConfigSchema::WINDOW="Window";
   const std::string ApplicationConfigSchema::NAME="Name";
   const std::string ApplicationConfigSchema::SCENE="Scene";
   const std::string ApplicationConfigSchema::CAMERA="Camera";
   const std::string ApplicationConfigSchema::VIEW="View";
   const std::string ApplicationConfigSchema::VIEWPORT="Viewport";
   const std::string ApplicationConfigSchema::LOG="Log";
   const std::string ApplicationConfigSchema::LIBRARY_PATH="LibraryPath";
   
   const std::string ApplicationConfigSchema::X="X";
   const std::string ApplicationConfigSchema::Y="Y";
   const std::string ApplicationConfigSchema::WIDTH="Width";
   const std::string ApplicationConfigSchema::HEIGHT="Height";

   const std::string ApplicationConfigSchema::VIEWPORT_X="X";
   const std::string ApplicationConfigSchema::VIEWPORT_Y="Y";
   const std::string ApplicationConfigSchema::VIEWPORT_WIDTH="Width";
   const std::string ApplicationConfigSchema::VIEWPORT_HEIGHT="Height";
   
   const std::string ApplicationConfigSchema::PIXELDEPTH="PixelDepth";
   const std::string ApplicationConfigSchema::REFRESHRATE="RefreshRate";
   const std::string ApplicationConfigSchema::SHOWCURSOR="ShowCursor";
   const std::string ApplicationConfigSchema::FULLSCREEN="FullScreen";
   const std::string ApplicationConfigSchema::REALIZE_UPON_CREATE="RealizeUponCreate";
   const std::string ApplicationConfigSchema::CHANGEDISPLAYRESOLUTION="ChangeDisplayResolution";
   const std::string ApplicationConfigSchema::VSYNC = "VSync";
   const std::string ApplicationConfigSchema::HIDE_WINDOWS_CONSOLE = "HideWindowsConsole";
   const std::string ApplicationConfigSchema::MULTI_SAMPLE="MultiSample";
   
   const std::string ApplicationConfigSchema::WINDOWINSTANCE="WindowInstance";
   const std::string ApplicationConfigSchema::SCENEINSTANCE="SceneInstance";
   const std::string ApplicationConfigSchema::CAMERAINSTANCE="CameraInstance";
   
   const std::string ApplicationConfigSchema::LOG_LEVEL="Level";
   const std::string ApplicationConfigSchema::GLOBAL_LOG_LEVEL="GlobalLog";

   const std::string ApplicationConfigSchema::APP_PROPERTIES="Properties";
   const std::string ApplicationConfigSchema::APP_PROPERTY="Property";
}
