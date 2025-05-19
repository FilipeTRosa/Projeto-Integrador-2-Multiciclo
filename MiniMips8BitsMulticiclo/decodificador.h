
struct instrucao decodificaInstrucao(struct instrucao inst);
void getOpcode(const char * palavra, char *opcode);
void getRs(const char *palavra, char *rs);
void getRt(const char *palavra, char *rt);
void getRd(const char *palavra, char *rd);
void getFunct(const char *palavra, char *funct);
void getImm(const char *palavra, char *imm);
void getAddr(const char *palavra, char *addr);
void estenderSinalImm(char * imm, char * immExtendido);
char* getNomeFunct(int funct);
char* getNomeOpcode(int opcode);