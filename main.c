#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>     // Biblioteca para a função time
#include <windows.h>  // Biblioteca do windows para a função Sleep

#define MAX_CARDS 11
#define MAX_DEALER_CARDS 5    // O dealer só pode ter no máximo 5 cartas
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
    int quantity;           // Quantidade de cartas que o jogador tinha antes de
                            // retirar uma
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
 @param direction Direção da busca (0 - do primeiro para o último, 1 - do
 último)
 @param players Vetor de jogadores
 */
void showPlays(Plays *plays, int quantity, int direction, Player players[2]);

/* Função que mostra a mão de um jogador

 @param player Jogador
 */
void showHand(Player player);

/* Função que atribui uma nova carta para um jogador e registra a jogada

 @param playerId Id do jogador
 @param player Jogador
 @param plays Jogadas
 */
void getCard(int playerId, Player *player, Plays *plays);

/* Função que cria uma nova jogada e a adiciona na lista de jogadas

 @param player Jogador
 @param plays Jogadas
 */
void createPlay(Plays *plays, int playerId, Player *player, char retrievedCard);

/* Função que verifica e declara empate ou os ganhadores

 @param players Jogadores
 */
void declareWinner(Plays *plays, Player players[2]);

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

    srand(time(NULL));
    players[0].cards[0] = cards[rand() % 13];
    players[0].quantity = 1;
    createPlay(&plays, 0, &players[0], players[0].cards[0]);

    players[0].cards[1] = cards[rand() % 13];
    players[0].quantity = 2;
    createPlay(&plays, 0, &players[0], players[0].cards[1]);

    srand(time(NULL) * 2);
    players[1].cards[0] = cards[rand() % 13];
    players[1].quantity = 1;
    createPlay(&plays, 1, &players[1], players[1].cards[0]);

    players[1].cards[1] = cards[rand() % 13];
    players[1].quantity = 2;
    createPlay(&plays, 1, &players[1], players[1].cards[1]);

    system("cls");

    return plays;
}

void createPlay(Plays *plays, int playerId, Player *player,
                char retrievedCard) {
    if (plays->first == NULL) {
        plays->first = malloc(sizeof(Play));
        plays->first->jogador = playerId;
        strcpy(plays->first->cards, player->cards);
        plays->first->retrievedCard = retrievedCard;
        plays->first->quantity = player->quantity;
        plays->first->next = NULL;
        plays->first->prev = NULL;
        plays->last = plays->first;
    } else {
        plays->last->next = malloc(sizeof(Play));
        plays->last->next->jogador = playerId;
        strcpy(plays->last->next->cards, player->cards);
        plays->last->next->retrievedCard = retrievedCard;
        plays->last->next->quantity = player->quantity;
        plays->last->next->next = NULL;
        plays->last->next->prev = plays->last;
        plays->last = plays->last->next;
    }
}

void showPlays(Plays *plays, int quantity, int direction, Player players[2]) {
    if (direction == 0) {
        Play *current = plays->first;

        if (quantity != -1) {
            printf("Mostrando as %d ultimas jogadas em ordem crescente\n",
                   quantity);
            int i = 0;
            int j;
            while (current != NULL && i < quantity) {
                printf("Indice: %d\n%s retirou a carta %c e ficou com: ", i,
                       players[current->jogador], current->retrievedCard);

                for (j = 0; j < current->quantity; j++) {
                    printf("%c ", current->cards[j]);
                }
                printf("\n\n");
                i++;

                current = current->next;
            }
        } else {
            printf("Mostrando TODAS as jogadas em ordem crescente\n");
            int i = 0;
            int j;
            while (current != NULL) {
                printf("Indice: %d\n%s retirou a carta %c e ficou com: ", i,
                       players[current->jogador].name, current->retrievedCard);

                for (j = 0; j < current->quantity; j++) {
                    printf("%c ", current->cards[j]);
                }
                printf("\n\n");
                i++;

                current = current->next;
            }
        }
    } else {
        Play *current = plays->last;

        if (quantity != -1) {
            printf("Mostrando as %d ultimas jogadas em ordem decrescente\n",
                   quantity);
            int i = 0;
            int j;
            while (current != NULL && i < quantity) {
                printf("Indice: %d\n%s retirou a carta %c e ficou com: ", i,
                       players[current->jogador].name, current->retrievedCard);

                for (j = 0; j < current->quantity; j++) {
                    printf("%c ", current->cards[j]);
                }
                printf("\n\n");
                i++;

                current = current->prev;
            }
        } else {
            printf("Mostrando TODAS as jogadas em ordem decrescente\n");
            int i = 0;
            int j;
            while (current != NULL) {
                printf("Indice: %d\n%s retirou a carta %c e ficou com: ", i,
                       players[current->jogador].name, current->retrievedCard);

                for (j = 0; j < current->quantity; j++) {
                    printf("%c ", current->cards[j]);
                }
                printf("\n\n");
                i++;

                current = current->prev;
            }
        }
    }
}

