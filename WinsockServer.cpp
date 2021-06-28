#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

/* 
O #pragma comment indica que o arquivo Ws2_32.lib é necessário. 
*/
#pragma comment(lib, "Ws2_32.lib")

#define EXIT_MESSAGE ".exit" // Defino a mensagem usada para encerrar a comunicação
#define DEFAULT_PORT 8080
#define MAXIMUM_CONNECTIONS 10
#define BUFFER_LENGTH 4096

int main(int argc, char *argv[]) {
	
	/* 
	Necessário para iniciar o winsock. 
	*/
	WSADATA wsaData;
	
	/* 
	Estruturas para referenciar o servidor e o cliente. 
	Vão conter as informações de cada um.
	*/
	struct sockaddr_in server;
	struct sockaddr_in client;
	
	/* 
	Socket do servidor e do cliente. 
	*/
	SOCKET serversock = INVALID_SOCKET;
	SOCKET clientsock = INVALID_SOCKET;
	
	/*
	Buffer para enviar e receber mensagem
	*/
	char buffer[BUFFER_LENGTH];

	/* 
	Verificando a versão da DLL (2,2), e testando o inicio do meu winsock. 
	*/
	if (WSAStartup(MAKEWORD(2,2), &wsaData) == INVALID_SOCKET) {
	    printf("WSAStartup failed \n");
	    WSACleanup();
	    return 1;
	}
	printf("Winsock iniciado com sucesso...\n\n");
	
	
	/* 
	Iniciando o socket e efetuando um teste. 
	AF_INET 	é usado para referenciar a familia IPv4
	SOCK_STREAM é usado para comunicação TCP
	IPPROTO_IP 	é o tipo de protocolo usado.
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
	memset(&server, 0, sizeof(server)); // Zero a estrutura server usando a função memset
	server.sin_addr.s_addr = htonl(INADDR_ANY); // Informo que o endereço será o localhost "127.0.0.1"
	server.sin_family = AF_INET; // Será utilizado IPv4 na comunicação
	server.sin_port = htons(DEFAULT_PORT); // E que será usada a porta 8080
	
	/* 
	Associando uma porta ao nosso socket criado e efetuando um teste. 
	Na função bind() eu passo o meu socket como parametro, um cast para o tipo sockaddr da minha estrutura server, e o tamanho do mesmo.
	*/
	if(bind(serversock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR){
		printf("Erro ao associar uma porta \n\n");
		WSACleanup();
		closesocket(serversock);
		return 1;
	}
	printf("Porta associada com sucesso...\n\n");
	
	
	/* 
	Colocando o socket para ouvir as conexões usando a função linsten()
	Passo como parametro o socket aberto e a quantidade de conexões permitidas.
	*/
	if(listen(serversock, MAXIMUM_CONNECTIONS) == SOCKET_ERROR){
		printf("Erro ao ouvir conexoes \n");
		WSACleanup();
		closesocket(serversock);
		return 1;
	}
	printf("Iniciado a escuta com sucesso...\n\n");
	
	
	/*
	Agora vamos aceitas as conexões efetuadas pelos clientes usando a função accept(),
	para isso nós vamos adicionar o retorno da função accept ao nosso clientsock, usando como parametro:
	o serversock, um cast para struct sockaddr do nosso client, e o tamanho do nosso clientsock para um ponteiro.
	*/
	
	int clientlen = sizeof(client);
	
	printf("Aguardando alguma conexao...\n");
	
	clientsock = accept(serversock, (struct sockaddr *)&client, &clientlen);
	if(clientsock == SOCKET_ERROR){
		printf("Erro ao aceitar conexoes \n");
		WSACleanup();
		closesocket(serversock);
		return 1;
	}
	printf("Conexao estabelecida com sucesso...\n\n");
	
	
	/*
	Agora vamos guardar em um buffer a mensagem que iremos enviar ao cliente assim que o mesmo se conectar.
	*/
	strcpy(buffer, "Conectado ao servidor!\n");
	
	/*
	Assim que o cliente se conectar, eu envio a mensagem do buffer pra ele e verifico se foi devidamente enviada.
	Caso tenha sido enviada eu entro em um while(true) e aguardo o cliente me retornar alguma mensagem.
	*/
	if(send(clientsock, buffer, strlen(buffer), 0) == SOCKET_ERROR){
		printf("Não foi possivel enviar uma mensagem ao client \n");
		WSACleanup();
		closesocket(serversock);
		return 1;
	}

	printf("Aguardando mensagem do cliente...\n");
	
	int stringlen; // Variavel para receber o tamanho da mensagem
	do{
		if(stringlen = recv(clientsock, buffer, BUFFER_LENGTH, 0) == SOCKET_ERROR){
			printf("Não foi possivel receber a mensagem do client \n");
			closesocket(serversock);
			closesocket(clientsock);
			WSACleanup();
			break;
		}else{
			buffer[stringlen-1] = '\0'; // Acrescenta o \0 para garantir término nulo para a string
			printf("Mensagem recebida: %s\n", buffer);
		}
	}while(strcmp(buffer, EXIT_MESSAGE) != 0);
	
	printf("\n...Servidor encerrado!...\n");
	closesocket(serversock);
	closesocket(clientsock);
	WSACleanup();
	
	return 0;
	
}
