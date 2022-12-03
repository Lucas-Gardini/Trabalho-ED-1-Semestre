#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>     // Biblioteca para a função time
#include <windows.h>  // Biblioteca do windows para a função Sleep

#define MAX_CARDS 11
#define MAX_DEALER_CARDS 5    // O dealer só pode ter no máximo 5 cartas
#define MAX_DEALER_POINTS 17  // O dealer só pode ter no máximo 17 pontos

char cards[13] = {'A', '2', '3', '4', '5', '6', '7',
                  '8', '9', 'D', 'J', 'Q', 'K'};
// D = 10, por ser caractere, não dava pra por '10'

struct p {
    char name[20];          // Nome do jogador
    int type;               // 1 - humano, 2 - computador
    char cards[MAX_CARDS];  // Cartas do jogador
    int quantity;           // Quantidade de cartas do jogador
    int stopped;            // 1 - parou, 0 - não parou
    int points;             // Pontos do jogador
} typedef Player;

struct j {
    int player;             // Jogador que fez retirou uma carta
    char cards[MAX_CARDS];  // Cartas que o jogador tinha antes de retirar uma
    int quantity;           // Quantidade de cartas que o jogador tinha antes de
    // retirar uma
    char retrievedCard;  // Carta que o jogador retirou
    struct j* next;      // Ponteiro para a próx jogada
    struct j* prev;      // Ponteiro para a jogada anterior
} typedef Play;

struct js {
    Play* first;  // Ponteiro para a primeira jogada
    Play* last;   // Ponteiro para a última jogada
    int size;     // Tamanho da lista
} typedef Plays;

/* Função que mostra mensagens de boas vindas, inicializa a Lista, define os
 * nomes de jogadores */
Plays startup(Player players[2]);

// Função para seguir o jogo no modo player vs player
void pvp(Plays* plays, Player players[2]);

// Função para seguir o jogo no modo player vs computador
void pve(Plays* plays, Player players[2]);

/* Função que retorna as jogadas realizadas, limitadas pela quantidade desejada
 */
void showPlays(Plays* plays, int quantity, Player players[2]);

// Função que mostra a mão de um jogador
void showHand(Player player);

// Função que atribui uma nova carta para um jogador e registra a jogada
void getCard(int playerId, Player* player, Plays* plays);

// Função que cria uma nova jogada e a adiciona na lista de jogadas
void createPlay(Plays* plays, int playerId, Player* player, char retrievedCard);

// Função que verifica e declara empate ou os ganhadores
// gameType = 1 - player vs player, 2 - player vs computador
void declareWinner(Plays* plays, Player players[2], int gameType);

// Função que retorna em uma jogada da lista
// gameType = 1 - player vs player, 2 - player vs computador
void rollback(Plays* plays, Player players[2], int rollBackTo, int gameType);

// Função que reseta os jogadores
void resetPlayers(Player players[2]);

int isValidCard(char card);

