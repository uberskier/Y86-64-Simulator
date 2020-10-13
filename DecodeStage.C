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
#include "Instructions.h"


bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   D * dreg = (D *) pregs[DREG];
   E * ereg = (E *) pregs[EREG];
   //dreg values

   //ereg values
   uint64_t icode = 0, ifun = 0, valC = 0, valA = 0, valB = 0;
   uint64_t dstE = RNONE,  dstM = RNONE, stat = SAOK, srcA = RNONE, srcB = RNONE, rA = RNONE, rB = RNONE;

   stat = dreg->getstat()->getOutput();
   icode = dreg->geticode()->getOutput();
   ifun = dreg->getifun()->getOutput();
   valC = dreg->getvalC()->getOutput();
   rA = dreg->getrA()->getOutput();
   rB = dreg->getrB()->getOutput();


   controlSrcA(icode, rA, srcA);
   controlSrcB(icode, rB, srcB);
   controlDstE(icode, rB, dstE);
   controlDstM(icode, rA, dstM);
   controlFwdA(valA, 0);
   controlFwdB(valB, 0);
   setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);
   return false;
}

void DecodeStage::doClockHigh(PipeReg ** pregs)
{
   E * ereg = (E *) pregs[EREG];

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

void DecodeStage::controlSrcA(uint64_t icode, uint64_t rA, uint64_t &srcA) {
   if (icode == IRRMOVQ || icode == IRMMOVQ || icode == IOPQ || icode == IPUSHQ) {
      srcA = rA;
   }
   else if (icode == IPOPQ || icode == IRET) {
      srcA = RSP;
   }
   else {
      srcA = RNONE;
   }
}

void DecodeStage::controlSrcB(uint64_t icode, uint64_t rB, uint64_t &srcB) {
   if (icode == IRMMOVQ || icode == IOPQ || icode == IMRMOVQ) {
      srcB = rB;
   }
   else if (icode == IPOPQ || icode == IRET || icode == ICALL || icode == IPUSHQ) {
      srcB = RSP;
   }
   else {
      srcB = RNONE;
   }
}

void DecodeStage::controlDstE(uint64_t icode, uint64_t rB, uint64_t &dstE) {
   if (icode == IRRMOVQ || icode == IIRMOVQ || icode == IOPQ) {
      dstE = rB;
   }
   else if (icode == IPUSHQ || icode == IPOPQ || icode == ICALL || icode == IRET) {
      dstE = RSP;
   }
   else {
      dstE = RNONE;
   }
}

void DecodeStage::controlDstM(uint64_t icode, uint64_t rA, uint64_t &dstM) {
   if (icode == IMRMOVQ || icode == IPOPQ) {
      dstM = rA;
   }
   else {
      dstM = RNONE;
   }
}

void DecodeStage::controlFwdA(uint64_t &valA, uint64_t d_rvalA) {
   valA = d_rvalA;
}

void DecodeStage::controlFwdB(uint64_t &valB, uint64_t d_rvalB) {
   valB = d_rvalB;
}