// generated by Fast Light User Interface Designer (fluid) version 1.0104

#include "psEditorGUI.h"

Fl_Menu_Item menu_[] = {
 {"&File", 0,  0, 0, 64, 0, 0, 14, 56},
 {"&New", 0,  (Fl_Callback*)psEditorGUI_New, 0, 0, 0, 0, 14, 56},
 {"&Open", 0,  (Fl_Callback*)psEditorGUI_Open, 0, 128, 0, 0, 14, 56},
 {"&Save", 0,  (Fl_Callback*)psEditorGUI_Save, 0, 0, 0, 0, 14, 56},
 {"Save &As", 0,  (Fl_Callback*)psEditorGUI_SaveAs, 0, 128, 0, 0, 14, 56},
 {"&Quit", 0,  (Fl_Callback*)psEditorGUI_Quit, 0, 0, 0, 0, 14, 56},
 {0},
 {"&View", 0,  0, 0, 64, 0, 0, 14, 56},
 {"&Compass", 0,  (Fl_Callback*)psEditorGUI_Compass, 0, 128, 0, 0, 14, 56},
 {"&XY Grid", 0,  (Fl_Callback*)psEditorGUI_XYGrid, 0, 0, 0, 0, 14, 56},
 {"&YZ Grid", 0,  (Fl_Callback*)psEditorGUI_YZGrid, 0, 0, 0, 0, 14, 56},
 {"X&Z Grid", 0,  (Fl_Callback*)psEditorGUI_XZGrid, 0, 0, 0, 0, 14, 56},
 {0},
 {0}
};

Fl_Choice *Particles_Alignment=(Fl_Choice *)0;

Fl_Menu_Item menu_Particles_Alignment[] = {
 {"Billboard", 0,  0, 0, 0, 0, 0, 14, 56},
 {"Fixed", 0,  0, 0, 0, 0, 0, 14, 56},
 {0}
};

Fl_Choice *Particles_Shape=(Fl_Choice *)0;

Fl_Menu_Item menu_Particles_Shape[] = {
 {"Point", 0,  0, 0, 0, 0, 0, 14, 56},
 {"Quad", 0,  0, 0, 0, 0, 0, 14, 56},
 {"Quad Triangle Strip", 0,  0, 0, 0, 0, 0, 14, 56},
 {"Hexagon", 0,  0, 0, 0, 0, 0, 14, 56},
 {"Line", 0,  0, 0, 0, 0, 0, 14, 56},
 {0}
};

Fl_Value_Input *Particles_Lifetime=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_MinSize=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_MaxSize=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_MinAlpha=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_MaxAlpha=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_Radius=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_Mass=(Fl_Value_Input *)0;

Fl_Input *Particles_Texture=(Fl_Input *)0;

Fl_Check_Button *Particles_Emissive=(Fl_Check_Button *)0;

Fl_Check_Button *Particles_Lighting=(Fl_Check_Button *)0;

Fl_Value_Input *Particles_MinR=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_MinG=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_MinB=(Fl_Value_Input *)0;

Fl_Button *Particles_MinColor=(Fl_Button *)0;

Fl_Value_Input *Particles_MaxR=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_MaxG=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_MaxB=(Fl_Value_Input *)0;

Fl_Button *Particles_MaxColor=(Fl_Button *)0;

Fl_Check_Button *Particles_Emitter_Endless=(Fl_Check_Button *)0;

Fl_Value_Input *Particles_Emitter_Lifetime=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_Emitter_StartTime=(Fl_Value_Input *)0;

Fl_Value_Input *Particles_Emitter_ResetTime=(Fl_Value_Input *)0;

Fl_Group *RandomRateCounterParameters=(Fl_Group *)0;

Fl_Value_Input *RandomRateCounter_MinRate=(Fl_Value_Input *)0;

Fl_Value_Input *RandomRateCounter_MaxRate=(Fl_Value_Input *)0;

Fl_Menu_Item menu_Counter[] = {
 {"Random Rate Counter", 0,  0, 0, 0, 0, 0, 14, 56},
 {0}
};

Fl_Group *SegmentPlacerParameters=(Fl_Group *)0;

Fl_Value_Input *SegmentPlacer_A_X=(Fl_Value_Input *)0;

Fl_Value_Input *SegmentPlacer_A_Y=(Fl_Value_Input *)0;

