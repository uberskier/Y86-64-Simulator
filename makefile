CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0
OBJ = yess.o Memory.o Tools.o RegisterFile.o \
ConditionCodes.o Loader.o D.o DecodeStage.o E.o ExecuteStage.o F.o FetchStage.o M.o \
MemoryStage.o PipeReg.o PipeRegField.o Simulate.o W.o WritebackStage.o
HF = Debug.h Memory.h Loader.h RegisterFile.h ConditionCodes.h PipeReg.h \
Stage.h Simulate.h

.C.o:
	$(CC) $(CFLAGS) $< -o $@

yess: $(OBJ)
yes.o: $(HF)
Memory.o: Memory.h Tools.h
Tools.o: Tools.h
RegisterFile.o: RegisterFile.h Tools.h
ConditionCodes.o: ConditionCodes.h Tools.h
Loader.o: Loader.h Memory.h
D.o: Instructions.h RegisterFile.h PipeReg.h PipeRegField.h D.h Status.h 
DecodeStage.o: RegisterFile.h PipeReg.h PipeRegField.h F.h D.h M.h W.h E.h Stage.h DecodeStage.h Status.h Debug.h
E.o: Instructions.h RegisterFile.h PipeReg.h PipeRegField.h E.h Status.h 
ExecuteStage.o: RegisterFile.h PipeReg.h PipeRegField.h F.h D.h M.h W.h E.h Stage.h ExecuteStage.h Status.h Debug.h
F.o: Instructions.h RegisterFile.h PipeReg.h PipeRegField.h F.h Status.h 
FetchStage.o: RegisterFile.h PipeReg.h PipeRegField.h F.h D.h M.h W.h Stage.h FetchStage.h Status.h Debug.h
M.o: Instructions.h RegisterFile.h PipeReg.h PipeRegField.h M.h Status.h 
MemoryStage.o: RegisterFile.h PipeReg.h PipeRegField.h F.h D.h M.h W.h Stage.h MemoryStage.h Status.h Debug.h
PipeReg.o: PipeReg.h
PipeRegField.o: PipeRegField.h
Simulate.o: PipeReg.h PipeRegField.h F.h D.h M.h W.h Stage.h ExecuteStage.h MemoryStage.h FetchStage.h DecodeStage.h WritebackStage.h Simulate.h Memory.h RegisterFile.h ConditionCodes.h
W.o: Instructions.h RegisterFile.h PipeReg.h PipeRegField.h W.h Status.h 
WritebackStage.o: RegisterFile.h PipeReg.h PipeRegField.h F.h D.h M.h W.h Stage.h WritebackStage.h Status.h Debug.h
clean:
	rm $(OBJ) yess

run:
	make clean
	make yess
	./run.sh

