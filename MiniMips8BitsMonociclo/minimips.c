#include "minimips.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


int conversorBinParaDecimal (int compDeDois, char * palavra){
    int decimal = 0;
    int len = strlen(palavra);
    int i, posicao = 0;

    //ASCII//
    // 0 = 48
    // 1 = 49
    // ... //

    if (compDeDois == 1)
    {
        /* complemento de 2 */
        if ((palavra[0] - '0') == 0)
        {
            /* numero positivo */
            return conversorBinParaDecimal(0, palavra);
        }else
        {
            char binario[len + 1]; //no windows reclama dessa linha
            binario[len] = '\0';
            for (int j = 0; j < len ; j++) // for para inverter os bits
            {
                if ((palavra[j] - '0') == 0)
                {
                    binario[j] = '1';

                }else
                {
                    binario[j] = '0';
                }
                
            }
            i = len -1;
            for (i; i>=0; i--)// for para somar 1
            {
                if ((binario[i] - '0') == 1)
                {
                    binario[i] = '0';
                }else
                {
                    binario[i] = '1';
                    break;
                }
            }
                /* numero negativo */
                int valor = conversorBinParaDecimal(0, binario);
                return -valor;
        }
        
    }else
    {
        /* não é complemento de 2 */
        decimal = 0;
        posicao = 0;
        i = len - 1;

        for (i; i>=0; i--)
        {
            int num = palavra[i] - '0';
            decimal += num * pow(2 , posicao);
            posicao++;
        }
    }

    return decimal;
    
}

// retorna uma copia da instrução
struct instrucao buscaInstrucao(struct memoria_instrucao * memoria, int pc){
    struct instrucao inst;    
    if (pc < 0 || pc >= memoria->tamanho) {
        printf("Erro: PC fora do intervalo.\n");
        return inst;  // Retorna uma instrução inválida (inicializada)
    }
    
    return memoria->mem_inst[pc];
}

void carregarDados(const char *nomeArquivo, struct memoria_dados *memDados){
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        return;
    }

    char linha[32];
    int posicao = 0;

    // Inicializa a memória com um valor padrão (0 para indicar "vazio")
    for (int i = 0; i < memDados->tamanho; i++) {
        memDados->mem_dados[i].dado = 0;
        strcpy(memDados->mem_dados[i].dado_char, "vazio");
    }

    while (posicao < memDados->tamanho && fgets(linha, sizeof(linha), arquivo) != NULL) {
        // Remove quebras de linha e espaços extras
        linha[strcspn(linha, "\r\n")] = '\0';

        if (strlen(linha) > 0) {  // Se a linha não estiver vazia
            memDados->mem_dados[posicao].dado = atoi(linha);
            snprintf(memDados->mem_dados[posicao].dado_char, sizeof(memDados->mem_dados[posicao].dado_char), "%d", memDados->mem_dados[posicao].dado);
        }
        posicao++;
    }

    fclose(arquivo);
}

void insereMemDados(struct memoria_dados *mem, int endereco, int valor, int sinalControle){
    if (sinalControle == 1)
    {
        mem->mem_dados[endereco].dado = valor;
    }
}

int getDado(struct memoria_dados *mem, int endereco){
    int valor = mem->mem_dados[endereco].dado;
    return valor;
}

