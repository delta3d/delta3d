#include "dtCore/ephemeris.h"
#include <time.h>

using namespace dtCore;

/* conversions among hours (of ra), degrees and radians. */
#define hrrad(x)        SGD_DEGREES_TO_RADIANS*15.0*x
#define radhr(x)        SGD_RADIANS_TO_DEGREES*x/15.0


/* info about the local observing circumstances and misc preferences */
typedef struct {
   SGDfloat n_mjd;   /* modified Julian date, ie, days since
                     * Jan 0.5 1900 (== 12 noon, Dec 30, 1899), utc.
                     * enough precision to get well better than 1 second. */
   SGDfloat n_lat;   /* latitude, >0 north, rads */
   SGDfloat n_lng;   /* longitude, >0 east, rads */
   SGDfloat n_elev;  /* elevation above sea level, earth radii */
} Now;


/* given the mean anomaly, ma, and the eccentricity, s, of elliptical motion,
* find the true anomaly, *nu, and the eccentric anomaly, *ea.
* all angles in radians.
*/
static void GetAnomaly (SGDfloat ma, SGDfloat s, SGDfloat *nu, SGDfloat *ea)
{
   SGDfloat m, fea;

   m = ma-(2.f*SGD_PI)*(int)(ma/(2.f*SGD_PI));
   if (m > SGD_PI) m -= (2.0*SGD_PI);
   if (m < -SGD_PI) m += (2.0*SGD_PI);
   fea = m;

   if (s < 1.0) {
      /* elliptical */
      SGDfloat dla;
      for (;;) {
         dla = fea-(s*sin(fea))-m;
         if (fabs(dla)<1e-6)
            break;
         dla /= 1-(s*cos(fea));
         fea -= dla;
      }
      *nu = 2*atan((SGDfloat)(sqrt((1+s)/(1-s))*tan(fea/2)));
   } else {
      /* hyperbolic */
      SGDfloat corr = 1;
      while (fabs(corr) > 0.000001) {
         corr = (m - s * sinh(fea) + fea) / (s*cosh(fea) - 1);
         fea += corr;
      }
      *nu = 2*atan((SGDfloat)(sqrt((s+1)/(s-1))*tanh(fea/2)));
   }
   *ea = fea;
}

static void range (SGDfloat *v, SGDfloat r)
{
   *v -= r*floor(*v/r);
}


/* given the modified JD, mjd, return the true geocentric ecliptic longitude
*   of the sun for the mean equinox of the date, *lsn, in radians, and the
*   sun-earth distance, *rsn, in AU. (the true ecliptic latitude is never more
*   than 1.2 arc seconds and so may be taken to be a constant 0.)
* if the APPARENT ecliptic longitude is required, correct the longitude for
*   nutation to the true equinox of date and for aberration (light travel time,
*   approximately  -9.27e7/186000/(3600*24*365)*2*pi = -9.93e-5 radians).
*/
static void sunpos( SGDfloat mjd, SGDfloat *lsn, SGDfloat *rsn)
{
   static SGDfloat last_mjd = -3691., last_lsn, last_rsn;
   SGDfloat t, t2;
   SGDfloat ls, ms;    /* mean longitude and mean anomoay */
   SGDfloat s, nu, ea; /* eccentricity, true anomaly, eccentric anomaly */
   SGDfloat a, b, a1, b1, c1, d1, e1, h1, dl, dr;

   if (mjd == last_mjd) {
      *lsn = last_lsn;
      *rsn = last_rsn;
      return;
   }

   t = mjd/36525.;
   t2 = t*t;
   a = 100.0021359*t;
   b = 360.*(a-(int)a);
   ls = 279.69668f+.0003025*t2+b;
   a = 99.99736042000039*t;
   b = 360*(a-(int)a);
   ms = 358.47583-(.00015+.0000033*t)*t2+b;
   s = .016751-.0000418*t-1.26e-07*t2;
   GetAnomaly(SGD_DEGREES_TO_RADIANS*ms, s, &nu, &ea);

   a = 62.55209472000015*t;
   b = 360*(a-(int)a);
   a1 = SGD_DEGREES_TO_RADIANS*153.23+b;
   a = 125.1041894*t;
   b = 360*(a-(int)a);
   b1 = SGD_DEGREES_TO_RADIANS*(216.57+b);
   a = 91.56766028*t;
   b = 360*(a-(int)a);
   c1 = SGD_DEGREES_TO_RADIANS*(312.69+b);
   a = 1236.853095*t;
   b = 360*(a-(int)a);
   d1 = SGD_DEGREES_TO_RADIANS*(350.74-.00144*t2+b);
   e1 = SGD_DEGREES_TO_RADIANS*(231.19+20.2*t);
   a = 183.1353208*t;
   b = 360*(a-(int)a);
   h1 = SGD_DEGREES_TO_RADIANS*(353.4+b);
   dl = .00134*cos(a1)+.00154*cos(b1)+.002*cos(c1)+.00179*sin(d1)+
      .00178*sin(e1);
   dr = 5.43e-06*sin(a1)+1.575e-05*sin(b1)+1.627e-05*sin(c1)+
      3.076e-05*cos(d1)+9.27e-06*sin(h1);
   *lsn = nu+SGD_DEGREES_TO_RADIANS*(ls-ms+dl);   
   range(lsn, 2.0*SGD_PI);
   last_lsn = *lsn;
   *rsn = last_rsn = 1.0000002*(1-s*cos(ea))+dr;
   last_mjd = mjd;
}



