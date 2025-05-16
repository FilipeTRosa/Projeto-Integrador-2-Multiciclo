typedef struct RegistradorInstrucao RegINST;

enum classe_inst{
    tipo_R, tipo_I, tipo_J, tipo_OUTROS
    };

enum tipo_mem{
    tipo_instrucao, tipo_dado
};

struct instrucao{
    enum tipo_mem tipo_mem;
    int instCount;
    enum classe_inst tipo_inst;
    char inst_char[17];
    char assembly[50];
    int opcode;
    int rs;
    int rt;
    int rd;
    int funct;
    int imm;
    int addr;
    int dado;
        };
    
struct memoria_instrucao{
    struct instrucao *mem_inst;
    int tamanho;
};

struct RegistradorInstrucao {
    struct instrucao inst;  
};


void carregarInstrucoes(const char *nomeArquivo, struct memoria_instrucao *mem);
void imprimeMemInstrucoes(struct memoria_instrucao *mem);
void imprimeInstrucao(struct instrucao inst);
void salvarAsm(const char *nomeArquivo, struct memoria_instrucao *memInst);
const char* imprimeTipo(enum classe_inst tipo);

// ------------------- DADOS ------------------------

void insereDadosMem(struct memoria_instrucao *mem, int endereco, int valor, int sinalControle);
int getDado(struct memoria_instrucao *mem, int endereco);