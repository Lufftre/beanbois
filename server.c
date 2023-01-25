#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ws.h>
#include "main.h"

#define MAX_PLAYERS 10
ws_cli_conn_t *clients[MAX_PLAYERS];
char *addresses[MAX_PLAYERS];
char *names[MAX_PLAYERS];
int nclients = 0;

void printPlayers()
{
    for (size_t i = 0; i < MAX_PLAYERS; i++)
    {
        printf("%d %s %s\n", i, addresses[i], names[i]);
    }
}

JoinMessage joinMessage;
void onopen(ws_cli_conn_t *client)
{
    char *cli;
    cli = ws_getaddress(client);
    printf("Connection opened, addr: %s\n", cli);

    for (size_t i = 0; i < MAX_PLAYERS; i++)
    {
        if (clients[i] == NULL)
        {
            clients[i] = client;
            addresses[i] = cli;
            nclients++;
            break;
        }
    }
}

void onclose(ws_cli_conn_t *client)
{
    char *cli;
    cli = ws_getaddress(client);
    printf("Connection closed, addr: %s\n", cli);
    for (size_t i = 0; i < MAX_PLAYERS; i++)
    {
        if (client == clients[i])
        {
            joinMessage.op = PLAYER_LEFT;
            strcpy(joinMessage.name, names[i]);
            ws_sendframe_bin(NULL, (void *)&joinMessage, sizeof(JoinMessage));

            addresses[i] = NULL;
            clients[i] = NULL;
            free(names[i]);
            names[i] = NULL;
            nclients--;
            break;
        }
    }
}

void onmessage(ws_cli_conn_t *client,
               const unsigned char *msg, uint64_t size, int type)
{
    char *cli;
    cli = ws_getaddress(client);
    printf("I receive a message: %s (%zu), from: %s\n", msg,
           size, cli);

    Message *m = (Message *)msg;
    printf("message op: %d, name %s\n", m->op);
    if (m->op == JOIN)
    {
        JoinMessage *m = (JoinMessage *)msg;
        printf("Player %s has joined\n", m->name);

        for (size_t i = 0; i < MAX_PLAYERS; i++)
        {
            if (names[i] != NULL)
            {
                joinMessage.op = PLAYER_JOIN;
                strcpy(joinMessage.name, names[i]);
                ws_sendframe_bin(client, (void *)&joinMessage, sizeof(JoinMessage));
            }
        }

        for (size_t i = 0; i < MAX_PLAYERS; i++)
        {
            if (client == clients[i])
            {
                char *name = malloc(sizeof(m->name));
                strcpy(name, m->name);
                names[i] = name;
                break;
            }
        }

        printPlayers();
        joinMessage.op = PLAYER_JOIN;
        strcpy(joinMessage.name, m->name);
        ws_sendframe_bin(NULL, (void *)&joinMessage, sizeof(JoinMessage));
    }
    else if (m->op == START_GAME)
    {
        ws_sendframe_bin(NULL, (void *)m, sizeof(Message));
    }
}

int main(void)
{
    struct ws_events evs;
    evs.onopen = &onopen;
    evs.onclose = &onclose;
    evs.onmessage = &onmessage;

    ws_socket(&evs, 8100, 0, 1000);
    return (0);
}
