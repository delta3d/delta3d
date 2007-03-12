
#include "dtChar/characterfilehandler.h"
#include "dtUtil/xercesutils.h"

using namespace dtChar;

void CharacterFileHandler::startElement( const XMLCh* const uri,const XMLCh* const localname,
                                                 const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs )
{
   dtUtil::XMLStringConverter elementName(localname);
   const std::string elementStr = elementName.ToString();

   dtUtil::AttributeSearch search;
   dtUtil::AttributeSearch::ResultMap results;
   results = search(attrs);

   if (elementStr == "skeleton")
   {
      mSkeletonFilename = results.find("filename")->second;
   }
   else if (elementStr == "animation")
   {
      mAnimationFilenames.push_back(results.find("filename")->second);
   }
   else if (elementStr == "mesh")
   {
      mMeshFilenames.push_back(results.find("filename")->second);
   }
   else if (elementStr == "material")
   {
      mMaterialFilenames.push_back(results.find("filename")->second);
   }

}

CharacterFileHandler::CharacterFileHandler()
{

}

CharacterFileHandler::~CharacterFileHandler()
{

}