#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "ConditionCodes.h"
#include "Tools.h"

/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   M * mreg = (M *) pregs[MREG];
   E * ereg = (E *) pregs[EREG];
   //dreg values
   uint64_t dstE = RNONE, dstM = RNONE, valE = 0, Cnd = 0;
   //ereg values
   uint64_t icode = 0, ifun = 0, valA = 0, valC = 0, e_valE = 0, valB = 0;
   uint64_t stat = SAOK;
   uint64_t aluA = 0, aluB = 0, alufun = 0, e_dstE = 0;

   stat = ereg->getstat()->getOutput();
   icode = ereg->geticode()->getOutput();
   ifun = ereg->getifun()->getOutput();
   valC = ereg->getvalC()->getOutput();
   valB = ereg->getvalB()->getOutput();
   //srcA = ereg->getsrcA()->getOutput();
   //srcB = ereg->getsrcB()->getOutput();
   valA = ereg->getvalA()->getOutput();
   dstE = ereg->getdstE()->getOutput();
   dstM = ereg->getdstM()->getOutput();

   aluA = AluA(valA, valC, icode);
   aluB = AluB(icode, valB);
   alufun = AluFun(icode, ifun);

   valE = ALUComp(alufun, aluA, aluB);

   if (set_cc(icode)) {
      Cnd = CCComp(valE, aluA, aluB, alufun);
   }

   
   setMInput(mreg, stat, icode, Cnd, valE, valA, dstE, dstM);
   return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void ExecuteStage::doClockHigh(PipeReg ** pregs)
{
   M * mreg = (M *) pregs[MREG];

   mreg->getstat()->normal();
   mreg->geticode()->normal();
   mreg->getCnd()->normal();
   mreg->getvalE()->normal();
   mreg->getvalA()->normal();
   mreg->getdstE()->normal();
   mreg->getdstM()->normal();
}

/* setMInput
 * provides the input to potentially be stored in the M register
 * during doClockHigh
 *
 * @param: mreg - pointer to the M register instance
 * @param: stat - value to be stored in the stat pipeline register within M
 * @param: icode - value to be stored in the icode pipeline register within M
 * @param: Cnd - value to be stored in the Cnd pipeline register within M
 * @param: valE - value to be stored in the valE pipeline register within M
 * @param: valA - value to be stored in the valA pipeline register within M
 * @param: dstE - value to be stored in the dstE pipeline register within M
 * @param: dstM - value to be stored in the dstM pipeline register within M
*/
void ExecuteStage::setMInput(M * mreg, uint64_t stat, uint64_t icode, 
                           uint64_t Cnd, uint64_t valE, uint64_t valA,
                           uint64_t dstE, uint64_t dstM)
{
   mreg->getstat()->setInput(stat);
   mreg->geticode()->setInput(icode);
   mreg->getCnd()->setInput(Cnd);
   mreg->getvalE()->setInput(valE);
   mreg->getvalA()->setInput(valA);
   mreg->getdstE()->setInput(dstE);
   mreg->getdstM()->setInput(dstM);
}

uint64_t ExecuteStage::ALUComp(uint64_t alufun, uint64_t aluA, uint64_t aluB) {
   if (alufun == ADDQ) {
      return aluA + aluB;
   }
   if (alufun == SUBQ) {
      return aluA - aluB;
   }
   if (alufun == ANDQ) {
      if (aluA == aluB) {
         return 1;
      }
      else {
         return 0;
      }
   }
   if (alufun == XORQ) {
      if (aluA == aluB) {
         return 0;
      }
      return 1;
   }
   return 0;
}

uint64_t ExecuteStage::CCComp(uint64_t valE, uint64_t aluA, uint64_t aluB, uint64_t alufun) {
    ConditionCodes * cndCodes = ConditionCodes::getInstance();
    bool error;
    uint8_t sign = Tools::sign(valE);
    bool add = false;
    bool sub = false;
    cndCodes->setConditionCode(false, OF, error);
    cndCodes->setConditionCode(false, SF, error);
    cndCodes->setConditionCode(false, ZF, error);
    if (sign == 1) {
       cndCodes->setConditionCode(true, SF, error);
    }
    if (valE == 0) {
       cndCodes->setConditionCode(true, ZF, error);
    }
    if (alufun == ADDQ) {
       add = Tools::addOverflow(aluA, aluB);
       if (add == true) {
          cndCodes->setConditionCode(true, OF, error);
       }
    }
    if (alufun == SUBQ) {
      sub = Tools::subOverflow(aluA, aluB);
       if (sub == true) {
          cndCodes->setConditionCode(true, OF, error);
       }
    }
    return cndCodes->getConditionCode((int64_t)valE, error);
}



/* AluA
 * gets the value for AluA based on instruction code
 * 
 *
 * @param: E_valA - valA value
 * @param: E_valC - valC value
 * @param: icode - icode instruction
 */
uint64_t ExecuteStage::AluA(uint64_t E_valA, uint64_t E_valC, uint64_t icode) {
   if (icode == IRRMOVQ || icode == IOPQ) {
      return E_valA;
   }
   if (icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ) {
      return E_valC;
   }
   if (icode == ICALL || icode == IPUSHQ) {
      return -8;
   }
   if (icode == IRET || icode == IPOPQ) {
      return 8;
   }
   return 0;
}

/* AluB
 * gets the value for AluB based on instruction code
 * 
 *
 * @param: icode - icode instruction
 * @param: E_valB - valb value
 */
uint64_t ExecuteStage::AluB(uint64_t icode, uint64_t E_valB) {
   if (icode == IRMMOVQ || icode == IMRMOVQ || icode == IOPQ || icode == ICALL || icode == IPUSHQ || icode == IRET || icode == IPOPQ) {
      return E_valB;
   }
   if (icode == IRRMOVQ || icode == IIRMOVQ) {
      return 0;
   }
   return 0;
}

/* AluFun
 * gets the fun value
 * 
 *
 * @param: icode - icode instruction
 * @param: E_ifun - ifun value
 */
uint64_t ExecuteStage::AluFun(uint64_t icode, uint64_t E_ifun) {
   if (icode == IOPQ) {
      return E_ifun;
   }
   return ADDQ;
} 

/* set_cc
 * sees if we need to set condition codes
 * 
 *
 * @param: icode - icode instruction
 */
bool ExecuteStage::set_cc(uint64_t icode) {
   if (icode == IOPQ) {
      return true;
   }
   else {
      return false;
   }
}

/* dstEComp
 * sets dstE
 * 
 *
 * @param: icode - icode instruction
 */
uint64_t ExecuteStage::dstEComp(uint64_t icode, uint64_t Cnd, uint64_t E_dstE) {
   if (icode == IRRMOVQ && !Cnd) {
      return RNONE;
   }
   else {
      return E_dstE;
   }
}
