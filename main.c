#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>  // Biblioteca do windows para a função Sleep

#define MAX_CARDS 11
#define DEALER_MAX_CARDS 5    // O dealer só pode ter no máximo 5 cartas
#define MAX_DEALER_POINTS 17  // O dealer só pode ter no máximo 17 pontos

char cards[13] = {'A', '2', '3', '4', '5', '6', '7',
                  '8', '9', 'D', 'J', 'Q', 'K'};  // D = 10, por ser caractere,
                                                  // não dava pra por '10'

struct p {
    char name[20];          // Nome do jogador
    int type;               // 1 - humano, 2 - computador
    char cards[MAX_CARDS];  // Cartas do jogador
    int quantity;           // Quantidade de cartas do jogador
    int stopped;            // 1 - parou, 0 - não parou
    int points;             // Pontos do jogador
} typedef Player;

struct j {
    int jogador;            // Jogador que fez retirou uma carta
    char cards[MAX_CARDS];  // Cartas que o jogador tinha antes de retirar uma
    char retrievedCard;     // Carta que o jogador retirou
    struct j *next;         // Ponteiro para a próx jogada
    struct j *prev;         // Ponteiro para a jogada anterior
} typedef Play;

struct js {
    Play *first;  // Ponteiro para a primeira jogada
    Play *last;   // Ponteiro para a última jogada
} typedef Plays;

/* Função que mostra mensagens de boas vindas, inicializa a Lista, define os
 nomes de jogadores

 @param plays Lista de jogadas
 @param quantity Quantidade de jogadas a serem retornadas
 @param direction Direção da busca
*/
Plays startup(Player players[2]);

/* Função para seguir o jogo no modo player vs player

*/
void pvp(Plays *plays, Player players[2]);

/* Função para seguir o jogo no modo player vs computador

*/
void pvc(Plays *plays, Player players[2]);

/* Função que retorna as jogadas realizadas, limitadas pela quantidade desejada
 * e com duas direções possíveis, do primeiro em diente, ou do último para trás

 @param plays Lista de jogadas
 @param quantity Quantidade de jogadas a serem retornadas
 @param direction Direção da busca
 */
void showPlays(Plays plays, int quantity, int direction);

void showHand(Player player);

void getCard(Player *player);

int main() {
    Player players[2];
    Plays *plays = malloc(sizeof(Plays));
    *plays = startup(players);  // Inicializando e preparando terreno
    if (players[1].type == 1) {
        pvp(plays, players);
    } else {
        pvc(plays, players);
    }
    return 0;
}

Plays startup(Player players[2]) {
    printf("----------------------------------------------\n");
    printf("|      Bem vindo ao jogo de BlackJack!       |\n");
    printf("|--------------------------------------------|\n");
    printf("|  Feito por: Lucas Gardini e Gabriel Luiz   |\n");
    printf("----------------------------------------------\n\n");

    // Inicializando a lista de jogadas
    Plays plays;
    plays.first = NULL;
    plays.last = NULL;

    // Obtendo informação do(s) jogador(es)
    printf("Qual o tipo de jogo?\n");
    printf("1 - Jogador vs Jogador\n2 - Jogador vs Computador\n> ");
    scanf("%d", &players[1].type);

    printf("\nDigite o nome do jogador 1\n> ");
    scanf("%s", &players[0].name);
    players[0].type = 1;

    if (players[1].type == 1) {
        printf("\nDigite o nome do jogador 2\n> ");
        scanf("%s", &players[1].name);
    } else {
        strcpy(players[1].name, "Computador");
    }

    printf("\n| %s VS %s |\n", players[0].name, players[1].name);

    Sleep(1250);

    printf("\nEmbaralhando cartas");
    for (int i = 0; i < 3; i++) {
        printf(".");
        Sleep(1000);
    }

    printf("\n\nDistribuindo cartas");
    for (int i = 0; i < 3; i++) {
        printf(".");
        Sleep(1000);
    }

    players[0].cards[0] = cards[rand() % 13];
    players[0].cards[1] = cards[rand() % 13];
    players[0].quantity = 2;

    players[1].cards[0] = cards[rand() % 13];
    players[1].cards[1] = cards[rand() % 13];
    players[1].quantity = 2;

    system("cls");

    return plays;
}

