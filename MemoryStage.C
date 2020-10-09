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
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"


bool MemoryStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   //dreg values
   uint64_t dstE = RNONE, dstM = RNONE, valE = 0, Cnd = 0, valM = 0;
   //ereg values
   uint64_t icode = 0, valA = 0;
   uint64_t stat = SAOK;

   stat = mreg->getstat()->getOutput();
   icode = mreg->geticode()->getOutput();
   valA = mreg->getvalA()->getOutput();
   dstE = mreg->getdstE()->getOutput();
   dstM = mreg->getdstM()->getOutput();
   valE = mreg->getvalE()->getOutput();
   Cnd = mreg->getCnd()->getOutput();
   
   setWInput(wreg, stat, icode, valE, valM, dstE, dstM);
   return false;
}

void MemoryStage::doClockHigh(PipeReg ** pregs)
{
   W * wreg = (W *) pregs[WREG];

   wreg->getstat()->normal();
   wreg->geticode()->normal();
   wreg->getvalE()->normal();
   wreg->getvalM()->normal();
   wreg->getdstE()->normal();
   wreg->getdstM()->normal();
}

void MemoryStage::setWInput(W * wreg, uint64_t stat, uint64_t icode, 
                           uint64_t valE, uint64_t valM,
                           uint64_t dstE, uint64_t dstM)
{
   wreg->getstat()->setInput(stat);
   wreg->geticode()->setInput(icode);
   wreg->getvalE()->setInput(valE);
   wreg->getvalM()->setInput(valM);
   wreg->getdstE()->setInput(dstE);
   wreg->getdstM()->setInput(dstM);
}