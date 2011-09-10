#include <prefix/dtcoreprefix.h>
#include <dtCore/ephemeris.h>
#include <osg/Math>
#include <cmath>

using namespace dtCore;

// Conversions among hours (of ra), degrees and radians.
template< typename T >
T hrrad( T x )
{
   return osg::DegreesToRadians(1.0)*15.0*x;
}

template< typename T >
T radhr( T x )
{
   return osg::RadiansToDegrees(1.0)*x/15.0;
}

/* info about the local observing circumstances and misc preferences */
struct Now
{
   double n_mjd;   /* modified Julian date, ie, days since
                     * Jan 0.5 1900 (== 12 noon, Dec 30, 1899), utc.
                     * enough precision to get well better than 1 second. */
   double n_lat;   /* latitude, >0 north, rads */
   double n_lng;   /* longitude, >0 east, rads */
   double n_elev;  /* elevation above sea level, earth radii */
};

/* given the mean anomaly, ma, and the eccentricity, s, of elliptical motion,
* find the true anomaly, *nu, and the eccentric anomaly, *ea.
* all angles in radians.
*/
static void GetAnomaly (double ma, double s, double *nu, double *ea)
{
   double m, fea;

   m = ma-(2.f*osg::PI)*int(ma/(2.f*osg::PI));
   if (m > osg::PI) m -= (2.0*osg::PI);
   if (m < -osg::PI) m += (2.0*osg::PI);
   fea = m;

   if (s < 1.0)
   {
      /* elliptical */
      double dla;
      for (;;)
      {
         dla = fea-(s*sin(fea))-m;
         if (std::abs(dla)<1e-6)
            break;
         dla /= 1-(s*cos(fea));
         fea -= dla;
      }
      *nu = 2*atan(double(sqrt((1+s)/(1-s))*tan(fea/2)));
   }
   else
   {
      /* hyperbolic */
      double corr = 1;
      while (std::abs(corr) > 0.000001)
      {
         corr = (m - s * sinh(fea) + fea) / (s*cosh(fea) - 1);
         fea += corr;
      }
      *nu = 2*atan(double(sqrt((s+1)/(s-1))*tanh(fea/2)));
   }
   *ea = fea;
}