Fl_Value_Input *SegmentPlacer_A_Z=(Fl_Value_Input *)0;

Fl_Value_Input *SegmentPlacer_B_X=(Fl_Value_Input *)0;

Fl_Value_Input *SegmentPlacer_B_Y=(Fl_Value_Input *)0;

Fl_Value_Input *SegmentPlacer_B_Z=(Fl_Value_Input *)0;

Fl_Group *SectorPlacerParameters=(Fl_Group *)0;

Fl_Value_Input *SectorPlacer_X=(Fl_Value_Input *)0;

Fl_Value_Input *SectorPlacer_Y=(Fl_Value_Input *)0;

Fl_Value_Input *SectorPlacer_Z=(Fl_Value_Input *)0;

Fl_Value_Input *SectorPlacer_MinRadius=(Fl_Value_Input *)0;

Fl_Value_Input *SectorPlacer_MaxRadius=(Fl_Value_Input *)0;

Fl_Value_Input *SectorPlacer_MinPhi=(Fl_Value_Input *)0;

Fl_Value_Input *SectorPlacer_MaxPhi=(Fl_Value_Input *)0;

Fl_Group *PointPlacerParameters=(Fl_Group *)0;

Fl_Value_Input *PointPlacer_X=(Fl_Value_Input *)0;

Fl_Value_Input *PointPlacer_Y=(Fl_Value_Input *)0;

Fl_Value_Input *PointPlacer_Z=(Fl_Value_Input *)0;

Fl_Group *MultiSegmentPlacerParameters=(Fl_Group *)0;

Fl_Browser *MultiSegmentPlacer_Vertices=(Fl_Browser *)0;

Fl_Button *MultiSegmentPlacer_DeleteVertex=(Fl_Button *)0;

Fl_Group *MultiSegmentPlacer_VertexParameters=(Fl_Group *)0;

Fl_Value_Input *MultiSegmentPlacer_X=(Fl_Value_Input *)0;

Fl_Value_Input *MultiSegmentPlacer_Y=(Fl_Value_Input *)0;

Fl_Value_Input *MultiSegmentPlacer_Z=(Fl_Value_Input *)0;

Fl_Menu_Item menu_Placer[] = {
 {"Point Placer", 0,  0, 0, 0, 0, 0, 14, 56},
 {"Sector Placer", 0,  0, 0, 0, 0, 0, 14, 56},
 {"Segment Placer", 0,  0, 0, 0, 0, 0, 14, 56},
 {"Multi Segment Placer", 0,  0, 0, 0, 0, 0, 14, 56},
 {0}
};

Fl_Group *RadialShooterParameters=(Fl_Group *)0;

Fl_Value_Input *RadialShooter_MinTheta=(Fl_Value_Input *)0;

Fl_Value_Input *RadialShooter_MaxTheta=(Fl_Value_Input *)0;

Fl_Value_Input *RadialShooter_MinPhi=(Fl_Value_Input *)0;

Fl_Value_Input *RadialShooter_MaxPhi=(Fl_Value_Input *)0;

Fl_Value_Input *RadialShooter_MinInitialSpeed=(Fl_Value_Input *)0;

Fl_Value_Input *RadialShooter_MaxInitialSpeed=(Fl_Value_Input *)0;

Fl_Value_Input *RadialShooter_MinInitialRotationalSpeedX=(Fl_Value_Input *)0;

Fl_Value_Input *RadialShooter_MinInitialRotationalSpeedY=(Fl_Value_Input *)0;

Fl_Value_Input *RadialShooter_MinInitialRotationalSpeedZ=(Fl_Value_Input *)0;

Fl_Value_Input *RadialShooter_MaxInitialRotationalSpeedX=(Fl_Value_Input *)0;

Fl_Value_Input *RadialShooter_MaxInitialRotationalSpeedY=(Fl_Value_Input *)0;

Fl_Value_Input *RadialShooter_MaxInitialRotationalSpeedZ=(Fl_Value_Input *)0;

Fl_Menu_Item menu_Shooter[] = {
 {"Radial Shooter", 0,  0, 0, 0, 0, 0, 14, 56},
 {0}
};

Fl_Browser *Program_Operators=(Fl_Browser *)0;

Fl_Button *Program_DeleteOperator=(Fl_Button *)0;

