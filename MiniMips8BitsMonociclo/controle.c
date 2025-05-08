#include "controle.h"
#include "decodificador.h"
#include "memoria.h"
#include "multiplexadores.h"
#include "minimips.h"
#include "step.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


CTRL* criaControle() {
    CTRL* new_controle = (CTRL *)malloc(sizeof(CTRL));
    
    new_controle->branch = 0;
    new_controle->EscMem = 0;
    new_controle->IouD = 0;
    new_controle->EscReg = 0;
    new_controle->IREsc = 0;
    new_controle->MemParaReg = 0;
    new_controle->PCEsc = 0;
    new_controle->PCFonte = 0;
    new_controle->RegDst = 0;
    new_controle->ULAControle = 0;
    new_controle->ULAFonteA = 0;
    new_controle->ULAFonteB = 0;

    return new_controle;
}

void setSignal(CTRL* control, int opcode, int funct, int *nextState) {
    
    switch(*nextState) {
        case 0: // Busca da Instrução
            control->EscMem = 0;
            control->ULAFonteA = 0;
            control->IouD = 0;
            control->IREsc = 1;
            control->ULAFonteB = 1;
            control->ULAControle = 0;
            control->PCEsc = 1;
            control->PCFonte = 0;
            control->RegDst = 1;
            *nextState = 1;
            break;
        case 1: // Decodificação e leitura dos REGS RS e RT
            control->ULAFonteA = 0;
            control->ULAFonteB = 2;
            control->ULAControle = 0;
            control->RegDst = 1;
            switch(opcode) {
                case 0: // TIPO R
                    if(funct == 0) { //ADD
                        control->ULAControle = 0;
                    }
                    else if(funct == 2) { // SUB
                        control->ULAControle = 2;
                    }
                    else if(funct == 4) { // AND
                        control->ULAControle = 4;
                    }
                    else if(funct == 5) { // OR
                        control->ULAControle = 5;
                    }
                    
                    *nextState = 7;
                    break;
                case 2: // JUMP
                    *nextState = 10;
                    break;
                case 4: // ADDI
                    *nextState = 2;
                    break;
                case 8: // BEQ
                    *nextState = 9;
                    break;
                case 11: // LW
                    *nextState = 2;
                    break;
                case 15: // SW
                    *nextState = 2;
                    break;
            }
            break;
        case 2: // Cálculo do endereço de acesso à memória / imediato
            control->ULAFonteA = 1;
            control->ULAFonteB = 2;
            control->ULAControle = 0;
            switch(opcode) {
                case 4: // ADDI
                    *nextState = 6;
                    break;
                case 11: // LW
                    *nextState = 3;
                    break;
                case 15: // SW
                    *nextState = 5;
                    break;
            }
            break;
        case 3: // Acesso à memória
            control->EscMem = 0;
            control->IouD = 1;
            control->ULAFonteA = 1;
            control->ULAFonteB = 2;

            *nextState = 4;
            break;
        case 4: // Escrita no Registrador RT
            control->EscReg = 1;
            control->MemParaReg = 1;
            control->RegDst = 0;
            control->ULAFonteA = 1;
            control->ULAFonteB = 2;

            *nextState = 0;
            break;
        case 5: // Acesso à memória
            control->EscMem = 1;
            control->IouD = 1;
            control->ULAFonteA = 1;
            control->ULAFonteB = 2;

            *nextState = 0;
            break;
        case 6: // Término da Instrução TIPO I
            control->EscMem = 0;
            control->EscReg = 1;
            control->RegDst = 0;
            control->MemParaReg = 0;
            control->ULAFonteA = 1;
            control->ULAFonteB = 2;

            *nextState = 0;
            break;
        case 7: // Execução
            control->ULAFonteA = 1;
            control->ULAFonteB = 0;
            control->RegDst = 1;

            *nextState = 8;
            break;
        case 8: // Término da Instrução TIPO R
            control->RegDst = 1;
            control->EscReg = 1;
            control->MemParaReg = 0;

            *nextState = 0;
            break;
        case 9: // Término do DESVIO CONDICIONAL
            control->ULAFonteA = 1;
            control->ULAFonteB = 2;
            control->ULAControle = 2;
            control->branch = 1;
            control->PCEsc = 0;
            control->PCFonte = 1;

            *nextState = 0;
            break;
        case 10: // Término do DESVIO INCONDICIONAL
            control->PCEsc = 1;
            control->PCFonte = 2;

            *nextState = 0;
            break;
    }
}
