#include <dtEditQt/configurationmanager.h>

#include <dtCore/refptr.h>
#include <dtUtil/xerceswriter.h>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/util/XMLString.hpp>


namespace dtEditQt
{
////////////////////////////////////////////////////////////////////////////////
ConfigurationManager::ConfigurationManager()
   : mLockViewDimensions(false)
{
}

////////////////////////////////////////////////////////////////////////////////
bool ConfigurationManager::ReadXML(const std::string& fileName)
{
   return true;  
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::WriteXML(const std::string& fileName)
{
   dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();
   writer->CreateDocument("STAGEConfig");

   XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();

   ToXML(*doc);

   writer->WriteFile(fileName);
   
}

////////////////////////////////////////////////////////////////////////////////
std::string& ConfigurationManager::GetVariable(const std::string& name)
{
   return mVariables[name];
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::SetVariable(const std::string& name, const std::string& value)
{
   mVariables[name] = value;
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::SetVariable(const std::string& name, int value)
{
   char val[1024];

   sprintf(val, "%d", value);

   SetVariable(name, val);
}

////////////////////////////////////////////////////////////////////////////////
void ConfigurationManager::ToXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc) const
{
   //Write each name/value pair
   std::map<std::string, std::string>::const_iterator it;

   XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* root = doc.getDocumentElement();      

   for(it = mVariables.begin(); it != mVariables.end(); ++it)
   {

      XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(it->first.c_str());
      XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(it->second.c_str());
      
      XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* element = doc.createElement(name);      
      XERCES_CPP_NAMESPACE_QUALIFIER DOMText* txt = doc.createTextNode(value);

      element->appendChild(txt);
      root->appendChild(element);

      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
      XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

   }
}


////////////////////////////////////////////////////////////////////////////////
bool ConfigurationManager::FromXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc)
{
   //Read each name/value pair

   return false;
}

} //namespace dtEditQt
