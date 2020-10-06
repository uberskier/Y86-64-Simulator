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
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"


bool WritebackStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   W * wreg = (W *) pregs[WREG];

   uint64_t icode = 0;

   wreg->geticode()->setInput(icode);

   if (icode == IHALT) {
      return true;
   }
   return false;
}

void WritebackStage::doClockHigh(PipeReg ** pregs)
{
    
}