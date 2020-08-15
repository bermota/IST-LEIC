/* Ficheiro: Main.c
   Descricao: Sistema de gestao de eventos desenvolvido em ANSI C. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eventos.h"

/* VARIAVEIS GLOBAIS */

/* Guardamos cada evento numa struct.
   Todos os eventos sao guardados numa array que eh mantida por ordem */
Evento eventos[MAXEVENTOS] = {0};

int num_eventos = 0; /* no inicio nao ha eventos agendados */


int main()
{
    char cmd;
    for (;;) { /* loop "infinito" */
        cmd = getchar();
        (void) getchar(); /* limpar espaco em branco */
        switch (cmd) {
            case 'x': /* o programa termina a pedido do utilizador*/
                return EXIT_SUCCESS;
            case 'a':
                adiciona_evento();
                continue;
            case 'l':
                lista_eventos();
                continue;
            case 's':
                lista_sala();
                continue;
            case 'r':
                remove_evento();
                continue;
            case 'i':
                altera_inicio_evento();
                continue;
            case 't':
                altera_duracao_evento();
                continue;
            case 'm':
                altera_sala_evento();
                continue;
            case 'A':
                adiciona_participante();
                continue;
            case 'R':
                remove_participante();
            default:
                ;
        }
    }
    return EXIT_FAILURE; /* nunca deviamos chegar aqui */
}


int termina_antes(Evento ev1, Evento ev2)
{
    int i;
    for (i=ANO; i <= DIA; ++i) {
        if (ev1.inicio[i] < ev2.inicio[i])
            return true;

        else if (ev1.inicio[i] > ev2.inicio[i])
            return false;
    }
    return ev1.inicio[MIN] +  ev1.dur <= ev2.inicio[MIN];
}


int eventos_sobrepostos(Evento ev1, Evento ev2)
{
    if (strcmp(ev1.descr, ev2.descr) == 0)
        return false; /* so compara eventos distintos */

    else /* um deles tem de terminar antes do outro */
        return !(termina_antes(ev1, ev2) || termina_antes(ev2, ev1));
}


int comeca_antes(Evento ev1, Evento ev2)
{
    int i;
    for (i=0; i <= MIN; ++i) {
        if (ev1.inicio[i] < ev2.inicio[i])
            return true;

        else if (ev1.inicio[i] > ev2.inicio[i])
            return false;
    }
    /* Ambos os eventos comecao no mesmo instante.
       Ordenemo-los por ordem crescente de sala */
    return (ev1.sala < ev2.sala);
}


void insere_evento(Evento ev)
{
    int i;
    for (i = num_eventos; comeca_antes(ev,eventos[i - 1]) && i > 0; --i)
        eventos[i] = eventos[i - 1]; /* shift para a direita para fazer espaco
        para o novo evento */
    
    eventos[i] = ev; /* adicionar o novo elemento na posicao correcta */
    ++num_eventos;

    return ;
}


int sala_livre(Evento ev)
{
    int i;
    for (i=0; i < num_eventos; ++i)
        if (eventos[i].sala == ev.sala && eventos_sobrepostos(ev, eventos[i])) {
            printf("Impossivel agendar evento %s. Sala%d ocupada.\n", ev.descr ,ev.sala);
            return false; /* encontramos um evento sobreposto */
        }

    return true; /* nao ha nenhuma sobreposicao de eventos */
}



int participante_livre(Evento ev, int num_part)
{
    int ev_a_testar, part_a_testar;
    for (ev_a_testar=0; ev_a_testar < num_eventos; ++ev_a_testar)
        if (eventos_sobrepostos(ev, eventos[ev_a_testar]))
            for(part_a_testar=0; part_a_testar < eventos[ev_a_testar].num_part; ++part_a_testar)
                if (strcmp(ev.part[num_part], eventos[ev_a_testar].part[part_a_testar]) == 0)
                    return false;
    return true; /* nao encontramos nenhuma sobreposicao de participantes */
}

int participantes_livres(Evento ev)
{
    int i;
    int res = true;
    for (i = 0; i < ev.num_part; ++i)
        if (!participante_livre(ev, i)) {
            printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n",
                   ev.descr, ev.part[i]);
            res = false;
        }

    return res;
}


