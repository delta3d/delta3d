// particlesystem.h: Declaration of the ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_PARTICLE_SYSTEM
#define DELTA_PARTICLE_SYSTEM

#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>

#include <osgDB/ReadFile>

#include <osgParticle/Emitter>

#include "drawable.h"
#include "notify.h"
#include "transformable.h"

namespace dtCore
{
   /**
    * A particle system.
    */
   class ParticleSystem : public Transformable,
                          public Drawable
   {
      DECLARE_MANAGEMENT_LAYER(ParticleSystem)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         ParticleSystem(std::string name = "ParticleSystem");

         /**
          * Destructor.
          */
         virtual ~ParticleSystem();
         
         /**
          * Loads a particle system from a file.
          *
          * @param filename the name of the file to load
          * @return true if loaded, false if not loaded
          */
         bool LoadFile(std::string filename);
         
         /**
          * Returns the name of the last loaded file.
          *
          * @return the filename
          */
         std::string GetFilename();
         
         /**
          * Enables or disables this particle system.  Particle systems
          * are enabled by default.
          *
          * @param enable true to enable the particle system, false
          * to disable it
          */
         void SetEnabled(bool enable);
         
         /**
          * Checks whether this particle system is enabled.
          *
          * @return true if the particle system is enabled, false
          * otherwise
          */
         bool IsEnabled();
         
         /**
          * Sets the parent-relative state of this particle system.  If
          * parent-relative mode is enabled, the entire particle system
          * will be positioned relative to the parent.  If disabled, only
          * the emitter will be positioned relative to the parent.  By
          * default, particle systems are not parent-relative.
          *
          * @param parentRelative true to enable parent-relative mode,
          * false to disable it
          */
         void SetParentRelative(bool parentRelative);
         
         /**
          * Returns the parent-relative state of this particle system.
          *
          * @return true if the particle system is in parent-relative mode,
          * false if not
          */
         bool IsParentRelative();
         
         /**
          * Returns this object's OpenSceneGraph node.
          *
          * @return the OpenSceneGraph node
          */
         virtual osg::Node* GetOSGNode();
         
         
      private:
         
         /**
          * The filename of the loaded particle system.
          */
         std::string mFilename;
         
         /**
          * Whether or not the particle system is enabled.
          */
         bool mEnabled;
         
         /**
          * Whether or not the particle system is in parent-relative
          * mode.
          */
         bool mParentRelative;
         
         /**
          * The OSG node.
          */
         osg::ref_ptr<osg::MatrixTransform> mNode;
   };
};

//
// Automatic library inclusion macros that use the #pragma/lib feature
//
#undef _AUTOLIBNAME
#if defined(_DEBUG)
   #define _AUTOLIBNAME  "osgParticled.lib"
#else
   #define _AUTOLIBNAME  "osgParticle.lib"
#endif

/* You may turn off this include message by defining _NOAUTOLIB */
#ifndef _NOAUTOLIBMSG
   #pragma message( "Will automatically link with " _AUTOLIBNAME )
#endif

#pragma comment(lib, _AUTOLIBNAME)

#endif // DELTA_PARTICLE_SYSTEM