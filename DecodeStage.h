class DecodeStage: public Stage
{
   private: 
      void setEInput(E * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA, uint64_t valB,
                           uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB);
      void controlSrcA(uint64_t icode, uint64_t rA, uint64_t &srcA);
      void controlSrcB(uint64_t icode, uint64_t rB, uint64_t &srcB);
      void controlDstE(uint64_t icode, uint64_t rB, uint64_t &dstE);
      void controlDstM(uint64_t icode, uint64_t rA, uint64_t &dstM);
      void controlFwdA(uint64_t &valA, uint64_t srcA, M * mreg, W * wreg, uint64_t e_dstE, uint64_t e_valE);
      void controlFwdB(uint64_t &valB, uint64_t srcB, M * mreg, W * wreg, uint64_t e_dstE, uint64_t e_valE);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      
};