#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"


bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   return false;
}

void DecodeStage::doClockHigh(PipeReg ** pregs)
{
    
}