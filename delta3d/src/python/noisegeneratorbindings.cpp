// noisegeneratorbindings.cpp: NoiseGenerator binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "pnoise.h"

using namespace boost::python;
using namespace dtCore;

void initNoiseGeneratorBindings()
{
   class_<NoiseGenerator>("NoiseGenerator", init<int, int, double, double, int, int, optional<int> >())
      .def("setOctaves", &NoiseGenerator::setOctaves)
      .def("setFrequency", &NoiseGenerator::setFrequency)
      .def("setAmplitude", &NoiseGenerator::setAmplitude)
      .def("setPersistence", &NoiseGenerator::setPersistence)
      .def("setWidth", &NoiseGenerator::setWidth)
      .def("setHeight", &NoiseGenerator::setHeight)
      .def("setSlices", &NoiseGenerator::setSlices)
      .def("makeNoiseTexture", &NoiseGenerator::makeNoiseTexture, return_internal_reference<>());
}