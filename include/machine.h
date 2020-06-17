#ifndef IJVM_EMULATOR_MACHINE_H
#define IJVM_EMULATOR_MACHINE_H

void doBIPUSH(void);

void doDUP(void);

void doERR(char *string);

void doGOTO(void);

void doHALT(void);

void doIADD(void);

void doIAND(void);

void doIFEQ(void);

void doIFLT(void);

void doICMPEQ(void);

void doIINC(void);

void doILOAD(void);

void doIN(void);

void doINVOKEVIRTUAL(void);

void doIOR(void);

void doIRETURN(void);

void doISTORE(void);

void doISUB(void);

void doLDC_W(void);

void doNOP(void);

void doOUT(void);

void doPOP(void);

void doSWAP(void);

void doIINCWIDE(void);

void doILOADWIDE(void);

void doISTOREWIDE(void);

void doWIDE(void);

void doNEWARRAY(void);

void doIALOAD(void);

void doIASTORE(void);

void doGC(void);

void doNETBIND(void);

void doNETCONNECT(void);

void doNETIN(void);

void doNETOUT(void);

void doNETCLOSE(void);


#endif
