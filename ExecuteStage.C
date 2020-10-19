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
   uint64_t icode = 0, valA = 0, valC = 0, e_valE = 0;
   uint64_t stat = SAOK;

   stat = ereg->getstat()->getOutput();
   icode = ereg->geticode()->getOutput();
   valC = ereg->getvalC()->getOutput();
   //valB = ereg->getvalB()->getOutput();
   //srcA = ereg->getsrcA()->getOutput();
   //srcB = ereg->getsrcB()->getOutput();
   valA = ereg->getvalA()->getOutput();
   dstE = ereg->getdstE()->getOutput();
   dstM = ereg->getdstM()->getOutput();

   e_valE = valC;
   valE = e_valE;


   
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