static void range (double *v, double r)
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
static void sunpos( double mjd, double *lsn, double *rsn)
{
   static double last_mjd = -3691., last_lsn, last_rsn;
   double t, t2;
   double ls, ms;    /* mean longitude and mean anomoay */
   double s, nu, ea; /* eccentricity, true anomaly, eccentric anomaly */
   double a, b, a1, b1, c1, d1, e1, h1, dl, dr;

   if (mjd == last_mjd)
   {
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
   GetAnomaly(osg::DegreesToRadians(1.0)*ms, s, &nu, &ea);

   a = 62.55209472000015*t;
   b = 360*(a-(int)a);
   a1 = osg::DegreesToRadians(1.0)*(153.23+b);
   a = 125.1041894*t;
   b = 360*(a-(int)a);
   b1 = osg::DegreesToRadians(1.0)*(216.57+b);
   a = 91.56766028*t;
   b = 360*(a-(int)a);
   c1 = osg::DegreesToRadians(1.0)*(312.69+b);
   a = 1236.853095*t;
   b = 360*(a-(int)a);
   d1 = osg::DegreesToRadians(1.0)*(350.74-.00144*t2+b);
   e1 = osg::DegreesToRadians(1.0)*(231.19+20.2*t);
   a = 183.1353208*t;
   b = 360*(a-(int)a);
   h1 = osg::DegreesToRadians(1.0)*(353.4+b);
   dl = .00134*cos(a1)+.00154*cos(b1)+.002*cos(c1)+.00179*sin(d1)+
      .00178*sin(e1);
   dr = 5.43e-06*sin(a1)+1.575e-05*sin(b1)+1.627e-05*sin(c1)+
      3.076e-05*cos(d1)+9.27e-06*sin(h1);
   *lsn = nu+osg::DegreesToRadians(1.0)*(ls-ms+dl);
   range(lsn, 2.0*osg::PI);
   last_lsn = *lsn;
   *rsn = last_rsn = 1.0000002*(1-s*cos(ea))+dr;
   last_mjd = mjd;
}

/* given a date in months, mn, days, dy, years, yr,
* return the modified Julian date (number of days elapsed since 1900 jan 0.5),
* *mjd.
*/
static void CalcMJD (int mn, double dy, int yr, double *mjd)
{
   static double last_mjd, last_dy;
   static int last_mn, last_yr;
   int b, d, m, y;
   int c;

   if (mn == last_mn && yr == last_yr && dy == last_dy)
   {
      *mjd = last_mjd;
      return;
   }

   m = mn;
   y = (yr < 0) ? yr + 1 : yr;
   if (mn < 3)
   {
      m += 12;
      y -= 1;
   }

   if (yr < 1582 || (yr == 1582 && (mn < 10 || (mn == 10 && dy < 15))))
   {
      b = 0;
   }
   else
   {
      int a;
      a = y/100;
      b = 2 - a + a/4;
   }

   if (y < 0)
   {
      c = int((365.25*y) - 0.75) - 694025L;
   }
   else
   {
      c = int(365.25*y) - 694025L;
   }

   d = int(30.6001*(m+1));

   *mjd = b + c + d + dy - 0.5;

   last_mn = mn;
   last_dy = dy;
   last_yr = yr;
   last_mjd = *mjd;
}

/* given the modified Julian date (number of days elapsed since 1900 jan 0.5,),
* mjd, return the calendar date in months, *mn, days, *dy, and years, *yr.
*/
static void CalcCalFromMJD (double mjd, int *mn, double *dy, int *yr)
{
   static double last_mjd, last_dy;
   static int last_mn, last_yr;
   double d, f;
   double i, a, b, ce, g;

   /* we get called with 0 quite a bit from unused epoch fields.
   * 0 is noon the last day of 1899.
   */
   if (mjd == 0.0)
   {
      *mn = 12;
      *dy = 31.5;
      *yr = 1899;
      return;
   }

   if (mjd == last_mjd)
   {
      *mn = last_mn;
      *yr = last_yr;
      *dy = last_dy;
      return;
   }

   d = mjd + 0.5;
   i = floor(d);
   f = d-i;
   if (f == 1)
   {
      f = 0;
      i += 1;
   }

   if (i > -115860.0)
   {
      a = floor((i/36524.25)+.9983573)+14;
      i += 1 + a - floor(a/4.0);
   }

   b = floor((i/365.25)+.802601);
   ce = i - floor((365.25*b)+.750001)+416;
   g = floor(ce/30.6001);
   *mn = int(g - 1);
   *dy = ce - floor(30.6001*g)+f;
   *yr = int(b + 1899);

   if (g > 13.5)
   {
      *mn = int(g - 13);
   }

   if (*mn < 2.5)
   {
      *yr = int(b + 1900);
   }

   if (*yr < 1)
   {
      *yr -= 1;
   }

   last_mn = *mn;
   last_dy = *dy;
   last_yr = *yr;
   last_mjd = mjd;
}

static double tnaught (double mjd)
{
   double dmjd;
   int m, y;
   double d;

   CalcCalFromMJD (mjd, &m, &d, &y);
   CalcMJD(1, 0., y, &dmjd);
   double t = dmjd/36525;
   double t0 = 6.57098e-2 * (mjd - dmjd) -
      (24 - (6.6460656 + (5.1262e-2 + (t * 2.581e-5))*t) -
      (2400 * (t - ((double(y) - 1900)/100))));
   return t0;
}

/* given a modified julian date, mjd, and a universally coordinated time, utc,
* return greenwich mean siderial time, *gst.
* N.B. mjd must be at the beginning of the day.
*/
static double GetGST (double mjd, double utc)
{
   static double lastmjd = -10000;
   static double t0;

   /* ratio of from synodic (solar) to sidereal (stellar) rate */
   const double SIDRATE  = .9972695677;

   if (mjd != lastmjd)
   {
      t0 = tnaught(mjd);
      lastmjd = mjd;
   }
   double gst = (1.0/SIDRATE)*utc + t0;
   range(&gst, 24.0);
   return gst;
}

static double GetMidday(double jd)
{
   return floor(jd-0.5)+0.5;
}

static double GetHour(double jd)
{
   return (jd-GetMidday(jd))*24.0;
}

static double GetLST( double mjd, double longitude )
{
   double lst = GetGST(GetMidday(mjd), GetHour(mjd));
   lst += radhr(longitude);
   range(&lst, 24.0);
   return lst;
}

/* given true ha and dec, tha and tdec, the geographical latitude, phi, the
* height above sea-level (as a fraction of the earths radius, 6378.16km),
* ht, and the equatorial horizontal parallax, ehp, find the apparent
* ha and dec, aha and adec allowing for parallax.
* all angles in radians. ehp is the angle subtended at the body by the
* earth's equator.
*/
static void CalcParallax (double tha, double tdec, double phi, double ht,
                   double ehp, double *aha, double *adec)
{
   static double last_phi = 1000.0, last_ht = -1000.0, rsp, rcp;
   double rp;   /* distance to object in Earth radii */
   double ctha;
   double stdec, ctdec;
   double tdtha, dtha;
   double caha;

   /* avoid calcs involving the same phi and ht */
   if (phi != last_phi || ht != last_ht)
   {
      double cphi, sphi, u;
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
   range(aha, 2*osg::PI);
   *adec = atan(caha*(rp*stdec-rsp)/(rp*ctdec*ctha-rcp));
}


/* the actual formula is the same for both transformation directions so
* do it here once for each way.
* N.B. all arguments are in radians.
*/
static void aaha_aux (double lat, double x, double y, double *p, double *q)
{
   static double lastlat = -1000.;
   static double sinlastlat, coslastlat;
   double sy, cy;
   double sx, cx;

   /* latitude doesn't change much, so try to reuse the sin and cos evals.
   */
   if (lat != lastlat)
   {
      sinlastlat = sin (lat);
      coslastlat = cos (lat);
      lastlat = lat;
   }

   sy = sin (y);
   cy = cos (y);
   sx = sin (x);
   cx = cos (x);

   double sq, cq;
   double a;
   double cp;

   const double EPS(1e-20);
   sq = (sy*sinlastlat) + (cy*coslastlat*cx);
   *q = asin (sq);
   cq = cos (*q);
   a = coslastlat*cq;
   if (a > -EPS && a < EPS)
   {
      a = a < 0 ? -EPS : EPS; /* avoid / 0 */
   }
   cp = (sy - (sinlastlat*sq))/a;
   if (cp >= 1.0)   /* the /a can be slightly > 1 */
   {
      *p = 0.0;
   }
   else if (cp <= -1.0)
   {
      *p = osg::PI;
   }
   else
   {
      *p = acos ((sy - (sinlastlat*sq))/a);
   }
   if (sx>0) *p = 2.0*osg::PI - *p;
}

/* given latitude (n+, radians), lat, hour angle (radians), ha, and declination
* (radians), dec,
* return altitude (up+, radians), alt, and
* azimuth (angle round to the east from north+, radians),
*/
static void CalcAltAz (double lat, double ha, double dec, double *alt, double *az)
{
   aaha_aux (lat, ha, dec, az, alt);
}

static void equitorial_aux (int sw, double mjd, double x, double y, double *p, double *q)
{
   static double lastmjd = -10000;   /* last mjd calculated */
   static double seps, ceps;   /* sin and cos of mean obliquity */
   double sx, cx, sy, cy, ty;

   if (mjd != lastmjd)
   {
      double eps;

      {//envilm_obliquity (mjd, &eps);      /* mean obliquity for date */
         double t;
         t = mjd/36525.0;
         eps = osg::DegreesToRadians(1.0)*(2.345229444E1
            - ((((-1.81E-3*t)+5.9E-3)*t+4.6845E1)*t)/3600.0);
      }
      seps = sin(eps);
      ceps = cos(eps);
      lastmjd = mjd;
   }

   sy = sin(y);
   cy = cos(y);            /* always non-negative */
   if (std::abs(cy)<1e-20) cy = 1e-20;      /* insure > 0 */
   ty = sy/cy;
   cx = cos(x);
   sx = sin(x);
   *q = asin((sy*ceps)-(cy*seps*sx*sw));
   *p = atan(double(((sx*ceps)+(ty*seps*sw))/cx));
   if (cx<0) *p += osg::PI;      /* account for atan quad ambiguity */
   range(p, 2.0*osg::PI);
}


/* given the modified Julian date, mjd, and a geocentric ecliptic latitude,
* *lat, and longititude, *lng, each in radians, find the corresponding
* equitorial ra and dec, also each in radians.
*/
static void CalcEquitorialRaDec (double mjd, double lat, double lng, double *ra, double *dec)
{
   equitorial_aux (-1, mjd, lng, lat, ra, dec);
}

static void sun_pos (Now *np, double *altitude, double *azimuth)
{
   double lsn, rsn;   /* true geoc lng of sun; dist from sn to earth*/
   double lst;      /* local sidereal time */
   double ehp;      /* angular diamter of earth from object */
   double ha;      /* hour angle */
   double ra, dec;      /* ra and dec now */
   double alt, az;      /* alt and az */
   double dhlong;

   sunpos(np->n_mjd, &lsn, &rsn);/*sun's true ecliptic long and dist */

   dhlong = lsn-osg::PI;   /* geo- to helio- centric */
   range(&dhlong, 2.0*osg::PI);

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
static double GetMJD( time_t GMT)
{
   /* GMT is seconds since 00:00:00 1/1/1970 UTC on UNIX systems;
   * mjd was 25567.5 then.
   */

  return 25567.5 + GMT/3600.0/24.0;
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
void dtCore::GetSunPos( time_t time,
                        double lat,
                        double lon,
                        double elev,
                        double *sun_alt,
                        double *sun_az )
{
   double alt, azm;
   Now np;

   np.n_mjd = GetMJD(time);
   np.n_lat = lat*osg::DegreesToRadians(1.0);
   np.n_lng = lon*osg::DegreesToRadians(1.0);
   np.n_elev = elev;

   sun_pos(&np, &alt, &azm);
   *sun_alt = alt*osg::RadiansToDegrees(1.0);
   *sun_az = azm*osg::RadiansToDegrees(1.0);
}
