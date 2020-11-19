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
   uint64_t rA = RNONE, rB = RNONE;
   bool mem_error = false;

   //code missing here to select the value of the PC
   //and fetch the instruction from memory
   //Fetching the instruction will allow the icode, ifun,
   //rA, rB, and valC to be set.
   //The lab assignment describes what methods need to be
   //written.
   
   bool error = false;
   f_pc = selectPC(freg, mreg, wreg);
   Memory * mem = Memory::getInstance();
   uint64_t iCodeiFun = mem->getByte(f_pc, error);
   if (error) {
      icode = INOP;
      ifun = FNONE;
   }
   else {
      icode = Tools::getBits(iCodeiFun, 4, 7);
      ifun = Tools::getBits(iCodeiFun, 0, 3);
   }

   bool instr_valid = instr_validate(icode);
   uint64_t stat = getStat(error, instr_valid, icode);
   
   bool needReg = needRegIds(icode);
   bool needVal = needValC(icode);
   valP = PCincrement(f_pc, needReg, needVal);

   //checks if needs RegIds and gets them for rA and rB
   if (needReg) {
      uint64_t regs = mem->getByte(f_pc + 1, error);
      getRegIds(f_pc, rA, rB, regs);
   }
   if (needVal) {
      buildValC(f_pc, valC, needReg);
   }

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

/*
 * selectPC:
 * Performs the logic for selecting the program counter
 * 
 *
 * @param: F * freg - Fetch register
 * @param: M * mreg - Memory register
 * @param: W * wreg - Writeback register
 */
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

/*
 * predictPC:
 * Predicts the PC for next instruction
 * 
 *
 * @param: f_icode - icode of instruction
 * @param: f_valC - valC could be location
 * @param: f_valP - valP could be location/
 */
uint64_t FetchStage::predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP) {
   if (f_icode == IJXX || f_icode == ICALL) {
      return f_valC;
   }
   return f_valP;
}

/*
 * needRegIds:
 * Determines if instruction needs Register IDs
 * 
 *
 * @param: icode - icode of instruction
 */
bool FetchStage::needRegIds(uint64_t icode){
   return (icode == IRRMOVQ || icode == IOPQ || icode == IPUSHQ || icode == IPOPQ || icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ);
}

/*
 * needValC:
 * Determines if instruction needs a ValC 
 * 
 *
 * @param: icode - icode of instruction
 */
bool FetchStage::needValC(uint64_t icode){
   return (icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ || icode == IJXX || icode == ICALL);
}

/*
 * PCincrement:
 * Increments the PC based on instruction
 * 
 *
 * @param: f_pc - current program counter
 * @param: needReg - boolean if instruction needs Register Ids
 * @param: needVal - boolean if instruction needs ValC
 */
uint64_t FetchStage::PCincrement(uint64_t f_pc, bool needReg, bool needVal) {
   if (needReg) {
      if (needVal) {
         return f_pc + 10;
      }
      else {
         return f_pc + 2;
      }
   }
   if (needVal) {
      return f_pc + 9;
   }
   return f_pc + 1;
}

/*
 * getRegIds:
 * Grabs register Ids from memory
 * 
 *
 * @param: f_pc - current program counter
 * @param: rA - pointer by reference to rA
 * @param: rB - pointer by reference to rB
 * @param: regs - uint of byte where registers are stored
 */
void FetchStage::getRegIds(uint64_t f_pc, uint64_t &rA, uint64_t &rB, uint64_t regs) {
   rA = Tools::getBits(regs, 4, 7);
   rB = Tools::getBits(regs, 0, 3);
}

// use tools buildLong with array in for loops
/*
 * buildValC:
 * builds then number for valc
 * 
 *
 * @param: f_pc - current program counter
 * @param: valC - pointer by reference to valC
 * @param: needReg - boolean if instruction needs reg ids
 */
void FetchStage::buildValC(uint64_t f_pc, uint64_t &valC, bool needReg) {
   Memory * memValC = Memory::getInstance();
   uint8_t newvalC[8];
   bool error;
   uint64_t count = 0;
   if (needReg) {
         uint8_t regval = memValC->getByte(f_pc + 2, error);
         newvalC[count] = regval;
         for (int x = 3; x < 10; x++) {
            regval = memValC->getByte(f_pc + x, error);
            if (!error) {
               count += 1;
               newvalC[count] = regval;
            }
         }
      }
   else {
         uint8_t valDest = memValC->getByte(f_pc + 1, error);
         newvalC[count] = valDest;
         for (int x = 2; x < 8; x++) {
            valDest = memValC->getByte(f_pc + x, error);
            if (!error) {
               count += 1;
               newvalC[count] = valDest;
            }
         }
   }
   valC = Tools::buildLong(newvalC);
}

bool FetchStage::instr_validate(uint64_t icode) {
   return (icode == INOP || icode == IHALT || icode == IRRMOVQ || icode == IIRMOVQ 
                         || icode == IRMMOVQ || icode == IMRMOVQ || icode == IOPQ 
                         || icode == IJXX || icode == ICALL || icode == IRET || icode == IPUSHQ || icode == IPOPQ);
}

uint64_t FetchStage::getStat(bool error, bool instr_valid, bool icode) {
   if (error) {
      return SADR;
   }
   if (!instr_valid) {
      return SINS;
   }
   if (icode == IHALT) {
      return SHLT;
   }
   return SAOK;
}