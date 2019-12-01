/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
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
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_osgoptimizerpanel.h"
#include <dtQt/osgoptimizerpanel.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgOptimizerPanel::OsgOptimizerPanel(QWidget* parent)
      : BaseClass(parent)
      , mUI(new Ui::OsgOptimizerPanel)
   {
      mUI->setupUi(this);

      mOptions = osgUtil::Optimizer::DEFAULT_OPTIMIZATIONS;

      CreateOptionMapping();

      UpdateUI();

      // Setup connections after updating the UI so that
      // sig/slots are not unnecessarily triggered.
      CreateConnections();
   }

   OsgOptimizerPanel::~OsgOptimizerPanel()
   {
      mOptionMap.clear();
      mCheckboxMap.clear();

      delete mUI;
      mUI = nullptr;
   }

   void OsgOptimizerPanel::CreateOptionMapping()
   {
      if ( ! mCheckboxMap.empty())
      {
         return;
      }

      typedef osgUtil::Optimizer Opt;

      mCheckboxMap[Opt::FLATTEN_STATIC_TRANSFORMS] = mUI->mFlattenStaticTransforms;
      mCheckboxMap[Opt::REMOVE_REDUNDANT_NODES] = mUI->mRemoveRedundantNodes;
      mCheckboxMap[Opt::REMOVE_LOADED_PROXY_NODES] = mUI->mRemoveLoadedProxyNodes;
      mCheckboxMap[Opt::COMBINE_ADJACENT_LODS] = mUI->mCombineAdjacentLODs;
      mCheckboxMap[Opt::SHARE_DUPLICATE_STATE] = mUI->mShareDuplicatState;
      mCheckboxMap[Opt::MERGE_GEOMETRY] = mUI->mMergeGeometry;
      mCheckboxMap[Opt::CHECK_GEOMETRY] = mUI->mCheckGeometry;
      mCheckboxMap[Opt::MAKE_FAST_GEOMETRY] = mUI->mMakeFastGeometry;
      mCheckboxMap[Opt::SPATIALIZE_GROUPS] = mUI->mSpatializeGroups;
      mCheckboxMap[Opt::COPY_SHARED_NODES] = mUI->mCopySharedNodes;
      mCheckboxMap[Opt::TRISTRIP_GEOMETRY] = mUI->mTristripGeometry;
      mCheckboxMap[Opt::TESSELLATE_GEOMETRY] = mUI->mTessellateGeometry;
      mCheckboxMap[Opt::OPTIMIZE_TEXTURE_SETTINGS] = mUI->mOptimizeTextureSettings;
      mCheckboxMap[Opt::MERGE_GEODES] = mUI->mMergeGeodes;
      mCheckboxMap[Opt::FLATTEN_BILLBOARDS] = mUI->mFlattenBillboards;
      mCheckboxMap[Opt::TEXTURE_ATLAS_BUILDER] = mUI->mTextureAtlasBuilder;
      mCheckboxMap[Opt::STATIC_OBJECT_DETECTION] = mUI->mStaticObjectDetection;
      mCheckboxMap[Opt::FLATTEN_STATIC_TRANSFORMS_DUPLICATING_SHARED_SUBGRAPHS] = mUI->mFlattenSTDSS;
      mCheckboxMap[Opt::INDEX_MESH] = mUI->mIndexMesh;
      mCheckboxMap[Opt::VERTEX_POSTTRANSFORM] = mUI->mVertexPostTransform;
      mCheckboxMap[Opt::VERTEX_PRETRANSFORM] = mUI->mVertexPreTransform;

      // Setup the reverse map.
      QCheckBox* checkbox = nullptr;
      OptionCheckboxMap::iterator curIter = mCheckboxMap.begin();
      OptionCheckboxMap::iterator endIter = mCheckboxMap.end();
      for (; curIter != endIter; ++curIter)
      {
         OptimizerOptions options = curIter->first;
         checkbox = curIter->second;

         mOptionMap.insert(std::make_pair(checkbox, options));
      }
   }

   void OsgOptimizerPanel::CreateConnections()
   {
      // Setup Checkboxes
      QCheckBox* checkbox = nullptr;
      OptionCheckboxMap::iterator curIter = mCheckboxMap.begin();
      OptionCheckboxMap::iterator endIter = mCheckboxMap.end();
      for (; curIter != endIter; ++curIter)
      {
         checkbox = curIter->second;

         connect(checkbox, SIGNAL(stateChanged(int)),
            this, SLOT(OnCheckboxChanged(int)));
      }

      // Setup Buttons
      connect(mUI->mButtonSelectAll, SIGNAL(clicked()),
         this, SLOT(OnSelectAllClicked()));
      connect(mUI->mButtonDefaults, SIGNAL(clicked()),
         this, SLOT(OnSelectDefaultsClicked()));
   }

   OsgOptimizerPanel::OptimizerOptions OsgOptimizerPanel::GetOptions() const
   {
      return mOptions;
   }

   void OsgOptimizerPanel::UpdateUI()
   {
      // Prevent signals from checkboxes while they are udating
      // so that problems do not occur from subsequent calls to
      // relevant slots tied to such signals.
      blockSignals(true);

      // Determine the checked state for each option.
      QCheckBox* checkbox = nullptr;
      OptionCheckboxMap::iterator curIter = mCheckboxMap.begin();
      OptionCheckboxMap::iterator endIter = mCheckboxMap.end();
      for (; curIter != endIter; ++curIter)
      {
         OptimizerOptions options = curIter->first;
         checkbox = curIter->second;

         bool isChecked = (options & mOptions) != 0;

         checkbox->setChecked(isChecked);
      }
      
      // Done updating the checkboxes. Enable their signals again.
      blockSignals(false);

      // Determine if the Presets list has anything to select.
      mUI->mPreset->setEnabled(mUI->mPreset->count() > 0);
   }

   void OsgOptimizerPanel::OnCheckboxChanged(int state)
   {
      QCheckBox* checkbox = static_cast<QCheckBox*>(sender());
      if (checkbox != nullptr)
      {
         CheckboxOptionMap::iterator foundIter = mOptionMap.find(checkbox);

         if (foundIter != mOptionMap.end())
         {
            OptimizerOptions ops = foundIter->second;

            if (state == Qt::Checked)
            {
               mOptions = OptimizerOptions(ops | mOptions);
            }
            else // Not Checked
            {
               mOptions = OptimizerOptions((~ops) & mOptions);
            }
         }
      }
   }

   void OsgOptimizerPanel::OnSelectAllClicked()
   {
      mOptions = osgUtil::Optimizer::ALL_OPTIMIZATIONS;

      UpdateUI();
   }

   void OsgOptimizerPanel::OnSelectDefaultsClicked()
   {
      mOptions = osgUtil::Optimizer::DEFAULT_OPTIMIZATIONS;

      UpdateUI();
   }

}
