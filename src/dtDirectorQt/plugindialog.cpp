/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author:
 * Edited By: Jeff P. Houde
 */
#include <prefix/dtdirectorqtprefix.h>
#include <dtDirectorQt/plugindialog.h>
#include <dtDirectorQt/pluginmanager.h>
#include <QtGui/QMenu>
#include <iostream>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   PluginDialog::PluginDialog(DirectorEditor* editor)
      : mEditor(editor)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   PluginDialog::~PluginDialog()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void PluginDialog::OnOpenDialog()
   {
      // create the plugin list
      QDialog dialog(mEditor);
      
      // load user interface from QT designer file
      Ui_PluginDialog ui;
      ui.setupUi(&dialog);
      
      connect(ui.mPluginList, SIGNAL(itemChanged(QListWidgetItem*)),
              this, SLOT(OnPluginChanged(QListWidgetItem*)));

      PluginManager* manager = mEditor->GetPluginManager();
      if (manager == NULL)
      {
         return;
      }

      // get a list of plugin factories
      std::list<std::string> plugins;
      manager->GetAvailablePlugins(plugins);

      for (std::list<std::string>::iterator i = plugins.begin(); i != plugins.end(); ++i)
      {
         // create list entry for this plugin
         QListWidgetItem* item = new QListWidgetItem((*i).c_str());
         std::string pluginDesc;
         try
         {
            dtDirector::PluginFactory* factory = manager->GetPluginFactory((*i));
            pluginDesc = factory->GetDescription();
         }
         catch (...)
         {
            //factory doesn't exist?
            pluginDesc = "No plugin description";
         }

         item->setToolTip(QString::fromStdString(pluginDesc));
         
         // set checkbox to checked if plugin is active
         if (manager->IsInstantiated(*i))
         {
            item->setCheckState(Qt::Checked);
         }
         else
         {
            item->setCheckState(Qt::Unchecked);
         }

         // can't disable system plugins
         if (!manager->IsSystemPlugin(*i))
         {
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
         }
         else
         {
            item->setFlags(Qt::ItemIsUserCheckable);
         }
         // add to list
         ui.mPluginList->addItem(item);
      }

      // show the dialog
      if (dialog.exec() == QDialog::Accepted)
      {
         OnApplyChanges(ui.mPluginList);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void PluginDialog::OnPluginChanged(QListWidgetItem* pluginItem)
   {
      dtDirector::PluginManager* manager = mEditor->GetPluginManager();
      if (!manager)
      {
         return;
      }

      const std::string pluginName = pluginItem->text().toStdString();

      // When activating a plugin, make sure we also activate any
      // plugins that it depends on.
      if (pluginItem->checkState() == Qt::Checked)
      {
         std::list<std::string> deps = manager->GetPluginDependencies(pluginName);
         while(!deps.empty())
         {
            std::string dependency = deps.front();
            deps.pop_front();

            QList<QListWidgetItem*> pluginItems = pluginItem->listWidget()->findItems(QString(dependency.c_str()), Qt::MatchExactly);

            // check if dependency can be fulfilled
            if (pluginItems.empty())
            {
               pluginItem->setCheckState(Qt::Unchecked);
               return;
            }

            for (int pluginIndex = 0; pluginIndex < pluginItems.size(); pluginIndex++)
            {
               QListWidgetItem* pluginItem = pluginItems[pluginIndex];
               if (pluginItem)
               {
                  pluginItem->setCheckState(Qt::Checked);

                  // now double check to see if those dependencies were turned on properly.
                  if (pluginItem->checkState() == Qt::Unchecked)
                  {
                     pluginItem->setCheckState(Qt::Unchecked);
                     return;
                  }
               }
            }
         }
      }
      // When deactivating a plugin, make sure to also deactivate any
      // plugins that depend on this one.
      else
      {
         std::list<std::string> allPlugins;
         manager->GetAvailablePlugins(allPlugins);
         while (!allPlugins.empty())
         {
            std::string plugin = allPlugins.front();
            allPlugins.pop_front();

            std::list<std::string> deps = manager->GetPluginDependencies(plugin);
            while (!deps.empty())
            {
               std::string dependency = deps.front();
               deps.pop_front();

               // If the active plugin depends on this plugin, then we need to stop that one too.
               if (dependency == pluginName)
               {
                  QList<QListWidgetItem*> pluginItems = pluginItem->listWidget()->findItems(QString(plugin.c_str()), Qt::MatchExactly);

                  // check if dependency can be fulfilled
                  if (pluginItems.empty())
                  {
                     break;
                  }

                  for (int pluginIndex = 0; pluginIndex < pluginItems.size(); pluginIndex++)
                  {
                     QListWidgetItem* pluginItem = pluginItems[pluginIndex];
                     if (pluginItem)
                     {
                        pluginItem->setCheckState(Qt::Unchecked);
                     }
                  }
                  break;
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void PluginDialog::OnApplyChanges(QListWidget* listWidget)
   {
      dtDirector::PluginManager* manager = mEditor->GetPluginManager();

      for (int i = 0; i < listWidget->count(); ++i)
      {
         QListWidgetItem* item = listWidget->item(i);
         std::string pluginName = item->text().toStdString();
         bool enabled = (item->checkState() == Qt::Checked);
               
         // can't load/unload system plugins
         if (manager->IsSystemPlugin(pluginName))
         {
            continue;
         }

         if (!enabled && manager->IsInstantiated(pluginName))
         {
            manager->StopPlugin(pluginName);
         }
         else if (enabled && !manager->IsInstantiated(pluginName))
         {
            manager->StartPlugin(pluginName);
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
