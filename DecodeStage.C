#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "E.h"
#include "Stage.h"
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"


bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   D * dreg = (D *) pregs[DREG];
   E * ereg = (E *) pregs[EREG];
   //dreg values
   uint64_t rA = RNONE, rB = RNONE, valP = 0;
   //ereg values
   uint64_t icode = 0, ifun = 0, valC = 0, valA = 0, valB = 0;
   uint64_t dstE = RNONE,  dstM = RNONE, stat = SAOK, srcA = RNONE, srcB = RNONE;

   dreg->getstat()->setInput(stat);
   dreg->geticode()->setInput(icode);
   dreg->getifun()->setInput(ifun);
   dreg->getvalC()->setInput(valC);
   dreg->getvalP()->setInput(valP);
   dreg->getrA()->setInput(rA);
   dreg->getrB()->setInput(rB);
   
   setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);
   return false;
}

void DecodeStage::doClockHigh(PipeReg ** pregs)
{
   D * dreg = (D *) pregs[DREG];
   E * ereg = (E *) pregs[EREG];

   dreg->getstat()->normal();
   dreg->geticode()->normal();
   dreg->getifun()->normal();
   dreg->getrA()->normal();
   dreg->getrB()->normal();
   dreg->getvalC()->normal();
   dreg->getvalP()->normal();
   ereg->getstat()->normal();
   ereg->geticode()->normal();
   ereg->getifun()->normal();
   ereg->getvalC()->normal();
   ereg->getvalA()->normal();
   ereg->getvalB()->normal();
   ereg->getdstE()->normal();
   ereg->getdstM()->normal();
   ereg->getsrcA()->normal();
   ereg->getsrcB()->normal();

}

void DecodeStage::setEInput(E * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA, uint64_t valB,
                           uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB)
{
   ereg->getstat()->setInput(stat);
   ereg->geticode()->setInput(icode);
   ereg->getifun()->setInput(ifun);
   ereg->getvalC()->setInput(valC);
   ereg->getvalA()->setInput(valA);
   ereg->getvalB()->setInput(valB);
   ereg->getdstE()->setInput(dstE);
   ereg->getdstM()->setInput(dstM);
   ereg->getsrcA()->setInput(srcA);
   ereg->getsrcB()->setInput(srcB);
}