void salvarMemoriaEmArquivo(const char *nomeArquivo, struct memoria_dados *memDados) {
    FILE *arquivo = fopen(nomeArquivo, "w"); // "w" cria ou sobrescreve
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    for (int i = 0; i < memDados->tamanho; i++) {
        fprintf(arquivo, "%d\n", memDados->mem_dados[i].dado);
    }

    fclose(arquivo);
}

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
    int n = 0, posicao = 0;

    while ((caractere = fgetc(arquivoEntrada)) != EOF) {
        if (caractere != '\n') {
            palavra[n] = caractere;
            n++;
        } else {
            if (n > 0) {
                palavra[n] = '\0';  // Coloca \0 no final da string
                strcpy(mem->mem_inst[posicao].inst_char, palavra);
                instrucaoDecodificada = decodificaInstrucao(mem->mem_inst[posicao]);
                mem->mem_inst[posicao].opcode = instrucaoDecodificada.opcode;
                mem->mem_inst[posicao].rs = instrucaoDecodificada.rs;
                mem->mem_inst[posicao].rt = instrucaoDecodificada.rt;
                mem->mem_inst[posicao].rd = instrucaoDecodificada.rd;
                mem->mem_inst[posicao].funct = instrucaoDecodificada.funct;
                mem->mem_inst[posicao].imm = instrucaoDecodificada.imm;
                mem->mem_inst[posicao].addr = instrucaoDecodificada.addr;
                mem->mem_inst[posicao].tipo_inst = instrucaoDecodificada.tipo_inst;
                strcpy(mem->mem_inst[posicao].assembly, instrucaoDecodificada.assembly);
                posicao++;
            }
            n = 0;  // Reseta para a próxima linha
        }
    }

    fclose(arquivoEntrada);    
}

BRegs* alocaBancoRegistradores() {

    BRegs* newBanco = (BRegs *)malloc(sizeof(BRegs));
    newBanco->registradores = NULL;
    newBanco->tamanho = 0;

    return newBanco;
}

regs* criaRegistrador() {

    regs *newReg = (regs *)malloc(sizeof(regs));
    newReg->id = 0;
    newReg->valor = 0;
    newReg->prox = NULL;

    return newReg;
}

void criaBanco(BRegs* bancoRegs, regs* reg){
    if(bancoRegs->registradores == NULL) {
        bancoRegs->registradores = reg;
        bancoRegs->registradores->id = bancoRegs->tamanho;
        bancoRegs->tamanho++;
    }
    else {
        regs *aux = bancoRegs->registradores;

        while(aux->prox != NULL) {
        aux = aux->prox;
        }
        aux->prox = reg;
        aux->prox->id = bancoRegs->tamanho;
        bancoRegs->tamanho++;
    }
}

void imprimeReg(regs* reg) {
      /**printf("====================\n");
      printf("ID: %d\n", reg->id);
      printf("Valor: %d\n", reg->valor);
      printf("====================\n");
    */
    printf("║ $%-2d: %-4d ", reg->id, reg->valor);
}

