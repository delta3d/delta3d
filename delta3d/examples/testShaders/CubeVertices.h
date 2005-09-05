#ifndef __CUBE_VERTICES_H__ 
#define __CUBE_VERTICES_H__

#include <osg/ref_ptr>
#include <osg/Vec2>
#include <osg/Vec3>


typedef struct _CV_
{

public:

	_CV_();

	 osg::ref_ptr<osg::Vec3Array> mVerts;
	 osg::ref_ptr<osg::Vec3Array> mVerts2;
	 osg::ref_ptr<osg::Vec3Array> mVerts3;

	 osg::ref_ptr<osg::Vec3Array> mNormals;

	 osg::ref_ptr<osg::Vec2Array> mTexCoords;
	 osg::ref_ptr<osg::Vec2Array> mTexCoords2;
	 osg::ref_ptr<osg::Vec2Array> mTexCoords3;

	 float mTexRepeat;
	 float mSize;

}CubeVertices;


//////////////////////////////////////////////////////////////////////////
//Control Variables
//////////////////////////////////////////////////////////////////////////

_CV_::_CV_()
{

	mTexRepeat = 4.0f;
	mSize = 50.0f;

	mVerts = new osg::Vec3Array(36);
	mVerts2 = new osg::Vec3Array(36);
	mVerts3 = new osg::Vec3Array(36);
	mNormals = new osg::Vec3Array(36);
	mTexCoords = new osg::Vec2Array(36);
	mTexCoords2 = new osg::Vec2Array(36);
	mTexCoords3 = new osg::Vec2Array(36);




//////////////////////////////////////////////////////////////////////////
//Verts 1
//////////////////////////////////////////////////////////////////////////


//front
   (*mVerts)[0] = osg::Vec3(mSize, mSize, -mSize);
   (*mVerts)[1] = osg::Vec3(-mSize, mSize, -mSize);
   (*mVerts)[2] = osg::Vec3(-mSize, -mSize, -mSize);

   (*mVerts)[3] = osg::Vec3 (mSize, -mSize, -mSize);
   (*mVerts)[4] = osg::Vec3 (mSize, mSize, -mSize);
   (*mVerts)[5] = osg::Vec3 (-mSize, -mSize, -mSize);

//back
   (*mVerts)[6] = osg::Vec3 (-mSize, mSize, mSize);
   (*mVerts)[7] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts)[8] = osg::Vec3 (-mSize, -mSize, mSize);

   (*mVerts)[9] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts)[10] = osg::Vec3 (mSize, -mSize, mSize);
   (*mVerts)[11] = osg::Vec3 (-mSize, -mSize, mSize);

//left
   (*mVerts)[12] = osg::Vec3 (-mSize, mSize, -mSize);
   (*mVerts)[13] = osg::Vec3 (-mSize, mSize, mSize);
   (*mVerts)[14] = osg::Vec3 (-mSize, -mSize, mSize);

   (*mVerts)[15] = osg::Vec3 (-mSize, -mSize, -mSize);
   (*mVerts)[16] = osg::Vec3 (-mSize, mSize, -mSize);
   (*mVerts)[17] = osg::Vec3 (-mSize, -mSize, mSize);

//right
   (*mVerts)[18] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts)[19] = osg::Vec3 (mSize, mSize, -mSize);
   (*mVerts)[20] = osg::Vec3 (mSize, -mSize, -mSize);

   (*mVerts)[21] = osg::Vec3 (mSize, -mSize, mSize);
   (*mVerts)[22] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts)[23] = osg::Vec3 (mSize, -mSize, -mSize);

//top
   (*mVerts)[24] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts)[25] = osg::Vec3 (-mSize, mSize, mSize);
   (*mVerts)[26] = osg::Vec3 (-mSize, mSize, -mSize);

   (*mVerts)[27] = osg::Vec3 (mSize, mSize, -mSize);
   (*mVerts)[28] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts)[29] = osg::Vec3 (-mSize, mSize, -mSize);

