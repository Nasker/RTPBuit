#pragma once

#include "BuitControlChanger.hpp"
#include <vector>
#include "Structs.h"

#define N_BUITS_CC 16

using namespace std;

class MatrixBuitControlChanger{
    vector<BuitControlChanger> controlChangers;
    public:
        MatrixBuitControlChanger();
        void toggleBuitCC(int buitID);
        RTPSequencesState getBuitCCStates();
        void updateAndSend(ControlCommand command);
};