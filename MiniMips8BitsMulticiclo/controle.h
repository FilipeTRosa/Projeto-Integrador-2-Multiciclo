typedef struct controle CTRL;

struct controle {
    int PCEsc;
    int IouD;
    int EscMem;
    int IREsc;
    int MemParaReg;
    int EscReg;
    int ULAFonteA;
    int ULAFonteB;
    int ULAControle;
    int PCFonte;
    int RegDst;
    int branch;
};


CTRL* criaControle();
//void setSignal(CTRL* control, int opcode, int funct);
void setSignal(CTRL* control, int *State, int opcode, int funct);
void imprimeControle(CTRL *controle);
void nextState(int *state, int opcode, int funct);