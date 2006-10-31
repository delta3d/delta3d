/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/10 - 10:13
 *
 * @file package_builder.cpp
 * @version 1.0
 */

// local
#include "package_builder.h"
#include "package_profile.h"
#include "job_jarlib.h"
#include "job_jardata.h"
#include "job_launchconfig.h"
#include "job_jnlp.h"
#include "job_manifest.h"
#include "job_deployment.h"
// xerces
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
// ansi
#include <iostream>



//======================================
// PUBLIC FUNCTIONS
//======================================

PackageBuilder::PackageBuilder() : mProfile(NULL)
{
}

PackageBuilder::~PackageBuilder()
{
}

void PackageBuilder::Initialize()
{
   // startup xerces
   try
   {
      XMLPlatformUtils::Initialize();
   }
   catch ( const XMLException &e )
   {      
      char* message = XMLString::transcode( e.getMessage() );
      std::cout << "[Error] XML toolkit initialization error:\n  "
                << message << std::endl;
      XMLString::release( &message );
   }

   // create profile
   mProfile = new PackageProfile();   
}

void PackageBuilder::Shutdown()
{
   // release profile from memory
   mProfile->ReleasePackageProfile();

   // destroy all the batch jobs
   for ( unsigned int i=0; i < mJobs.size(); i++ )
   {
      delete mJobs[i];
      mJobs[i] = NULL;
   }
   mJobs.clear();

   // shutdown xerces
   try
   {
      XMLPlatformUtils::Terminate();
   }
   catch ( const XMLException &e )
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::cout << "[Error] XML toolkit shutdown error:\n  "
                << message << std::endl;
      XMLString::release( &message );
   }
   delete mProfile;
   mProfile = NULL;
}

void PackageBuilder::Build(const std::string &inputfile, const std::string &output)
{
   // get keystore password, used for signing JAR files
   std::string password;
   std::cout << "Enter keystore password (for .jar signing): ";
   std::cin  >> password;
   std::cout << std::endl;

   // load up package profile
   if (mProfile->LoadPackageProfile( inputfile ) == false)
   {
      std::cout << "Exiting abnormally..." << std::endl;
      return;
   }

   mProfile->SetOutput( output );
   mProfile->SetKeystorePassword( password );

   //-------------------------------
   // KICKOFF JOBS
   //-------------------------------

   // deploy files
   JobDeployment *jobDeploy = new JobDeployment();
   jobDeploy->Execute( mProfile );

   // generate jnlp
   JobJNLP *jobJNLP = new JobJNLP();
   jobJNLP->Execute( mProfile );

   // generate launching program config file
   JobLaunchConfig *jobLaunchConfig = new JobLaunchConfig();
   jobLaunchConfig->Execute( mProfile );

   // jar up libarary files
   JobJARLibrary *jobJARlibs = new JobJARLibrary();
   jobJARlibs->Execute( mProfile );

   // jar up data files
   JobJARData *jobJARdata = new JobJARData();
   jobJARdata->Execute( mProfile );

   // SCORM specific stuff
   if ( mProfile->IsSCORMCompliant() )
   {
      JobManifest *jobManifest = new JobManifest();
      jobManifest->Execute( mProfile );
   }
}
