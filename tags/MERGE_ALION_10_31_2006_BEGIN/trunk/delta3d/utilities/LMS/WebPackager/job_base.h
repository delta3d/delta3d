/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/07 - 11:32
 *
 * @file job_base.h
 * @version 1.0
 */
#ifndef PACKAGER_JOB_BASE
#define PACKAGER_JOB_BASE


//=====================================
// CONSTANTS, MACROS, DEFINITIONS
//=====================================

// forward refs
class PackageProfile;


//=====================================
// CLASS
//=====================================

class JobBase
{
   public:
      JobBase();
      ~JobBase();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      virtual void Execute( PackageProfile *profile );

   private:      
};

#endif
