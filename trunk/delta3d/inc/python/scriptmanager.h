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
   class DT_EXPORT ScriptManager : public dtCore::Base
	{

      DECLARE_MANAGEMENT_LAYER(ScriptManager)
	
	public:
	
	   ScriptManager();
	   ~ScriptManager();
	   
      ///Load a Python script (.py) into memory
	   void Load( std::string filename );
	   
      ///Execute the loaded Python script
	   void Run();

      ///Load a Python script and execute it immediately
	   void Run( std::string filename );

   protected:
   
      PyObject* mFileObject;
      std::string mFilename;
      
	};
	
}

#endif // DELTA_SCRIPT_MANAGER