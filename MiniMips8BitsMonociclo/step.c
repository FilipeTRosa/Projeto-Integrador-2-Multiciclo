#include "memoria.h"
#include "multiplexadores.h"
#include "minimips.h"
#include "controle.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "decodificador.h"

void step(int *parada, int *pc, struct memoria_instrucao *memInst, BRegs *bancoReg, CTRL *controle, descPilha *pilha, struct estatistica * stat, int *estadoControle){

    int *buscaReg = NULL;
    int regDest = 0;
    int RegA = 0, RegB = 0;
    int fonte1 = 0, fonte2 = 0;
    int *resultadoULA = NULL;
    Mux* mux = NULL;
    struct instrucao instBuscada;
    instBuscada = buscaInstrucao(memInst, *pc);
    if (strcmp(instBuscada.inst_char, "0000000000000000") == 0) //condição DEFAULT de parada do programa
    {
        printf("Programa finalizado com sucesso!!!\n");
        *parada = 0;

    } else {

        setSignal(controle, instBuscada.opcode, instBuscada.funct, *estadoControle);
        mux = criaMux(instBuscada.rs, instBuscada.rd, 0, controle->RegDst);
        regDest = muxFuncition(mux);

        buscaReg = buscaBancoRegs(bancoReg, instBuscada.rs, instBuscada.rt, regDest);
        RegA = buscaReg[0];
        RegB = buscaReg[1];

        mux = criaMux(*pc, RegA, 0, controle->ULAFonteA);
        fonte1 = muxFuncition(mux);
        mux = criaMux(RegB, 1, 5, controle->ULAFonteB); // No lugar do 5 vai o valor da extensão de sinal
        fonte2 = muxFuncition(mux);

        resultadoULA = processamentoULA(fonte1, fonte2, controle->ULAControle);
    }

        
}


    









    //-> controle(estado, opcode ....);

    //-> atualiza PC ... (Sem sinal controle)

    //-> Mux escolha PC ou Saida ULA (IouD, ...)

    //-> Memoria  escrita ou leitura (EscMem, ...)

    //-> Atualiza RI (IREsc, ...)

    //-> Atualiza RDM .... (sem sinal controle)

    //-> Mux registrador destino (RegDst, ...) 

    //-> Mux memoria-reg (MemParaReg, ...) - do RDM ou Saida ULA

    //-> Acesso banco reg (EscReg, ...)

    //-> Atualiza reg A - (Saida 1 do Breg)

    //-> Atualiza reg B - (Saida 2 do Breg)

    //-> Mux Operando 1 ULA (UlaFonteA, ...) - PC ou Breg

    //-> Mux Operando 2 ULA (UlaFonteB, ...) - Breg ou RI/imm

    //-> ULA (ControleUla, ...)

    //-> Atualiza ULAsaida

    //-> Mux atualizaPC (PCFonte, ...) - ULAsaida ou RI