void imprimeBanco(BRegs* bancoRegs) {
    /**regs *aux = bancoRegs->registradores;

    while(aux->prox != NULL) {
        imprimeReg(aux);
        aux = aux->prox;
    }
        */
        regs *aux = bancoRegs->registradores;
        int contador = 0;
        printf("\n== Banco de Registradores ==\n");
        while (aux != NULL && aux->id <= 7) {
           printf("$%-2d: %-4d  ", aux->id, aux->valor);
           contador++;
   
           if (contador % 4 == 0) {
               printf("\n");
           }
   
           aux = aux->prox;
        }
   
       if (contador % 4 != 0) {
           printf("\n");
       }
       printf("============================\n");
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

void imprimeInstrucao(struct instrucao inst){ 
    printf("Binario: [%s], ASM: [%s], opcode: [%d], rs: [%d], rt: [%d], rd: [%d], funct: [%d], imm: [%d], addr: [%d], tipo: [%s]\n",
        inst.inst_char, inst.assembly, inst.opcode, inst.rs,
        inst.rt, inst.rd, inst.funct,
        inst.imm, inst.addr, imprimeTipo(inst.tipo_inst));
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

void imprimeDado(struct dado dado){
    printf("Valor: [%d]\n", dado.dado);
}

void imprimeMemDados(struct memoria_dados *mem){
    printf("==== Memoria de dados ====\n");
    for (int i = 0; i < mem->tamanho; i++)
    {
        printf("Posicao: [%d], ", i);
        imprimeDado(mem->mem_dados[i]);
    }
    printf("==========================\n");
}

int* buscaBancoRegs(BRegs* bancoRegs, int rs, int rt, int rd, int defDest) {
    
    regs *aux = bancoRegs->registradores;
    int* vetBusca = (int *)malloc(3 * sizeof(int));

    if(defDest == 0) {
        rd = rt;
    }

    while(aux != NULL & aux->id != rs) {
        aux = aux->prox;
    }

    vetBusca[0] = aux->valor;
    aux = bancoRegs->registradores;

    while(aux->id != rt) {
        aux = aux->prox;
    }

    vetBusca[1] = aux->valor;
    aux = bancoRegs->registradores;

    while(aux->id != rd) {
        aux = aux->prox;
    }

    vetBusca[2] = aux->id;

    return vetBusca;
}

int* processamentoULA(int op1, int op2, int ulaOP) {

    int* vetResultadoULA = (int *)malloc(3 * sizeof(int));
    char result[8];
    char operando1[8];
    char operando2[8];

    vetResultadoULA[2] = 0;

    switch(ulaOP) {
        case 0:
        case 1:
        case 3:
        case 7:
            vetResultadoULA[0] = op1 + op2;
            vetResultadoULA[1] = verificaOverflow(vetResultadoULA[0]);
            break;
        case 2:
            vetResultadoULA[0] = op1 - op2;
            vetResultadoULA[1] = verificaOverflow(vetResultadoULA[0]);
            break;
        case 4:
            converteDecimalParaBinario(operando1, op1);
            converteDecimalParaBinario(operando2, op2);

            for(int i = 0; i < 8; i++) {
                if(operando1[i] == operando2[i]) {
                    result[i] = operando1[i];
                }
            }

            vetResultadoULA[0] = conversorBinParaDecimal(0, result);
            vetResultadoULA[1] = verificaOverflow(vetResultadoULA[0]);
            break;
        case 5:
            converteDecimalParaBinario(operando1, 0);
            converteDecimalParaBinario(operando2, 0);

            for(int i = 0; i < 8; i++) {
                if(operando1[i] == 1 || operando2[i] == 1) {
                    result[i] = 1;
                } 
            }

            vetResultadoULA[0] = conversorBinParaDecimal(0, result);
            vetResultadoULA[1] = verificaOverflow(vetResultadoULA[0]);
            break;
        case 6://beq
            vetResultadoULA[2] = comparaRegs(op1, op2);
            break;
    }

    return vetResultadoULA;
}

int fuctionMux(int op1, int op2, int controleULA) {

    if(controleULA == 0) {
        return op1;
    }
    else if(controleULA == 1) {
        return op2;
    }
}

int verificaOverflow(int opResult) {
    int flag = 0;

    if(opResult > 127 || opResult < -128) {
        flag = 1;
    }

    return flag;
}


int comparaRegs(int op1, int op2) {
    
    int flag = 0;

    if((op1 - op2) == 0) {
        flag = 1;
    }

    return flag;
}

void salvaDadoReg(BRegs* bancoRegs, int resultadoULA, int vetBuscaReg, int sinalControle) { 
    if (sinalControle == 1)
    {
        regs *aux = bancoRegs->registradores;

        while(aux->id != vetBuscaReg) {
            aux = aux->prox;
        }

        aux->valor = resultadoULA;
    }
}

struct estatistica * criaStat() {
    struct estatistica * new_stat = (struct estatistica *)malloc(sizeof(struct estatistica));
    new_stat->back = 0;
    new_stat->tipoI = 0;
    new_stat->tipoJ = 0;
    new_stat->tipoR = 0;
    new_stat->totalInstrucoes = 0;

    return new_stat;
}



CTRL* criaControle() {
    CTRL* new_controle = (CTRL *)malloc(sizeof(CTRL));
    new_controle->regDest = 0;
    new_controle->srcB = 0;
    new_controle->memReg = 0;
    new_controle->ulaOP = 0;
    new_controle->memWrite = 0;
    new_controle->regWrite = 0;
    new_controle->branch = 0;

    return new_controle;
}

void setSignal(CTRL* control, int opcode, int funct) {
    
    switch(opcode) {
        case 0: //TIPO R
            if(funct == 0) { //ADD
                control->ulaOP = 0;
            }
            else if(funct == 2) {//SUB
                control->ulaOP = 2;
            }
            else if(funct == 4) {//AND
                control->ulaOP = 4;
            }
            else if (funct == 5) {//OR
                control->ulaOP = 5;
            }

            control->regDest = 1;
            control->srcB = 0;
            control->memReg = 0;
            control->memWrite = 0;
            control->regWrite = 1;
            control->branch = 100000;
            break;
        case 2://jump
            control->regDest = 0;
            control->srcB = 0;
            control->memReg = 0;
            control->memWrite = 0;
            control->regWrite = 0;
            control->branch = 0;
            control->ulaOP = 0;
            
            break;
        case 4: //ADDI
            control->regDest = 0;
            control->srcB = 1;
            control->memReg = 1;
            control->memWrite = 0;
            control->regWrite = 1;
            control->branch = 1000000000;
            control->ulaOP = 1;
            break;
        case 8: //BEQ
            control->regDest = 0;
            control->srcB = 0;
            control->memReg = 0;
            control->memWrite = 0;
            control->regWrite = 0;
            control->branch = 1;
            control->ulaOP = 6;
            break;
        case 11://LW
            control->regDest = 0;
            control->srcB = 1;
            control->memReg = 0;
            control->memWrite = 0;
            control->regWrite = 1;
            control->branch = 0;
            control->ulaOP = 3; //o certo seria 3 mas ta dando conflito na ULA com o case 3

            break;
        case 15://SW
            control->regDest = 0;
            control->srcB = 1;
            control->memReg = 0;
            control->memWrite = 1;
            control->regWrite = 0;
            control->branch = 0;
            control->ulaOP = 7;
            break;
        default:
            printf("\nOpcode nao pertence ao conjunto de instrucoes!!\n");
    }

}

void imprimeControle(CTRL *controle){
    printf("\nControle\n");
    printf("regDest: [%d], srcB: [%d], memReg: [%d], ulaOP: [%d], memWrite: [%d], regWrite: [%d], branch: [%d]\n",
        controle->regDest, controle->srcB, controle->memReg, controle->ulaOP, controle->memWrite, controle->regWrite, controle->branch);
}

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
    /**immExtendido[0] = imm[0];
    immExtendido[1] = imm[1];

    strcpy(immExtendido + 2, imm);

    immExtendido[strlen(imm) + 2 ]= '\0';
    */
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
        sprintf(inst.assembly, "%s $%d, $%d, %d", nomeOp, inst.rt, inst.rs, inst.imm);
        //Fim tipo

    
        //imprimeInstrucao(inst);
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

void inverteString(const char *origem, char *destino) {
    int tamanho = strlen(origem);
    for (int i = 0; i < tamanho; i++) {
        destino[i] = origem[tamanho - 1 - i]; 
    }
    destino[tamanho] = '\0'; 
}

void converteDecimalParaBinario(char * palavra, int num){
    char aux[9];
    int i = 0;
    int negativo = (num < 0); // guada 1 para verdadeiro e 0 parra falso
    //ASCII//
    // 0 = 48
    // 1 = 49
    // ... //
    
    if (negativo) {
        num = -num; // inverte o sinal
    }

    if (num == 0) {
        strcpy(palavra, "00000000");
        return;
    }

    while (num > 0) {
        aux[i++] = (num % 2) + '0'; 
        num /= 2;
    }

    while (i < 8) {
        aux[i++] = '0';
    }

    aux[i] = '\0';

    inverteString(aux, palavra);

    // Se for negativo, aplica complemento de dois
    if (negativo) {
        // inverte os bits
        for (int j = 0; j < 8; j++) {
            palavra[j] = (palavra[j] == '0') ? '1' : '0';
        }

        //Soma 1
        for (int j = 7; j >= 0; j--) {
            if (palavra[j] == '0') {
                palavra[j] = '1';
                break;
            } else {
                palavra[j] = '0';
            }
        }
    }

    palavra[8] = '\0'; // Garante que a string está terminada

}

void imprimeULA(int *resultadoULA){
    printf("Resultado ULA \n");
    printf("Resultado: [%d], Oveflow: [%d], ComparaREG: [%d]\n",
    resultadoULA[0], resultadoULA[1], resultadoULA[2]);
}

void step(int *parada, int *pc, struct memoria_dados *memDados, struct memoria_instrucao *memInst, BRegs *bancoReg, CTRL *controle, descPilha *pilha, struct estatistica * stat){
    int *buscaReg = NULL;
    struct instrucao instBuscada;
    instBuscada = buscaInstrucao(memInst, *pc);
    if (strcmp(instBuscada.inst_char, "0000000000000000") == 0) //condição DEFAULT de parada do programa
    {
        printf("Programa finalizado com sucesso!!!\n");
        *parada = 0;
    }else
    {
        //AQUI COLOCAR O NO DA PILHA
        BRegs* copiaBanco = copiaBancoRegistradores(bancoReg);
        struct memoria_dados* copiaMemDados = copiaMemoriaDados(memDados);
        nodoPilha *newNodo = criaNodo(*pc, copiaBanco, copiaMemDados);
        inserePilha(pilha, newNodo);

        //setando variaveis de funcinamento
        int *vetBusca = NULL;
        int *resultadoULA = NULL;
        int operando2;
        //fim configuração.

        printf("\n ********* Inicio da Instrução ********* \n");
        printf("->PC: [%d]\n",*pc);
        printf("->Instrução executada: [%s]\n", instBuscada.assembly);
        printf("->Registradores estado antigo");
        imprimeBanco(bancoReg);
        if (strlen(instBuscada.inst_char) > 1)
        {
            setSignal(controle, instBuscada.opcode, instBuscada.funct);
            //imprimeControle(controle);
            switch (instBuscada.tipo_inst)
            {
            case tipo_R:
                stat->tipoR++;
                vetBusca = buscaBancoRegs(bancoReg, instBuscada.rs, instBuscada.rt, instBuscada.rd, controle->regDest); // retorna [[rs][rt][rd]]
                operando2 = fuctionMux(vetBusca[1], instBuscada.imm, controle->srcB);// Mux para saber de onde vem o op2 da ula ->> REG/IMM <<-
                resultadoULA = processamentoULA(vetBusca[0], operando2, controle->ulaOP);// retorna [[resultado][overflow][comparaREG]]
                //testar flag antes
                salvaDadoReg(bancoReg,resultadoULA[0], vetBusca[2], controle->regWrite);
                printf("->Registradores estado novo");
                imprimeBanco(bancoReg);
                *pc = *pc + 1;
                break;
            case tipo_I:
                stat->tipoI++;
                if (instBuscada.opcode == 8) //beq
                {
                    vetBusca = buscaBancoRegs(bancoReg, instBuscada.rs, instBuscada.rt, instBuscada.rd, controle->regDest); // retorna [[rs][rt][rd]]
                    operando2 = fuctionMux(vetBusca[1], instBuscada.imm, controle->srcB);// Mux para saber de onde vem o op2 da ula ->> REG/IMM <<-
                    resultadoULA = processamentoULA(vetBusca[0], operando2, controle->ulaOP);// retorna [[resultado][overflow][comparaREG]]
                    if (resultadoULA[2] == 1 && controle->branch == 1)
                    {
                        *pc = *pc + instBuscada.imm + 1;
                        break;
                    }else
                    {
                        *pc = *pc + 1;
                        break;
                    }    
                }
                //teste addi
                vetBusca = buscaBancoRegs(bancoReg, instBuscada.rs, instBuscada.rt, instBuscada.rd, controle->regDest); // retorna [[rs][rt][rd]]
                operando2 = fuctionMux(vetBusca[1], instBuscada.imm, controle->srcB);// Mux para saber de onde vem o op2 da ula ->> REG/IMM <<-
                resultadoULA = processamentoULA(vetBusca[0], operando2, controle->ulaOP);// retorna [[resultado][overflow][comparaREG]]
                if (controle->memReg == 1 && controle->regWrite == 1) 
                {
                    //se for ADDI
                    salvaDadoReg(bancoReg,resultadoULA[0], vetBusca[2], controle->regWrite);
                }
                if (controle->memReg == 0 && controle->regWrite == 1)
                {
                    //Se for LW
                    salvaDadoReg(bancoReg, getDado(memDados, resultadoULA[0]), vetBusca[2], controle->regWrite);
                }
                printf("->Registradores estado novo"); 
                imprimeBanco(bancoReg);
                //se for SW só salva na mem se memWrite = 1
                insereMemDados(memDados, resultadoULA[0], vetBusca[1], controle->memWrite);
                *pc = *pc + 1;
                break;
            case tipo_J:
                stat->tipoJ++;
                *pc = 0 + instBuscada.addr;
                break;
            default:
                break;
            }
            printf(" ********* FIM da Instrução ********* \n\n");
            stat->totalInstrucoes++;
        }else
        {
            printf("\n Instrucao invalida.\n");
        }
    }
}

descPilha* criarPilha() {
    descPilha* new_pilha = (descPilha *)malloc(sizeof(descPilha));

    new_pilha->instrucoes = NULL;
    new_pilha->tamanho = 0;

    return new_pilha;
}

void printStack(descPilha * pilha){
    nodoPilha *aux = pilha->instrucoes;
    while (aux != NULL)
    {
        imprimeBanco(aux->bancoRegs);
        aux = aux->prox;
    }
    
}


void inserePilha(descPilha* pilha, nodoPilha* nodo) {
    
    if(pilha->tamanho == 0) {
        pilha->instrucoes = nodo;
        pilha->tamanho++;
    }
    else {
        nodoPilha *aux = pilha->instrucoes;
        pilha->instrucoes = nodo;
        nodo->prox = aux;
        pilha->tamanho++;
    }
}

nodoPilha* removePilha(descPilha* pilha) {

    nodoPilha *aux = pilha->instrucoes;

    if(pilha->tamanho == 0) {
        printf("\nNenhuma instrucao foi executada!\n");
    }
    else {
        pilha->instrucoes = aux->prox;
        pilha->tamanho--;
    }

    return aux;
}

nodoPilha* criaNodo(int pc, BRegs* bancoRegs, memDados* memoriaDados) {
    
    nodoPilha *new_nodo = (nodoPilha *)malloc(sizeof(nodoPilha));

    new_nodo->bancoRegs = bancoRegs;
    new_nodo->pc = pc;
    new_nodo->memoriaDados = memoriaDados;
    new_nodo->prox = NULL;

    return new_nodo;
}

struct memoria_dados* copiaMemoriaDados(struct memoria_dados* memoriaDados) {
    struct memoria_dados *newMemoria = (struct memoria_dados *)malloc(sizeof(struct memoria_dados));
    newMemoria->mem_dados = (struct dado *)malloc(memoriaDados->tamanho * sizeof(struct dado));

    for(int i = 0; i < memoriaDados->tamanho; i++) {
        newMemoria->mem_dados[i] = memoriaDados->mem_dados[i];
    }

    return newMemoria;
}

BRegs* copiaBancoRegistradores(BRegs* bancoRegs) {
    
    BRegs *newBanco = (BRegs *)malloc(sizeof(BRegs));
    newBanco->registradores = NULL;
    newBanco->tamanho = 0;

    regs *aux = bancoRegs->registradores;
    regs *copiaAux = NULL;

    while(aux->prox != NULL) {
        copiaAux = criaRegistrador();
        copiaAux->valor = aux->valor;
        criaBanco(newBanco, copiaAux);
        aux = aux->prox;
    }

    return newBanco;
}

void imprimeLogNoTerminal(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de log: %s\n", nomeArquivo);
        return;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        printf("%s", linha);
    }

    fclose(arquivo);
}

void imprimeEstatistica(struct estatistica * est) {
    printf("===== Estatísticas da Execução =====\n");
    printf("Total de Instruções: %d\n", est->totalInstrucoes);
    printf("Tipo R: %d\n", est->tipoR);
    printf("Tipo I: %d\n", est->tipoI);
    printf("Tipo J: %d\n", est->tipoJ);
    printf("Back: %d\n", est->back);
    printf("Total Back + Instruções: %d \n", (est->back + est->totalInstrucoes));
    printf("====================================\n");
}