#pragma once
#include "sg.h"
#include <time.h>

namespace dtCore
{
   ///Get the Sun's alt and azimuth based on date/time, eyepoint lat,long, elev
   void GetSunPos(time_t time, SGDfloat lat, SGDfloat lon, SGDfloat elev,
                  SGDfloat *sun_alt, SGDfloat *sun_az);

   ///Get the Greenwich Mean Time for the give day and time
   time_t GetGMT(int year, int month, int day, int hour, int min, int sec);
}