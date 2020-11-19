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
#include "Instructions.h"
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
bool MemoryStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];

   Memory * mem = Memory::getInstance();
   //dreg values
   uint64_t dstE = RNONE, dstM = RNONE, valE = 0, valA;
   valM = 0;
   //ereg values
   uint64_t icode = 0;
   bool error = false;
   stat = SAOK;

   stat = mreg->getstat()->getOutput();
   icode = mreg->geticode()->getOutput();
   valA = mreg->getvalA()->getOutput();
   dstE = mreg->getdstE()->getOutput();
   dstM = mreg->getdstM()->getOutput();
   valE = mreg->getvalE()->getOutput();

   uint64_t addr = mem_addr(icode, valE, valA);
   
   if (mem_read(icode)) {
      valM = mem->getLong(addr, error);
      if (error) {
         stat = SADR;
      }
   }
   if (mem_write(icode)) {
      mem->putLong(valA, addr, error);
      if (error) {
         stat = SADR;
      }
   }
   
   setWInput(wreg, stat, icode, valE, valM, dstE, dstM);
   return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
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

/* setWInput
 * provides the input to potentially be stored in the W register
 * during doClockHigh
 *
 * @param: wreg - pointer to the W register instance
 * @param: stat - value to be stored in the stat pipeline register within W
 * @param: icode - value to be stored in the icode pipeline register within W
 * @param: valE - value to be stored in the valE pipeline register within W
 * @param: valM - value to be stored in the valM pipeline register within W
 * @param: dstE - value to be stored in the dstE pipeline register within W
 * @param: dstM - value to be stored in the dstM pipeline register within W
*/
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

uint64_t MemoryStage::mem_addr(uint64_t icode, uint64_t M_valE, uint64_t M_valA)
{
   if (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL || icode == IMRMOVQ) {
      return M_valE;
   }
   if (icode == IPOPQ || icode == IRET) {
      return M_valA;
   }
   return 0;
}

bool MemoryStage::mem_read(uint64_t icode) {
   return (icode == IMRMOVQ || icode == IPOPQ || icode == IRET);
}

bool MemoryStage::mem_write(uint64_t icode) {
   return (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL);
}

uint64_t MemoryStage::getm_valM() {
   return valM;
}

uint64_t MemoryStage::getm_stat() {
   return stat;
}