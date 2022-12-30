#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

/* 
O #pragma comment indica que o arquivo Ws2_32.lib � necess�rio. 
*/
#pragma comment(lib, "Ws2_32.lib")

#define EXIT_MESSAGE ".exit" // Defino a mensagem usada para encerrar a comunica��o
#define DEFAULT_PORT 8080
#define BUFFER_LENGTH 4096
#define SERVER_IP "127.0.0.1"

int main(int argc, char *argv[]) {
	char buffer[BUFFER_LENGTH]; // Buffer usado para receber e enviar mensagens
	
	/* 
	Necess�rio para iniciar o winsock. 
	*/
	WSADATA wsaData;
	
	/* 
	Estrutura para referenciar o servidor. 
	V�o conter as informa��es do mesmo.
	*/
	struct sockaddr_in server;
	
	/* 
	Socket do servidor. 
	*/
	SOCKET serversock = INVALID_SOCKET;

	/* 
	Verificando a vers�o da DLL (2,2), e testando o inicio do meu winsock. 
	*/
	if (WSAStartup(MAKEWORD(2,2), &wsaData) == INVALID_SOCKET) {
	    printf("WSAStartup failed \n");
	    WSACleanup();
	    return 1;
	}
	printf("Winsock iniciado com sucesso...\n\n");
	
	
	/* 
	Iniciando o socket e efetuando um teste. 
	AF_INET 	� usado para referenciar a familia IPv4
	SOCK_STREAM � usado para comunica��o TCP
	IPPROTO_IP 	� o tipo de protocolo usado.
	*/
	serversock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if(serversock == INVALID_SOCKET){
		printf("Erro ao iniciar socket \n");
		WSACleanup();
		closesocket(serversock);
		return 1;
	}
	printf("Socket aberto com sucesso...\n\n");
	
	
	/* 
	Preenchendo a estrutura com os dados do servidor. 
	*/
	memset(&server, 0, sizeof(server)); // Zero a estrutura server usando a fun��o memset
	server.sin_addr.s_addr = inet_addr(SERVER_IP); // Informo que o endere�o ser� o localhost "127.0.0.1"
	server.sin_family = AF_INET; // Ser� utilizado IPv4 na comunica��o
	server.sin_port = htons(DEFAULT_PORT); // E que ser� usada a porta 8080

	/*
	Agora eu crio a conex�o com o servidor usando a fun��o connect(),
	tamb�m efetuarei um teste para verificar se a conex�o foi devidamente estabelecida.
	*/
	if(connect(serversock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR){
		printf("Falha na conexao...\n");
		WSACleanup();
		closesocket(serversock);
		return 1;
	}
		printf("Conexao com o servidor estabelecida com sucesso... \n\n");
		
		/*
		Caso a conex�o seja estabelecida com sucesso eu irei receber a mensagem que foi enviada do servidor e printar na tela
		*/
		int stringlen;
		stringlen = recv(serversock, buffer, BUFFER_LENGTH, 0);
		buffer[stringlen] = '\0'; // Acrescenta o \0 para garantir t�rmino nulo para a string
		printf("Mensagem recebida do servidor: %s\n", buffer);
	
	
	/*
	Logo ap�s entrarei com as mensagens que ser�o enviadas ao servidor
	para isso usarei um while true para enviar as mensagens.
	*/
	printf("Digite as mensagens para o servidor: \n\n");
	
	do{
		printf("Mensagem: ");
		fgets(buffer, BUFFER_LENGTH, stdin); // Pego o buffer do teclado
		buffer[strlen(buffer)-1] = '\0';
		
		if(send(serversock, buffer, BUFFER_LENGTH, 0) == SOCKET_ERROR){
			printf("Erro ao enviar a mensagem ao servidor! \n");
			WSACleanup();
			closesocket(serversock);
			break;
		}
		
	}while(strcmp(buffer, EXIT_MESSAGE) != 0);
	
	printf("\n...Cliente encerrado!...\n");
	closesocket(serversock);
	WSACleanup();
	
	return 0;
	
}
