// terrainBindings.cpp
//    python bindings for dtTerrain::Terrain 
//
// May 16, 2006, Jerry Isdale
// minimal bindings provided, use CreateTerrain factory method
// since we only have dted and soarx
// provide access to soarxdrawable::setBufferSize
///////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <sstream>

#include <python/dtpython.h>

#include <dtTerrain/terrain.h>
#include <dtTerrain/soarxdrawable.h>

using namespace boost::python;
using namespace dtTerrain;
using namespace dtCore;


void initTerrainBindings()
{

	//class_< Terrain, bases<Base>, Terrain*, boost::noncopyable >("Terrain",no_init) // init<optional<std::string> >())
	class_< Terrain, bases<DeltaDrawable>, dtCore::RefPtr<Terrain>, boost::noncopyable >("Terrain", no_init) 
		.def("SetBufferSize",SoarXDrawable::SetBufferSize)
		.staticmethod("SetBufferSize")
		.def("GetHeight",&Terrain::GetHeight)
		.def("IsClearLineOfSight", &Terrain::IsClearLineOfSight)
        ;

}

