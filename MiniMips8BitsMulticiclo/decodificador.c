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




void getOpcode(const char *palavra, char *opcode){
    strncpy(opcode, palavra + 0, 4);
    opcode[4] = '\0';
}

void getRs(const char *palavra, char *rs){
    strncpy(rs, palavra + 4, 3);
    rs[3] = '\0';
}

void getRt(const char *palavra, char *rt){
    strncpy(rt, palavra + 7, 3);
    rt[3] = '\0';
}

void getRd(const char *palavra, char *rd){
    strncpy(rd, palavra + 10, 3);
    rd[3] = '\0';
}

void getFunct(const char *palavra, char *funct){
    strncpy(funct, palavra + 13, 3);
    funct[3] = '\0';
}

void getImm(const char *palavra, char *imm){
    strncpy(imm, palavra + 10, 6);
    imm[6] = '\0';
}

void getAddr(const char *palavra, char *addr){
    strncpy(addr, palavra + 8, 8);
    addr[9] = '\0';
}

void estenderSinalImm(char * imm, char * immExtendido){

   int len = strlen(imm);
   immExtendido[0] = imm[0];
   immExtendido[1] = imm[0];
   strcpy(immExtendido + 2, imm);
   immExtendido[len + 2] = '\0';

}

char* getNomeFunct(int funct) {
    switch(funct) {
        case 0: return "add";
        case 2: return "sub";
        case 4: return "and";
        case 5: return "or";
    }
}

char* getNomeOpcode(int opcode) {
    switch(opcode) {
        case 4: return "addi";
        case 11: return "lw";
        case 15: return "sw";
        case 8: return "beq";
        case 2: return "j";
    }
}

struct instrucao decodificaInstrucao(struct instrucao inst){
    
    //struct instrucao instruc;
    char palavra[17];
    strcpy(palavra, inst.inst_char);

    //Variaveis da instrucao
    char assembly[50];
    char opcode[4];
    char rs[4];
    char rt[4];
    char funct[4];
    char rd[4];
    char imm[6];
    char addr[9];
    char immExtendido[9];

    //Opcode
    getOpcode(inst.inst_char, opcode);
    inst.opcode = conversorBinParaDecimal(0,opcode);
    //Fim do opcode
    
    if (inst.opcode == 0)
    {
        // TIPO R
        
        //RS
        getRs(inst.inst_char,rs);
        inst.rs = conversorBinParaDecimal(0,rs); //coloca o rs na instrucao
        //Fim RS
        
        //RT
        getRt(inst.inst_char, rt);
        inst.rt = conversorBinParaDecimal(0,rt);
        //fim RT

        //RD
        getRd(inst.inst_char, rd);
        inst.rd = conversorBinParaDecimal(0,rd);
        //fim RD

        //Funct
        getFunct(inst.inst_char, funct);
        inst.funct = conversorBinParaDecimal(0,funct);
        //fim Funct
        
        //Tipo instrucao
        inst.tipo_inst = tipo_R;

        //pegar nome funct e montar assemblyu
        char *nomeFunc = getNomeFunct(inst.funct);
        sprintf(inst.assembly, "%s $%d, $%d, $%d", nomeFunc, inst.rd, inst.rs, inst.rt);
        //Fim tipo
        //imprimeInstrucao(inst);
    }else if(inst.opcode == 4 || inst.opcode == 11 || inst.opcode == 15 || inst.opcode == 8)
    {
        // TIPO I
        
        //RS
        getRs(inst.inst_char,rs);
        inst.rs = conversorBinParaDecimal(0,rs); //coloca o rs na instrucao
        //Fim RS
        
        //RT
        getRt(inst.inst_char, rt);
        inst.rt = conversorBinParaDecimal(0,rt);
        //fim RT

        //imm
        getImm(inst.inst_char, imm);
        //printf("\n Imm antes %s \n", imm);
        estenderSinalImm(imm, immExtendido);
        //printf("\n Imm depois %s \n", immExtendido);
        inst.imm = conversorBinParaDecimal(1,immExtendido); //complemento de 2
        //Fim imm

        //Tipo instrucao
        inst.tipo_inst = tipo_I;

        //pegar nome opcode e montar assemblyu
        char *nomeOp = getNomeOpcode(inst.opcode);
        if (inst.opcode == 11 || inst.opcode == 15)
        {
            sprintf(inst.assembly, "%s $%d, %d($%d)", nomeOp, inst.rt,  inst.imm,inst.rs);
        }else
        {
            sprintf(inst.assembly, "%s $%d, $%d, %d", nomeOp, inst.rt, inst.rs, inst.imm);

        }

    }
    if (inst.opcode == 2)
    {
        //printf("Entrou no tipo J");
        // TIPO J
        //Addr
        getAddr(inst.inst_char, addr);
        inst.addr = conversorBinParaDecimal(0,addr);
        //Fim Addr
        
        //Tipo instrucao
        inst.tipo_inst = tipo_J;
        //assembly
        sprintf(inst.assembly, "j %d", inst.addr);

        //Fim tipo
    }
    
    return inst;
    
}
