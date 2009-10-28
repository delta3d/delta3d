#include "mapexporter.h"
#include "ui_mapexporter.h"

#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/editordata.h>

#include <dtDAL/map.h>
#include <dtDAL/project.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/containeractorproperty.h>
#include <dtDAL/arrayactorpropertybase.h>

#include <QtGui/QFileDialog>

#include <osg/Node>
#include <osg/Group>
#include <osgDB/FileNameUtils>

const std::string MapExporterPlugin::PLUGIN_NAME = "Map Exporter";


////////////////////////////////////////////////////////////////////////////////
MapExporterPlugin::MapExporterPlugin(MainWindow* mw)
   : mMainWindow(mw)
   , mExportButton(NULL)
{
   // apply layout made with QT designer
   Ui_MapExporterTool ui;
   ui.setupUi(this);

   mMainWindow->addDockWidget(Qt::RightDockWidgetArea, this);

   mExportButton = ui.mExportButton;

   connect(mExportButton, SIGNAL(clicked()), this, SLOT(onExportButtonPressed()));
}

////////////////////////////////////////////////////////////////////////////////
MapExporterPlugin::~MapExporterPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void MapExporterPlugin::Destroy()
{
   mExportButton = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void MapExporterPlugin::closeEvent(QCloseEvent* event)
{
   mMainWindow->GetPluginManager()->StopPlugin(PLUGIN_NAME);
}

////////////////////////////////////////////////////////////////////////////////
void MapExporterPlugin::onExportButtonPressed()
{
   // Now get the new file name to save as...
   QFileInfo filename = QFileDialog::getSaveFileName(this,
      tr("Save Map Package"),
      QString(),
      tr("Delta Package(*.dtpkg)"));

   if (!filename.filePath().isEmpty())
   {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      dtUtil::Packager packager;

      // Find out all of the assets used for the map.
      dtDAL::Map* map = EditorData::GetInstance().getCurrentMap();
      if (map)
      {
         // Add the map file to the package.
         std::string contextPath = dtDAL::Project::GetInstance().GetContext();
         std::string path = contextPath + "\\maps\\" + map->GetFileName();
         packager.AddFile(path, "maps");

         std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > proxies;
         map->GetAllProxies(proxies);

         for (int proxyIndex = 0; proxyIndex < (int)proxies.size(); proxyIndex++)
         {
            dtDAL::ActorProxy* proxy = proxies[proxyIndex];
            if (proxy)
            {
               std::vector<const dtDAL::ActorProperty*> properties;
               proxy->GetPropertyList(properties);

               for (int propIndex = 0; propIndex < (int)properties.size(); propIndex++)
               {
                  AddResourcesFromProperty(properties[propIndex], packager);
               }

               dtCore::DeltaDrawable* drawable = proxy->GetActor();
               if (drawable)
               {
                  AddResourcesFromNode(drawable->GetOSGNode(), packager);
               }
            }
         }
      }

      std::string path = filename.absolutePath().toStdString();
      path += '/' + filename.baseName().toStdString();
      packager.PackPackage(path, true);

      QApplication::restoreOverrideCursor();
   }
}

////////////////////////////////////////////////////////////////////////////////
bool MapExporterPlugin::AddResourcesFromProperty(const dtDAL::ActorProperty* prop, dtUtil::Packager& packager)
{
   if (prop->GetPropertyType().IsResource())
   {
      // Check if this property is a resource.
      const dtDAL::ResourceActorProperty* resourceProp =
         dynamic_cast<const dtDAL::ResourceActorProperty*>(prop);

      if (resourceProp)
      {
         // Since this is a resource property, extract the resource from it and add
         // it to the package.
         dtDAL::ResourceDescriptor* descriptor = resourceProp->GetValue();

         if (descriptor && !descriptor->GetResourceIdentifier().empty())
         {
            std::string contextPath = dtDAL::Project::GetInstance().GetContext();
            std::string resourcePath = dtDAL::Project::GetInstance().GetResourcePath(*descriptor);
            std::string path = contextPath + '\\' + resourcePath;

            std::string resourceDir = osgDB::getFilePath(resourcePath);

            packager.AddFile(path, resourceDir);
         }

         return true;
      }

      return false;
   }

   // If this property is a container, check each property inside it.
   if (prop->GetPropertyType() == dtDAL::DataType::CONTAINER)
   {
      bool returnVal = false;

      const dtDAL::ContainerActorProperty* containerProp =
         dynamic_cast<const dtDAL::ContainerActorProperty*>(prop);

      if (containerProp)
      {
         int propCount = containerProp->GetPropertyCount();
         for (int propIndex = 0; propIndex < propCount; propIndex++)
         {
            const dtDAL::ActorProperty* childProp = containerProp->GetProperty(propIndex);
            if (childProp)
            {
               returnVal |= AddResourcesFromProperty(childProp, packager);
            }
         }
      }

      return returnVal;
   }

   // If this property is an array, then check each index of the array.
   if (prop->GetPropertyType() == dtDAL::DataType::ARRAY)
   {
      bool returnVal = false;

      const dtDAL::ArrayActorPropertyBase* arrayProp =
         dynamic_cast<const dtDAL::ArrayActorPropertyBase*>(prop);

      if (arrayProp)
      {
         int propCount = arrayProp->GetArraySize();
         for (int propIndex = 0; propIndex < propCount; propIndex++)
         {
            // For an array, we only need to check the first index to see if it has
            // a valid resource property.  We only iterate through the rest of the
            // array if the first one was valid.
            arrayProp->SetIndex(propIndex);

            if (!AddResourcesFromProperty(arrayProp->GetArrayProperty(), packager))
            {
               break;
            }

            returnVal = true;
         }
      }

      return returnVal;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool MapExporterPlugin::AddResourcesFromNode(osg::Node* node, dtUtil::Packager& packager)
{
   if (!node) return false;

   // Get the state set of this node and find any shaders assigned.
   osg::StateSet* ss = node->getStateSet();
   if (ss)
   {
      //// Find all textures.
      //osg::StateSet::TextureAttributeList& textureList = ss->getTextureAttributeList();

      //for (int textureIndex = 0; textureIndex < (int)textureList.size(); textureIndex++)
      //{
      //   osg::StateSet::AttributeList& attributes = textureList[textureIndex];

      //   for (AttributeList::iterator i = attributes.begin();
      //      i != attributes.end(); ++i)
      //   {
      //      osg::StateAttribute* attribute = i->second.first.get();
      //      if (attribute && attribute->isTextureAttribute())
      //      {
      //      }
      //   }
      //}

      // Find all attributes.
      osg::StateSet::AttributeList& attributes = ss->getAttributeList();
      for (osg::StateSet::AttributeList::iterator i = attributes.begin();
         i != attributes.end(); ++i)
      {
         osg::StateAttribute* attribute = i->second.first.get();
         if (attribute)
         {
            // Find any shader attributes.
            if (attribute->getType() == osg::StateAttribute::PROGRAM)
            {
               osg::Program* program = dynamic_cast<osg::Program*>(attribute);
               if (!program) continue;

               int shaderCount = (int)program->getNumShaders();
               for (int shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++)
               {
                  osg::Shader* shader = program->getShader(shaderIndex);
                  if (shader)
                  {
                     std::string path = shader->getFileName();
                     std::string resourceDir = "shaders";

                     packager.AddFile(path, resourceDir);
                  }
               }
            }
         }
      }
   }

   // If this node is a group, iterate through all its' children.
   osg::Group* group = node->asGroup();
   if (group)
   {
      for (int childIndex = 0; childIndex < (int)group->getNumChildren(); childIndex++)
      {
         AddResourcesFromNode(group->getChild(childIndex), packager);
      }
   }
}

////////////////////////////////////////////////////////////

namespace MapExporter
{
class DT_MAP_EXPORTER_EXPORT PluginFactory : public dtEditQt::PluginFactory
{
public:

   PluginFactory() {}
   ~PluginFactory() {}

   /** get the name of the plugin */
   virtual std::string GetName() { return MapExporterPlugin::PLUGIN_NAME; }

   /** get a description of the plugin */
   virtual std::string GetDescription() { return "Exports a map with all its' assets to a packaged file."; }

   virtual void GetDependencies(std::list<std::string>& deps) 
   {
   }

    /** construct the plugin and return a pointer to it */
   virtual Plugin* Create(MainWindow* mw) 
   {
      mPlugin = new MapExporterPlugin(mw);
      return mPlugin;
   }

   virtual void Destroy() 
   {
      delete mPlugin;
   }

private:

   Plugin* mPlugin;
}; 
} //namespace MapExporterPlugin

extern "C" DT_MAP_EXPORTER_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new MapExporter::PluginFactory;
}