Evento le_evento(void)
{
    char linha[MAXLINHA];
    Evento ev = {0};
    int min, hora, num_args;
    fgets(linha, MAXLINHA, stdin);
    num_args = sscanf(linha, "%[^:]:%2d%2d%4d:%2d%2d:%d:%d:%[^:]:%[^:\n]:%[^:\n]:%[^:\n]\n",
            ev.descr, &ev.inicio[DIA], &ev.inicio[MES], &ev.inicio[ANO], &hora, &min, &ev.dur,
            &ev.sala, ev.part[0], ev.part[1], ev.part[2], ev.part[3]);

    /* numero de argumentos lidos antes dos participantes */
    ev.num_part = num_args - 8;

    ev.inicio[MIN] = hora * 60 + min;
    return ev; /* retorna evento com dados preenchidos (ainda nao foi adicionado)*/
}


void imprime_evento(Evento ev)
{
    int i, hora, min;
    hora = ev.inicio[MIN] / 60;
    min  = ev.inicio[MIN] % 60;
    printf("%s %02d%02d%d %02d%02d %d Sala%d %s\n*",
            ev.descr, ev.inicio[DIA], ev.inicio[MES], ev.inicio[ANO],
            hora, min, ev.dur, ev.sala, ev.part[0]);

    /* imprimir cada participante que nao eh responsavel */
    for (i=1; i < ev.num_part; ++i)
        printf(" %s", ev.part[i]);

    putchar('\n');
    return ;
}


void lista_eventos(void)
{
    int i;
    for (i=0; i< num_eventos; ++i)
        imprime_evento(eventos[i]);

    return ;
}


void lista_sala(void)
{
    int i, sala;
    scanf("%d\n", &sala); /* lemos numero da sala e limpamos linebreak */

    for (i=0; i < num_eventos; ++i)
        if (eventos[i].sala == sala) /* se o evento esta na sala pretendida */
            imprime_evento(eventos[i]);

    return ;
}

int evento_valido(Evento ev)
{
    return sala_livre(ev) && participantes_livres(ev);
}


void adiciona_evento(void)
{
    Evento ev = le_evento();

    if (evento_valido(ev))
        insere_evento(ev);

    return ;
}


int apaga_evento(char descr[MAXSTR])
{
    int i, j;
    for (i=0; i < num_eventos; ++i)
        if (strcmp(descr, eventos[i].descr) == 0) {
            for (j = i; j < num_eventos - 1; ++j) {
             /* shift para a esquerda de eventos posteriores */
                eventos[j] = eventos[j+1];
            }
            --num_eventos; /* removemos um evento com sucesso */
            return true;
        }

    return false; /* nao conseguimos encontrar o evento */
}


void remove_evento(void)
{
    char descr[MAXSTR]; /* descricao do evento a apagar */
    scanf("%[^\n]\n", descr); /* descricao do evento a apagar */

    if (!apaga_evento(descr)) /* so nao conseguimos apagar eventos inexistentes */
        printf("Evento %s inexistente.\n", descr);

    return ;
}


int encontra_evento(char descr[MAXSTR])
{
    int i;
    for (i = 0; i < num_eventos; ++i)
        if (strcmp(eventos[i].descr, descr) == 0)
            return i;

    return -1;
}


void altera_inicio_evento(void)
{
    int pos;
    Evento ev; /* a versao alterada*/
    char descr[MAXSTR];
    int hora, min;
    int inicio; /* inicio do novo evento em minutos */

    scanf("%[^:]:%2d%2d\n", descr, &hora, &min);
    inicio = 60*hora + min;

    if ((pos = encontra_evento(descr)) >= 0) { /* se o evento existe */
        ev = eventos[pos]; /* mantemos os atributos originais */
        ev.inicio[MIN] = inicio; /* alterando o inicio */
        
        if (!sala_livre(ev) || !participantes_livres(ev))
            return ; /* nao podemos alterar evento */
        
        else {
            apaga_evento(ev.descr);
            insere_evento(ev);
            return ;
        }
    }

    else {
    printf("Evento %s inexistente.\n", descr); 
    return ;
    }

}

