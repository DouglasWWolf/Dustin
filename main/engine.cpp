
//=========================================================================================================
// engine.cpp - Implements the engine that handles the incoming UDP packets
//=========================================================================================================
#include "globals.h"


void CEngine::on_incoming_packet(const char* packet, int length)
{
    printf("%s\n", packet);

}