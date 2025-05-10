typedef struct multiplexador Mux;


struct multiplexador {

    int input1;
    int input2;
    int input3;
    int selection;

};

Mux* criaMux(int input1, int input2, int input3, int selection); // Aloca um novo mux
int muxFuncition(Mux* mux); // Funcionamento do mux