Fl_Group *FluidFrictionParameters=(Fl_Group *)0;

Fl_Value_Input *FluidFriction_Density=(Fl_Value_Input *)0;

Fl_Value_Input *FluidFriction_Viscosity=(Fl_Value_Input *)0;

Fl_Value_Input *FluidFriction_OverrideRadius=(Fl_Value_Input *)0;

Fl_Group *ForceParameters=(Fl_Group *)0;

Fl_Value_Input *Force_X=(Fl_Value_Input *)0;

Fl_Value_Input *Force_Y=(Fl_Value_Input *)0;

Fl_Value_Input *Force_Z=(Fl_Value_Input *)0;

Fl_Group *AccelerationParameters=(Fl_Group *)0;

Fl_Value_Input *Acceleration_X=(Fl_Value_Input *)0;

Fl_Value_Input *Acceleration_Y=(Fl_Value_Input *)0;

Fl_Value_Input *Acceleration_Z=(Fl_Value_Input *)0;

Fl_Double_Window* make_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(598, 585, "Particle System Editor");
    w = o;
    { Fl_Menu_Bar* o = new Fl_Menu_Bar(0, 0, 595, 25);
      o->menu(menu_);
    }
    { Fl_Tabs* o = new Fl_Tabs(5, 35, 615, 555);
      { Fl_Group* o = new Fl_Group(5, 60, 590, 520, "Particles");
        { Fl_Choice* o = Particles_Alignment = new Fl_Choice(80, 75, 105, 25, "Alignment");
          o->down_box(FL_BORDER_BOX);
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetAlignment);
          o->menu(menu_Particles_Alignment);
        }
        { Fl_Choice* o = Particles_Shape = new Fl_Choice(80, 110, 150, 25, "Shape");
          o->down_box(FL_BORDER_BOX);
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetShape);
          o->menu(menu_Particles_Shape);
        }
        { Fl_Value_Input* o = Particles_Lifetime = new Fl_Value_Input(80, 145, 80, 25, "Lifetime");
          o->maximum(10);
          o->step(0.01);
          o->value(2);
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetLifetime);
        }
        { Fl_Value_Input* o = Particles_MinSize = new Fl_Value_Input(80, 250, 80, 25, "Size");
          o->maximum(10);
          o->step(0.01);
          o->value(0.2);
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetMinSize);
        }
        { Fl_Value_Input* o = Particles_MaxSize = new Fl_Value_Input(180, 250, 80, 25, "to");
          o->maximum(10);
          o->step(0.01);
          o->value(0.2);
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetMaxSize);
        }
        { Fl_Value_Input* o = Particles_MinAlpha = new Fl_Value_Input(80, 285, 80, 25, "Alpha");
          o->step(0.01);
          o->value(1);
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetMinAlpha);
        }
        { Fl_Value_Input* o = Particles_MaxAlpha = new Fl_Value_Input(180, 285, 80, 25, "to");
          o->step(0.01);
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetMaxAlpha);
        }
        { Fl_Value_Input* o = Particles_Radius = new Fl_Value_Input(80, 180, 80, 25, "Radius");
          o->maximum(10);
          o->step(0.01);
          o->value(0.2);
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetRadius);
        }
        { Fl_Value_Input* o = Particles_Mass = new Fl_Value_Input(80, 215, 80, 25, "Mass");
          o->maximum(10);
          o->step(0.01);
          o->value(0.1);
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetMass);
        }
        { Fl_Input* o = Particles_Texture = new Fl_Input(380, 80, 125, 25, "Texture");
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetTexture);
        }
        { Fl_Button* o = new Fl_Button(510, 80, 25, 25, "...");
          o->callback((Fl_Callback*)psEditorGUI_Particles_ChooseTexture);
        }
        { Fl_Check_Button* o = Particles_Emissive = new Fl_Check_Button(380, 110, 120, 25, "Emissive");
          o->down_box(FL_DOWN_BOX);
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetEmissive);
        }
        { Fl_Check_Button* o = Particles_Lighting = new Fl_Check_Button(380, 135, 20, 25, "Lighting");
          o->down_box(FL_DOWN_BOX);
          o->callback((Fl_Callback*)psEditorGUI_Particles_SetLighting);
        }
        { Fl_Group* o = new Fl_Group(80, 320, 140, 95);
          o->box(FL_ENGRAVED_BOX);
          { Fl_Value_Input* o = Particles_MinR = new Fl_Value_Input(100, 325, 80, 25, "R");
            o->step(0.01);
            o->value(1);
            o->callback((Fl_Callback*)psEditorGUI_Particles_SetMinR);
          }
          { Fl_Value_Input* o = Particles_MinG = new Fl_Value_Input(100, 355, 80, 25, "G");
            o->step(0.01);
            o->value(1);
            o->callback((Fl_Callback*)psEditorGUI_Particles_SetMinG);
          }
          { Fl_Value_Input* o = Particles_MinB = new Fl_Value_Input(100, 385, 80, 25, "B");
            o->step(0.01);
            o->value(1);
            o->callback((Fl_Callback*)psEditorGUI_Particles_SetMinB);
          }
          { Fl_Button* o = Particles_MinColor = new Fl_Button(185, 325, 25, 85, "...");
            o->callback((Fl_Callback*)psEditorGUI_Particles_ChooseMinColor);
          }
          o->end();
        }
        new Fl_Box(35, 355, 45, 25, "Color");
        { Fl_Group* o = new Fl_Group(245, 320, 140, 95);
          o->box(FL_ENGRAVED_BOX);
          { Fl_Value_Input* o = Particles_MaxR = new Fl_Value_Input(265, 325, 80, 25, "R");
            o->step(0.01);
            o->value(1);
            o->callback((Fl_Callback*)psEditorGUI_Particles_SetMaxR);
          }
          { Fl_Value_Input* o = Particles_MaxG = new Fl_Value_Input(265, 355, 80, 25, "G");
            o->step(0.01);
            o->value(1);
            o->callback((Fl_Callback*)psEditorGUI_Particles_SetMaxG);
          }
          { Fl_Value_Input* o = Particles_MaxB = new Fl_Value_Input(265, 385, 80, 25, "B");
            o->step(0.01);
            o->value(1);
            o->callback((Fl_Callback*)psEditorGUI_Particles_SetMaxB);
          }
          { Fl_Button* o = Particles_MaxColor = new Fl_Button(350, 325, 25, 85, "...");
            o->callback((Fl_Callback*)psEditorGUI_Particles_ChooseMaxColor);
          }
          o->end();
        }
        new Fl_Box(220, 355, 25, 25, "to");
        { Fl_Group* o = new Fl_Group(80, 425, 305, 120, "Emitter");
          o->box(FL_ENGRAVED_BOX);
          o->align(FL_ALIGN_LEFT);
          { Fl_Check_Button* o = Particles_Emitter_Endless = new Fl_Check_Button(280, 475, 80, 25, "Endless");
            o->down_box(FL_DOWN_BOX);
            o->value(1);
            o->callback((Fl_Callback*)psEditorGUI_Particles_SetEmitterEndless);
          }
          { Fl_Value_Input* o = Particles_Emitter_Lifetime = new Fl_Value_Input(180, 440, 80, 25, "Lifetime");
            o->maximum(10);
            o->step(0.01);
            o->value(5);
            o->callback((Fl_Callback*)psEditorGUI_Particles_SetEmitterLifetime);
          }
          { Fl_Value_Input* o = Particles_Emitter_StartTime = new Fl_Value_Input(180, 475, 80, 25, "Start Time");
            o->maximum(10);
            o->step(0.01);
            o->callback((Fl_Callback*)psEditorGUI_Particles_SetEmitterStartTime);
          }
          { Fl_Value_Input* o = Particles_Emitter_ResetTime = new Fl_Value_Input(180, 510, 80, 25, "Reset Time");
            o->maximum(10);
            o->step(0.01);
            o->callback((Fl_Callback*)psEditorGUI_Particles_SetEmitterResetTime);
          }
          o->end();
        }
        o->end();
      }
      { Fl_Group* o = new Fl_Group(5, 60, 590, 520, "Counter");
        o->hide();
        { Fl_Group* o = RandomRateCounterParameters = new Fl_Group(10, 310, 580, 160, "Random Rate Counter Parameters");
          o->box(FL_DOWN_BOX);
          o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
          o->hide();
          { Fl_Value_Input* o = RandomRateCounter_MinRate = new Fl_Value_Input(290, 355, 80, 25, "Min. Rate");
            o->maximum(100);
            o->step(0.1);
            o->value(20);
            o->callback((Fl_Callback*)psEditorGUI_RandomRateCounter_SetMinRate);
          }
          { Fl_Value_Input* o = RandomRateCounter_MaxRate = new Fl_Value_Input(290, 390, 80, 25, "Max. Rate");
            o->maximum(100);
            o->step(0.1);
            o->value(30);
            o->callback((Fl_Callback*)psEditorGUI_RandomRateCounter_SetMaxRate);
          }
          o->end();
        }
        { Fl_Choice* o = new Fl_Choice(225, 130, 180, 25, "Counter Type");
          o->down_box(FL_BORDER_BOX);
          o->callback((Fl_Callback*)psEditorGUI_Counter_SetType);
          o->menu(menu_Counter);
        }
        o->end();
      }
      { Fl_Group* o = new Fl_Group(5, 60, 590, 520, "Placer");
        o->hide();
        { Fl_Group* o = SegmentPlacerParameters = new Fl_Group(10, 310, 580, 160, "Segment Placer Parameters");
          o->box(FL_DOWN_BOX);
          o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
          o->hide();
          { Fl_Group* o = new Fl_Group(30, 340, 260, 105, "Vertex A");
            o->box(FL_ENGRAVED_BOX);
            { Fl_Value_Input* o = SegmentPlacer_A_X = new Fl_Value_Input(120, 350, 80, 25, "X");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->value(-1);
              o->callback((Fl_Callback*)psEditor_SegmentPlacer_VertexA_SetX);
            }
            { Fl_Value_Input* o = SegmentPlacer_A_Y = new Fl_Value_Input(120, 380, 80, 25, "Y");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditor_SegmentPlacer_VertexA_SetY);
            }
            { Fl_Value_Input* o = SegmentPlacer_A_Z = new Fl_Value_Input(120, 410, 80, 25, "Z");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditor_SegmentPlacer_VertexA_SetZ);
            }
            o->end();
          }
          { Fl_Group* o = new Fl_Group(310, 340, 260, 105, "Vertex B");
            o->box(FL_ENGRAVED_BOX);
            { Fl_Value_Input* o = SegmentPlacer_B_X = new Fl_Value_Input(400, 350, 80, 25, "X");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->value(1);
              o->callback((Fl_Callback*)psEditor_SegmentPlacer_VertexB_SetX);
            }
            { Fl_Value_Input* o = SegmentPlacer_B_Y = new Fl_Value_Input(400, 380, 80, 25, "Y");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditor_SegmentPlacer_VertexB_SetY);
            }
            { Fl_Value_Input* o = SegmentPlacer_B_Z = new Fl_Value_Input(400, 410, 80, 25, "Z");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditor_SegmentPlacer_VertexB_SetZ);
            }
            o->end();
          }
          o->end();
        }
        { Fl_Group* o = SectorPlacerParameters = new Fl_Group(10, 310, 580, 160, "Sector Placer Parameters");
          o->box(FL_DOWN_BOX);
          o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
          o->hide();
          { Fl_Group* o = new Fl_Group(25, 335, 165, 105, "Center");
            o->box(FL_ENGRAVED_BOX);
            { Fl_Value_Input* o = SectorPlacer_X = new Fl_Value_Input(70, 345, 80, 25, "X");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_SectorPlacer_Center_SetX);
            }
            { Fl_Value_Input* o = SectorPlacer_Y = new Fl_Value_Input(70, 375, 80, 25, "Y");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_SectorPlacer_Center_SetY);
            }
            { Fl_Value_Input* o = SectorPlacer_Z = new Fl_Value_Input(70, 405, 80, 25, "Z");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_SectorPlacer_Center_SetZ);
            }
            o->end();
          }
          { Fl_Value_Input* o = SectorPlacer_MinRadius = new Fl_Value_Input(300, 360, 80, 25, "Min. Radius");
            o->maximum(10);
            o->step(0.1);
            o->callback((Fl_Callback*)psEditorGUI_SectorPlacer_SetMinRadius);
          }
          { Fl_Value_Input* o = SectorPlacer_MaxRadius = new Fl_Value_Input(300, 390, 80, 25, "Max. Radius");
            o->maximum(10);
            o->step(0.1);
            o->value(1);
            o->callback((Fl_Callback*)psEditorGUI_SectorPlacer_SetMaxRadius);
          }
          { Fl_Value_Input* o = SectorPlacer_MinPhi = new Fl_Value_Input(480, 360, 80, 25, "Min. Phi");
            o->maximum(6.2832);
            o->step(0.01);
            o->callback((Fl_Callback*)psEditorGUI_SectorPlacer_SetMinPhi);
          }
          { Fl_Value_Input* o = SectorPlacer_MaxPhi = new Fl_Value_Input(480, 390, 80, 25, "Max. Phi");
            o->maximum(6.2832);
            o->step(0.01);
            o->value(6.2832);
            o->callback((Fl_Callback*)psEditorGUI_SectorPlacer_SetMaxPhi);
          }
          o->end();
        }
        { Fl_Group* o = PointPlacerParameters = new Fl_Group(10, 310, 580, 160, "Point Placer Parameters");
          o->box(FL_DOWN_BOX);
          o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
          o->hide();
          { Fl_Value_Input* o = PointPlacer_X = new Fl_Value_Input(260, 340, 80, 25, "X");
            o->minimum(-10);
            o->maximum(10);
            o->step(0.1);
            o->callback((Fl_Callback*)psEditorGUI_PointPlacer_SetX);
          }
          { Fl_Value_Input* o = PointPlacer_Y = new Fl_Value_Input(260, 370, 80, 25, "Y");
            o->minimum(-10);
            o->maximum(10);
            o->step(0.1);
            o->callback((Fl_Callback*)psEditorGUI_PointPlacer_SetY);
          }
          { Fl_Value_Input* o = PointPlacer_Z = new Fl_Value_Input(260, 400, 80, 25, "Z");
            o->minimum(-10);
            o->maximum(10);
            o->step(0.1);
            o->callback((Fl_Callback*)psEditorGUI_PointPlacer_SetZ);
          }
          o->end();
        }
        { Fl_Group* o = MultiSegmentPlacerParameters = new Fl_Group(10, 310, 580, 160, "Multi Segment Placer Parameters");
          o->box(FL_DOWN_BOX);
          o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
          o->hide();
          { Fl_Browser* o = MultiSegmentPlacer_Vertices = new Fl_Browser(20, 320, 170, 105, "Vertices");
            o->type(2);
            o->callback((Fl_Callback*)psEditorGUI_MultiSegmentPlacer_VerticesChanged);
          }
          { Fl_Button* o = new Fl_Button(195, 370, 115, 25, "Add Vertex");
            o->callback((Fl_Callback*)psEditorGUI_MultiSegmentPlacer_AddVertex);
          }
          { Fl_Button* o = MultiSegmentPlacer_DeleteVertex = new Fl_Button(195, 400, 115, 25, "Delete Vertex");
            o->callback((Fl_Callback*)psEditorGUI_MultiSegmentPlacer_DeleteVertex);
          }
          { Fl_Group* o = MultiSegmentPlacer_VertexParameters = new Fl_Group(350, 320, 230, 105, "Vertex Parameters");
            o->box(FL_ENGRAVED_BOX);
            o->align(FL_ALIGN_BOTTOM);
            o->hide();
            { Fl_Value_Input* o = MultiSegmentPlacer_X = new Fl_Value_Input(430, 330, 80, 25, "X");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_MultiSegmentPlacer_Vertex_SetX);
            }
            { Fl_Value_Input* o = MultiSegmentPlacer_Y = new Fl_Value_Input(430, 360, 80, 25, "Y");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_MultiSegmentPlacer_Vertex_SetY);
            }
            { Fl_Value_Input* o = MultiSegmentPlacer_Z = new Fl_Value_Input(430, 390, 80, 25, "Z");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_MultiSegmentPlacer_Vertex_SetZ);
            }
            o->end();
          }
          o->end();
        }
        { Fl_Choice* o = new Fl_Choice(235, 145, 155, 25, "Placer Type");
          o->down_box(FL_BORDER_BOX);
          o->callback((Fl_Callback*)psEditorGUI_Placer_SetType);
          o->menu(menu_Placer);
        }
        o->end();
      }
      { Fl_Group* o = new Fl_Group(5, 60, 590, 520, "Shooter");
        o->hide();
        { Fl_Group* o = RadialShooterParameters = new Fl_Group(10, 310, 580, 270, "Radial Shooter Parameters");
          o->box(FL_DOWN_BOX);
          o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
          { Fl_Value_Input* o = RadialShooter_MinTheta = new Fl_Value_Input(105, 340, 85, 25, "Min. Theta");
            o->maximum(3.1416);
            o->step(0.01);
            o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMinTheta);
          }
          { Fl_Value_Input* o = RadialShooter_MaxTheta = new Fl_Value_Input(105, 370, 85, 25, "Max. Theta");
            o->maximum(3.1416);
            o->step(0.01);
            o->value(0.3927);
            o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMaxTheta);
          }
          { Fl_Value_Input* o = RadialShooter_MinPhi = new Fl_Value_Input(265, 340, 85, 25, "Min. Phi");
            o->maximum(6.2832);
            o->step(0.01);
            o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMinPhi);
          }
          { Fl_Value_Input* o = RadialShooter_MaxPhi = new Fl_Value_Input(265, 370, 85, 25, "Max. Phi");
            o->maximum(6.2832);
            o->step(0.01);
            o->value(6.2832);
            o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMaxPhi);
          }
          { Fl_Value_Input* o = RadialShooter_MinInitialSpeed = new Fl_Value_Input(480, 340, 85, 25, "Min. Initial Speed");
            o->maximum(50);
            o->step(0.1);
            o->value(10);
            o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMinInitialSpeed);
          }
          { Fl_Value_Input* o = RadialShooter_MaxInitialSpeed = new Fl_Value_Input(480, 370, 85, 25, "Max. Initial Speed");
            o->maximum(50);
            o->step(0.1);
            o->value(10);
            o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMaxInitialSpeed);
          }
          { Fl_Group* o = new Fl_Group(80, 430, 205, 105, "Min. Initial Rotational Speed");
            o->box(FL_ENGRAVED_BOX);
            { Fl_Value_Input* o = RadialShooter_MinInitialRotationalSpeedX = new Fl_Value_Input(145, 440, 85, 25, "X");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMinInitialRotationalSpeedX);
            }
            { Fl_Value_Input* o = RadialShooter_MinInitialRotationalSpeedY = new Fl_Value_Input(145, 470, 85, 25, "Y");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMinInitialRotationalSpeedY);
            }
            { Fl_Value_Input* o = RadialShooter_MinInitialRotationalSpeedZ = new Fl_Value_Input(145, 500, 85, 25, "Z");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMinInitialRotationalSpeedZ);
            }
            o->end();
          }
          { Fl_Group* o = new Fl_Group(315, 430, 205, 105, "Max. Initial Rotational Speed");
            o->box(FL_ENGRAVED_BOX);
            { Fl_Value_Input* o = RadialShooter_MaxInitialRotationalSpeedX = new Fl_Value_Input(380, 440, 85, 25, "X");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMaxInitialRotationalSpeedX);
            }
            { Fl_Value_Input* o = RadialShooter_MaxInitialRotationalSpeedY = new Fl_Value_Input(380, 470, 85, 25, "Y");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMaxInitialRotationalSpeedY);
            }
            { Fl_Value_Input* o = RadialShooter_MaxInitialRotationalSpeedZ = new Fl_Value_Input(380, 500, 85, 25, "Z");
              o->minimum(-10);
              o->maximum(10);
              o->step(0.1);
              o->callback((Fl_Callback*)psEditorGUI_RadialShooter_SetMaxInitialRotationalSpeedZ);
            }
            o->end();
          }
          o->end();
        }
        { Fl_Choice* o = new Fl_Choice(230, 140, 150, 25, "Shooter Type");
          o->down_box(FL_BORDER_BOX);
          o->callback((Fl_Callback*)psEditorGUI_Shooter_SetType);
          o->menu(menu_Shooter);
        }
        o->end();
      }
      { Fl_Group* o = new Fl_Group(5, 60, 590, 520, "Program");
        o->hide();
        { Fl_Browser* o = Program_Operators = new Fl_Browser(135, 125, 150, 130, "Operators");
          o->type(2);
          o->callback((Fl_Callback*)psEditor_Program_OperatorsChanged);
          o->align(FL_ALIGN_TOP);
        }
        { Fl_Button* o = new Fl_Button(295, 125, 130, 25, "New Force");
          o->callback((Fl_Callback*)psEditorGUI_Program_NewForce);
        }
        { Fl_Button* o = new Fl_Button(295, 160, 130, 25, "New Acceleration");
          o->callback((Fl_Callback*)psEditorGUI_Program_NewAcceleration);
        }
        { Fl_Button* o = new Fl_Button(295, 195, 130, 25, "New Fluid Friction");
          o->callback((Fl_Callback*)psEditorGUI_Program_NewFluidFriction);
        }
        { Fl_Button* o = Program_DeleteOperator = new Fl_Button(295, 230, 130, 25, "Delete Operator");
          o->callback((Fl_Callback*)psEditorGUI_Program_DeleteOperator);
        }
        { Fl_Group* o = FluidFrictionParameters = new Fl_Group(10, 310, 580, 160, "Fluid Friction Parameters");
          o->box(FL_DOWN_BOX);
          o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
          o->hide();
          { Fl_Button* o = new Fl_Button(315, 335, 90, 25, "Air");
            o->callback((Fl_Callback*)psEditorGUI_FluidFriction_Air);
          }
          { Fl_Button* o = new Fl_Button(315, 365, 90, 25, "Water");
            o->callback((Fl_Callback*)psEditorGUI_FluidFriction_Water);
          }
          { Fl_Value_Input* o = FluidFriction_Density = new Fl_Value_Input(220, 335, 80, 25, "Density");
            o->step(0.001);
            o->value(1.2929);
            o->callback((Fl_Callback*)psEditorGUI_FluidFriction_SetDensity);
          }
          { Fl_Value_Input* o = FluidFriction_Viscosity = new Fl_Value_Input(220, 365, 80, 25, "Viscosity");
            o->step(0.0001);
            o->value(1.8e-005);
            o->callback((Fl_Callback*)psEditorGUI_FluidFriction_SetViscosity);
          }
          { Fl_Value_Input* o = FluidFriction_OverrideRadius = new Fl_Value_Input(220, 395, 80, 25, "Override Radius");
            o->maximum(10);
            o->step(0.1);
            o->callback((Fl_Callback*)psEditorGUI_FluidFriction_SetOverrideRadius);
          }
          o->end();
        }
        { Fl_Group* o = ForceParameters = new Fl_Group(10, 310, 580, 160, "Force Parameters");
          o->box(FL_DOWN_BOX);
          o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
          o->hide();
          { Fl_Value_Input* o = Force_X = new Fl_Value_Input(240, 340, 115, 25, "X");
            o->minimum(-10);
            o->maximum(10);
            o->step(0.1);
            o->callback((Fl_Callback*)psEditorGUI_Force_SetX);
          }
          { Fl_Value_Input* o = Force_Y = new Fl_Value_Input(240, 370, 115, 25, "Y");
            o->minimum(-10);
            o->maximum(10);
            o->step(0.1);
            o->callback((Fl_Callback*)psEditorGUI_Force_SetY);
          }
          { Fl_Value_Input* o = Force_Z = new Fl_Value_Input(240, 400, 115, 25, "Z");
            o->minimum(-10);
            o->maximum(10);
            o->step(0.1);
            o->callback((Fl_Callback*)psEditorGUI_Force_SetZ);
          }
          o->end();
        }
        { Fl_Group* o = AccelerationParameters = new Fl_Group(10, 310, 580, 160, "Acceleration Parameters");
          o->box(FL_DOWN_BOX);
          o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
          o->hide();
          { Fl_Value_Input* o = Acceleration_X = new Fl_Value_Input(240, 340, 115, 25, "X");
            o->minimum(-10);
            o->maximum(10);
            o->step(0.1);
            o->callback((Fl_Callback*)psEditorGUI_Acceleration_SetX);
          }
          { Fl_Value_Input* o = Acceleration_Y = new Fl_Value_Input(240, 370, 115, 25, "Y");
            o->minimum(-10);
            o->maximum(10);
            o->step(0.1);
            o->callback((Fl_Callback*)psEditorGUI_Acceleration_SetY);
          }
          { Fl_Value_Input* o = Acceleration_Z = new Fl_Value_Input(240, 400, 115, 25, "Z");
            o->minimum(-10);
            o->maximum(10);
            o->step(0.1);
            o->callback((Fl_Callback*)psEditorGUI_Acceleration_SetZ);
          }
          o->end();
        }
        o->end();
      }
      o->end();
    }
    o->end();
  }
  return w;
}
