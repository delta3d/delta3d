from PyDtCore import *
from PyDtABC import *

from math import *
from time import *

def Distance(a,b):
    return sqrt( pow(a[0]-b[0], 2) + pow(a[1]-b[1], 2) + pow(a[2]-b[2], 2) )

class TestLightsApp(Application):

    countOne = 0.0
    countTwo = 0.0
    countThree = 0.0
    
    mWarehouse = Object( "Warehouse" )
    mSphere = Object( "HappySphere" )
    
    # create a global spot light.
    mGlobalSpot = SpotLight( 1, "GlobalSpotlight" )
    # create a positional light.
    mPositional = PositionalLight( 2, "PositionalLight" )
    # create a global infinite light.
    mGlobalInfinite = InfiniteLight( 4, "GlobalInfiniteLight" )
    
    mOmm = OrbitMotionModel()

    def Config(self):
        
        Application.Config( self )
        
        self.GetScene().UseSceneLight( 0 )
        
        self.warehouse = Object( "Warehouse" )
        self.warehouse.LoadFile( "models/warehouse.ive" )
        self.AddDrawable( self.warehouse )
        
        trans = Transform()
        
        trans.SetTranslation( Vec3(5.0, 10.0, 2.0) )
        self.mGlobalSpot.SetTransform( trans )
        self.mGlobalSpot.SetSpotCutoff( 20.0 )
        self.mGlobalSpot.SetSpotExponent( 50.0 )
        
        self.GetScene().AddDrawable( self.mGlobalSpot )
        
        self.mSphere.LoadFile( "models/physics_happy_sphere.ive" )
        trans.Set( 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 )
        self.mSphere.SetTransform( trans )
        
        self.mPositional.SetDiffuse( 1.0, 1.0, 0.0, 1.0 ) # yellow light
        self.mPositional.AddChild( self.mSphere ) #move sphere along with light

        self.GetScene().AddDrawable( self.mPositional )
        self.mPositional.SetEnabled( 0 )

        
        self.GetScene().AddDrawable( self.mGlobalInfinite )
        self.mGlobalInfinite.SetEnabled( 0 )
        
        #set camera stuff
        trans.SetTranslation( Vec3(30.0, -20.0, 25.0) )
        trans.SetRotation( Vec3(40.0, -33.0, 0.0 ) )
        self.GetCamera().SetTransform( trans )
        
        origin = [ 0.0, 0.0, 0.0 ]

        xyz = Vec3()
        trans.GetTranslation(xyz)
        
        camLoc = [ xyz.x, xyz.y, xyz.z ]
        
        self.mOmm.SetDefaultMappings( self.GetKeyboard(), self.GetMouse() )
        self.mOmm.SetTarget( self.GetCamera() )
        self.mOmm.SetDistance( Distance( camLoc, origin ) )

    def KeyPressed( self, keyboard, key, character ) :
        verdict = 0
        if key is KeyboardKey.Key_Escape :
            self.Quit()
            verdict = 1
        elif key is KeyboardKey.Key_1 :
            self.mGlobalSpot.SetEnabled( not self.mGlobalSpot.GetEnabled() )
            verdict = 1
        elif key is KeyboardKey.Key_2 :
            self.mPositional.SetEnabled( not self.mPositional.GetEnabled() )
            verdict = 1
        elif key is KeyboardKey.Key_3 :
            if self.mPositional.GetLightingMode() is Light.GLOBAL :
                self.mPositional.SetLightingMode( Light.LOCAL )
            else :
                self.mPositional.SetLightingMode( Light.GLOBAL )
            verdict = 1
        elif key is KeyboardKey.Key_4 :
            self.mGlobalInfinite.SetEnabled( not self.mGlobalInfinite.GetEnabled() )
            verdict = 1

        return verdict
    
    def PreFrame(self, deltaFrameTime):
    
        #increment some values at different rates
        self.countOne += 50.0*deltaFrameTime
        self.countTwo += 60.0*deltaFrameTime
        self.countThree += 70.0*deltaFrameTime
        
        #cap at 360
        if( self.countOne > 360.0 ):
            self.countOne -= 360.0
        
        if( self.countTwo > 360.0 ):
            self.countTwo -= 360.0
        
        if( self.countThree > 360.0 ):
            self.countThree -= 360.0
        
        #scale values to 0.0-1.0
        redValue = ( cos( radians(self.countOne) ) + 1.0 ) / 2.0
        greenValue = ( cos( radians(self.countTwo) ) + 1.0 ) / 2.0
        blueValue = ( cos( radians(self.countThree) ) + 1.0 ) / 2.0
        
        #modify all global lights
        self.mGlobalSpot.SetDiffuse( redValue, greenValue, blueValue, 1.0 ) #change color
        
        #rotate the spotlight
        trans = Transform()
        self.mGlobalSpot.GetTransform( trans )
        trans.SetRotation( self.countOne, 0.0, 0.0 )
        self.mGlobalSpot.SetTransform( trans )
        
        self.mPositional.SetAttenuation( 1.0, greenValue/2.0, blueValue/2.0 ) #change attenutation
        
        #move the global positional light in a circle
        tx = 2*cos( radians(self.countOne) ) + 3.0
        ty = 2*sin( radians(self.countOne) ) + 7.0
        trans.SetTranslation( Vec3(tx, ty, 2.0) )
        self.mPositional.SetTransform( trans )
        
        self.mGlobalInfinite.SetDiffuse( redValue, greenValue, blueValue, 1.0 ) #change color
        self.mGlobalInfinite.SetAzimuthElevation( self.countOne, self.countTwo ) #change direction

SetDataFilePathList( GetDeltaRootPath() + '/examples/testPython/;' +
                     GetDeltaDataPathList() )

app = TestLightsApp( 'config.xml' )

app.Config()
app.Run()
