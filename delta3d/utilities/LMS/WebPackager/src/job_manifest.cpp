/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/07 - 11:33
 *
 * @file job_manifest.cpp
 * @version 1.0
 */

// local
#include <job_manifest.h>
#include <package_profile.h>
#include <package_utils.h>
// delta-3d
#include <dtUtil/fileutils.h>
// ansi
#include <iostream>
#include <cctype>
#include <algorithm>
#include <sstream>


//======================================
// CONSTANTS, MACROS, DEFINITIONS
//======================================

// forward refs
class PackageProfile;


struct ToUpper :
  public std::unary_function<int, int>
{
public:
  int operator()(int i) const { return std::toupper(i); }
};

//======================================
// PUBLIC FUNCTIONS
//======================================

JobManifest::JobManifest() : JobXML()
{
}

JobManifest::~JobManifest()
{ 
}

void JobManifest::Execute( PackageProfile *profile )
{
   XMLNode *root = CreateDocument( "manifest" );
   XMLNode *curr = root;
   XMLNode *parent = root;

   // locals
   std::string outputDir  = profile->GetOutputDirectory();
   std::string appParams  = profile->GetApplicationLaunchParams();

   // schemas
   SetAttribute( root, "identifier",         "MANIFEST-TestAARLms-Simulation" );
   SetAttribute( root, "version",            "1.3" );
   SetAttribute( root, "xmlns",              "http://www.imsglobal.org/xsd/imscp_v1p1" );
   SetAttribute( root, "xmlns:lom",          "http://ltsc.ieee.org/xsd/LOM" );
   SetAttribute( root, "xmlns:xsi",          "http://www.w3.org/2001/XMLSchema-instance" );
   SetAttribute( root, "xmlns:adlcp",        "http://www.adlnet.org/xsd/adlcp_v1p3" );
   SetAttribute( root, "xmlns:adlseq",       "http://www.adlnet.org/xsd/adlseq_v1p3" );
   SetAttribute( root, "xmlns:imsss",        "http://www.imsglobal.org/xsd/imsss" );
   SetAttribute( root, "xmlns:adlnav",       "http://www.adlnet.org/xsd/adlnav_v1p3" );
   SetAttribute( root, "xsi:schemaLocation", "http://www.imsglobal.org/xsd/imscp_v1p1 imscp_v1p1.xsd http://ltsc.ieee.org/xsd/LOM lom.xsd http://www.adlnet.org/xsd/adlcp_v1p3 adlcp_v1p3.xsd http://www.adlnet.org/xsd/adlseq_v1p3 adlseq_v1p3.xsd http://www.imsglobal.org/xsd/imsss imsss_v1p0.xsd http://www.adlnet.org/xsd/adlnav_v1p3 adlnav_v1p3.xsd" );

   // metadata
   curr = AddElement( root, "metadata" );
   AddElement( curr, "schema", "ADL SCORM" );
   AddElement( curr, "schemaversion", "CAM 1.3" );

   // SCORM objectives details
   {
      // organizations
      parent = AddElement( root, "organizations" );
      SetAttribute( parent, "default", "ORG-DEFAULT" );
      parent = AddElement( parent, "organization" );
      SetAttribute( parent, "identifier", "ORG-DEFAULT" );
      SetAttribute( parent, "structure", "hierarchical" );
      SetAttribute( parent, "adlseq:objectivesGlobalToSystem", "false" );

      // title
      std::string appName = profile->GetApplicationName();
      AddElement( parent, "title", appName );

      // item
      XMLNode *module = AddElement( parent, "item" );
      SetAttribute( module, "identifier", "module1" );
      AddElement( module, "title", "Module 1" );

      // SCO related information - Sharable Content Objects
      unsigned int numSCOs = profile->GetSCORM_SCO_Count();

      // generate metadata for each SCO
      {
         for ( unsigned int idx=0; idx<numSCOs; idx++ )
         {
            std::string launchPage;
            std::string scoLaunchParams;
            std::string tempStr;
            Options     scoOpts;

            // item
            parent = AddElement( module, "item" );

            //create incremental id string for identifier
            std::stringstream ss;
            std::string strItemNum;
            ss << (idx + 1);
            ss >> strItemNum;
            std::string itemID = "ITEM-" + strItemNum;

            SetAttribute( parent, "identifier", itemID );
            SetAttribute( parent, "isvisible",  "true" );            
            profile->GetSCORM_SCO_LaunchPage( idx, launchPage );
            tempStr = "RES-";
            tempStr += GetFileNameNoExt( launchPage );

	    //            std::transform(tempStr.begin(),tempStr.end(),tempStr.begin(),ToUpper);
	    tempStr = ToUpperCase(tempStr);

            SetAttribute( parent, "identifierref", tempStr );
            profile->GetSCORM_SCO_Options( idx, scoOpts );            
            AddElement( parent, "title", scoOpts.Get( "id" ) );

            // launch parameters
            profile->GetSCORM_SCO_LaunchParams( idx, scoLaunchParams );            
            if ( !(appParams.length() == 0 && scoLaunchParams.length() == 0) )
            {
               tempStr =  appParams;
               tempStr += " ";
               tempStr += scoLaunchParams;
               AddElement( parent, "adlcp:dataFromLMS", tempStr );
            }

            // imsss
            //parent = AddElement( parent, "imsss:sequencing" );
            XMLNode *sequencing = AddElement( parent, "imsss:sequencing" );
            //curr = AddElement( parent, "imsss:controlMode" );
            curr = AddElement( sequencing, "imsss:controlMode" );
            SetAttribute( curr, "choice", "true" );
            SetAttribute( curr, "choiceExit", "true" );
            SetAttribute( curr, "flow", "true" );
            SetAttribute( curr, "forwardOnly", "false" );      

            // get the SCORM objectives from the profile
            std::vector<std::string> objectives;
            std::vector<Options>     options;
            profile->GetSCORM_SCO_Objectives( idx, objectives, options );

            // objectives
            //parent = AddElement( parent, "imsss:objectives" );
            parent = AddElement( sequencing, "imsss:objectives" );
            curr = AddElement( parent, "imsss:primaryObjective" );
            SetAttribute( curr, "satisfiedByMeasure", "false" );
            SetAttribute( curr, "objectiveID", "PRIMARYOBJ" );
            for ( unsigned int j=0; j<objectives.size(); j++ )
            {
               XMLNode *obj = AddElement( parent, "imsss:objective" );
               SetAttribute( obj, "objectiveID", options[j].Get( "id" ) );
               curr = AddElement( obj, "imsss:mapInfo" );
               SetAttribute( curr, "targetObjectiveID", options[j].Get( "name" ) );
               SetAttribute( curr, "readSatisfiedStatus", "true" );
               SetAttribute( curr, "writeSatisfiedStatus", "true" );
               SetAttribute( curr, "readNormalizedMeasure", "true" );
               SetAttribute( curr, "writeNormalizedMeasure", "true" );
            }

            //delivery controls
            XMLNode *deliveryControls = AddElement( sequencing, "imsss:deliveryControls" );
            SetAttribute( deliveryControls, "completionSetByContent", "true" );
            SetAttribute( deliveryControls, "objectiveSetByContent", "true" );
         }
      }

      // generate resources for each SCO
      {
         // get full list of files in output directory
         std::vector<std::string> webResources;
         collectAllFiles( outputDir, "", webResources );

         // resources section
         XMLNode *resources = AddElement( root, "resources" );

         // list web resources for each SCO
         for ( unsigned int idx=0; idx<numSCOs; idx++ )
         {
            // web resources
            XMLNode *resource = AddElement( resources, "resource" );
            std::string launchPage;
            profile->GetSCORM_SCO_LaunchPage( idx, launchPage );
            std::string tempStr = "RES-";
            tempStr += GetFileNameNoExt( launchPage );
	    tempStr = ToUpperCase(tempStr);

	    //            std::transform(tempStr.begin(),tempStr.end(),tempStr.begin(),std::toupper);
            SetAttribute( resource, "identifier", tempStr );
            SetAttribute( resource, "type", "webcontent" );
            SetAttribute( resource, "href", launchPage );
            SetAttribute( resource, "adlcp:scormType", "sco" );
            
            // list output files as web resources
            for ( unsigned int j=0; j<webResources.size(); j++ )
            {
               curr = AddElement( resource, "file" );
               SetAttribute( curr, "href", webResources[j] );
            }
         }
      }
   }

   // save out the file
   std::string filename  = RelativeToAbsolutePath( "imsmanifest.xml", outputDir );
   SaveToFile( filename );

   // cleanup
   CloseDocument();

   // confirmation
   std::cout << "IMSManifest created." << std::endl;
}


