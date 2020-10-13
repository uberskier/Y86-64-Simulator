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
#include "FetchStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "Tools.h"
#include "Memory.h"


/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
 
bool FetchStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];

   uint64_t f_pc = 0, icode = 0, ifun = 0, valC = 0, valP = 0;
   uint64_t rA = RNONE, rB = RNONE, stat = SAOK;

   //code missing here to select the value of the PC
   //and fetch the instruction from memory
   //Fetching the instruction will allow the icode, ifun,
   //rA, rB, and valC to be set.
   //The lab assignment describes what methods need to be
   //written.
   
   bool error;
   f_pc = selectPC(freg, mreg, wreg);
   Memory * mem = Memory::getInstance();
   uint64_t iCodeiFun = mem->getByte(f_pc, error);
   icode = Tools::getBits(iCodeiFun, 4, 7);
   ifun = Tools::getBits(iCodeiFun, 0, 3);
   
   
   if (needRegIds(icode)) {
      uint64_t regs = mem->getByte(f_pc + 1, error);
      rA = Tools::getBits(regs, 4, 7);
      rB = Tools::getBits(regs, 0, 3);
   }
   
   if(needValC(icode)){
      if (needRegIds(icode)) {
         uint64_t regval = mem->getByte(f_pc + 2, error);
         valC = regval;
         for (int x = 3; x < 8; x++) {
            regval = mem->getByte(f_pc + x, error);
            if (!error) {
               valC = valC << 8;
               valC += regval;
            }
         }
      }
      else {
         uint64_t valDest = mem->getByte(f_pc + 1, error);
         valC = valDest;
         for (int x = 2; x < 8; x++) {
            valDest = mem->getByte(f_pc + x, error);
            if (!error) {
               valC = valC<< 8;
               valC += valDest;
            }
         }
      }
   }
   
   bool needReg = needRegIds(icode);
   bool needVal = needValC(icode);
   valP = PCincrement(f_pc, needReg, needVal);
   //The value passed to setInput below will need to be changed
   freg->getpredPC()->setInput(predictPC(icode, valC, valP));
   
   //provide the input values for the D register
   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);
   
   return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void FetchStage::doClockHigh(PipeReg ** pregs)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];

   freg->getpredPC()->normal();
   dreg->getstat()->normal();
   dreg->geticode()->normal();
   dreg->getifun()->normal();
   dreg->getrA()->normal();
   dreg->getrB()->normal();
   dreg->getvalC()->normal();
   dreg->getvalP()->normal();
}

/* setDInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(D * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
   dreg->getstat()->setInput(stat);
   dreg->geticode()->setInput(icode);
   dreg->getifun()->setInput(ifun);
   dreg->getrA()->setInput(rA);
   dreg->getrB()->setInput(rB);
   dreg->getvalC()->setInput(valC);
   dreg->getvalP()->setInput(valP);
}

uint64_t FetchStage::selectPC(F * freg, M * mreg, W * wreg) {
   uint64_t m_icode = mreg->geticode()->getOutput(), w_icode = wreg->geticode()->getOutput(), m_Cnd = mreg->getCnd()->getOutput(),
                      m_valA = mreg->getvalA()->getOutput(), w_valM = wreg->getvalM()->getOutput();
   

   if (m_icode == IJXX && !m_Cnd) {
      return m_valA;
   }
   if (w_icode == IRET){
      return w_valM;
   }
   return freg->getpredPC()->getOutput();
   
}
 
uint64_t FetchStage::predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP) {
   if (f_icode == IJXX || f_icode == ICALL) {
      return f_valC;
   }
   return f_valP;
}


bool FetchStage::needRegIds(uint64_t icode){
   return (icode == IRRMOVQ || icode == IOPQ || icode == IPUSHQ || icode == IPOPQ || icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ);
}


bool FetchStage::needValC(uint64_t icode){
   return (icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ || icode == IJXX || icode == ICALL);
}

uint64_t FetchStage::PCincrement(uint64_t f_pc, bool needReg, bool needVal) {
   if (needReg) {
      if (needVal) {
         return f_pc + 10;
      }
   }
   if (needVal) {
      return f_pc + 9;
   }
   return f_pc + 1;
}

void FetchStage::getRegIds() {

}

void FetchStage::buildValC() {
   
}