/* para comando 't' */
void altera_duracao_evento(void)
{
    int pos;
    Evento ev = {0}; /* a versao alterada*/
    char descr[MAXSTR];
    int dur; /* duracao do novo evento em minutos */

    scanf("%[^:]:%d\n", descr, &dur);

    if ((pos = encontra_evento(descr)) >= 0) {
        ev = eventos[pos]; /* mantemos os atributos originais */
        ev.dur = dur; /* alterando a duracao */
        
        if (!sala_livre(ev) || !participantes_livres(ev))
            return ; /* nao podemos alterar evento */
        
        else {
            apaga_evento(ev.descr);
            insere_evento(ev);
            return ;
        }
    }

    else {
    printf("Evento %s inexistente.\n", descr); 
    return ;
    }

}


void altera_sala_evento(void)
{
    int pos;
    Evento ev = {0}; /* a versao alterada*/
    char descr[MAXSTR];
    int sala; /* duracao do novo evento em minutos */

    scanf("%[^:]:%d\n", descr, &sala);

    if ((pos = encontra_evento(descr)) >= 0) {
        ev = eventos[pos]; /* mantemos os atributos originais */
        ev.sala = sala; /* alterando a sala */
        
        if (!sala_livre(ev) || !participantes_livres(ev))
            return ; /* nao podemos alterar evento */
        
        else {
            apaga_evento(ev.descr);
            insere_evento(ev);
            return ;
        }
    }

    else {
    printf("Evento %s inexistente.\n", descr); 
    return ;
    }

}


void adiciona_participante(void)
{
    Evento ev = {0};
    int pos;
    int i;
    char descr[MAXSTR], part[MAXSTR];

    char linha[MAXLINHA];
    
    fgets(linha, MAXLINHA, stdin);

    sscanf(linha, "%[^:]:%[^\n]", descr, part);

    if ((pos = encontra_evento(descr)) >= 0) {
        if (eventos[pos].num_part >= 4) { /* incluindo responsavel */
            printf("Impossivel adicionar participante. Evento %s ja tem 3 participantes.\n", descr);
            return ;
        }

        for (i = 0; i < eventos[pos].num_part; ++i) {
            if (strcmp(eventos[pos].part[i], part) == 0) {
                return ; /* o participante ja esta no evento */
            }
        }

        ev = eventos[pos]; /* placeholder para evento modificado */
        strcpy(ev.part[ev.num_part], part);
        ev.num_part += 1;

        if (!participante_livre(ev, ev.num_part - 1)) {
            printf("Impossivel adicionar participante. Participante %s tem um evento sobreposto.\n",
            part);
            return ;
        }
        eventos[pos] = ev; /* conseguimos actualizar evento */
    }

    else {
    printf("Evento %s inexistente.\n", descr); 
    return ;
    }

}


int encontra_participante(Evento ev, char part[MAXSTR])
{
    int i;
    for (i = 0; i < ev.num_part; ++i) {
        if (strcmp(ev.part[i], part) == 0) {
            return i;
        }
    }
    return -1;
}


void remove_participante(void)
{
    int pos; /* posicao do evento na array com todos os eventos */
    int i;
    char descr[MAXSTR], part[MAXSTR];
    char linha[MAXLINHA];
    
    fgets(linha, MAXLINHA, stdin);
    sscanf(linha, "%[^:]:%[^\n]", descr, part);

    if ((pos = encontra_evento(descr)) >= 0) {
        i = encontra_participante(eventos[pos], part); /* posicao do participante a remover */
        if (i < 0) { return ; } /* o participante a remover nao foi encontrado */

        if (eventos[pos].num_part <= 2) {/* incluindo responsavel */
            printf("Impossivel remover participante. Participante %s e o unico participante no evento %s.\n",
            part, descr);
            return ;
        }

        for (;i < eventos[pos].num_part; ++i) {
            strcpy(eventos[pos].part[i], eventos[pos].part[i + 1]); /* arrastar para a esquerda */
        }
        --eventos[pos].num_part;
    }

    else {
    printf("Evento %s inexistente.\n", descr); 
    return ;
    }

}