int main() {
    int i;

    // Alocando memória para os jogadores e jogadas
    Player players[2];
    Plays* plays = malloc(sizeof(Plays));

    *plays = startup(players);  // Inicializando e preparando terreno

    printf("\n\nDistribuindo cartas");
    for (i = 0; i < 3; i++) {
        printf(".");
        Sleep(250);
    }

    // Definindo modo de jogo
    if (players[1].type == 1) {
        pvp(plays, players);
    } else {
        pve(plays, players);
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
    plays.size = 0;

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

    int i;

    printf("\nEmbaralhando cartas");
    for (i = 0; i < 3; i++) {
        printf(".");
        Sleep(1000);
    }

    // Inicializando as mãos dos jogadores
    strcpy(players[0].cards, "");
    strcpy(players[1].cards, "");

    // Inicializando as variáveis de controle
    players[0].quantity = 0;
    players[1].quantity = 0;

    players[0].stopped = 0;
    players[1].stopped = 0;

    players[0].points = 0;
    players[1].points = 0;

    system("cls");

    return plays;
}

void createPlay(Plays* plays, int playerId, Player* player,
                char retrievedCard) {
    // Inicialização e alocação de memoria complexa (dispensa explicação)

    // Caso seja a primeira jogada, aloca e cria a primeira jogada
    if (plays->first == NULL) {
        plays->first = malloc(sizeof(Play));
        plays->first->player = playerId;
        strcpy(plays->first->cards, player->cards);
        plays->first->cards[strlen(plays->first->cards)] = '\0';
        plays->first->retrievedCard = retrievedCard;
        plays->first->quantity = player->quantity;
        plays->first->next = NULL;
        plays->first->prev = NULL;
        plays->last = plays->first;
    } else {
        plays->last->next = malloc(sizeof(Play));
        plays->last->next->player = playerId;
        strcpy(plays->last->next->cards, player->cards);
        plays->last->next->cards[strlen(plays->last->next->cards)] = '\0';
        plays->last->next->retrievedCard = retrievedCard;
        plays->last->next->quantity = player->quantity;
        plays->last->next->next = NULL;
        plays->last->next->prev = plays->last;
        plays->last = plays->last->next;
    }

    plays->size = plays->size + 1;
}

void showPlays(Plays* plays, int quantity, Player players[2]) {
    Play* current = plays->first;

    // Verificando se irá mostrar todas as jogadas, ou limitar a algumas
    if (quantity != -1) {
        printf("\nMostrando as %d primeiras jogadas em ordem crescente\n",
               quantity);
        int i = 0;
        int j;
        while (current != NULL && i < quantity) {
            printf("Indice: %d\n%s retirou a carta %c e ficou com: ", i,
                   players[current->player].name, current->retrievedCard);

            for (j = 0; j < current->quantity; j++) {
                printf("%c ", current->cards[j]);
            }
            printf("\n\n");
            i++;

            current = current->next;
        }
    } else {
        printf("\nMostrando TODAS as jogadas em ordem crescente\n");
        int i = 0;
        int j;
        while (current != NULL) {
            printf("Indice: %d\n%s retirou a carta %c e ficou com: ", i,
                   players[current->player].name, current->retrievedCard);

            for (j = 0; j < current->quantity; j++) {
                printf("%c ", current->cards[j]);
            }
            printf("\n\n");
            i++;

            current = current->next;
        }
    }
}

void getCard(int playerId, Player* player, Plays* plays) {
    // Gerando carta aleatória e atribuindo ao jogador
    srand(time(NULL));

    char card = cards[rand() % 13];

    player->cards[player->quantity] = card;
    player->quantity++;

    createPlay(plays, playerId, player, card);
}

void checkPoints(Player* player) {
    // Verificando os valores das cartas e somando os pontos
    int points = 0;
    int i;
    for (i = 0; i < player->quantity; i++) {
        if (player->cards[i] == 'A') {
            points += 11;
        } else if (player->cards[i] == 'D' || player->cards[i] == 'J' ||
                   player->cards[i] == 'Q' || player->cards[i] == 'K') {
            points += 10;
        } else {
            points += player->cards[i] - '0';
        }
    }

    // Se passou de 21, verifica o valor da carta A (ela vale 11 pontos no
    // início, mas volta a valer 1 no final)
    if (points > 21) {
        for (i = 0; i < player->quantity; i++) {
            if (player->cards[i] == 'A') {
                points -= 10;
            }
        }
    }

    // Se o jogador passou de 21 pontos, ele perde
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
    printf("\t\n------Cartas %s------\n", player.name);

    // Loop pelas cartas do jogador
    for (i = 0; i < player.quantity; i++) {
        if (player.cards[i] == 'D') {  // D = 10
            printf(
                "---\n"
                "|10|\n"
                "---\n",
                player.cards[i]);
        } else {
            printf(
                "---\n"
                "|%c|\n"
                "---\n",
                player.cards[i]);
        }
    }

    printf("\nTotal: %d\n", player.points);
}

// Pergunta pra J1 -> Continua? Se não, J1.stopped = 1
// Pergunta pra J2 -> Continua? Se não, J1.stopped = 1
void pvp(Plays* plays, Player players[2]) {
    int opP1, opP2;
    printf("\t\n-----Jogador contra Jogador-----\n\t");
    checkPoints(&players[0]);

    while (players[0].stopped != 1) {
        if (players[0].quantity == 0) {
            opP1 = 1;
        } else {
            printf(
                "\n%s voce deseja puxar outra carta? (1-Sim; 2-Nao; "
                "3-Mostrar "
                "Jogadas)\n> ",
                players[0].name);
            scanf("%d", &opP1);
        }

        system("cls");

        switch (opP1) {
                // Puxa uma carta
            case 1:
                getCard(0, &players[0], plays);
                break;
                // Para de puxar cartas
            case 2:
                players[0].stopped = 1;
                break;
                // Mostra as jogadas
            case 3:
                showPlays(plays, -1, players);
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
        if (players[1].quantity == 0) {
            opP2 = 1;
        } else {
            printf(
                "\n%s voce deseja puxar outra carta? (1-Sim; 2-Nao; "
                "3-Mostrar "
                "Jogadas)\n> ",
                players[0].name);
            scanf("%d", &opP2);
        }

        system("cls");

        switch (opP2) {
                // Puxa uma carta
            case 1:
                getCard(0, &players[1], plays);
                break;
                // Para de puxar cartas
            case 2:
                players[1].stopped = 1;
                break;
                // Mostra as jogadas
            case 3:
                showPlays(plays, -1, players);
                break;
            default:
                printf("ERRO");
                break;
        }

        checkPoints(&players[1]);
    }

    declareWinner(plays, players, 1);
}

void pve(Plays* plays, Player players[2]) {
    int op;
    printf("\t\n-----Jogador contra Computador-----\n\t");
    checkPoints(&players[0]);

    while (players[0].stopped != 1) {
        if (players[0].quantity == 0) {
            op = 1;
        } else {
            printf(
                "\n%s voce deseja puxar outra carta? (1-Sim; 2-Nao; "
                "3-Mostrar "
                "Jogadas)\n> ",
                players[0].name);
            scanf("%d", &op);
        }

        system("cls");

        switch (op) {
                // Puxa uma carta
            case 1:
                getCard(0, &players[0], plays);
                break;
                // Para de puxar cartas
            case 2:
                players[0].stopped = 1;
                break;
                // Mostra as jogadas
            case 3:
                showPlays(plays, -1, players);
                break;
            default:
                printf("ERRO");
                break;
        }

        checkPoints(&players[0]);
    }

    // Jogadas do computador (automáticas até ele atingir algum dos fatores de
    // parada)
    checkPoints(&players[1]);
    while (players[1].stopped != 1) {
        if (players[1].points >= MAX_DEALER_POINTS ||
            players[1].quantity >= MAX_DEALER_CARDS) {
            players[1].stopped = 1;
        } else {
            getCard(1, &players[1], plays);
        }

        Sleep(1250);

        if (players[1].stopped == 0) {
            checkPoints(&players[1]);
            Sleep(1250);
        }
    }

    declareWinner(plays, players, 2);
}

void declareWinner(Plays* plays, Player players[2], int gameType) {
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
    printf("\n\nDeseja ver as jogadas? (1-Sim; 2-Nao)\n> ");
    scanf("%d", &choice);

    system("cls");

    if (choice == 1) {
        int quantity;
        printf("\nDeseja ver ate qual jogada? (-1 para todas)\n> ");
        scanf("%d", &quantity);

        showPlays(plays, quantity, players);

        printf("\nDeseja voltar para alguma jogada? (1-Sim; 2-Nao)\n> ");
        scanf("%d", &choice);

        if (choice == 1) {
            int play;
            printf("Qual jogada deseja voltar?\n> ");
            scanf("%d", &play);

            rollback(plays, players, play, gameType);
        } else {
            printf("----------------------------------------------\n");
            printf("|             Obrigado por jogar!            |\n");
            printf("|--------------------------------------------|\n");
            printf("|           1 - Retornar | 2 - Sair          |\n");
            printf("----------------------------------------------\n\n");

            scanf("%d", &choice);

            system("cls");

            if (choice == 1) {
                resetPlayers(players);
                main();
            } else {
                exit(0);
            }
        }
    } else {
        printf("----------------------------------------------\n");
        printf("|             Obrigado por jogar!            |\n");
        printf("|--------------------------------------------|\n");
        printf("|           1 - Retornar | 2 - Sair          |\n");
        printf("----------------------------------------------\n\n");

        scanf("%d", &choice);

        system("cls");

        if (choice == 1) {
            resetPlayers(players);
            main();
        } else {
            exit(0);
        }
    }
}

void resetPlayers(Player players[2]) {
    players[0].points = 0;
    players[0].quantity = 0;
    players[0].stopped = 0;
    strcpy(players[0].cards, "");

    players[1].points = 0;
    players[1].quantity = 0;
    players[1].stopped = 0;
    strcpy(players[1].cards, "");
}

void rollback(Plays* plays, Player players[2], int rollBackTo, int gameType) {
    // Inicializando as mãos dos jogadores
    strcpy(players[0].cards, "");
    strcpy(players[1].cards, "");

    // Resetando variáveis, para serem repostas pela posição de retorno desejada
    players[0].cards[0] = '\0';
    players[1].cards[0] = '\0';

    players[0].stopped = 0;
    players[1].stopped = 0;

    players[0].points = 0;
    players[1].points = 0;

    int i;

    Play* current = plays->first;

    if (rollBackTo <= 0) {
        // Resetando para o primeiro estado
        int cardsQuantity = current->quantity;

        players[0].cards[0] = current->cards[0];
        players[0].quantity = 1;

        strcpy(players[1].cards, "");
        players[1].quantity = 0;
    } else {
        // Loopando por todas as jogadas e refazendo a mão do jogador
        for (i = -1; i < rollBackTo - 1; i++) {
            if (current == NULL) continue;

            if (current->player == 0) {
                int cardsQuantity = current->quantity;
                char playerCards[MAX_CARDS];

                strcpy(players[0].cards, "");
                players[1].quantity = 0;

                // Recriando a mão do jogador a partir da jogada
                int k = 0;
                for (k; k < cardsQuantity; k++) {
                    players[0].cards[k] = current->cards[k];
                    players[0].quantity = k + 1;
                }

                k++;

                players[0].cards[k] = current->retrievedCard;
                players[0].quantity = k;

                // players[0].cards[k] = '\0';
            } else {
                int cardsQuantity = current->quantity;
                char playerCards[MAX_CARDS];

                strcpy(players[1].cards, "");
                players[1].quantity = 0;

                int k = 0;
                for (k; k < cardsQuantity; k++) {
                    players[1].cards[k] = current->cards[k];
                    players[1].quantity = k + 1;
                }

                k++;

                players[1].cards[k] = current->retrievedCard;
                players[1].quantity = k;

                // players[1].cards[k] = '\0';
            }

            current = current->next;
        }
    }

    // Verificando os pontos restantes
    // checkPoints(&players[0]);
    // checkPoints(&players[1]);

    // Liberando memória das jogadas que não serão mais utilizadas
    Play* last = plays->last;
    Play* prev = last->prev;
    while (prev != current) {
        prev = last->prev;
        free(last);
        last = prev;
    }

    // Definindo a jogada desejada como a última
    current->next = NULL;
    plays->last = current;
    plays->size = rollBackTo;

    // Reiniciando o jogo
    if (gameType == 1) {
        pvp(plays, players);
    } else {
        pve(plays, players);
    }
}

// Verficando se não veio lixo de memória
int isValidCard(char card) {
    if (card == 'A' || card == '2' || card == '3' || card == '4' ||
        card == '5' || card == '6' || card == '7' || card == '8' ||
        card == '9' || card == 'D' || card == 'J' || card == 'Q' ||
        card == 'K') {
        return 1;
    } else {
        printf("invalida-");
        return 0;
    }
}