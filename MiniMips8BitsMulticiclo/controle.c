#include "memoria.h"
#include "controle.h"
#include "decodificador.h"
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

void nextState(int *state, int opcode, int funct) {
    switch(*state) {
        case 0: // Busca da Instrução
            *state = 1;
            break;
        case 1: // Decodificação e leitura dos REGS RS e RT
            switch(opcode) {
                case 0: // TIPO R
                    *state = 7;
                    break;
                case 2: // JUMP
                    *state = 10;
                    break;
                case 4: // ADDI
                    *state = 2;
                    break;
                case 8: // BEQ
                    *state = 9;
                    break;
                case 11: // LW
                    *state = 2;
                    break;
                case 15: // SW
                    *state = 2;
                    break;
            }
            break;
        case 2: // Cálculo do endereço de acesso à memória / imediato
            switch(opcode) {
                case 4: // ADDI
                    *state = 6;
                    break;
                case 11: // LW
                    *state = 3;
                    break;
                case 15: // SW
                    *state = 5;
                    break;
            }
            break;
        case 3: // Acesso à memória
            *state = 4;
            break;
        case 4: // Escrita no Registrador RT
            *state = 0;
            break;
        case 5: // Acesso à memória
            *state = 0;
            break;
        case 6: // Término da Instrução TIPO I
            *state = 0;
            break;
        case 7: // Execução
            *state = 8;
            break;
        case 8: // Término da Instrução TIPO R
            *state = 0;
            break;
        case 9: // Término do DESVIO CONDICIONAL
            *state = 0;
            break;
        case 10: // Término do DESVIO INCONDICIONAL
            *state = 0;
            break;
    }
}

void setSignal(CTRL* control, int *state, int opcode, int funct) {
    
    switch(*state) {
        case 0: // Busca da Instrução
            control->PCEsc = 1;
            control-> IouD = 0;
            control->EscMem = 0;
            control->IREsc = 1;
            control->MemParaReg = 0;
            control->EscReg = 0;
            control->ULAFonteA = 0;
            control->ULAFonteB = 1;
            control->ULAControle = 0;
            control->PCFonte = 0;
            control->RegDst = 1;
            control->branch = 0;
            break;
        case 1: // Decodificação e leitura dos REGS RS e RT
            control->PCEsc = 0;
            control-> IouD = 0;
            control->EscMem = 0;
            control->IREsc = 0;
            control->MemParaReg = 0;
            control->EscReg = 0;
            control->ULAFonteA = 0;
            control->ULAFonteB = 2;
            control->ULAControle = 0;
            control->PCFonte = 0;
            control->RegDst = 1;
            control->branch = 0;
            break;
        case 2: // Cálculo do endereço de acesso à memória / imediato
            control->PCEsc = 0;
            control-> IouD = 0;
            control->EscMem = 0;
            control->IREsc = 0;
            control->MemParaReg = 0;
            control->EscReg = 0;
            control->ULAFonteA = 1;
            control->ULAFonteB = 2;
            control->ULAControle = 0;
            control->PCFonte = 0;
            control->RegDst = 0;
            control->branch = 0;
            break;
        case 3: // Acesso à memória
            control->PCEsc = 0;
            control-> IouD = 1;
            control->EscMem = 0;
            control->IREsc = 0;
            control->MemParaReg = 0;
            control->EscReg = 0;
            control->ULAFonteA = 1;
            control->ULAFonteB = 2;
            control->ULAControle = 0;
            control->PCFonte = 0;
            control->RegDst = 0;
            control->branch = 0;
            break;
        case 4: // Escrita no Registrador RT
            control->PCEsc = 0;
            control-> IouD = 0;
            control->EscMem = 0;
            control->IREsc = 0;
            control->MemParaReg = 1;
            control->EscReg = 1;
            control->ULAFonteA = 1;
            control->ULAFonteB = 2;
            control->ULAControle = 0;
            control->PCFonte = 0;
            control->RegDst = 0;
            control->branch = 0;
            break;
        case 5: // Acesso à memória
            control->PCEsc = 0;
            control-> IouD = 1;
            control->EscMem = 1;
            control->IREsc = 0;
            control->MemParaReg = 0;
            control->EscReg = 0;
            control->ULAFonteA = 1;
            control->ULAFonteB = 2;
            control->ULAControle = 0;
            control->PCFonte = 0;
            control->RegDst = 0;
            control->branch = 0;
            break;
        case 6: // Término da Instrução TIPO I
            control->PCEsc = 0;
            control-> IouD = 0;
            control->EscMem = 0;
            control->IREsc = 0;
            control->MemParaReg = 0;
            control->EscReg = 1;
            control->ULAFonteA = 1;
            control->ULAFonteB = 2;
            control->ULAControle = 0;
            control->PCFonte = 0;
            control->RegDst = 0;
            control->branch = 0;
            break;
        case 7: // Execução
            control->PCEsc = 0;
            control-> IouD = 0;
            control->EscMem = 0;
            control->IREsc = 0;
            control->MemParaReg = 0;
            control->EscReg = 0;
            control->ULAFonteA = 1;
            control->ULAFonteB = 0;
            control->ULAControle = 0;
            control->PCFonte = 0;
            control->RegDst = 1;
            control->branch = 0;
            switch (funct)
            {
            case 0:
                control->ULAControle = 0;
                break;
            case 2:
                control->ULAControle = 2;
                break;
            case 4:
                control->ULAControle = 4;
                break;
            case 5:
                control->ULAControle = 5;
                break;
            default:
                break;
            }
            break;
        case 8: // Término da Instrução TIPO R
            control->PCEsc = 0;
            control-> IouD = 0;
            control->EscMem = 0;
            control->IREsc = 0;
            control->MemParaReg = 0;
            control->EscReg = 1;
            control->ULAFonteA = 1;
            control->ULAFonteB = 0;
            if (funct == 2)
            {
                control->ULAControle = 2;
            }else
            {
                control->ULAControle = 0;
            }        
            control->PCFonte = 0;
            control->RegDst = 1;
            control->branch = 0;
            break;
        case 9: // Término do DESVIO CONDICIONAL
            control->PCEsc = 0;
            control->IouD = 0;
            control->EscMem = 0;
            control->IREsc = 0;
            control->MemParaReg = 0;
            control->EscReg = 0;
            control->ULAFonteA = 1;
            control->ULAFonteB = 0;
            control->ULAControle = 6;
            control->PCFonte = 1;
            control->RegDst = 0;
            control->branch = 1;
            break;
        case 10: // Término do DESVIO INCONDICIONAL
            control->PCEsc = 1;
            control->IouD = 0;
            control->EscMem = 0;
            control->IREsc = 0;
            control->MemParaReg = 0;
            control->EscReg = 0;
            control->ULAFonteA = 0;
            control->ULAFonteB = 0;
            control->ULAControle = 0;
            control->PCFonte = 2;
            control->RegDst = 0;
            control->branch = 0;
            break;
    }
}

void imprimeControle(CTRL *controle){
    printf("\nControle\n");
    printf("RegDst: [%d], ULAFonteA: [%d], ULAFonteB: [%d], MemParaReg: [%d], ULAControle: [%d], EscMem: [%d]\n",
        controle->RegDst, controle->ULAFonteA, controle->ULAFonteB, controle->MemParaReg, controle->ULAControle, controle->EscMem);
    printf("EscReg: [%d], branch: [%d], IouD: [%d], IREsc: [%d], PCEsc: [%d], PCFonte: [%d]\n",
        controle->EscReg, controle->branch, controle->IouD, controle->IREsc, controle->PCEsc, controle->PCFonte);
}
