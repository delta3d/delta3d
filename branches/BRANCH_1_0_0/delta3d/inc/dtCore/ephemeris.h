#ifndef DELTA_EPHEMERIS
#define DELTA_EPHEMERIS

#include "sg.h"
#include <time.h>

#include "dtCore/export.h"

namespace dtCore
{
   ///Get the Sun's alt and azimuth based on date/time, eyepoint lat,long, elev
   DT_EXPORT void GetSunPos(time_t time, SGDfloat lat, SGDfloat lon, SGDfloat elev,
                            SGDfloat *sun_alt, SGDfloat *sun_az);

   ///Get the Greenwich Mean Time for the give day and time
   DT_EXPORT time_t GetGMT(int year, int month, int day, int hour, int min, int sec);
}

#endif // DELTA_EPHEMERIS
