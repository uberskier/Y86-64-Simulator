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

   mreg->getstat()->setInput(stat);
   mreg->geticode()->setInput(icode);
   mreg->getvalA()->setInput(valA);
   mreg->getdstE()->setInput(dstE);
   mreg->getdstM()->setInput(dstM);
   mreg->getvalE()->setInput(valE);
   mreg->getCnd()->setInput(Cnd);
   
   setWInput(wreg, stat, icode, valE, valM, dstE, dstM);
   return false;
}

void MemoryStage::doClockHigh(PipeReg ** pregs)
{
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];

   mreg->getstat()->normal();
   mreg->geticode()->normal();
   mreg->getCnd()->normal();
   mreg->getvalE()->normal();
   mreg->getvalA()->normal();
   mreg->getdstE()->normal();
   mreg->getdstM()->normal();

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