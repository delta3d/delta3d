#include <dtAnim/PoseMeshLoader.h>
#include <dtAnim/PoseMeshXML.h>
#include <dtUtil/xercesparser.h>

using namespace dtAnim;

PoseMeshLoader::PoseMeshLoader()
{
}

PoseMeshLoader::~PoseMeshLoader()
{
}

bool PoseMeshLoader::Load(const std::string& file, MeshDataContainer& toFill)
{
   PoseMeshFileHandler handler;
   dtUtil::XercesParser parser;

   bool result = parser.Parse(file, handler, "");   

   if( result )
   {
      toFill = handler.GetData();
   }

   return result;  
}

