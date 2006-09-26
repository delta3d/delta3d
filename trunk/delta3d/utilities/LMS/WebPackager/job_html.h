/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 13:48
 *
 * @file job_html.h
 * @version 1.0
 * @brief Description..
 */
#ifndef PACKAGER_JOB_HTML
#define PACKAGER_JOB_HTML

// local
#include "job_sgml.h"


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;


//======================================
// CLASS
//======================================

class JobHTML : public JobSGML
{
   public:
      JobHTML();
      ~JobHTML();

      /**
       * Kicks off the batch job for this class.
       * @param profile The object containing the package configuration file.
       */
      virtual void Execute( PackageProfile *profile );

   private:
};

#endif
