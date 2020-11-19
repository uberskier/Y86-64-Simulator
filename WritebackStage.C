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

/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool WritebackStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   W * wreg = (W *) pregs[WREG];
   //commented code for unused variables

   //uint64_t  valM = 0, dstM = RNONE;
   uint64_t icode = 0;

   icode = wreg->geticode()->getOutput();
   //stat = wreg->getstat()->getOutput();
   //valE = wreg->getvalE()->getOutput();
   //valM = wreg->getvalM()->getOutput();
   //dstE = wreg->getdstE()->getOutput();
   //dstM = wreg->getdstM()->getOutput();

   
   if (icode == IHALT) {
      return true;
   }
   return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void WritebackStage::doClockHigh(PipeReg ** pregs)
{
   W * wreg = (W *) pregs[WREG];
   uint64_t dstE = wreg->getdstE()->getOutput();
   uint64_t valE = wreg->getvalE()->getOutput();
   uint64_t valM = wreg->getvalM()->getOutput();
   uint64_t dstM = wreg->getdstM()->getOutput();
   bool error;
   RegisterFile * regFile = RegisterFile::getInstance();
   regFile->writeRegister(valE, dstE, error);
   regFile->writeRegister(valM, dstM, error);
}