//bottom
   (*mVerts)[30] = osg::Vec3 (mSize, -mSize, -mSize);
   (*mVerts)[31] = osg::Vec3 (-mSize, -mSize, -mSize);
   (*mVerts)[32] = osg::Vec3 (-mSize, -mSize, mSize);

   (*mVerts)[33] = osg::Vec3 (mSize, -mSize, mSize);
   (*mVerts)[34] = osg::Vec3 (mSize, -mSize, -mSize);
   (*mVerts)[35] = osg::Vec3 (-mSize, -mSize, mSize);




//////////////////////////////////////////////////////////////////////////
//Verts 2
//////////////////////////////////////////////////////////////////////////

//front
   (*mVerts2)[0] = osg::Vec3 (-mSize, mSize, -mSize);
   (*mVerts2)[1] = osg::Vec3 (-mSize, -mSize, -mSize);
   (*mVerts2)[2] = osg::Vec3 (mSize, mSize, -mSize);

   (*mVerts2)[3] = osg::Vec3 (mSize, mSize, -mSize);
   (*mVerts2)[4] = osg::Vec3 (-mSize, -mSize, -mSize);
   (*mVerts2)[5] = osg::Vec3 (mSize, -mSize, -mSize);

//back
   (*mVerts2)[6] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts2)[7] = osg::Vec3 (-mSize, -mSize, mSize);
   (*mVerts2)[8] = osg::Vec3 (-mSize, mSize, mSize);

   (*mVerts2)[9] = osg::Vec3 (mSize, -mSize, mSize);
   (*mVerts2)[10] = osg::Vec3 (-mSize, -mSize, mSize);
   (*mVerts2)[11] = osg::Vec3 (mSize, mSize, mSize);

//left
   (*mVerts2)[12] = osg::Vec3 (-mSize, mSize, mSize);
   (*mVerts2)[13] = osg::Vec3 (-mSize, -mSize, mSize);
   (*mVerts2)[14] = osg::Vec3 (-mSize, mSize, -mSize);

   (*mVerts2)[15] = osg::Vec3 (-mSize, mSize, -mSize);
   (*mVerts2)[16] = osg::Vec3 (-mSize, -mSize, mSize);
   (*mVerts2)[17] = osg::Vec3 (-mSize, -mSize, -mSize);

//right
   (*mVerts2)[18] = osg::Vec3 (mSize, mSize, -mSize);
   (*mVerts2)[19] = osg::Vec3 (mSize, -mSize, -mSize);
   (*mVerts2)[20] = osg::Vec3 (mSize, mSize, mSize);

   (*mVerts2)[21] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts2)[22] = osg::Vec3 (mSize, -mSize, -mSize);
   (*mVerts2)[23] = osg::Vec3 (mSize, -mSize, mSize);

//top
   (*mVerts2)[24] = osg::Vec3 (-mSize, mSize, mSize);
   (*mVerts2)[25] = osg::Vec3 (-mSize, mSize, -mSize);
   (*mVerts2)[26] = osg::Vec3 (mSize, mSize, mSize);

   (*mVerts2)[27] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts2)[28] = osg::Vec3 (-mSize, mSize, -mSize);
   (*mVerts2)[29] = osg::Vec3 (mSize, mSize, -mSize);

//bottom
   (*mVerts2)[30] = osg::Vec3 (-mSize, -mSize, -mSize);
   (*mVerts2)[31] = osg::Vec3 (-mSize, -mSize, mSize);
   (*mVerts2)[32] = osg::Vec3 (mSize, -mSize, -mSize);

   (*mVerts2)[33] = osg::Vec3 (mSize, -mSize, -mSize);
   (*mVerts2)[34] = osg::Vec3 (-mSize, -mSize, mSize);
   (*mVerts2)[35] = osg::Vec3 (mSize, -mSize, mSize);




//////////////////////////////////////////////////////////////////////////
//Verts 3
//////////////////////////////////////////////////////////////////////////

//front
   (*mVerts3)[0] = osg::Vec3 (-mSize, -mSize, -mSize);
   (*mVerts3)[1] = osg::Vec3 (mSize, mSize, -mSize);
   (*mVerts3)[2] = osg::Vec3 (-mSize, mSize, -mSize);

   (*mVerts3)[3] = osg::Vec3 (-mSize, -mSize, -mSize);
   (*mVerts3)[4] = osg::Vec3 (mSize, -mSize, -mSize);
   (*mVerts3)[5] = osg::Vec3 (mSize, mSize, -mSize);