void showPlays(Plays plays, int quantity, int direction) {
    if (direction == 0) {
        Play *current = plays.first;

        int i = 0;
        while (current != NULL && i < quantity) {
            printf("%d - %s retirou a carta %s e ficou com %s \n", i,
                   current->jogador, current->retrievedCard, current->cards);
            current = current->next;
            i++;
        }
    } else {
        Play *current = plays.last;

        int i = 0;
        while (current != NULL && i < quantity) {
            printf("%d - %s retirou a carta %s e ficou com %s \n", i,
                   current->jogador, current->retrievedCard, current->cards);
            current = current->prev;
            i++;
        }
    }
}

void getCard(Player *player) {
    player->cards[player->quantity] = cards[rand() % 13];
    player->quantity++;
}

void checkPoints(Player *player) {
    int points = 0;
    for (int i = 0; i < player->quantity; i++) {
        if (player->cards[i] == 'A') {
            points += 11;
        } else if (player->cards[i] == 'D' || player->cards[i] == 'J' ||
                   player->cards[i] == 'Q' || player->cards[i] == 'K') {
            points += 10;
        } else {
            points += player->cards[i] - '0';
        }
    }

    if (points > 21) {
        for (int i = 0; i < player->quantity; i++) {
            if (player->cards[i] == 'A') {
                points -= 10;
            }
        }
    }

    if (points > 21) {
        player->stopped = 1;
    }

    player->points = points;

    showHand(*player);
}

void showHand(Player player) {
    int i;
    printf("\t\n------Cartas Player %s------\n", player.name);

    for (i = 0; i < player.quantity; i++) {
        if (player.cards[i] == 'D') {
            printf(
                "---\n"
                "|10|\n"
                "---\n"
                "\n",
                player.cards[i]);
        } else {
            printf(
                "---\n"
                "|%c|\n"
                "---\n"
                "\n",
                player.cards[i]);
        }
    }

    printf("\nTotal: %d", player.points);
}

// Pergunta pra J1 -> Continua? Se não, J1.stopped = 1
// Pergunta pra J2 -> Continua? Se não, J1.stopped = 1
void pvp(Plays *plays, Player players[2]) {
    int opP1, opP2;
    printf("\t\n-----Jogador contra Jogador-----\n\t");
    checkPoints(&players[0]), checkPoints(&players[1]);

    while (players[0].stopped != 1) {
        printf("\nJogador 1 voce deseja puxar outra carta? (1-Sim; 2-Nao)\n> ");
        scanf("%d", &opP1);

        system("cls");

        switch (opP1) {
            case 1:
                getCard(&players[0]);
                break;
            case 2:
                players[0].stopped = 1;
                break;
            case 3:
                showHand(players[0]);
                break;
            default:
                printf("ERRO");
                break;
        }

        checkPoints(&players[0]);
    }

    system("cls");
    checkPoints(&players[1]);
    while (players[1].stopped != 1) {
        printf("\nJogador 2 voce deseja puxar outra carta? (1-Sim; 2-Nao)\n> ");
        scanf("%d", &opP2);

        switch (opP2) {
            case 1:
                getCard(&players[1]);
                break;
            case 2:
                players[1].stopped = 1;
                break;
            case 3:
                showHand(players[1]);
                break;
            default:
                printf("ERRO");
                break;
        }

        checkPoints(&players[1]);
    }

    printf("\n\nPontos jogador 1: %d", players[0].points);
    printf("\n\nPontos jogador 2: %d", players[1].points);
}

void pvc(Plays *plays, Player players[2]) {}