//======================================
// PRIVATE FUNCTIONS
//======================================

void JobManifest::collectAllFiles( std::string srcDir, std::string relativeDir, std::vector<std::string> &filenames )
{
   dtUtil::FileUtils &fileUtils = dtUtil::FileUtils::GetInstance();

   // traverse files in source directory
   dtUtil::DirectoryContents files = fileUtils.DirGetFiles( srcDir );
   for( dtUtil::DirectoryContents::const_iterator itr = files.begin(); itr!=files.end(); ++itr )
   {
      std::string child = itr->c_str();
      std::string newSrc = RelativeToAbsolutePath( child, srcDir );
      dtUtil::FileInfo fileInfo = fileUtils.GetFileInfo( newSrc );

      // ignore certain files or directories
      if ( child == "." || child == ".." )
      {
         continue;
      }

      // handle directories
      else if ( fileInfo.fileType == dtUtil::DIRECTORY )
      {
         std::string newRelativeDir;
         if ( relativeDir.length() == 0 )
            newRelativeDir = child;
         else
         {
            newRelativeDir = relativeDir;
            newRelativeDir += '/';
            newRelativeDir += child;
         }         
         collectAllFiles( newSrc, newRelativeDir, filenames );
      }

      // handle files
      else if ( fileInfo.fileType == dtUtil::REGULAR_FILE )
      {
         std::string file;
         if ( relativeDir.length() == 0 )
            file = child;            
         else
            file = relativeDir + '/' + child;
         filenames.push_back( file );
      }
   }
}