/* given a date in months, mn, days, dy, years, yr,
* return the modified Julian date (number of days elapsed since 1900 jan 0.5),
* *mjd.
*/
static void CalcMJD (int mn, SGDfloat dy, int yr, SGDfloat *mjd)
{
   static SGDfloat last_mjd, last_dy;
   static int last_mn, last_yr;
   int b, d, m, y;
   int c;

   if (mn == last_mn && yr == last_yr && dy == last_dy) {
      *mjd = last_mjd;
      return;
   }

   m = mn;
   y = (yr < 0) ? yr + 1 : yr;
   if (mn < 3) {
      m += 12;
      y -= 1;
   }

   if (yr < 1582 || yr == 1582 && (mn < 10 || mn == 10 && dy < 15)) 
      b = 0;
   else {
      int a;
      a = y/100;
      b = 2 - a + a/4;
   }

   if (y < 0)
      c = (int)((365.25*y) - 0.75) - 694025L;
   else
      c = (int)(365.25*y) - 694025L;

   d = (int)(30.6001*(m+1));

   *mjd = b + c + d + dy - 0.5;

   last_mn = mn;
   last_dy = dy;
   last_yr = yr;
   last_mjd = *mjd;
}


/* given the modified Julian date (number of days elapsed since 1900 jan 0.5,),
* mjd, return the calendar date in months, *mn, days, *dy, and years, *yr.
*/
static void CalcCalFromMJD (SGDfloat mjd, int *mn, SGDfloat *dy, int *yr)
{
   static SGDfloat last_mjd, last_dy;
   static int last_mn, last_yr;
   SGDfloat d, f;
   SGDfloat i, a, b, ce, g;

   /* we get called with 0 quite a bit from unused epoch fields.
   * 0 is noon the last day of 1899.
   */
   if (mjd == 0.0) {
      *mn = 12;
      *dy = 31.5;
      *yr = 1899;
      return;
   }

   if (mjd == last_mjd) {
      *mn = last_mn;
      *yr = last_yr;
      *dy = last_dy;
      return;
   }

   d = mjd + 0.5;
   i = floor(d);
   f = d-i;
   if (f == 1) {
      f = 0;
      i += 1;
   }

   if (i > -115860.0) {
      a = floor((i/36524.25)+.9983573)+14;
      i += 1 + a - floor(a/4.0);
   }

   b = floor((i/365.25)+.802601);
   ce = i - floor((365.25*b)+.750001)+416;
   g = floor(ce/30.6001);
   *mn = (int)(g - 1);
   *dy = ce - floor(30.6001*g)+f;
   *yr = (int)(b + 1899);

   if (g > 13.5)
      *mn = (int)(g - 13);
   if (*mn < 2.5)
      *yr = (int)(b + 1900);
   if (*yr < 1)
      *yr -= 1;

   last_mn = *mn;
   last_dy = *dy;
   last_yr = *yr;
   last_mjd = mjd;
}


