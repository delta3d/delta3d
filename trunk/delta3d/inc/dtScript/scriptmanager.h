/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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
*/

#ifndef DELTA_SCRIPT_MANAGER
#define DELTA_SCRIPT_MANAGER

#include <string>
#include "dtCore/base.h"
#include "dtCore/export.h"
#include "Python.h"

namespace dtScript
{
   /** This class assists the user in executing Python scripts from within
   * the C++ Delta3D world. Just instantiate a ScriptManager, call Load(filename)
   * on your Python script, and then sucessive Run() calls will execute it.
   * Alternatively you can call Run(filename) to load and execute in one step (but
   * it will not save the compiled Python object in memory.
   */
   class DT_EXPORT ScriptManager : public dtCore::Base
	{

      DECLARE_MANAGEMENT_LAYER(ScriptManager)
	
	public:
	
	   ScriptManager();
	   virtual ~ScriptManager();
	   
      ///Load a Python script (.py) into memory
	   void Load( const std::string& filename );

      ///Load a Python script and execute it immediately. Use the default paramters to use a pre-loaded script.
	   void Run( const std::string& filename = "" );

   protected:
   
      PyObject *mFileObject;
      PyThreadState *mThreadState;
      std::string mFilename;
      
	};
	
}

#endif // DELTA_SCRIPT_MANAGER
