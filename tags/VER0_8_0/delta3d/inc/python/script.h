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

#ifndef DELTA_SCRIPT
#define DELTA_SCRIPT

#include <string>
#include "dtCore/export.h"
#include "Python.h"

namespace dtScript
{
	class DT_EXPORT Script
	{
	public:
	   Script();
	   ~Script();
	   
	   inline void Run( std::string filename )
      { 
         PyRun_SimpleFile(PyFile_AsFile(PyFile_FromString(const_cast<char*>(filename.c_str()), "r")), const_cast<char*>(filename.c_str()));
      }
	};
	
}

#endif // DELTA_SCRIPT