static SGDfloat tnaught (SGDfloat mjd)
{
   SGDfloat dmjd;
   int m, y;
   SGDfloat d;
   SGDfloat t, t0;

   CalcCalFromMJD (mjd, &m, &d, &y);
   CalcMJD(1, 0., y, &dmjd);
   t = dmjd/36525;
   t0 = 6.57098e-2 * (mjd - dmjd) - 
      (24 - (6.6460656 + (5.1262e-2 + (t * 2.581e-5))*t) -
      (2400 * (t - (((SGDfloat)y - 1900)/100))));
   return (t0);
}


/* given a modified julian date, mjd, and a universally coordinated time, utc,
* return greenwich mean siderial time, *gst.
* N.B. mjd must be at the beginning of the day.
*/
static SGDfloat GetGST (SGDfloat mjd, SGDfloat utc)
{
   SGDfloat gst;
   static SGDfloat lastmjd = -10000;
   static SGDfloat t0;
   
   /* ratio of from synodic (solar) to sidereal (stellar) rate */
   const SGDfloat SIDRATE  = .9972695677;

   if (mjd != lastmjd) {
      t0 = tnaught(mjd);
      lastmjd = mjd;
   }
   gst = (1.0/SIDRATE)*utc + t0;
   range(&gst, 24.0);
   return (gst);
}


static SGDfloat GetMidday(SGDfloat jd)
{
   return (floor(jd-0.5)+0.5);
}

static SGDfloat GetHour(SGDfloat jd)
{
   return ((jd-GetMidday(jd))*24.0);
}



static SGDfloat GetLST( SGDfloat mjd, SGDfloat longitude )
{
   SGDfloat lst;
   lst = GetGST(GetMidday(mjd), GetHour(mjd));
   lst += radhr(longitude);
   range(&lst, 24.0);
   return (lst);
}

/* given true ha and dec, tha and tdec, the geographical latitude, phi, the
* height above sea-level (as a fraction of the earths radius, 6378.16km),
* ht, and the equatorial horizontal parallax, ehp, find the apparent
* ha and dec, aha and adec allowing for parallax.
* all angles in radians. ehp is the angle subtended at the body by the
* earth's equator.
*/
static void CalcParallax (SGDfloat tha, SGDfloat tdec, SGDfloat phi, SGDfloat ht,
                   SGDfloat ehp, SGDfloat *aha, SGDfloat *adec)
{
   static SGDfloat last_phi = 1000.0, last_ht = -1000.0, rsp, rcp;
   SGDfloat rp;	/* distance to object in Earth radii */
   SGDfloat ctha;
   SGDfloat stdec, ctdec;
   SGDfloat tdtha, dtha;
   SGDfloat caha;

   /* avoid calcs involving the same phi and ht */
   if (phi != last_phi || ht != last_ht) {
      SGDfloat cphi, sphi, u;
      cphi = cos(phi);
      sphi = sin(phi);
      u = atan(9.96647e-1*sphi/cphi);
      rsp = (9.96647e-1*sin(u))+(ht*sphi);
      rcp = cos(u)+(ht*cphi);
      last_phi  =  phi;
      last_ht  =  ht;
   }

   rp = 1/sin(ehp);

   ctha = cos(tha);
   stdec = sin(tdec);
   ctdec = cos(tdec);
   tdtha = (rcp*sin(tha))/((rp*ctdec)-(rcp*ctha));
   dtha = atan(tdtha);
   *aha = tha+dtha;
   caha = cos(*aha);
   range(aha, 2*SGD_PI);
   *adec = atan(caha*(rp*stdec-rsp)/(rp*ctdec*ctha-rcp));
}


