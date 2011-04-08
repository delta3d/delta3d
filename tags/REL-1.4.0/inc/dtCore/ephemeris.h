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

#ifndef DELTA_EPHEMERIS
#define DELTA_EPHEMERIS

#include <dtCore/export.h>
#include <ctime>

namespace dtCore
{
   ///Get the Sun's alt and azimuth based on date/time, eyepoint lat,long, elev
   DT_CORE_EXPORT void GetSunPos(   time_t time, 
                                    double lat, 
                                    double lon, 
                                    double elev,
                                    double *sun_alt, 
                                    double *sun_az );

   ///Get the Greenwich Mean Time for the give day and time
   DT_CORE_EXPORT time_t GetGMT(int year, int month, int day, int hour, int min, int sec);
}

#endif // DELTA_EPHEMERIS