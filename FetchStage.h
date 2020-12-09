//class to perform the combinational logic of
//the Fetch stage
class FetchStage: public Stage
{
   private:
      void setDInput(D * dreg, uint64_t stat, uint64_t icode, uint64_t ifun, 
                     uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
      uint64_t selectPC(F * freg, M * mreg, W * wreg);
      uint64_t predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP);
      bool needRegIds(uint64_t icode);
      bool needValC(uint64_t icode);
      uint64_t PCincrement(uint64_t f_pc, bool needReg, bool needVal);
      void getRegIds(uint64_t f_pc, uint64_t &rA, uint64_t &rB, uint64_t regs);
      void buildValC(uint64_t f_pc, uint64_t &valC, bool needReg);
      bool instr_validate(uint64_t icode);
      uint64_t getStat(bool error, bool instr_valid, bool icode);
      bool F_stall;
      bool D_stall;
      bool getF_stall(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, uint64_t D_icode, uint64_t M_icode);
      bool getD_stall(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB);
      void calculateControlSignals(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, uint64_t e_Cnd, uint64_t D_icode, uint64_t M_icode);
      bool D_bubble;
      void getD_bubble(uint64_t E_icode, uint64_t e_Cnd, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, uint64_t D_icode, uint64_t M_icode);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);

};