void getCard(int playerId, Player *player, Plays *plays) {
    // Gerando carta aleatória e atribuindo ao jogador
    srand(time(NULL));

    char card = cards[rand() % 13];

    player->cards[player->quantity] = card;
    player->quantity++;

    createPlay(plays, playerId, player, card);
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
        printf("%s estourou com %d pontos\n", player->name, points);
        player->stopped = 1;

        Sleep(1250);
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
                "---"
                "|10|"
                "---"
                "",
                player.cards[i]);
        } else {
            printf(
                "---"
                "|%c|"
                "---"
                "",
                player.cards[i]);
        }
    }

    printf("\nTotal: %d\n", player.points);
}

// Pergunta pra J1 -> Continua? Se não, J1.stopped = 1
// Pergunta pra J2 -> Continua? Se não, J1.stopped = 1
void pvp(Plays *plays, Player players[2]) {
    int opP1, opP2;
    printf("\t\n-----Jogador contra Jogador-----\n\t");
    checkPoints(&players[0]), checkPoints(&players[1]);

    while (players[0].stopped != 1) {
        printf(
            "\nJogador 1 voce deseja puxar outra carta? (1-Sim; 2-Nao; "
            "3-Mostrar "
            "Jogadas)\n>");
        scanf("%d", &opP1);

        system("cls");

        switch (opP1) {
            case 1:
                getCard(0, &players[0], plays);
                break;
            case 2:
                players[0].stopped = 1;
                break;
            case 3:
                showPlays(plays, -1, 0, players);
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
        printf(
            "\nJogador 2 voce deseja puxar outra carta? (1-Sim; 2-Nao; "
            "3-Mostrar "
            "Jogadas)\n>");
        scanf("%d", &opP2);

        system("cls");

        switch (opP2) {
            case 1:
                getCard(1, &players[1], plays);
                break;
            case 2:
                players[1].stopped = 1;
                break;
            case 3:
                showPlays(plays, -1, 0, players);
                break;
            default:
                printf("ERRO");
                break;
        }

        checkPoints(&players[1]);
    }

    declareWinner(plays, players);
}

void pvc(Plays *plays, Player players[2]) {
    int op;
    printf("\t\n-----Jogador contra Computador-----\n\t");
    checkPoints(&players[0]), checkPoints(&players[1]);

    while (players[0].stopped != 1) {
        printf(
            "\nJogador voce deseja puxar outra carta? (1-Sim; 2-Nao; 3-Mostrar "
            "Jogadas)\n>");
        scanf("%d", &op);

        system("cls");

        switch (op) {
            case 1:
                getCard(0, &players[0], plays);
                break;
            case 2:
                players[0].stopped = 1;
                break;
            case 3:
                showPlays(plays, -1, 0, players);
                break;
            default:
                printf("ERRO");
                break;
        }

        checkPoints(&players[0]);
    }

    while (players[1].stopped != 1) {
        getCard(1, &players[1], plays);

        checkPoints(&players[1]);
        if (players[1].points >= MAX_DEALER_POINTS ||
            players[1].quantity == MAX_DEALER_CARDS) {
            players[1].stopped = 1;
        }
    }

    declareWinner(plays, players);
}

void declareWinner(Plays *plays, Player players[2]) {
    system("cls");

    printf("\n\nPontos %s: %d", players[0].name, players[0].points);
    printf("\n\nPontos %s: %d\n", players[1].name, players[1].points);

    int J1Bursted = players[0].points > 21;
    int J2Bursted = players[1].points > 21;

    if (J1Bursted && J2Bursted) {
        printf("\n%s e %s estouraram!", players[0].name, players[1].name);

    } else {
        if (J1Bursted && !J2Bursted) {
            printf("\n%s estourou!", players[0].name);
            printf("\n%s venceu!", players[1].name);
        } else if (!J1Bursted && J2Bursted) {
            printf("\n%s estourou!", players[1].name);
            printf("\n%s venceu!", players[0].name);
        } else {
            if (players[0].points == 21 && players[1].points == 21) {
                printf("\n\nOs dois jogadores fizeram 21 pontos!",
                       players[0].name);
            }

            if (players[0].points == 21) {
                printf("\n%s fez 21 pontos!", players[0].name);
            }

            if (players[1].points == 21) {
                printf("\n%s fez 21 pontos!", players[1].name);
            }

            if (players[0].points > players[1].points) {
                printf("\n%s venceu!", players[0].name);
            }

            if (players[1].points > players[0].points) {
                printf("\n%s venceu!", players[1].name);
            }

            if (players[0].points == players[1].points) {
                printf("\n\n%s e %s empataram!", players[0].name,
                       players[1].name);
            }
        }
    }

    int choice;
    printf("\n\nDeseja ver as jogadas? (1-Sim; 2-Nao)\n>");
    scanf("%d", &choice);

    if (choice == 1) {
        int direction;
        printf(
            "\nDeseja ver em ordem crescente ou decrescente? (1-Crescente; "
            "2-Decrescente)\n>");
        scanf("%d", &direction);

        int quantity;
        printf("\nQuantas jogadas deseja ver? (-1 para todas)\n>");
        scanf("%d", &quantity);

        showPlays(plays, quantity, direction - 1, players);

        printf("\nDeseja voltar para alguma jogada? (1-Sim; 2-Nao)\n>");
        scanf("%d", &choice);

        if (choice == 1) {
            int play;
            printf("Qual jogada deseja voltar?\n>");
            scanf("%d", &play);
        }
    }
}