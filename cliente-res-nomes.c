#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

#define MAX_MSG 1024

/*
	Experimento 02
	
	Cliente resolve nome DNS, para depois conectar no servidor.
	Envia mensagem e imprime resposta recebida

	Cliente conecta em um servidor WEB e envia STRING HTTP para 
	requisitar o documento index da raiz
	string padrao: GET / HTTP/1.1\r\n\r\n
*/

int main(int argc, char *argv[])
{
	// variaveis
	int socket_desc;
	struct sockaddr_in servidor;
	char *mensagem;
	char resposta_servidor[MAX_MSG];
	int tamanho;

	// variaveis para resolver nomes para IPs
	char *hostname = "www.google.com.br";
	char endereco_ip[100];
	struct hostent *he;
	struct in_addr **addr_list;
	int i;

	/*****************************************/
	/* para resolver nomes para IPs */
	if ((he = gethostbyname(hostname)) == NULL)
	{
		herror("Nao foi possivel resolver: ");
		return -1;
	}

	// convertendo h_addr_list para in_addr
	addr_list = (struct in_addr **)he->h_addr_list;

	for (i = 0; addr_list[i] != NULL; i++)
	{
		//Return the first one;
		strcpy(endereco_ip, inet_ntoa(*addr_list[i]));
	}
	printf("%s resolvido para : %s\n", hostname, endereco_ip);
	/*****************************************/

	/*****************************************/
	/* Criando um socket */
	// AF_INET = ARPA INTERNET PROTOCOLS
	// SOCK_STREAM = orientado a conexao
	// 0 = protocolo padrao para o tipo escolhido -- TCP
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_desc == -1)
	{
		printf("Nao foi possivel criar socket\n");
		return -1;
	}

	/* Informacoes para conectar no servidor */
	// IP do servidor
	// familia ARPANET
	// Porta - hton = host to network short (2bytes)
	servidor.sin_addr.s_addr = inet_addr(endereco_ip);
	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(80);

	//Conectando no servidor remoto
	if (connect(socket_desc, (struct sockaddr *)&servidor, sizeof(servidor)) < 0)
	{
		printf("Nao foi possivel conectar\n");
		return -1;
	}
	printf("Conectado no servidor\n");
	/*****************************************/

	/*******COMUNICAO - TROCA DE MENSAGENS **************/

	//Enviando uma mensagem
	mensagem = "GET / HTTP/1.1\r\n\r\n";
	if (send(socket_desc, mensagem, strlen(mensagem), 0) < 0)
	{
		printf("Erro ao enviar mensagem\n");
		return -1;
	}
	puts("Dados enviados\n");

	//Recebendo resposta do servidor
	if ((tamanho = recv(socket_desc, resposta_servidor, MAX_MSG, 0)) < 0)
	{
		printf("Falha ao receber resposta\n");
		return -1;
	}
	printf("Resposta recebida\n");
	resposta_servidor[tamanho] = '\0';
	puts(resposta_servidor);

	/*****************************************/
	close(socket_desc);

	printf("Cliente finalizado com sucesso!\n");
	return 0;
}
