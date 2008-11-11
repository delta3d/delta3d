/* 
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
*/

#ifndef DELTA_SCRIPT_MANAGER
#define DELTA_SCRIPT_MANAGER

#include <Python.h>
#include <dtCore/base.h>
#include <dtScript/export.h>
#include <OpenThreads/Thread>

namespace dtScript
{
   /** 
    * This class assists the user in executing Python scripts from within
    * the C++ Delta3D world. Just instantiate a ScriptManager, call Load(filename)
    * on your Python script, and then sucessive Run() calls will execute it by
    * kicking off a new thread. Alternatively you can call Run(filename) to load 
    * and execute in one step.
    */
   class DT_SCRIPT_EXPORT ScriptManager : public dtCore::Base,
                                          public OpenThreads::Thread
	{

      DECLARE_MANAGEMENT_LAYER(ScriptManager)
	
	public:
	
	   ScriptManager( const std::string& name = "ScriptManager" );

   protected:

	   virtual ~ScriptManager();

   public:

      ///Load a Python script (.py) into memory
	   void Load( const std::string& filename );

      ///Load a Python script and execute it immediately. Use the default paramters to use a pre-loaded script.
	   void Run( const std::string& filename = "" );
	   
	   ///Stops the ScriptManager thread. Make sure to call this before you exit your app.
	   void Stop() { cancel(); }

   protected:
   
      virtual void run();
   
      PyObject* mFileObject;
      std::string mFilename;
	};
}

#endif // DELTA_SCRIPT_MANAGER