//back
   (*mVerts3)[6] = osg::Vec3 (-mSize, -mSize, mSize);
   (*mVerts3)[7] = osg::Vec3 (-mSize, mSize, mSize);
   (*mVerts3)[8] = osg::Vec3 (mSize, mSize, mSize);

   (*mVerts3)[9] = osg::Vec3 (-mSize, -mSize, mSize);
   (*mVerts3)[10] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts3)[11] = osg::Vec3 (mSize, -mSize, mSize);

//left
   (*mVerts3)[12] = osg::Vec3 (-mSize, -mSize, mSize);
   (*mVerts3)[13] = osg::Vec3 (-mSize, mSize, -mSize);
   (*mVerts3)[14] = osg::Vec3 (-mSize, mSize, mSize);

   (*mVerts3)[15] = osg::Vec3 (-mSize, -mSize, mSize	);
   (*mVerts3)[16] = osg::Vec3 (mSize, -mSize, -mSize);
   (*mVerts3)[17] = osg::Vec3 (mSize, mSize, -mSize);

//right
   (*mVerts3)[18] = osg::Vec3 (mSize, -mSize, -mSize);
   (*mVerts3)[19] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts3)[20] = osg::Vec3 (mSize, mSize, -mSize);

   (*mVerts3)[21] = osg::Vec3 (mSize, -mSize, -mSize);
   (*mVerts3)[22] = osg::Vec3 (mSize, -mSize, mSize);
   (*mVerts3)[23] = osg::Vec3 (mSize, mSize, mSize);

//top
   (*mVerts3)[24] = osg::Vec3 (-mSize, mSize, -mSize);
   (*mVerts3)[25] = osg::Vec3 (mSize, mSize, mSize);
   (*mVerts3)[26] = osg::Vec3 (-mSize, mSize, mSize);

   (*mVerts3)[27] = osg::Vec3 (-mSize, mSize, -mSize);
   (*mVerts3)[28] = osg::Vec3 (mSize, mSize, -mSize );
   (*mVerts3)[29] = osg::Vec3 (mSize, mSize, mSize);

//bottom
   (*mVerts3)[30] = osg::Vec3 (-mSize, -mSize, mSize);
   (*mVerts3)[31] = osg::Vec3 (mSize, -mSize, -mSize);
   (*mVerts3)[32] = osg::Vec3 (-mSize, -mSize, -mSize);

   (*mVerts3)[33] = osg::Vec3 (-mSize, -mSize, mSize);
   (*mVerts3)[34] = osg::Vec3 (mSize, -mSize, mSize);
   (*mVerts3)[35] = osg::Vec3 (mSize, -mSize, -mSize);




//////////////////////////////////////////////////////////////////////////
//Normals
//////////////////////////////////////////////////////////////////////////

//front
   (*mNormals)[0] = osg::Vec3 (0.0f, 0.0f, 1.0f);
   (*mNormals)[1] = osg::Vec3 (0.0f, 0.0f, 1.0f);
   (*mNormals)[2] = osg::Vec3 (0.0f, 0.0f, 1.0f);

   (*mNormals)[3] = osg::Vec3 (0.0f, 0.0f, 1.0f);
   (*mNormals)[4] = osg::Vec3 (0.0f, 0.0f, 1.0f);
   (*mNormals)[5] = osg::Vec3 (0.0f, 0.0f, 1.0f);

//back
   (*mNormals)[6] = osg::Vec3 (0.0f, 0.0f, -1.0f);
   (*mNormals)[7] = osg::Vec3 (0.0f, 0.0f, -1.0f);
   (*mNormals)[8] = osg::Vec3 (0.0f, 0.0f, -1.0f);

   (*mNormals)[9] = osg::Vec3 (0.0f, 0.0f, -1.0f);
   (*mNormals)[10] = osg::Vec3 (0.0f, 0.0f, -1.0);
   (*mNormals)[11] = osg::Vec3 (0.0f, 0.0f, -1.0);

