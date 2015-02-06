
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "MotionModelToolbar.h"



////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(MotionModelType);
const MotionModelType MotionModelType::FLY("Fly", MM_FLY);
const MotionModelType MotionModelType::ORBIT("Orbit", MM_ORBIT);
const MotionModelType MotionModelType::UFO("UFO", MM_UFO);

MotionModelType::MotionModelType(const std::string& name, MotionModelTypeE value)
   : BaseClass(name)
   , mValue(value)
{
   AddInstance(this);
}

MotionModelType::~MotionModelType()
{}

MotionModelTypeE MotionModelType::GetValue() const
{
   return mValue;
}



/////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
MotionModelToolbar::MotionModelToolbar(QWidget* parent)
   : BaseClass(parent)
   , mCurrentMotionModel(&MotionModelType::ORBIT)
{
   mUI.setupUi(this);

   // Set initial checked button.
   // TODO:

   CreateConnections();

   UpdateUI();
}

MotionModelToolbar::~MotionModelToolbar()
{}

void MotionModelToolbar::CreateConnections()
{
   // BUTTONS
   connect(mUI.mButtonFly, SIGNAL(pressed()), this, SLOT(OnButtonClick()));
   connect(mUI.mButtonOrbit, SIGNAL(pressed()), this, SLOT(OnButtonClick()));
   connect(mUI.mButtonUFO, SIGNAL(pressed()), this, SLOT(OnButtonClick()));

   // SPINNERS
   connect(mUI.mSpeed, SIGNAL(valueChanged(double)),
      this, SLOT(OnSpeedChanged(double)));
}

void MotionModelToolbar::UpdateUI()
{
   mUI.mMotionModelName->setText(tr(mCurrentMotionModel->GetName().c_str()));
}

const MotionModelType& MotionModelToolbar::GetCurrentMotioModel() const
{
   return *mCurrentMotionModel;
}

void MotionModelToolbar::OnButtonClick()
{
   const MotionModelType* mmType = mCurrentMotionModel;

   QObject* button = sender();
   // Determine which motion model button has been clicked.
   if (button == mUI.mButtonFly)
   {
      mmType = &MotionModelType::FLY;
   }
   else if (button == mUI.mButtonOrbit)
   {
      mmType = &MotionModelType::ORBIT;
   }
   else if (button == mUI.mButtonUFO)
   {
      mmType = &MotionModelType::UFO;
   }

   if (mCurrentMotionModel != mmType)
   {
      mCurrentMotionModel = mmType;

      UpdateUI();

      SignalMotionModelSelected(mCurrentMotionModel->GetValue());
   }
}

void MotionModelToolbar::OnSpeedChanged(double speed)
{
   SignalMotionModelSpeedChanged(mCurrentMotionModel->GetValue(), mUI.mSpeed->value());
}

