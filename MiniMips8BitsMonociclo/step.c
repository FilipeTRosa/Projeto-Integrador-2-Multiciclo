#include "memoria.h"
#include "controle.h"
#include "multiplexadores.h"
#include "minimips.h"
#include "decodificador.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

void step(int *parada,int *pc, struct memoria_instrucao *memInst, BRegs *bancoReg, CTRL *controle, descPilha *pilha, struct estatistica *stat, int *estadoControle, int *regSaidaUla, RegMDR* regMDR, int *RegA, int *RegB, RegINST* regIR) {
    int *buscaReg = NULL;
    int regDest = 0;
    int fonte1 = 0, fonte2 = 0;
    int *resultadoULA = NULL;
    Mux* mux = NULL;

    setSignal(controle, estadoControle);

    mux = criaMux(*pc, *regSaidaUla, 0, controle->IouD);
    int endIouD = muxFuncition(mux);

    atualizaIR(regIR, buscaInstrucao(memInst, endIouD), controle->IREsc);

    // FALTA ADICIONAR AQUI O REGISTRADOR MDR

    mux = criaMux(regIR->inst.rt, regIR->inst.rd, 0, controle->RegDst);
    regDest = muxFuncition(mux);

    buscaReg = buscaBancoRegs(bancoReg, regIR->inst.rs, regIR->inst.rt, regDest);

    *RegA = buscaReg[0];

    *RegB = buscaReg[1];

    mux = criaMux(*pc, RegA, 0, controle->ULAFonteA);
    fonte1 = muxFuncition(mux);

    Mux* auxMux = criaMux(RegB, 1, regIR->inst.imm, controle->ULAFonteB);
    fonte2 = muxFuncition(auxMux);

    resultadoULA = processamentoULA(fonte1, fonte2, controle->ULAControle);

     *regSaidaUla = regSaidaULA(resultadoULA[0], 0);

     mux = criaMux(*regSaidaUla, 2, 0, controle->MemParaReg); // NO LUGAR DO DOIS SERA USADO O REGISTRADOR MDR
     int dataWrite = muxFuncition(mux);

     salvaDadoReg(bancoReg, dataWrite, buscaReg[2], controle->EscReg);

    mux = criaMux(resultadoULA[0], *regSaidaUla, *pc, controle->PCFonte);
    
    if(controle->PCEsc) {
        *pc = muxFuncition(mux);
    }

    nextState(estadoControle, regIR->inst.opcode, regIR->inst.funct);
}
