#include "memoria.h"
#include "controle.h"
#include "multiplexadores.h"
#include "minimips.h"
#include "decodificador.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


void step(int *parada, int *pc, struct memoria_instrucao *memInst, BRegs *bancoReg, CTRL *controle, descPilha *pilha, struct estatistica *stat, int *estadoControle, int *regSaidaUla, RegMDR* regMDR, int *RegA, int *RegB, RegINST* regIR) {
    int *buscaReg = NULL;
    int regDest = 0;
    int fonte1 = 0, fonte2 = 0;
    int *resultadoULA = NULL;
    int dataWrite = 0;
    Mux* mux = NULL;
    //  variaveis para pegar o valor no ciclo atual
    int SaidaULA = 0;
    int MDR = 0;
    struct instrucao novaInstrucao = {0};


    if (strcmp(regIR->inst.inst_char, "0000000000000000") == 0) {
        printf("Programa finalizado com sucesso!!!\n");
        *parada = 0;
        return;
    }

    setSignal(controle, estadoControle, regIR->inst.opcode, regIR->inst.funct);

    printf("\n════════════════════════════════════════════");
    printf("\n ********* INÍCIO DO CLOCK *********");
    printf("\n════════════════════════════════════════════");
    printf("\n->PC: [%d] |",*pc);
    printf("->Instrução em IR: [%s]", regIR->inst.assembly);
    printf("\n-> Estado controlador: [%d]", *estadoControle);
    //imprimeControle(controle);
    //printf("\n════════════════════════════════════════════");
    //printf("\n-> Registradores (antes do ciclo):");
    imprimeBanco(bancoReg);
    printf("════════════════════════════════════════════");

    // Instrução
    if (controle->IREsc) {
        mux = criaMux(*pc, *regSaidaUla, 0, controle->IouD);
        int enderecoInstrucao = muxFuncition(mux);
        novaInstrucao = buscaInstrucao(memInst, enderecoInstrucao);
    }

    // Registradores
    mux = criaMux(regIR->inst.rt, regIR->inst.rd, 0, controle->RegDst);
    regDest = muxFuncition(mux);
    buscaReg = buscaBancoRegs(bancoReg, regIR->inst.rs, regIR->inst.rt, regDest);
 

    // ULA
    Mux* mux1 = criaMux(*pc, *RegA, 0, controle->ULAFonteA);
    fonte1 = muxFuncition(mux1);
    Mux* mux2 = criaMux(*RegB, 1, regIR->inst.imm, controle->ULAFonteB);
    fonte2 = muxFuncition(mux2);
    printf("\nFontes ULA -> 1:[%d] - 2:[%d]", fonte1, fonte2);  

    resultadoULA = processamentoULA(fonte1, fonte2, controle->ULAControle);
    SaidaULA = resultadoULA[0];
    printf("\nResultado ULA -> Calc:[%d] - Over:[%d] - Comp:[%d]", resultadoULA[0], resultadoULA[1], resultadoULA[2]); 
    // Memória 
    insereDadosMem(memInst, *regSaidaUla, *RegB, controle->EscMem); // 
    MDR = getDado(memInst, regIR->inst.imm); // MDR do clock atual para usar no prox.

    // registrador 
    mux = criaMux(*regSaidaUla, regMDR->dado, 0, controle->MemParaReg);
    dataWrite = muxFuncition(mux);
    salvaDadoReg(bancoReg, dataWrite, regDest, controle->EscReg);

    // Incrementa PC
    Mux *muxPC = criaMux(resultadoULA[0], *regSaidaUla, regIR->inst.addr, controle->PCFonte);
    if ((controle->branch && resultadoULA[2]) || controle->PCEsc) {
        *pc = muxFuncition(muxPC);
    }

    // Atualiza registradores intermediarios
    atualizaIR(regIR, novaInstrucao, controle->IREsc);
    atualizaMDR(regMDR, MDR);
    *regSaidaUla = SaidaULA;
    *RegA = buscaReg[0];
    *RegB = buscaReg[1];

    nextState(estadoControle, regIR->inst.opcode, regIR->inst.funct);

    printf("\n════════════════════════════════════════════");
    printf("\n ********* Fim do CLOCK *********");
    printf("\n════════════════════════════════════════════");
    printf("\n->PC: [%d] |",*pc);
    printf("->Instrução em IR: [%s]\n", regIR->inst.assembly);
    printf("->Prox estado controlador [%d]", *estadoControle);
    //printf("\n════════════════════════════════════════════");
    //printf("\n-> Registradores (Depois do ciclo):");
    imprimeBanco(bancoReg);
    printf("════════════════════════════════════════════");

}
