#include "controle.h"
#include "memoria.h"
#include "minimips.h"
#include "step.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "multiplexadores.h"



Mux* criaMux(int input1, int input2, int input3, int selection) {
    
    Mux* newMux = (Mux *)malloc(sizeof(Mux));

    newMux->input1 = input1;
    newMux->input2 = input2;
    newMux->input3 = input3;
    newMux->selection = selection;

    return newMux;
}

int muxFuncition(Mux* mux) {

    if(mux->selection == 0) {
        return mux->input1;
    }
    else if(mux->selection == 1) {
        return mux->input2;
    }
    else if(mux->selection == 2) {
        return mux->input3;
    }

}