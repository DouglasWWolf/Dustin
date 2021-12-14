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



//=========================================================================================================
// These are the various sorts of packeets we can be sent
//=========================================================================================================
enum
{
    CMD_HEADER  = 0x00,
    PCB0_HEADER = 0x01,
    PCB1_HEADER = 0x02,
    PCB2_HEADER = 0x03,
    PCB3_HEADER = 0x04,
    STAT_HEADER = 0x05
};

// This is a 1 bit for each packet in a full set of frames
const int FRAME_SET = (1 << CMD_HEADER  ) | (1 << PCB0_HEADER) | (1 << PCB1_HEADER)
                    | (1 << PCB2_HEADER ) | (1 << PCB3_HEADER);
//=========================================================================================================


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

    // Packet type is the first byte of the message, and we bump the pointer to the actual data
    int packet_type = *message++;

    // Stuff the packet into the correct buffer
    switch (packet_type)
    {
        case CMD_HEADER:
            memcpy(command_packet, message, sizeof command_packet);
            break;

        case PCB0_HEADER:
            memcpy(data_packet_0, message, sizeof data_packet_0);
            break;

        case PCB1_HEADER:
            memcpy(data_packet_1, message, sizeof data_packet_1);
            break;

        case PCB2_HEADER:
            memcpy(data_packet_2, message, sizeof data_packet_2);
            break;

        case PCB3_HEADER:
            memcpy(data_packet_3, message, sizeof data_packet_3);
            break;

        case STAT_HEADER:
//?            send_status();
            return;

        default:
            printf(">>>> IGNORING UNKNOWN PACKET! <<<<\n");
  //?          send_error();
            return;
    }

    // Keep track of which packets we have received
    m_rcvd_flags |= (1 << packet_type);

    if (m_rcvd_flags & FRAME_SET)
    {
        printf("Full set of frame received\n");
        m_rcvd_flags = 0;
    }
}
//=========================================================================================================
