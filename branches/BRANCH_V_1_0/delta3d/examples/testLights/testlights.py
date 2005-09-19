from dtCore import *
from dtABC import *

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
        
        SetDataFilePathList ( '../../data' )
        
        self.GetScene().UseSceneLight( 0 )
        
        self.warehouse = Object( "Warehouse" )
        self.warehouse.LoadFile( "models/warehouse.ive" )
        self.AddDrawable( self.warehouse )
        
        trans = Transform()
        
        trans.Set( 5.0, 10.0, 2.0, 0.0, 0.0, 0.0 )
        self.mGlobalSpot.SetTransform( trans )
        self.mGlobalSpot.SetSpotCutoff( 20.0 )
        self.mGlobalSpot.SetSpotExponent( 50.0 )
        
        self.GetScene().AddDrawable( self.mGlobalSpot )
        
        self.mSphere.LoadFile( "models/physics_happy_sphere.ive" )
        
        self.mPositional.SetDiffuse( 1.0, 1.0, 0.0, 1.0 ) # yellow light
        self.mPositional.AddChild( self.mSphere ) #move sphere along with light

        self.GetScene().AddDrawable( self.mPositional )
        self.mPositional.SetEnabled( 1 )
        
        self.GetScene().AddDrawable( self.mGlobalInfinite )
        self.mGlobalInfinite.SetEnabled( 1 )
        
        #set camera stuff
        trans.Set( 30.0, -20.0, 25.0, 40.0, -33.0, 0.0 )
        self.GetCamera().SetTransform( trans )
        
        origin = [ 0.0, 0.0, 0.0 ]
        
        x = trans.GetTranslationX()
        y = trans.GetTranslationY()
        z = trans.GetTranslationZ()
        
        camLoc = [ x, y, z ]
        
        self.mOmm.SetDefaultMappings( self.GetKeyboard(), self.GetMouse() )
        self.mOmm.SetTarget( self.GetCamera() )
        self.mOmm.SetDistance( Distance( camLoc, origin ) )

    #def KeyPressed(self,keyboard,key,character):
    
    def PreFrame(self, deltaFrameTime):
    
        #increment some values at different rates
        self.countOne +=0.5
        self.countTwo += 0.6
        self.countThree += 0.7
        
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
        trans.Set( tx, ty, 2.0, 0.0, 0.0, 0.0 )
        self.mPositional.SetTransform( trans )
        
        self.mGlobalInfinite.SetDiffuse( redValue, greenValue, blueValue, 1.0 ) #change color
        self.mGlobalInfinite.SetDirection( self.countOne, self.countTwo, self.countThree ) #change direction

app = TestLightsApp( 'config.xml' )

app.Config()
app.Run()