//left
   (*mNormals)[12] = osg::Vec3 (1.0f, 0.0f, 0.0f);
   (*mNormals)[13] = osg::Vec3 (1.0f, 0.0f, 0.0f);
   (*mNormals)[14] = osg::Vec3 (1.0f, 0.0f, 0.0f);

   (*mNormals)[15] = osg::Vec3 (1.0f, 0.0f, 0.0f);
   (*mNormals)[16] = osg::Vec3 (1.0f, 0.0f, 0.0f);
   (*mNormals)[17] = osg::Vec3 (1.0f, 0.0f, 0.0f);

//right
   (*mNormals)[18] = osg::Vec3 (-1.0f, 0.0f, 0.0f);
   (*mNormals)[19] = osg::Vec3 (-1.0f, 0.0f, 0.0f);
   (*mNormals)[20] = osg::Vec3 (-1.0f, 0.0f, 0.0f);

   (*mNormals)[21] = osg::Vec3 (-1.0f, 0.0f, 0.0f);
   (*mNormals)[22] = osg::Vec3 (-1.0f, 0.0f, 0.0f);
   (*mNormals)[23] = osg::Vec3 (-1.0f, 0.0f, 0.0f);

//top
   (*mNormals)[24] = osg::Vec3 (0.0f, -1.0f, 0.0f);
   (*mNormals)[25] = osg::Vec3 (0.0f, -1.0f, 0.0f);
   (*mNormals)[26] = osg::Vec3 (0.0f, -1.0f, 0.0f);

   (*mNormals)[27] = osg::Vec3 (0.0f, -1.0f, 0.0f);
   (*mNormals)[28] = osg::Vec3 (0.0f, -1.0f, 0.0f);
   (*mNormals)[29] = osg::Vec3 (0.0f, -1.0f, 0.0f);

//bottom
   (*mNormals)[30] = osg::Vec3 (0.0f, 1.0f, 0.0f);
   (*mNormals)[31] = osg::Vec3 (0.0f, 1.0f, 0.0f);
   (*mNormals)[32] = osg::Vec3 (0.0f, 1.0f, 0.0f);

   (*mNormals)[33] = osg::Vec3 (0.0f, 1.0f, 0.0f);
   (*mNormals)[34] = osg::Vec3 (0.0f, 1.0f, 0.0f);
   (*mNormals)[35] = osg::Vec3 (0.0f, 1.0f, 0.0f);




//////////////////////////////////////////////////////////////////////////
// Tex Coords1
//////////////////////////////////////////////////////////////////////////

//front
   (*mTexCoords)[0] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords)[1] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords)[2] = osg::Vec2 (0.0f, mTexRepeat);

   (*mTexCoords)[3] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords)[4] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords)[5] = osg::Vec2 (0.0f, mTexRepeat);

//back
   (*mTexCoords)[6] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords)[7] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords)[8] = osg::Vec2 (mTexRepeat, mTexRepeat);

   (*mTexCoords)[9] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords)[10] = osg::Vec2 (0.0f, 4.0);
   (*mTexCoords)[11] = osg::Vec2 (mTexRepeat, 4.0);

//left
   (*mTexCoords)[12] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords)[13] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords)[14] = osg::Vec2 (mTexRepeat, mTexRepeat);

   (*mTexCoords)[15] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords)[16] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords)[17] = osg::Vec2 (mTexRepeat, mTexRepeat);

//right
   (*mTexCoords)[18] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords)[19] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords)[20] = osg::Vec2 (mTexRepeat, mTexRepeat);

   (*mTexCoords)[21] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords)[22] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords)[23] = osg::Vec2 (mTexRepeat, mTexRepeat);

//top
   (*mTexCoords)[24] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords)[25] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords)[26] = osg::Vec2 (0.0f, mTexRepeat);

   (*mTexCoords)[27] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords)[28] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords)[29] = osg::Vec2 (0.0f, mTexRepeat);

//bottom
   (*mTexCoords)[30] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords)[31] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords)[32] = osg::Vec2 (0.0f, 0.0f);

   (*mTexCoords)[33] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords)[34] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords)[35] = osg::Vec2 (0.0f, 0.0f);



