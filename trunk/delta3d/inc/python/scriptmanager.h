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
#include "dtCore/export.h"
#include "Python.h"

namespace dtScript
{
	class DT_EXPORT ScriptManager
	{
	
	public:
	
	   ScriptManager();
	   ~ScriptManager();
	   
	   inline void Load( std::string filename )
	   {
	      mFilename = filename;
	      mFileObject = PyFile_FromString( const_cast<char*>(mFilename.c_str()), "r");
	   }
	   
	   inline void Run()
      { 
         PyRun_SimpleFile(PyFile_AsFile(mFileObject), const_cast<char*>(mFilename.c_str()));
      }
      
	   inline void Run( std::string filename )
      { 
         Load( filename );
         Run();
      }
   protected:
   
      PyObject* mFileObject;
      std::string mFilename;
      
	};
	
}

#endif // DELTA_SCRIPT_MANAGER