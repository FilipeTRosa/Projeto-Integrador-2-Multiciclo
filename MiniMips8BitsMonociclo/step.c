#include "memoria.h"
#include "multiplexadores.h"
#include "minimips.h"
#include "controle.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "decodificador.h"

void step(int *parada, int *pc, struct memoria_instrucao *memInst, BRegs *bancoReg, CTRL *controle, descPilha *pilha, struct estatistica * stat, int * estadoControle){

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

}