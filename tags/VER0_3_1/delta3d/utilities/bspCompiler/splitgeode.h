// splitgeode.h: Declaration of the SplitGeode function.
//
//////////////////////////////////////////////////////////////////////

#ifndef SPLIT_GEODE
#define SPLIT_GEODE

#include <vector>

#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Matrix>
#include <osg/PrimitiveSet>
#include <osg/Transform>
#include <osg/Node>
#include <osg/Plane>

/**
 * Splits a geode across a plane.
 *
 * @param plane the plane to split across
 * @param path the path to the geode
 * @param aboveGeodes the paths to geodes above the plane
 * @param belowGeodes the paths to geodes below the plane
 */
void SplitGeode(const osg::Plane& plane,
                osg::NodePath& path,
                std::vector<osg::NodePath>& abovePaths,
                std::vector<osg::NodePath>& belowPaths);

#endif // SPLIT_GEODE
