//
// Created by ubuntu on 20/06/22.
//

#include "lav_path.h"

int Lav_path::state;

void Lav_path::init()
{
    state = WAIT_PATH;
}