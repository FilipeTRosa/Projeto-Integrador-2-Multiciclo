#include "controle.h"
#include "memoria.h"
#include "decodificador.h"
#include "multiplexadores.h"
#include "minimips.h"
#include "step.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>



void salvarAsm(const char *nomeArquivo, struct memoria_instrucao *memInst) {
    FILE *arquivo = fopen(nomeArquivo, "w"); // "w" cria ou sobrescreve
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    for (int i = 0; i < memInst->tamanho; i++) {
        fprintf(arquivo, "%s\n", memInst->mem_inst[i].assembly);
    }

    fclose(arquivo);
}

void carregarInstrucoes(const char *nomeArquivo, struct memoria_instrucao *mem){
    struct instrucao instrucaoDecodificada;
    FILE *arquivoEntrada = fopen(nomeArquivo, "r");  
    if (!arquivoEntrada) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        return;
    }

    char caractere;
    char palavra[17];  // Tamanho instrução + \0
    int n = 0, posicao = 0, instCount = 1, dado;
    int modoDados = 0;

    while ((caractere = fgetc(arquivoEntrada)) != EOF) {
        if (caractere != '\n') {
            palavra[n] = caractere;
            n++;
        } else {
            if (n > 0) {
                palavra[n] = '\0';
                //int lengPalavra = strlen(palavra);
                if (strcmp(palavra, ".data") == 0) //
                {   
                     for (; posicao < 128; posicao++) { // Preencher ate a posição 127 com instruções vazias
                        strcpy(mem->mem_inst[posicao].inst_char, "0000000000000000");
                        mem->mem_inst[posicao].tipo_mem = tipo_instrucao;
                        mem->mem_inst[posicao].opcode = -1; // Instrução nula
                        mem->mem_inst[posicao].instCount = instCount++;
                    }
                    modoDados = 1;
                }else if(modoDados){//  seção de dados
                    int len = strlen(palavra);
                    //const char *ultimos8 = (len > 8) ? palavra + (len - 8) : palavra;
                    char ultimos8[9];
                    ultimos8[8] = '\0';
                    int j = 9;
                    for (int i = 0; i < 8; i++)
                     {
                         ultimos8[i] = palavra[j];
                        j++;
                     }
                    dado = conversorBinParaDecimal(1, ultimos8);
                    strcpy(mem->mem_inst[posicao].inst_char, palavra);
                    mem->mem_inst[posicao].dado = dado;
                    mem->mem_inst[posicao].tipo_mem = tipo_dado;
                    
                    posicao++;
                }else{//seção de instruções
                    palavra[n] = '\0';  // Coloca \0 no final da string
                    strcpy(mem->mem_inst[posicao].inst_char, palavra);
                    instrucaoDecodificada = decodificaInstrucao(mem->mem_inst[posicao]);
                    mem->mem_inst[posicao].tipo_mem = tipo_instrucao;
                    mem->mem_inst[posicao].instCount = instCount;
                    mem->mem_inst[posicao].opcode = instrucaoDecodificada.opcode;
                    mem->mem_inst[posicao].rs = instrucaoDecodificada.rs;
                    mem->mem_inst[posicao].rt = instrucaoDecodificada.rt;
                    mem->mem_inst[posicao].rd = instrucaoDecodificada.rd;
                    mem->mem_inst[posicao].funct = instrucaoDecodificada.funct;
                    mem->mem_inst[posicao].imm = instrucaoDecodificada.imm;
                    mem->mem_inst[posicao].addr = instrucaoDecodificada.addr;
                    mem->mem_inst[posicao].dado = 0;
                    mem->mem_inst[posicao].tipo_inst = instrucaoDecodificada.tipo_inst;
                    strcpy(mem->mem_inst[posicao].assembly, instrucaoDecodificada.assembly);
                    posicao++;
                    instCount++;
                }
            }
            n = 0;  // Reseta para a próxima linha
        }
    }
      if (modoDados) {
        for (; posicao < 256; posicao++) {
            strcpy(mem->mem_inst[posicao].inst_char, "0000000000000000");
            mem->mem_inst[posicao].dado = 0;
            mem->mem_inst[posicao].tipo_mem = tipo_dado;
        }
    }

    fclose(arquivoEntrada);    
}

void imprimeInstrucao(struct instrucao inst){
    if (inst.tipo_mem == 0)
    {
        printf("Binario: [%s], ASM: [%s], dado: {%d}, opcode: [%d], rs: [%d], rt: [%d], rd: [%d], funct: [%d], imm: [%d], addr: [%d], tipo: [%s], instCount: [%d]\n",
            inst.inst_char, inst.assembly,inst.dado, inst.opcode, inst.rs,
            inst.rt, inst.rd, inst.funct,
            inst.imm, inst.addr, imprimeTipo(inst.tipo_inst), inst.instCount);
    }else
    {
        printf("Binario: [%s], Valor:[%d]\n",
            inst.inst_char, inst.dado);
    }
}

void imprimeMemInstrucoes(struct memoria_instrucao *mem){
    printf("==== Memoria de instruçoes ====\n");
    for (int i = 0; i < mem->tamanho; i++)
    {
        printf("Posicao: [%d], ", i);
        imprimeInstrucao(mem->mem_inst[i]);
    }
    printf("===============================\n");
}

const char* imprimeTipo(enum classe_inst tipo) {
    switch (tipo) {
        case tipo_R:      return "tipo_R";
        case tipo_I:      return "tipo_I";
        case tipo_J:      return "tipo_J";
        case tipo_OUTROS: return "tipo_OUTROS";
        default:          return "DESCONHECIDO";
    }
}

void atualizaIR(RegINST* ir, struct instrucao nova_inst, int sinalControle) {
    if (sinalControle) { 
        ir->inst = nova_inst; 
    }
}

void atualizaMDR(RegMDR *regMDR, int new_dados) {
    
    regMDR->dado = new_dados;
}

RegINST* criaRegIR() {
    RegINST *newReg = (RegINST *)malloc(sizeof(RegINST));
   
    return newReg;
}

RegMDR* criaRegMDR() {
    RegMDR *newReg = (RegMDR *)malloc(sizeof(RegMDR));

    return newReg;
}


void insereDadosMem(struct memoria_instrucao *mem, int endereco, int valor, int sinalControle){
    int desvidoMemoriaInst = 128;
    if (sinalControle == 1)
    {   
        char palavra[17] = "00000000";
        char sufixo[9] = "00000000";
        converteDecimalParaBinario(sufixo, valor);
        strcat(palavra, sufixo);
        strcpy(mem->mem_inst[desvidoMemoriaInst + endereco].inst_char, palavra);
        mem->mem_inst[desvidoMemoriaInst + endereco].dado = valor;
    }
}

int getDado(struct memoria_instrucao *mem, int endereco){
    int desvidoMemoriaInst = 128;
    int valor = mem->mem_inst[desvidoMemoriaInst + endereco].dado;
    return valor;
}

void salvarMemoriaEmArquivo(const char *nomeArquivo, struct memoria_instrucao *mem) {
    FILE *arquivo = fopen(nomeArquivo, "w"); // "w" cria ou sobrescreve
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    for (int i = 0; i < (mem->tamanho); i++) {
        if (i == 127)
        {
            fprintf(arquivo, "%s\n", ".data");
        }else
        {
            fprintf(arquivo, "%s\n", mem->mem_inst[i].inst_char);
        }
    }

    fclose(arquivo);
}
