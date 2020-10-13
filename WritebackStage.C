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

   uint64_t icode = 0, stat = 0, valE = 0, valM = 0, dstE = RNONE, dstM = RNONE;

   icode = wreg->geticode()->getOutput();
   stat = wreg->getstat()->getOutput();
   valE = wreg->getvalE()->getOutput();
   valM = wreg->getvalM()->getOutput();
   dstE = wreg->getdstE()->getOutput();
   dstM = wreg->getdstM()->getOutput();


   if (icode == IHALT) {
      return true;
   }
   return false;
}

void WritebackStage::doClockHigh(PipeReg ** pregs)
{
   W * wreg = (W *) pregs[WREG];
   uint64_t dstE = wreg->getdstE()->getOutput();
   uint64_t valE = wreg->getvalE()->getOutput();
   bool error;
   RegisterFile * regFile = RegisterFile::getInstance();
   regFile->writeRegister(valE, dstE, error);
}

