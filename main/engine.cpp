//=========================================================================================================
// engine.cpp - Implements the engine that handles the incoming UDP packets
//=========================================================================================================
#include "globals.h"

const int FRAME_SIZE = 577;

//=========================================================================================================
// These are the various kinds of packets we can receive
//=========================================================================================================
unsigned char command_packet[8];
unsigned char data_packet_0[FRAME_SIZE * 2];
unsigned char data_packet_1[FRAME_SIZE * 2];
unsigned char data_packet_2[FRAME_SIZE * 2];
unsigned char data_packet_3[FRAME_SIZE * 2];
//=========================================================================================================

enum packet_type_t 
{
    CMD_HEADER  = 0x00,
    PCB0_HEADER = 0x01,
    PCB1_HEADER = 0x02,
    PCB2_HEADER = 0x03,
    PCB3_HEADER = 0x04,
    STAT_HEADER = 0x05
};


//=========================================================================================================
// init() - Called once at program startup
//=========================================================================================================
void CEngine::init()
{
    // Indicate that we have not yet received any packets
    m_rcvd_flags = 0;
}
//=========================================================================================================



//=========================================================================================================
// on_incoming_packet() - Called whenever a UDP packet arrives and needs to be handled
//=========================================================================================================
void CEngine::on_incoming_packet(const char* message, int length)
{
    // Tell the engineer what kind of packet we received
    printf("Rcvd packet type %i\n", *message);

    // Stuff the packet into the correct buffer
    switch (*packet)
    {
        case CMD_HEADER:
            memcpy(command_packet, message+1, sizeof command_packet);
            break;

        case PCB0_HEADER:
            memcpy(data_packet0, message+1, sizeof data_packet0);
            break;

        case PCB1_HEADER:
            memcpy(data_packet1, message+1, sizeof data_packet1);
            break;

        case PCB2_HEADER:
            memcpy(data_packet2, message+1, sizeof data_packet2);
            break;

        case PCB0_HEADER:
            memcpy(data_packet3, message+1, sizeof data_packet3);
            break;

        case STAT_HEADER:
//?            send_status();
            return;

        default:
            printf(">>>> IGNORING UNKNOWN PACKET! <<<<\n");
  //?          send_error();
            continue;
    }

    // Keep track of which packets we have received
    m_rcvd_flags |= (1 << *packet);

    printf("Rcvd %i\n", m_rcvd_flags);
}
//=========================================================================================================
