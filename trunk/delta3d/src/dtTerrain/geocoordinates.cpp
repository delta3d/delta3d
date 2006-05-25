/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* @author Matthew W. Campbell
*/
#include "dtTerrain/geocoordinates.h"
#include "dtUtil/exception.h"
#include <osg/Math>

#include <iostream>

namespace dtTerrain
{
	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_ENUM(GeoCoordinatesException);
	GeoCoordinatesException GeoCoordinatesException::OUT_OF_BOUNDS("OUT_OF_BOUNDS");

	//////////////////////////////////////////////////////////////////////////
	//These constants are hardcoded from the WGS84 standard.
	const double GeoCoordinates::EQUATORIAL_RADIUS = 6378137.0;
	const double GeoCoordinates::POLAR_RADIUS = 6356752.3142451794975639668;
	const double GeoCoordinates::FLATTENING  = 1.0/298.257223563;

	//////////////////////////////////////////////////////////////////////////
	GeoCoordinates::GeoCoordinates()
	{
		mUpdateCartesianPoint = false;
		mLatitude = mLongitude = 0.0;
		mCartesianPoint = osg::Vec3(0,0,0);
	}

	//////////////////////////////////////////////////////////////////////////
	void GeoCoordinates::SetLatitude(int degrees, int minutes, int seconds)
	{
		if (degrees < -90 || degrees > 90)
			EXCEPT(GeoCoordinatesException::OUT_OF_BOUNDS,
			"Latitude degrees must be in the range of -90 to 90 inclusive.");

		if (minutes < 0 || minutes > 59)
			EXCEPT(GeoCoordinatesException::OUT_OF_BOUNDS,
			"Latitude minutes must be in the range of 0 to 59.");

		if (seconds < 0 || seconds > 59)
			EXCEPT(GeoCoordinatesException::OUT_OF_BOUNDS,
			"Latitude seconds must be in the range of 0 to 59.");

		if (degrees < 0)
		{
			mLatitude = -(osg::absolute((double)degrees) + ((double)minutes/60.0) + 
				((double)seconds / 3600.0));
		}
		else
		{
			mLatitude = degrees + ((double)minutes/60.0) + ((double)seconds/3600.0);
		}

		mUpdateCartesianPoint = true;
	}   

	//////////////////////////////////////////////////////////////////////////
	void GeoCoordinates::GetLatitude(int &degrees, int &minutes, int &seconds)
	{
		degrees = (int)floor(mLatitude);
		minutes = (int)floor((mLatitude-degrees) * 60.0);
		seconds = (int)floor((((mLatitude-degrees)*60.0)-minutes) * 60.0);
	}

	//////////////////////////////////////////////////////////////////////////
	void GeoCoordinates::SetLongitude(int degrees, int minutes, int seconds)
	{
		if (degrees < -180 || degrees > 180)
			EXCEPT(GeoCoordinatesException::OUT_OF_BOUNDS,
			"Longitude degrees must be in the range of -180 to 180 inclusive.");

		if (minutes < 0 || minutes > 59)
			EXCEPT(GeoCoordinatesException::OUT_OF_BOUNDS,
			"Longitude minutes must be in the range of 0 to 59.");

		if (seconds < 0 || seconds > 59)
			EXCEPT(GeoCoordinatesException::OUT_OF_BOUNDS,
			"Longitude seconds must be in the range of 0 to 59.");

		if (degrees < 0)
		{
			mLongitude = -(osg::absolute((double)degrees) + ((double)minutes/60.0) + 
				((double)seconds / 3600.0));
		}
		else
		{
			mLongitude = degrees + ((double)minutes/60.0) + ((double)seconds/3600.0);
		}

		mUpdateCartesianPoint = true;
	}   

	//////////////////////////////////////////////////////////////////////////
	void GeoCoordinates::GetLongitude(int &degrees, int &minutes, int &seconds)
	{
		degrees = (int)floor(mLongitude);
		minutes = (int)floor((mLongitude-degrees) * 60.0);
		seconds = (int)floor((((mLongitude-degrees)*60.0)-minutes) * 60.0);
	}   

	//////////////////////////////////////////////////////////////////////////
	void GeoCoordinates::SetCartesianPoint(const osg::Vec3 &point)
	{
		// azuma: removed negation on mLatitude, so that as latitude increases, Y increases
		mLatitude = (point.y() / EQUATORIAL_RADIUS) * osg::RadiansToDegrees(1.0);
		mLongitude = (point.x() / EQUATORIAL_RADIUS) * osg::RadiansToDegrees(1.0);
		mAltitude = point.z();

		//Finally, make sure we keep the cartesian origin around.
		mCartesianPoint = point;
	}

	//////////////////////////////////////////////////////////////////////////
	const osg::Vec3 &GeoCoordinates::GetCartesianPoint()
	{
		if (mUpdateCartesianPoint)
		{
			mCartesianPoint.x() = (mLongitude * EQUATORIAL_RADIUS) * osg::DegreesToRadians(1.0);
			// azuma: removed negation on mLatitude, so that as latitude increases, Y increases
			mCartesianPoint.y() = (mLatitude * EQUATORIAL_RADIUS) * osg::DegreesToRadians(1.0);
			mCartesianPoint.z() = mAltitude;        
			mUpdateCartesianPoint = false;
		}

		return mCartesianPoint;
	} 

	// need this 'cause python deals with Vec3 not Vec3d
	void GeoCoordinates::GetCartesianPoint( osg::Vec3& point )
	{
      osg::Vec3 tmp = GetCartesianPoint();

		point.set( tmp);
	}


}