/* the actual formula is the same for both transformation directions so
* do it here once for each way.
* N.B. all arguments are in radians.
*/
static void aaha_aux (SGDfloat lat, SGDfloat x, SGDfloat y, SGDfloat *p, SGDfloat *q)
{
   static SGDfloat lastlat = -1000.;
   static SGDfloat sinlastlat, coslastlat;
   SGDfloat sy, cy;
   SGDfloat sx, cx;

   /* latitude doesn't change much, so try to reuse the sin and cos evals.
   */
   if (lat != lastlat) {
      sinlastlat = sin (lat);
      coslastlat = cos (lat);
      lastlat = lat;
   }

   sy = sin (y);
   cy = cos (y);
   sx = sin (x);
   cx = cos (x);

   SGDfloat sq, cq;
   SGDfloat a;
   SGDfloat cp;

#define	EPS	(1e-20)
   sq = (sy*sinlastlat) + (cy*coslastlat*cx);
   *q = asin (sq);
   cq = cos (*q);
   a = coslastlat*cq;
   if (a > -EPS && a < EPS)
      a = a < 0 ? -EPS : EPS; /* avoid / 0 */
   cp = (sy - (sinlastlat*sq))/a;
   if (cp >= 1.0)	/* the /a can be slightly > 1 */
      *p = 0.0;
   else if (cp <= -1.0)
      *p = SGD_PI;
   else
      *p = acos ((sy - (sinlastlat*sq))/a);
   if (sx>0) *p = 2.0*SGD_PI - *p;
}

/* given latitude (n+, radians), lat, hour angle (radians), ha, and declination
* (radians), dec,
* return altitude (up+, radians), alt, and
* azimuth (angle round to the east from north+, radians),
*/
static void CalcAltAz (SGDfloat lat, SGDfloat ha, SGDfloat dec, SGDfloat *alt, SGDfloat *az)
{
   aaha_aux (lat, ha, dec, az, alt);
}


static void equitorial_aux (int sw, SGDfloat mjd, SGDfloat x, SGDfloat y, SGDfloat *p, SGDfloat *q)
{
   static SGDfloat lastmjd = -10000;	/* last mjd calculated */
   static SGDfloat seps, ceps;	/* sin and cos of mean obliquity */
   SGDfloat sx, cx, sy, cy, ty;

   if (mjd != lastmjd) 
   {
      SGDfloat eps;
      
      {//envilm_obliquity (mjd, &eps);		/* mean obliquity for date */
         SGDfloat t;
         t = mjd/36525.0;
         eps = SGD_DEGREES_TO_RADIANS*(2.345229444E1
            - ((((-1.81E-3*t)+5.9E-3)*t+4.6845E1)*t)/3600.0);
      }
      seps = sin(eps);
      ceps = cos(eps);
      lastmjd = mjd;
   }

   sy = sin(y);
   cy = cos(y);				/* always non-negative */
   if (fabs(cy)<1e-20) cy = 1e-20;		/* insure > 0 */
   ty = sy/cy;
   cx = cos(x);
   sx = sin(x);
   *q = asin((sy*ceps)-(cy*seps*sx*sw));
   *p = atan((SGDfloat)(((sx*ceps)+(ty*seps*sw))/cx));
   if (cx<0) *p += SGD_PI;		/* account for atan quad ambiguity */
   range(p, 2.0*SGD_PI);
}


/* given the modified Julian date, mjd, and a geocentric ecliptic latitude,
* *lat, and longititude, *lng, each in radians, find the corresponding
* equitorial ra and dec, also each in radians.
*/
static void CalcEquitorialRaDec (SGDfloat mjd, SGDfloat lat, SGDfloat lng, SGDfloat *ra, SGDfloat *dec)
{
   equitorial_aux (-1, mjd, lng, lat, ra, dec);
}


