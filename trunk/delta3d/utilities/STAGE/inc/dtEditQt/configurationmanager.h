#ifndef _STAGE_CONFIGURATION_MANAGER_H__
#define _STAGE_CONFIGURATION_MANAGER_H__

#include <map>

#include <dtDAL/serializeable.h>

#include <osg/Vec2>

namespace dtEditQt
{

class ConfigurationManager : public dtDAL::XMLSerializeable 
{
public:
   ConfigurationManager();

   bool ReadXML(const std::string& fileName);
   void WriteXML(const std::string& fileName);

   std::string& GetVariable(const std::string& name);
   void SetVariable(const std::string& name, const std::string& value);
   void SetVariable(const std::string& name, int value);

   //implementations of pure virtuals
   void ToXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc) const;
   bool FromXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc);

private:
   std::map<std::string, std::string> mVariables;     // name/value pairs

   bool mLockViewDimensions;     //if true, user cannot change view dimensions
                                 //(e.g. if there is a single perspective view, user
                                 // cannot resize to reveal the other "hidden" views).

};

} //namespace dtEditQt

#endif //_STAGE_CONFIGURATION_MANAGER_H__
