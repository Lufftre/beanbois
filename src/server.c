#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ws.h"
#include "main.h"

ws_cli_conn_t *clients[5];
int nclients = 0;

void broadcast(char msg[])
{
    for (size_t i = 0; i < nclients; i++)
    {
        ws_sendframe_txt(clients[i], msg);
    }
}

void onopen(ws_cli_conn_t *client)
{
    char *cli;
    cli = ws_getaddress(client);
    printf("Connection opened, addr: %s\n", cli);
    clients[nclients] = client;
    nclients++;
}

void onclose(ws_cli_conn_t *client)
{
    char *cli;
    cli = ws_getaddress(client);
    printf("Connection closed, addr: %s\n", cli);
}

void onmessage(ws_cli_conn_t *client,
    const unsigned char *msg, uint64_t size, int type)
{
    char *cli;
    cli = ws_getaddress(client);
    printf("I receive a message: %s (%zu), from: %s\n", msg,
        size, cli);
    
    Message *m = (Message*)msg;
    printf("message op: %d, name %s\n", m->op, m->name);
    broadcast(m->name);



}

int main(void)
{
	ws_socket(&(struct ws_server){
		.host = "0.0.0.0",
		.port = 8100,
		.thread_loop   = 0,
		.timeout_ms    = 1000,
		.evs.onopen    = &onopen,
		.evs.onclose   = &onclose,
		.evs.onmessage = &onmessage
	});
    return (0);
}
