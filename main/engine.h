#pragma once
//=========================================================================================================
// engine.h - Defines the engine that handles the incoming UDP packets
//=========================================================================================================

class CEngine
{
public:
    // Called once at program startup
    void    init();

    // When a new packet arrives via UDP, this routine gets called
    void    on_incoming_packet(const char* packet, int length);

protected:

    // A set of bitflags with 1 bit per packet type
    int     m_rcvd_flags;

};