//////////////////////////////////////////////////////////////////////////
// TexCoords 2
//////////////////////////////////////////////////////////////////////////
//front
   (*mTexCoords2)[0] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords2)[1] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords2)[2] = osg::Vec2 (mTexRepeat, 0.0f);

   (*mTexCoords2)[3] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords2)[4] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords2)[5] = osg::Vec2 (mTexRepeat, mTexRepeat);

//back
   (*mTexCoords2)[6] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords2)[7] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords2)[8] = osg::Vec2 (mTexRepeat, 0.0f);

   (*mTexCoords2)[9] = osg::Vec2  (0.0f, mTexRepeat);
   (*mTexCoords2)[10] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords2)[11] = osg::Vec2  (0.0f, 0.0);

//left
   (*mTexCoords2)[12] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords2)[13] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords2)[14] = osg::Vec2 (0.0f, 0.0f);

   (*mTexCoords2)[15] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords2)[16] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords2)[17] = osg::Vec2 (0.0f, mTexRepeat);

//right
   (*mTexCoords2)[18] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords2)[19] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords2)[20] = osg::Vec2 (0.0f, 0.0f);

   (*mTexCoords2)[21] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords2)[22] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords2)[23] = osg::Vec2 (0.0f, mTexRepeat);

//top
   (*mTexCoords2)[24] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords2)[25] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords2)[26] = osg::Vec2 (mTexRepeat, 0.0f);

   (*mTexCoords2)[27] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords2)[28] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords2)[29] = osg::Vec2 (mTexRepeat, mTexRepeat);

//bottom
   (*mTexCoords2)[30] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords2)[31] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords2)[32] = osg::Vec2 (mTexRepeat, mTexRepeat);

   (*mTexCoords2)[33] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords2)[34] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords2)[35] = osg::Vec2 (mTexRepeat, 0.0f);


//////////////////////////////////////////////////////////////////////////
// Tex Coords 3
//////////////////////////////////////////////////////////////////////////

//front
   (*mTexCoords3)[0] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords3)[1] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords3)[2] = osg::Vec2 (0.0f, 0.0f);

   (*mTexCoords3)[3] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords3)[4] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords3)[5] = osg::Vec2 (mTexRepeat, 0.0f);

//back
   (*mTexCoords3)[6] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords3)[7] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords3)[8] = osg::Vec2 (0.0f, 0.0f);

   (*mTexCoords3)[9] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords3)[10] = osg::Vec2 (0.0f, 0.0);
   (*mTexCoords3)[11] = osg::Vec2 (0.0f, 4.0);

//left
   (*mTexCoords3)[12] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords3)[13] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords3)[14] = osg::Vec2 (mTexRepeat, 0.0f);

   (*mTexCoords3)[15] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords3)[16] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords3)[17] = osg::Vec2 (0.0f, 0.0f);

//right
   (*mTexCoords3)[18] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords3)[19] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords3)[20] = osg::Vec2 (mTexRepeat, 0.0f);

   (*mTexCoords3)[21] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords3)[22] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords3)[23] = osg::Vec2 (0.0f, 0.0f);

//top
   (*mTexCoords3)[24] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords3)[25] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords3)[26] = osg::Vec2 (0.0f, 0.0f);

   (*mTexCoords3)[27] = osg::Vec2 (0.0f, mTexRepeat);
   (*mTexCoords3)[28] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords3)[29] = osg::Vec2 (mTexRepeat, 0.0f);

//bottom
   (*mTexCoords3)[30] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords3)[31] = osg::Vec2 (mTexRepeat, mTexRepeat);
   (*mTexCoords3)[32] = osg::Vec2 (0.0f, mTexRepeat);

   (*mTexCoords3)[33] = osg::Vec2 (0.0f, 0.0f);
   (*mTexCoords3)[34] = osg::Vec2 (mTexRepeat, 0.0f);
   (*mTexCoords3)[35] = osg::Vec2 (mTexRepeat, mTexRepeat);




}

#endif // __CUBE_VERTICES_H__