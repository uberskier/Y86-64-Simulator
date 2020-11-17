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
#include "ExecuteStage.h"
#include "MemoryStage.h"

/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */

bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   D * dreg = (D *) pregs[DREG];
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   ExecuteStage * estage = (ExecuteStage *) stages[ESTAGE];
   uint64_t e_dstE = estage->gete_dstE(), e_valE = estage->gete_valE();
   MemoryStage * mstage = (MemoryStage *) stages[MSTAGE];
   uint64_t m_valM = mstage->getm_valM();

   //dreg values

   //ereg values
   uint64_t icode = 0, ifun = 0, valC = 0, valA = 0, valB = 0, valP = 0;
   uint64_t dstE = RNONE,  dstM = RNONE, stat = SAOK, srcA = RNONE, srcB = RNONE, rA = RNONE, rB = RNONE;

   stat = dreg->getstat()->getOutput();
   icode = dreg->geticode()->getOutput();
   ifun = dreg->getifun()->getOutput();
   valC = dreg->getvalC()->getOutput();
   rA = dreg->getrA()->getOutput();
   rB = dreg->getrB()->getOutput();
   valP = dreg->getvalP()->getOutput();


   controlSrcA(icode, rA, srcA);
   controlSrcB(icode, rB, srcB);
   controlDstE(icode, rB, dstE);
   controlDstM(icode, rA, dstM);

   valA = controlFwdA(srcA, mreg, wreg, e_dstE, e_valE, valP, m_valM, icode);
   valB = controlFwdB(srcB, mreg, wreg, e_dstE, e_valE, m_valM);
   setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);
   return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
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

/* setEInput
 * provides the input to potentially be stored in the E register
 * during doClockHigh
 *
 * @param: ereg - pointer to the E register instance
 * @param: stat - value to be stored in the stat pipeline register within E
 * @param: icode - value to be stored in the icode pipeline register within E
 * @param: ifun - value to be stored in the ifun pipeline register within E
 * @param: valC - value to be stored in the valC pipeline register within E
 * @param: valA - value to be stored in the valA pipeline register within E
 * @param: valB - value to be stored in the valB pipeline register within E
 * @param: dstE - value to be stored in the dstE pipeline register within E
 * @param: dstM - value to be stored in the dstM pipeline register within E
 * @param: srcA - value to be stored in the srcA pipeline register within E
 * @param: srcB - value to be stored in the srcB pipeline register within E
*/
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

/* controlSrcA
 * changes srcA according to instruction
 * 
 *
 * @param: icode - instruction code
 * @param: rA - rA register
 * @param: srcA - pointer to srcA to change
 */
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

/* controlSrcB
 * changes srcB according to instruction
 * 
 *
 * @param: icode - instruction code
 * @param: rB - rB register
 * @param: srcB - pointer to srcB to change
 */
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

/* controlDstE
 * changes dstE according to instruction
 * 
 *
 * @param: icode - instruction code
 * @param: rB - rB register
 * @param: dstE - pointer to dstE to change
 */
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

/* controlDstM
 * changes dstM according to instruction
 * 
 *
 * @param: icode - instruction code
 * @param: rA - rA register
 * @param: dstM - pointer to dstM to change
 */
void DecodeStage::controlDstM(uint64_t icode, uint64_t rA, uint64_t &dstM) {
   if (icode == IMRMOVQ || icode == IPOPQ) {
      dstM = rA;
   }
   else {
      dstM = RNONE;
   }
}

/* controlFwdA
 * forwarding changes for valA
 * acts weird with rnone at last
 *
 * @param: srcA - srcA
 * @param: M * mreg - memory register
 * @param: W * wreg - writeback register
 * @param: e_dstE - dstE
 * @param: e_valE = valE
 * @param: valP - valP
 * @param: m_valM - valM
 * @param: icode - icode
 */
uint64_t DecodeStage::controlFwdA(uint64_t srcA, M * mreg, W * wreg, uint64_t e_dstE, uint64_t e_valE, uint64_t valP, uint64_t m_valM, uint64_t icode) {
   RegisterFile * regfile = RegisterFile::getInstance();

   uint64_t M_dstE = mreg->getdstE()->getOutput(), M_valE = mreg->getvalE()->getOutput(), W_dstE = wreg->getdstE()->getOutput(),
   W_valE = wreg->getvalE()->getOutput(), M_dstM = mreg->getdstM()->getOutput(),
   W_dstM = wreg->getdstM()->getOutput(), W_valM = wreg->getvalM()->getOutput();
   if (icode == ICALL || icode == IJXX) {
      return valP;
   }
   if (srcA == RNONE) {
      return 0;
   }
   if (srcA == e_dstE) {
      return e_valE;
   }
   if (srcA == M_dstM) {
      //printf("check: %x\n", m_valM);
      return m_valM;
   }
   if (srcA == M_dstE) {
      return M_valE;
   }
   if (srcA == W_dstM) {
      return W_valM;
   }
   if (srcA == W_dstE) {
      return W_valE;
   }
   bool error;
   return regfile->readRegister((int32_t)srcA, error);
}

/* controlFwdA
 * forwarding changes for valB
 * 
 *
 * @param: valB - pointer to change valB
 * @param: d_rvalB - value to changer valB
 */
uint64_t DecodeStage::controlFwdB(uint64_t srcB, M * mreg, W * wreg, uint64_t e_dstE, uint64_t e_valE, uint64_t m_valM) {
   RegisterFile * regfile = RegisterFile::getInstance();
   bool error;

   uint64_t M_dstE = mreg->getdstE()->getOutput(), M_valE = mreg->getvalE()->getOutput(), W_dstE = wreg->getdstE()->getOutput(),
   W_valE = wreg->getvalE()->getOutput(), M_dstM = mreg->getdstM()->getOutput(),
   W_dstM = wreg->getdstM()->getOutput(), W_valM = wreg->getvalM()->getOutput();

   if (srcB == RNONE) {
      return 0;
   }
   if (srcB == e_dstE) {
      return e_valE;
   }
   if (srcB == M_dstM) {
      return m_valM;
   }
   if (srcB == M_dstE) {
      return M_valE;
   }
   if (srcB == W_dstM) {
      return W_valM;
   }
   if (srcB == W_dstE) {
      return W_valE;
   }
   return regfile->readRegister((int32_t)srcB, error);
}