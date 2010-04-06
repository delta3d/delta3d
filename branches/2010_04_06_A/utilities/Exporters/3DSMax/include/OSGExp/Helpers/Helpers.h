/*
 * OSGExp - 3D Studio Max plugin for exporting OpenSceneGraph models.
 * Copyright (C) 2003  VR-C
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *	FILE:			OSGHelper.h
 *
 *	DESCRIPTION:	Include files for other classes which are going to
 *					use the OSG helper objects.
 *
 *  CREATED BY:		Rune Schmidt jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 06.02.2003
 *
 *                  12.10.2005 Joran: Modified stateset helper to a super stateset
 *                  helper.
 */


#define OSGHELPER_CLASS_ID			Class_ID(0x2e49ce6c, 0x322f8ff2)
#define BILLBOARD_CLASS_ID			Class_ID(0x37e33d34, 0x411f89e1)
#define LOD_CLASS_ID				Class_ID(0x5c06d205, 0x2264c9e7)
#define SEQUENCE_CLASS_ID			Class_ID(0x5cf57ee1, 0x1fa8de70)
#define STATESET_CLASS_ID			Class_ID(0x5954f484, 0x52216d13)
#define NODEMASK_CLASS_ID			Class_ID(0x12f2305e, 0x21ff5adf)
#define SWITCH_CLASS_ID				Class_ID(0x63c70cf7, 0x622b4eb1)
#define IMPOSTOR_CLASS_ID			Class_ID(0x2e3b4f86, 0x69d16a99)
#define OCCLUDER_CLASS_ID			Class_ID(0x27982df8, 0x4b013d18)
#define VISIBILITYGROUP_CLASS_ID	Class_ID(0x6d497a78, 0x233a77f5)
#define OSGGROUP_CLASS_ID			Class_ID(0xBADE14E3, 0x37E4EC5B)
#define DOFTRANSFORM_CLASS_ID		Class_ID(0xC3D112D5, 0x17DCEB32)

// When new classes are added then remember to changed the DllEntry.
//extern ClassDesc2* GetOSGHelperDesc();
extern ClassDesc2* GetBillboardDesc();
extern ClassDesc2* GetStateSetDesc();
extern ClassDesc2* GetLODDesc();
extern ClassDesc2* GetSequenceDesc();
extern ClassDesc2* GetNodeMaskDesc();
extern ClassDesc2* GetSwitchDesc();
extern ClassDesc2* GetImpostorDesc();
extern ClassDesc2* GetOccluderDesc();
extern ClassDesc2* GetVisibilityGroupDesc();
extern ClassDesc2* GetOSGGroupDesc();
extern ClassDesc2* GetDOFTransDesc();

#define PLANE_CLASS_ID Class_ID(0x81f1dfc, 0x77566f65)

enum { 
	bilbo_mode,
	bilbo_axis_x,
	bilbo_axis_y,
	bilbo_axis_z,
	bilbo_normal_x,
	bilbo_normal_y,
	bilbo_normal_z,
	bilbo_nodes
};

// Stateset stuff
enum{
	render_bin_num,
	render_bin_name,
	render_bin_mode,
	glmode_cullface,
	glmode_normalize,
	glmode_turnofflighting,
	stateset_nodes,
    render_bin_enable,
    polygonmode_enable,
    polygonmode_frontface,
    polygonmode_backface,
    polygonoffset_enable,
    polygonoffset_factor,
    polygonoffset_unit,
    polygonoffset_depthfunc,
    glmode_alphablending
};

enum {
    rollup_stateset,
    rollup_polygonoffset,
    rollup_polygonmode,
    rollup_renderbin,
    rollup_glmode
};

#define NUM_LOD_OBJECTS 5
enum{
	lod_center_x,
	lod_center_y,
	lod_center_z,
	lod_min,
	lod_max,
	lod_usepaged,
	lod_paged_filename,
   lod_node,
   // The following new parameter ID is to be used by a list widget.
   // The ID value had to be offset from the previous value (lod_node)
   // by 5 since the older version of the interface used 5 buttons,
   // which are no longer going to be used and are now hidden.
   // The hidden buttons need to remain so that Max does not crash
   // when trying to load parameter blocks for older saves of the LOD
   // helper that had used such buttons.
   lod_node_array = lod_node + NUM_LOD_OBJECTS
};

#define NUM_SEQUENCE_OBJECTS 5
enum{
	sequence_begin,
	sequence_end,
	sequence_loop,
	sequence_speed,
	sequence_reps,
	sequence_mode,
	sequence_approach,
	sequence_sample_node,
	sequence_frame_per_sample,
	sequence_time,
	sequence_node
};

enum{
	nodemask_mode,
	nodemask_value,
	nodemask_nodes
};

enum{
	switch_nodes,
	switch_enable_children1,
	switch_enable_children2,
	switch_enable_children3,
	switch_enable_children4,
	switch_enable_children5,
	switch_enable_children6,
	switch_enable_children7,
	switch_enable_children8,
	switch_enable_children9,

};

enum{
	threshold_value,
	impostor_nodes
};

enum{
	occluder_planes,
};

enum{
	visibilitygroup_volume,
	visibilitygroup_nodes
};

enum{
	osggroup_nodes
};

enum{
	doftrans_nodes,
	dof_mult_order,
	dof_min_hpr_h,
	dof_min_hpr_p,
	dof_min_hpr_r,
	dof_max_hpr_h,
	dof_max_hpr_p,
	dof_max_hpr_r,
	dof_min_translate_x,
	dof_min_translate_y,
	dof_min_translate_z,
	dof_max_translate_x,
	dof_max_translate_y,
	dof_max_translate_z,
	dof_min_scale_x,
	dof_min_scale_y,
	dof_min_scale_z,
	dof_max_scale_x,
	dof_max_scale_y,
	dof_max_scale_z
};

