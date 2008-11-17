#include <dtAnim/posemeshloader.h>
#include <dtAnim/posemeshxml.h>

#include <dtUtil/xercesparser.h>
#include <dtUtil/log.h>

using namespace dtAnim;

////////////////////////////////////////////////////////////////////////////////
PoseMeshLoader::PoseMeshLoader()
{
}

////////////////////////////////////////////////////////////////////////////////
PoseMeshLoader::~PoseMeshLoader()
{
}

////////////////////////////////////////////////////////////////////////////////
bool PoseMeshLoader::Load(const std::string& file, MeshDataContainer& toFill)
{
   PoseMeshFileHandler handler;
   dtUtil::XercesParser parser;

   bool result = parser.Parse(file, handler, "");   

   if( result )
   {
      toFill = handler.GetData();
   }
   else
   {
      LOG_ERROR("Unable to load pose mesh file: " + file);
   }

   return result;  
}

