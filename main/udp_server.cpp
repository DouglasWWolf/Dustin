
//=========================================================================================================
// udp_server.cpp - Implements a UDP message server task
//=========================================================================================================
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/param.h>
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "esp_system.h"

#include "udp_server.h"
#include "common.h"
#include "parser.h"
#include "globals.h"
#include "history.h"

// An identifier that is used by the logging facility
static const char *TAG = "udp_server";

// This is the UDP port number that we're listening on
const int SERVER_PORT = 1000;

// This is contains the information about the server socket
static struct sockaddr_in6 sock_desc;

// When we receive a UDP message, this will contain the IP address of the sender
static struct sockaddr_in6 source_addr; 

// This contains the socket descriptor of the socket when it's open
static int  sock = -1;

// This is incoming UDP message
static char input[1000];


//========================================================================================================= 
// hard_shutdown() - Ensures that the listening socket and the server socket are closed
//========================================================================================================= 
static void hard_shutdown()
{
    // Shut down the server socket
    if (sock != -1)
    {
        shutdown(sock, 0);
        close(sock);
        sock = -1;
    }

}
//========================================================================================================= 


//========================================================================================================= 
// recv_udp_message() - Fetches a UDP datagram from the network
//
// On Exit: input[]     = The message, nul terminated
//          source_addr = The IP address of the sender
//========================================================================================================= 
static void recv_udp_message()
{
    while (true)
    {
        // Wait for a message to arrive
        socklen_t socklen = sizeof(source_addr);
        int len = recvfrom(sock, input, sizeof(input) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

        // If we received a message, nul-terminate the string and return to the caller
        if (len >= 0)
        {
            printf("%s\n", input);
            input[len] = 0;
            return;
        }
        

        // Log the fact that we somehow got a corrupted message
        ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
    }

}
//========================================================================================================= 



//========================================================================================================= 
//  task - provides a continuously running UDP server that listens for messages and responds to them.
//========================================================================================================= 
void CUDPServer::task()
{
    
    // We're going to treat "sock_desc" as though it were a "sock_addr_in"
    sockaddr_in& sockaddr = *(sockaddr_in*)&sock_desc;

    // Configure the socket descriptor
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(SERVER_PORT);

    // Create the socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return;
    }

    // Bind the socket to our interface using the socket descriptor we created
    int err = bind(sock, (struct sockaddr *)&sock_desc, sizeof(sock_desc));
    if (err < 0)
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        return;
    }

    // Tell the engineer that the socket is built and we're ready for incoming data
    printf(">>>> Waiting for incoming UDP messages <<<<\n");

    // We're going to do this forever...
    while (true)
    {
        // Fetch a UDP message
        recv_udp_message();

        // Call the appropriate command handler
        printf("Got a packet!\n");
    }
}
//========================================================================================================= 



//=========================================================================================================
// launch_task() - Calls the "task()" routine in the specified object
//
// Passed: *pvParameters points to the object that we want to use to run the task
//=========================================================================================================
static void launch_task(void *pvParameters)
{
    // Fetch a pointer to the object that is going to run out task
    CUDPServer* p_object = (CUDPServer*) pvParameters;
    
    // And run the task for that object!
    p_object->task();
}
//=========================================================================================================



//=========================================================================================================
// start() - Starts the UDP server task
//=========================================================================================================
void CUDPServer::begin()
{
    xTaskCreatePinnedToCore(launch_task, "udp_server", 4096, this, DEFAULT_TASK_PRI, &m_task_handle, TASK_CPU);

    // The server is running!
    m_is_running = true;
}
//=========================================================================================================



//=========================================================================================================
// stop() - Stops the UDP server task
//=========================================================================================================
void CUDPServer::stop()
{
    // Kill the task if it's running
    if (m_is_running)
    {
        vTaskDelete(m_task_handle);
        m_is_running = false;
    }

    // And close the socket if it's open
    hard_shutdown();
}
//=========================================================================================================
