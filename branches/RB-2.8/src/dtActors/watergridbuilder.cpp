#include <dtActors/watergridbuilder.h>

#include <dtUtil/mathdefines.h>

namespace dtActors
{
   ////////////////////////////////////////////////////////////////////////////////
   osg::Node* WaterGridBuilder::CreateQuad(osg::Texture2D* texture, int renderBin)
   {
      float offset = 10.0f;

      osg::Geometry* geo = new osg::Geometry;
      geo->setUseDisplayList(false);
      osg::Vec4Array* colors = new osg::Vec4Array;
      colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
      geo->setColorArray(colors);
      geo->setColorBinding(osg::Geometry::BIND_OVERALL);
      osg::Vec3Array *vx = new osg::Vec3Array;
      vx->push_back(osg::Vec3(-offset, -offset, 0));
      vx->push_back(osg::Vec3(offset, -offset, 0));
      vx->push_back(osg::Vec3(offset, offset, 0 ));
      vx->push_back(osg::Vec3(-offset, offset, 0));
      geo->setVertexArray(vx);
      osg::Vec3Array *nx = new osg::Vec3Array;
      nx->push_back(osg::Vec3(0, 0, 1));
      geo->setNormalArray(nx);

      if(texture != NULL)
      {
         osg::Vec2Array *tx = new osg::Vec2Array;
         tx->push_back(osg::Vec2(0, 0));
         tx->push_back(osg::Vec2(1, 0));
         tx->push_back(osg::Vec2(1, 1));
         tx->push_back(osg::Vec2(0, 1));
         geo->setTexCoordArray(0, tx);
         geo->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
      }

      geo->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
      osg::Geode *geode = new osg::Geode;
      geode->addDrawable(geo);
      geode->setCullingActive(false);
      osg::StateSet* ss = geode->getOrCreateStateSet();
      ss->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
      ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
      ss->setRenderBinDetails(renderBin, "RenderBin");
      return geode;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Texture2D* WaterGridBuilder::CreateTexture(int width, int height, bool mipMap)
   {
      osg::Texture2D* tex = new osg::Texture2D();
      tex->setTextureSize(width, height);
      tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
      tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
      tex->setInternalFormat(GL_RGBA);
      if(mipMap)
      {
         tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR_MIPMAP_LINEAR);
         tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
         tex->setUseHardwareMipMapGeneration(true);
      }
      else
      {
         tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
         tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
      }
      return tex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Geometry* WaterGridBuilder::BuildRadialGrid(float &outComputedRadialDistance, float numRows, float numColumns)
   {
      osg::Geometry* geometry = new osg::Geometry();

      int N = numRows; // rows from center outward
      int K = numColumns;// columns around the circle

      //calculate num verts and num indices
      int numVerts = N * K;
      int numIndices = (N - 1) * (K - 1) * 6;

      //lets make the geometry
      dtCore::RefPtr<osg::Vec4Array> pVerts = new osg::Vec4Array(numVerts);
      dtCore::RefPtr<osg::DrawElementsUInt> pIndices = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
      pIndices->reserve(numIndices);

      float a0 = 0.5f;
      float a1 = 1.205f; // 5.0f;
      float outerMostRingDistance = 1250.0; // the furthest rings get an extra reach.
      float middleRingDistance = 12.5; // Middle rings get a minor boost too.
      int numOuterRings = 10;
      int numMiddleRings = 50;
      float innerExpBase = 1.0015f;
      float middleExpBase = 1.05f;
      float outerExpBase = 1.5f;
      //float exponent = 3;

      float r = a0;
      for(int i = 0; i < N; ++i)
      {
         float radiusIncrement = a0;
         // Radius extends with each row/ring.
         if (i != 0)
            radiusIncrement += a1 * powf(innerExpBase, i); //= a0 + a1 * powf(base, i);

         // Final rows/rings get an extra boost - solves some shrinkage & horizon problems.
         if ((i + numOuterRings) > N)
            radiusIncrement += outerMostRingDistance * powf(outerExpBase, (i + numOuterRings - N));
         // Middle rows/rings get a little boost - solves some shrinkage & horizon problems
         else if ((i + numOuterRings + numMiddleRings) > N)
            radiusIncrement += middleRingDistance * powf(middleExpBase, (i + numOuterRings + numMiddleRings - N));

         r += radiusIncrement;

         for(int j = 0; j < K; ++j)
         {
            //float PI2j = osg::PI * 2.0f * j;
            float x = 0.0;
            if(j == K - 1)
            {
               //setting this to one makes the water mesh fully wrap around
               x = 1;
            }
            else
            {
               //we are setting our vertex position as a -1,1 quantized angle and a depth
               x = float((2.0 * (double(j) / double(K))) - 1.0);
            }
            float y = r;
            float z = radiusIncrement; // We put the radius increment into the Z so we can use it in the shader
            float groupNum = float(i);
            (*pVerts)[(i * K) + j ].set(x, y, z, groupNum);
         }
      }

      LOG_ALWAYS("WATER GRID REACH IN METERS = " + dtUtil::ToString(r));

      for(int i = 0; i < N - 1; ++i)
      {
         for(int j = 0; j < K - 1; ++j)
         {
            int JPlusOne = (j + 1);// % K;

            pIndices->addElement( (i * K) + j );
            pIndices->addElement( ((i + 1) * K) + j );
            pIndices->addElement( ((i + 1) * K) + (JPlusOne) );

            pIndices->addElement( ((i + 1) * K) + (JPlusOne) );
            pIndices->addElement( (i * K) + (JPlusOne) );
            pIndices->addElement( (i * K) + j );
         }
      }


      geometry->setVertexArray(pVerts.get());
      geometry->addPrimitiveSet(pIndices.get());

      return geometry;
   }


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void WaterGridBuilder::BuildWavesFromSeaState(WaterGridActor::SeaState* seaState, std::vector<WaterGridActor::Wave>& waveList)
   {
      int numWaves = 16;

      float waveLenMod = 0.35f;
      float ampMod = 0.4f;

      if(seaState == &WaterGridActor::SeaState::SeaState_0)
      {
         AddRandomWaves(waveList, waveLenMod * 9.1667f, ampMod * 1.4217f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 17.3667f, ampMod * 1.7533f, 1.15f, 3.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 21.5667f, ampMod * 1.0831f, 2.1f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 45.766f, ampMod * 1.1757f, 1.0f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 75.766f, ampMod * 2.1757f, 1.0f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_1)
      {
         //AddRandomWaves(waveList, waveLenMod * 8.667f, ampMod * 00.9667f, 1.5f, 5.0f, numWaves);
         AddRandomWaves(waveList, waveLenMod * 11.1667f, ampMod * 1.4217f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 15.3667f, ampMod * 1.7533f, 1.15f, 3.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 31.5667f, ampMod * 1.0831f, 2.1f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 85.766f, ampMod * 2.1757f, 1.0f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 115.766f, ampMod * 2.1757f, 1.0f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_2)
      {         
         //AddRandomWaves(waveList, waveLenMod * 12.667f, ampMod * 1.29667f, 1.75f, 6.0f, numWaves);
         AddRandomWaves(waveList, waveLenMod * 14.1667f, ampMod * 1.427f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 27.3667f, ampMod * 1.676f, 1.15f, 3.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 43.5667f, ampMod * 1.387f, 2.1f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 95.766f, ampMod * 2.4951f, 1.0f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 175.766f, ampMod * 3.0951f, 1.0f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_3)
      {
         //AddRandomWaves(waveList, waveLenMod * 16.667f, ampMod * 1.5667f, 2.0f, 6.5f, numWaves);

         AddRandomWaves(waveList, waveLenMod * 19.1667f, ampMod * 1.0431f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 29.3667f, ampMod * 1.2761f, 1.15f, 3.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 34.5667f, ampMod * 1.63f, 2.1f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 118.766f, ampMod * 2.1791f, 1.0f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 238.766f, ampMod * 2.91f, 1.0f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_4)
      {
         AddRandomWaves(waveList, waveLenMod * 18.3167f, ampMod * 1.1167f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 22.667f, ampMod * 1.331f, 1.15f, 3.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 44.1667f, ampMod * 1.71f, 2.1f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 147.66f, ampMod * 2.3517f, 1.0f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 277.66f, ampMod * 3.0517f, 1.0f, 5.5f, 1);

      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_5)
      {
         AddRandomWaves(waveList, waveLenMod * 39.667f, ampMod * 1.1967f, 1.75f, 4.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 51.667f, ampMod * 1.667f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 97.66f, ampMod * 1.3f, 1.5f, 5.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 151.66f, ampMod * 2.191f, 1.5f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 521.66f, ampMod * 3.91f, 1.5f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_6)
      {
         AddRandomWaves(waveList, waveLenMod * 91.667f, ampMod * 0.9147f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 152.66f, ampMod * 1.514f, 1.5f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 164.33, ampMod * 2.7541f, 1.5f, 5.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 235.33, ampMod * 3.4667, 2.0f, 5.0f, 3);
         AddRandomWaves(waveList, waveLenMod * 849.667, ampMod * 4.133f, 2.5f, 5.0f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_7)
      {
         AddRandomWaves(waveList, waveLenMod * 105.667f, ampMod * 1.167f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 217.66f, ampMod * 1.41f, 1.5f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 320.533, ampMod * 2.341f, 2.0f, 5.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 412.233, ampMod * 3.667, 2.0f, 5.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 1019.667, ampMod * 5.33f, 2.5f, 5.0f, 2);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_8)
      {
         AddRandomWaves(waveList, waveLenMod * 219.667f, ampMod * 1.167f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 321.66f, ampMod * 2.14f, 1.5f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 323.33, ampMod * 2.341f, 3.0f, 5.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 543.33, ampMod * 4.667, 2.0f, 5.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 1250.667, ampMod * 9.33f, 1.5f, 2.5f, 2);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_9)
      {
         AddRandomWaves(waveList, waveLenMod * 220.667f, ampMod * 1.1667f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 260.66f, ampMod * 1.14f, 1.5f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 490.133, ampMod * 2.341f, 1.5f, 5.0f, 3);
         AddRandomWaves(waveList, waveLenMod * 510.63, ampMod * 2.667, 2.0f, 5.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 1500.667, ampMod * 11.33f, 1.5f, 2.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_10)
      {
         AddRandomWaves(waveList, waveLenMod * 227.667f, ampMod * 1.1667f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 267.66f, ampMod * 2.14f, 1.5f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 5213.33, ampMod * 4.341f, 2.0f, 5.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 533.33, ampMod * 4.667, 2.0f, 5.0f, 3);
         AddRandomWaves(waveList, waveLenMod * 1500.667, ampMod * 18.33f, 1.5f, 2.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_11)
      {
         AddRandomWaves(waveList, waveLenMod * 177.667f, ampMod * 2.1667f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 212.667f, ampMod * 3.1667f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 523.33, ampMod * 4.20f, 2.0f, 5.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 528.33, ampMod * 4.667, 2.0f, 5.0f, 3);
         AddRandomWaves(waveList, waveLenMod * 1500.667, ampMod * 24.33f, 1.5f, 2.5f, 1);

      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_12)
      {        
         AddRandomWaves(waveList, waveLenMod * 218.667f, ampMod * 2.1667f, 2.0f, 4.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 326.667f, ampMod * 3.1667f, 2.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 533.33, ampMod * 3.667, 2.0f, 5.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 538.33, ampMod * 4.67f, 2.5f, 5.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 1500.667, ampMod * 29.33f, 1.5f, 2.5f, 1);
      }

   }


   /////////////////////////////////////////////////////////////////////////////
   void WaterGridBuilder::AddRandomWaves(std::vector<WaterGridActor::Wave>& waveList, float meanWaveLength, float meanAmplitude, float minPeriod, float maxPeriod, unsigned numWaves)
   {
      float waveLenStart = meanWaveLength / 2.0f;
      float waveLenIncrement = meanWaveLength / numWaves;

      float ampStart = meanAmplitude / 2.0f;
      float ampIncrement = meanAmplitude / numWaves;

      for(unsigned i = 0; i < numWaves; ++i, waveLenStart += waveLenIncrement, ampStart += ampIncrement)
      {
         WaterGridActor::Wave w;
         w.mWaveLength = waveLenStart;
         w.mAmplitude = ampStart;
         w.mSpeed = dtUtil::RandFloat(minPeriod, maxPeriod);
         w.mSteepness = 1.0f;
         w.mDirectionInDegrees = 90.0f + dtUtil::RandFloat(-10.3333f, 10.3333f);
         SetWaveDirection(w);
         waveList.push_back(w); // -3
      }

   }


   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridBuilder::BuildTextureWaves(std::vector<WaterGridActor::TextureWave>& waveList)
   {
      const float kArray[] = {1.33f, 1.76f, 3.0f, 2.246f,
         1.0f, 3.71f, 1.0f, 1.75f,
         1.5f, 1.0f, 1.0f, 2.0f,
         2.2f, 2.0f, 1.113f, 1.0f,
         1.33f, 1.76f, 3.0f, 2.246f,
         1.0f, 3.71f, 1.0f, 1.75f,
         1.5f, 1.0f, 1.0f, 2.0f,
         2.2f, 2.0f, 1.113f, 1.0f};

      const float waveLengthArray[] = {0.1788f, 0.0535f, 0.12186f, 0.24f,
         0.14f, 0.116844f, 0.97437f, 0.0805f,
         0.067f, 0.3565f, 0.67135f , 0.191f,
         0.155f, 0.13917f, 0.275f, .448f,
         0.1788f, 0.0535f, 0.12186f, 0.24f,
         0.14f, 0.116844f, 0.97437f, 0.0805f,
         0.067f, 0.3565f, 0.67135f , 0.191f,
         0.155f, 0.13917f, 0.275f, .448f};

      const float waveSpeedArray[] = {0.0953f, 0.03839f, 0.0311f, 0.04221f,
         0.11497f, 0.143213f, 0.14571f, 0.051181f,

         0.01473f, 0.1531f, 0.2131f, 0.0221f,
         0.121497f, 0.1213f, 0.14571f, 0.1181f,
         0.0953f, 0.03839f, 0.0311f, 0.04221f,
         0.11497f, 0.143213f, 0.14571f, 0.051181f,

         0.01473f, 0.1531f, 0.2131f, 0.0221f,
         0.121497f, 0.1213f, 0.14571f, 0.1181f};

      for(int i = 0; i < WaterGridActor::MAX_TEXTURE_WAVES; ++i)
      {
         WaterGridActor::TextureWave tw;
         tw.mSteepness = kArray[i];
         tw.mSpeed = waveSpeedArray[i];
         tw.mWaveLength = waveLengthArray[i];

         waveList.push_back(tw);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridBuilder::SetWaveDirection(WaterGridActor::Wave& wave)
   {
      wave.mDirection.set(sin(osg::DegreesToRadians(wave.mDirectionInDegrees)),
         cos(osg::DegreesToRadians(wave.mDirectionInDegrees)));

      wave.mDirection.normalize();
   }

}
////////////////////////////////////////////////////////////////////////////////
