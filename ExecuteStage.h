class ExecuteStage: public Stage
{
   private: 
      void setMInput(M * mreg, uint64_t stat, uint64_t icode, 
                           uint64_t Cnd, uint64_t valE, uint64_t valA,
                           uint64_t dstE, uint64_t dstM);
      uint64_t AluA(uint64_t E_valA, uint64_t E_valC, uint64_t icode);
      uint64_t AluB(uint64_t icode, uint64_t E_valB);
      uint64_t AluFun(uint64_t icode, uint64_t E_ifun);
      bool set_cc(uint64_t icode);
      uint64_t dstEComp(uint64_t icode, uint64_t Cnd, uint64_t E_dstE);
      uint64_t ALUComp(uint64_t alufun, uint64_t aluA, uint64_t aluB, bool setcc);
      void CCComp(uint64_t valE, uint64_t aluA, uint64_t aluB, uint64_t alufun);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      
};