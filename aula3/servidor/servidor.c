/*============================== UDP SERVER ==========================
  This UDP server has the objective of showing the content of received datagrams, and the IP and Ports of origin.
===============================================================================*/

#include <stdio.h>
#include <winsock.h>
#pragma comment (lib, "Ws2_32.lib")

#define SERV_UDP_PORT 6000
#define BUFFERSIZE    4096
#define INET_ADDRSTRLEN 16

void Abort(char* msg);

/*________________________________ main ________________________________________
*/

int main(int argc, char* argv[]) {
    SOCKET sockfd;
    int msg_len, iResult, nbytes;
    struct sockaddr_in serv_addr, client_addr;
    char buffer[BUFFERSIZE];
    WSADATA wsaData;
    int client_addr_len = sizeof(client_addr);

    /*=============== BOOTS WINSOCKS ==============*/
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        getchar();
        exit(1);
    }

    /*============ CREATES SOCKET TO RECEIVE/SEND UDP DATAGRAMS ============*/
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
        Abort("Could not open socket.");

    /*=============== ASSOCIATES SOCKET TO LISTENING ADDRESS ===============*/
    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; /*Address Family: Internet*/
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /*Host TO Network Long*/
    serv_addr.sin_port = htons(SERV_UDP_PORT);  /*Host TO Network Short*/

    /* Associates socket to the desired port */
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
        Abort("Could not associate socket to the port.");

    /*================ INTERACTIVELY AWAITS FOR CLIENTS MESSAGE =============*/
    while (1) {
        fprintf(stderr, "<SER1>Waiting for datagram...\n");

        /*================= RECEIVES CLIENTS MESSAGE ==================*/
        memset(buffer, 0, BUFFERSIZE); // Clears buffer for net message
        nbytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_addr_len);

        if (nbytes == SOCKET_ERROR)
            Abort("Error receiving datagram.");

        /*================= SHOWS ORIGIN LOCATION ==================*/
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);  // Converts IP to string
        int client_port = ntohs(client_addr.sin_port);  // Converts port 

        printf("<SER1>Message received from [%s:%d]: %s\n", client_ip, client_port, buffer);

        /*================= REENVIA A MENSAGEM PARA O CLIENTE ==================*/
        if (sendto(sockfd, buffer, nbytes, 0, (struct sockaddr*)&client_addr, client_addr_len) == SOCKET_ERROR)
            Abort("Error resending the message to the client.");

        printf("<SER1>Message sent back to [%s:%d]\n", client_ip, client_port);
    }
}

/*________________________________ Abort________________________________________
  Shows an error message with its associated code about Winsocks.
  Ends application with exit status 1 (EXIT_FAILURE)
________________________________________________________________________________*/

void Abort(char* msg) {
    WSACleanup();
    fprintf(stderr, "<SERV>Fatal error: <%s> (%d)\n", msg, WSAGetLastError());
    exit(EXIT_FAILURE);
}
