/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 13:37
 *
 * @file job_sgml.h
 * @version 1.0
 */
#ifndef PACKAGER_JOB_SGML
#define PACKAGER_JOB_SGML

// local
#include "job_base.h"


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;


//======================================
// CLASS
//======================================

class JobSGML : public JobBase
{
   public:
      JobSGML();
      ~JobSGML();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      virtual void Execute( PackageProfile *profile );

   private:
};

#endif
