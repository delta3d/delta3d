#include <dtUtil/noiseutility.h>
#include <boost/python.hpp>
//#include <python/dtpython.h>
using namespace boost::python;
using namespace dtUtil;


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SeamlessNoise_GetNoise, SeamlessNoise::GetNoise, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Fractal_FBM, FBM, 1, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Fractal_Turbulence, Turbulence, 1, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Fractal_Marble, Marble, 1, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Fractal_Island, IslandFractal, 1, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Fractal_RigidMultiFractal, RigidMultiFractal, 1, 7)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Fractal_HeteroFractal, HeteroFractal, 1, 6)



void init_NoiseBindings()
{

   class_<Noise1f, Noise1f*, boost::noncopyable>("Noise1f", init< optional<unsigned int> >())
   .def("Reseed", &Noise1f::Reseed)
   .def("GetNoise", &Noise1f::GetNoise);

   class_<Noise2f, Noise2f*, boost::noncopyable>("Noise2f", init< optional<unsigned int> >())
      .def("Reseed", &Noise2f::Reseed)
      .def("GetNoise", &Noise2f::GetNoise);

   class_<Noise3f, Noise3f*, boost::noncopyable>("Noise3f", init< optional<unsigned int> >())
      .def("Reseed", &Noise3f::Reseed)
      .def("GetNoise", &Noise3f::GetNoise);

   class_<Noise1d, Noise1d*, boost::noncopyable>("Noise1d", init< optional<unsigned int> >())
      .def("Reseed", &Noise1d::Reseed)
      .def("GetNoise", &Noise1d::GetNoise);

   class_<Noise2d, Noise2d*, boost::noncopyable>("Noise2d", init< optional<unsigned int> >())
      .def("Reseed", &Noise2d::Reseed)
      .def("GetNoise", &Noise2d::GetNoise);

   class_<Noise3d, Noise3d*, boost::noncopyable>("Noise3d", init< optional<unsigned int> >())
      .def("Reseed", &Noise3d::Reseed)
      .def("GetNoise", &Noise3d::GetNoise);

   //class_<SeamlessNoise, SeamlessNoise*, boost::noncopyable>("SeamlessNoise", init< optional<unsigned int> >())
   //   .def("Reseed", &SeamlessNoise::Reseed)
   //   .def("SetRepeat", &SeamlessNoise::SetRepeat)
   //   .def("GetNoise", &SeamlessNoise::GetNoise, SeamlessNoise_GetNoise());


   class_<Fractal2f, Fractal2f*, boost::noncopyable>("Fractal2f")
      .def("FBM", &Fractal2f::FBM, Fractal_FBM())
      .def("Turbulence", &Fractal2f::Turbulence, Fractal_Turbulence())
      .def("Marble", &Fractal2f::Marble, Fractal_Marble())
      .def("IslandFractal", &Fractal2f::IslandFractal, Fractal_Island())
      .def("RigidMultiFractal", &Fractal2f::RigidMultiFractal, Fractal_RigidMultiFractal())
      .def("HeteroFractal", &Fractal2f::HeteroFractal, Fractal_HeteroFractal());


   class_<Fractal3f, Fractal3f*, boost::noncopyable>("Fractal3f")
      .def("FBM", &Fractal3f::FBM, Fractal_FBM())
      .def("Turbulence", &Fractal3f::Turbulence, Fractal_Turbulence())
      .def("Marble", &Fractal3f::Marble, Fractal_Marble())
      .def("IslandFractal", &Fractal3f::IslandFractal, Fractal_Island())
      .def("RigidMultiFractal", &Fractal3f::RigidMultiFractal, Fractal_RigidMultiFractal())
      .def("HeteroFractal", &Fractal3f::HeteroFractal, Fractal_HeteroFractal());


   //class_<SeamlessFractal, SeamlessFractal*, boost::noncopyable>("SeamlessFractal")
   //   .def("FBM", &SeamlessFractal::FBM, Fractal_FBM())
   //   .def("Turbulence", &SeamlessFractal::Turbulence, Fractal_Turbulence())
   //   .def("Marble", &SeamlessFractal::Marble, Fractal_Marble())
   //   .def("IslandFractal", &SeamlessFractal::IslandFractal, Fractal_Island())
   //   .def("RigidMultiFractal", &SeamlessFractal::RigidMultiFractal, Fractal_RigidMultiFractal())
   //   .def("HeteroFractal", &SeamlessFractal::HeteroFracal, Fractal_HeteroFracal());


}

