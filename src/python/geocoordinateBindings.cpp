// geocoordinateBindings.cpp
//    python bindings for dtTerrain::Geocoordinates
//
// May 16, 2006, Jerry Isdale
///////////////////////////////////////////////////////////

#include <boost/python.hpp>
#include <dtTerrain/geocoordinates.h>

using namespace boost::python;
using namespace dtTerrain;

namespace
{
   std::string geocoord_repr(GeoCoordinates * self)
   {
      std::ostringstream ost;
      ost << "GeoCoord(" << self->GetLatitude() << ", " << self->GetLongitude() << ", " << self->GetAltitude() << ")";
      return ost.str();
   }

   std::string geocoord_str(GeoCoordinates * self)
   {
      std::ostringstream ost;
      ost << "GeoCoord(" << self->GetLatitude() << ", " << self->GetLongitude() << ", " << self->GetAltitude() << ")";
      return ost.str();
   }
} // namespace

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SetLat_overloads,SetLatitude,1,3);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SetLong_overloads,SetLongitude,1,3);

void initGeoCoordinatesBindings()
{
   void (GeoCoordinates::*SetLat_Dbl)(double) = &GeoCoordinates::SetLatitude;
   void (GeoCoordinates::*SetLat_Int)(int, int, int) = &GeoCoordinates::SetLatitude;
   void (GeoCoordinates::*SetLong_Dbl)(double) = &GeoCoordinates::SetLongitude;
   void (GeoCoordinates::*SetLong_Int)(int, int, int)	= &GeoCoordinates::SetLongitude;

   double (GeoCoordinates::*GetLat_Dbl)() const = &GeoCoordinates::GetLatitude;
   //void (GeoCoordinates::*GetLat_Int)(int&, int&, int&) = &GeoCoordinates::GetLatitude;
   double (GeoCoordinates::*GetLong_Dbl)() const = &GeoCoordinates::GetLongitude;
   //void (GeoCoordinates::*GetLong_Int)(int&, int&, int&) = &GeoCoordinates::GetLongitude;

   void (GeoCoordinates::*SetCP1)(const osg::Vec3&) = &GeoCoordinates::SetCartesianPoint;
   void (GeoCoordinates::*GetCP1)(osg::Vec3&) = &GeoCoordinates::GetCartesianPoint;

   class_<GeoCoordinates, GeoCoordinates*, boost::noncopyable>("GeoCoordinates")
      .def(self == self)
      .def(self < self)
      .def("SetLatitude", SetLat_Dbl)
      .def("SetLatitude", SetLat_Int, SetLat_overloads())
      .def("SetLongitude", SetLong_Dbl)
      .def("SetLongitude", SetLong_Int, SetLong_overloads())
      .def("SetAltitude", &GeoCoordinates::SetAltitude)
      .def("GetLatitude", GetLat_Dbl)
      //.def("GetLatitude", GetLat_Int)
      .def("GetLongitude", GetLong_Dbl)
      //.def("GetLongitude", GetLong_Int)
      .def("GetAltitude", &GeoCoordinates::GetAltitude)
      .def("SetCartesianPoint", SetCP1)
      .def("GetCartesianPoint", GetCP1)
      .def("SetOrigin", &GeoCoordinates::SetOrigin)
      .staticmethod("SetOrigin")
      .def("GetOrigin", &GeoCoordinates::GetOrigin)
      .staticmethod("GetOrigin")
      .def("__str__", &GeoCoordinates::ToString)
      .def("__repr__", &geocoord_repr)
      ;
}
