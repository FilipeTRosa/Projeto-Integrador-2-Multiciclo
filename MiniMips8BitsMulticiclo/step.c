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
    if (strcmp(regIR->inst.inst_char, "0000000000000000") == 0)
    {
        printf("Programa finalizado com sucesso!!!\n");
        *parada = 0;
    }else{
        
        setSignal(controle, estadoControle,  regIR->inst.opcode , regIR->inst.funct);

        mux = criaMux(*pc, *regSaidaUla, 0, controle->IouD);
        int endIouD = muxFuncition(mux);

        atualizaIR(regIR, buscaInstrucao(memInst, endIouD), controle->IREsc);

        printf("\n ********* Inicio do CLOCK ********* \n");
        printf("->PC: [%d]\n",*pc);
        printf("->Instrução executada: [%s]\n", regIR->inst.assembly);
        printf("->Estado controlador [%d] \n", *estadoControle);
        imprimeControle(controle);
        printf("->Registradores estado antigo");
        imprimeBanco(bancoReg);

        // FALTA ADICIONAR AQUI O REGISTRADOR MDR

        atualizaMDR(regMDR, getDado(memInst, regIR->inst.imm)); // LUGAR DO NULL SERÁ UTILIZADO A FUNÇÃO QUE BUSCA O DADO NA MEMORIA

        mux = criaMux(regIR->inst.rt, regIR->inst.rd, 0, controle->RegDst);
        regDest = muxFuncition(mux);

        buscaReg = buscaBancoRegs(bancoReg, regIR->inst.rs, regIR->inst.rt, regDest);

        *RegA = buscaReg[0];

        *RegB = buscaReg[1];

        mux = criaMux(*pc, *RegA, 0, controle->ULAFonteA);
        fonte1 = muxFuncition(mux);
        printf("Fonte 1 [%d] \n", fonte1);

        Mux* auxMux = criaMux(*RegB, 1, regIR->inst.imm, controle->ULAFonteB);
        fonte2 = muxFuncition(auxMux);
        printf("Fonte 2 [%d] \n", fonte2);

        resultadoULA = processamentoULA(fonte1, fonte2, controle->ULAControle);

        *regSaidaUla = regSaidaULA(resultadoULA[0], 0);
        printf("ULA Resultado [0] ->[%d] \n", resultadoULA[0]);
        printf("ULA Overflow  [1] ->[%d] \n", resultadoULA[1]);
        printf("ULA compara   [2] ->[%d] \n", resultadoULA[2]);
        printf("regULA Saida      ->[%d] \n", *regSaidaUla);


        mux = criaMux(*regSaidaUla, regMDR->dado, 0, controle->MemParaReg);
        int dataWrite = muxFuncition(mux);

        salvaDadoReg(bancoReg, dataWrite, buscaReg[2], controle->EscReg);
     
        mux = criaMux(resultadoULA[0], *regSaidaUla, regIR->inst.addr, controle->PCFonte);
        
        insereDadosMem(memInst,*regSaidaUla,*RegB, controle->EscMem);
        
        if(controle->PCEsc) {
            *pc = muxFuncition(mux);
        }

        nextState(estadoControle, regIR->inst.opcode, regIR->inst.funct);

        printf("\n ********* Fim do CLOCK ********* \n");
        printf("->PC: [%d]\n",*pc);
        printf("->Instrução executada: [%s]\n", regIR->inst.assembly);
        printf("->Estado controlador [%d] \n", *estadoControle);
        printf("->Registradores estado novo");
        imprimeBanco(bancoReg);
    }
}
