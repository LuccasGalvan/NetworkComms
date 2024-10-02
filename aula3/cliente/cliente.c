/*=========================== UDP Client =============================================
  Client sends messages to the server and presents the local port given to the socket
======================================================================================*/

#include <winsock.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

#define SERV_HOST_ADDR "127.0.0.1"
#define SERV_UDP_PORT  6000

#define BUFFERSIZE     4096

void Abort(char* msg);

/*________________________________ main _______________________________________
*/

int main(int argc, char* argv[]) {
    SOCKET sockfd;
    int msg_len, iResult, nbytes, i, serv_check_len;
    int ip_idx, port_idx, msg_idx;
    struct sockaddr_in serv_addr, local_addr, serv_check;
    char buffer[BUFFERSIZE];
    WSADATA wsaData;
    int local_addr_len = sizeof(local_addr);  // make sure size is right

    /*========================= SYNTAX TEST =========================*/
    if (argc != 7) {
        fprintf(stderr, "Syntax: %s -msg msg_to_send -ip dest_ip -port dest_port\n", argv[0]);
        getchar();
        exit(EXIT_FAILURE);
    }

    /*=============== BOOTS SOCKETS ==============*/
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        getchar();
        exit(1);
    }

    /*=============== CREATES SOCKET TO SEND/RECEIVE DATAGRAMS ==============*/
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET)
        Abort("Could not create socket");

    /*=========== SEARCHES THROUGH THE FLAGS AND KEEPS ARGUMENT INDEXES ===========*/
    for (i = 1; i < argc ; i++) {
        if (!strcmp(argv[i], "-ip"))
            ip_idx = i + 1;

        if (!strcmp(argv[i], "-port"))
            port_idx = i + 1;
        
        if (!strcmp(argv[i], "-msg"))
            msg_idx = i + 1;
    }

    /*================= FILLS IN SERVER ADDRESS ====================*/
    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; /*Address Family: Internet*/
    serv_addr.sin_addr.s_addr = inet_addr(argv[ip_idx]); /*IP no formato "dotted decimal" => 32 bits*/
    serv_addr.sin_port = htons(argv[port_idx]); /*Host TO Network Short*/

    /*====================== SENDS MESSAGE TO THE SERVER ==================*/
    msg_len = strlen(argv[msg_idx]);
    if (sendto(sockfd, argv[msg_idx], msg_len + 1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
        Abort("The communication subsystem could not find the datagram");

    printf("<CLI1>Message sent. Awaiting response...\n");

    /*===================== OBTAINS AND SHOWS THE LOCAL ADDRESS ====================*/
    // Boots local_addr
    memset((char*)&local_addr, 0, sizeof(local_addr));

    if (getsockname(sockfd, (struct sockaddr*)&local_addr, &local_addr_len) == SOCKET_ERROR)
        Abort("Error obtaining local socket info");

    printf("<CLI1>Local port automatically assigned: %d\n", ntohs(local_addr.sin_port));



    /*====================== RECEIVES MSG FROM SERVER ==================*/
    serv_check_len = sizeof(serv_check);
    nbytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&serv_check, &serv_check_len);
    if (nbytes == SOCKET_ERROR)
        Abort("There was an error when receiving the reply from the server.");

    printf("<CLI1>Received reply: %s\n", buffer);

    buffer[nbytes] = '\0';

    if ((serv_check.sin_port==serv_addr.sin_port) && (!strcmp(inet_ntoa(serv_check.sin_addr), inet_ntoa(serv_addr.sin_addr))))
        printf("Msg came from the server.");
    else
        printf("Msg came from an imposter");

    /*========================= FECHA O SOCKET ===========================*/
    closesocket(sockfd);
    WSACleanup();

    printf("\n");
    getchar();
    exit(EXIT_SUCCESS);
}

/*________________________________ Abort________________________________________
  Shows an error message with its associated code about Winsocks.
  Ends application with exit status 1 (EXIT_FAILURE)
________________________________________________________________________________*/

void Abort(char* msg) {
    fprintf(stderr, "<CLI1>Fatal error: <%s> (%d)\n", msg, WSAGetLastError());
    exit(EXIT_FAILURE);
}
