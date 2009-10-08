// rtsmotionmodelbindings.cpp: RTSMotionModel binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/rtsmotionmodel.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>
#include <dtCore/logicalinputdevice.h>

using namespace boost::python;
using namespace dtCore;

void initRTSMotionModelBindings()
{
	class_<RTSMotionModel, bases<MotionModel>, dtCore::RefPtr<RTSMotionModel> >("RTSMotionModel", init<Keyboard*, Mouse*, optional<bool> >())
	   .def("AxisStateChanged", &RTSMotionModel::AxisStateChanged)
	   .def("SetTerrain", &RTSMotionModel::SetTerrain)
	   ;
}