static void sun_pos (Now *np, SGDfloat *altitude, SGDfloat *azimuth)
{
   SGDfloat lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
   SGDfloat lst;		/* local sidereal time */
   SGDfloat ehp;		/* angular diamter of earth from object */
   SGDfloat ha;		/* hour angle */
   SGDfloat ra, dec;		/* ra and dec now */
   SGDfloat alt, az;		/* alt and az */
   SGDfloat dhlong;

   sunpos(np->n_mjd, &lsn, &rsn);/*sun's true ecliptic long and dist */

   dhlong = lsn-SGD_PI;	/* geo- to helio- centric */
   range(&dhlong, 2.0*SGD_PI);

   /* convert geocentric ecliptic lat/long to equitorial ra/dec of date */
   CalcEquitorialRaDec( np->n_mjd, 0.0, lsn, &ra, &dec);

   /* find alt/az based on unprecessed ra/dec */
   lst = GetLST(np->n_mjd, np->n_lng);
   ha = hrrad(lst) - ra;
   ehp = (2.0 * 6378.0 / 146.0e6) /rsn /* op->s_edist*/;
   CalcParallax (ha, dec, np->n_lat, np->n_elev, ehp, &ha, &dec);
   CalcAltAz (np->n_lat, ha, dec, &alt, &az);
   *altitude = alt;
   *azimuth = az;
}


/** Given a GMT, convert it to a Modified Julian Date */
static SGDfloat GetMJD( time_t GMT)
{
   /* GMT is seconds since 00:00:00 1/1/1970 UTC on UNIX systems;
   * mjd was 25567.5 then.
   */

  return (25567.5 + GMT/3600.0/24.0);
}


/** Get the altitude and azimuth of the Sun given the GMT time/date, a reference
  * Latitude/longitude/elevation.
  *
  * @param time : the GMT time 
  * @param lat : Reference latitude (in degrees, positive = northern hemisphere)
  * @param lon : Reference longitude (in degrees, negative = west)
  * @param elev : Reference elevation (in Earth radii, 1 = Earth surface)
  * @param sun_alt : The output sun altitude (in degrees above the horizon)
  * @param sun_az : The output sun Azimuth (in degrees (from North?))
  */
void dtCore::GetSunPos(time_t time, SGDfloat lat, SGDfloat lon,
                    SGDfloat elev, SGDfloat *sun_alt, SGDfloat *sun_az)
{
   SGDfloat alt, azm;
   Now np;

   np.n_mjd = GetMJD(time);
   np.n_lat = lat*SGD_DEGREES_TO_RADIANS;
   np.n_lng = lon*SGD_DEGREES_TO_RADIANS;
   np.n_elev = elev;

   sun_pos(&np, &alt, &azm);
   *sun_alt = alt*SGD_RADIANS_TO_DEGREES;
   *sun_az = azm*SGD_RADIANS_TO_DEGREES;
}

/**
Return time_t for Sat Mar 21 12:00:00 GMT

On many systems it is ambiguous if mktime() assumes the input is in
GMT, or local timezone.  To address this, a new function called
timegm() is appearing.  It works exactly like mktime() but
explicitely interprets the input as GMT.

timegm() is available and documented under FreeBSD.  It is
available, but completely undocumented on my current Debian 2.1
distribution.

In the absence of timegm() we have to guess what mktime() might do.

Many older BSD style systems have a mktime() that assumes the input
time in GMT.  But FreeBSD explicitly states that mktime() assumes
local time zone

The mktime() on many SYSV style systems (such as Linux) usually
returns its result assuming you have specified the input time in
your local timezone.  Therefore, in the absence if timegm() you
have to go to extra trouble to convert back to GMT.

If you are having problems with incorrectly positioned astronomical
bodies, this is a really good place to start looking.

* @param year current GMT year
* @param month current GMT month
* @param day current GMT day
* @param hour current GMT hour
* @param min current minute
* @param sec current second
* @return The number of seconds past midnight (00:00:00), January 1, 1970,
*         coordinated universal time (UTC)
*/
time_t dtCore::GetGMT(int year, int month, int day, int hour, int min, int sec)
{
   struct tm mt;

   mt.tm_mon = month;
   mt.tm_mday = day;
   mt.tm_year = year;
   mt.tm_hour = hour;
   mt.tm_min = min;
   mt.tm_sec = sec;
   mt.tm_isdst = -1; // let the system determine the proper time zone

   time_t ret = mktime(&mt);

   return ret;
}
