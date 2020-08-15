/* Ficheiro: eventos.h
   Descricao: Header file com prototipos de
   funcoes utilizados no programa e struct para o evento.
   */


#define true 1
#define false 0


#define MAXEVENTOS 1000

#define MAXSTR 64
#define MAXPART 4 /* incluindo responsavel pelo evento */

#define MAXLINHA 512 /* numero maximo de caracteres numa linha */




#define ANO 0
#define MES 1
#define DIA 2
#define MIN 3


typedef struct evento {
    char descr[MAXSTR];
    int inicio[4]; /* ano, mes, dia, inicio em minutos desde a meia noite */
    int dur; /* duracao em minutos */
    int sala; /* numero da sala eh um inteiro entre 1 e 10 */
    int num_part; /* numero de participantes incluindo organizador */

    char part[MAXPART][MAXSTR]; /* responsavel e' o primeiro participante */ 
} Evento;



/* Retorna true sse ev1 termina antes de ev2.
    E' util para testar sobreposicao de eventos. */
int termina_antes(Evento ev1, Evento ev2);


/* Indica se dois eventos DISTINCTOS estao sobrepostos
 * Nota: Um evento nunca esta sobreposto com ele proprio */
int eventos_sobrepostos(Evento ev1, Evento ev2);


/* Chave utilizada para ordenacao de eventos.
   Pretendemos ordenar primeiro os eventos cronologicamente,
   depois por numero de sala crescente.
   Como nao ha eventos sobrepostos basta
   compar a data e hora de inicio dos eventos.
   
   Retorna true sse ev1 ser apresentado antes de ev2, 
   valido para os comandos l e s.*/
int comeca_antes(Evento ev1, Evento ev2);


/*Insere um evento na nossa array ordenada de eventos.
  Incrementa num_eventos. */
void insere_evento(Evento ev);


/* Retorna true sse nao teremos um conflito por sala ocupada ao adicionarmos
o Evento ev.
Imprime mensagem de erro para o utilizador. */
int sala_livre(Evento ev);


/* Dado um evento e o numero de um participante (0 eh responsavel pelo evento)
 * esta funcao vai indicar se esse participante ja esta presente noutros
 * eventos sobrepostos */
int participante_livre(Evento ev, int num_part);


/* Retorna true sse todos os participantes estiveram livres.
Gera mensagens de erro para o utilizador */
int participantes_livres(Evento ev);


/* Le um evento do input do utilizador utilizando scanf.

Nota: guardamos o numero da sala do evento como um inteiro entre 0 e 9

A expressao %[^:] indica ao scanf a classe de caracteres que nao sao ':',
ou seja, ler ate encontrar o caracter ':'.

A expressao %[^:\n] eh semelhante mas garante que nao lemos um linebreak
No fim colocamos em \n para limpar este caracter do stdin */
Evento le_evento(void);


/* Dado um evento, esta funcao imprime-o no formato especificado pelo enunciado.
   Nota: O numero da sala eh guardado como um inteiro entre 1 e 10. */
void imprime_evento(Evento ev);


/* Indica se um Evento ev seria valido se fosse agendado.
 Devolve true sse puder se agendado.*/
int evento_valido(Evento ev);


/* Dada uma descricao retorna a a posicao na array de eventos onde
o evento se encontra. Caso nao exista retorna -1. */
int encontra_evento(char descr[MAXSTR]);


/* Indica o indice de um participante num evento em ev.part
retorna -1 caso deste participante nao participe no evento. */
int encontra_participante(Evento ev, char part[MAXSTR]);


/* Dada a descricao de um evento, vai tentar remove-lo.
Se existir ele eh removido e a funcao retorna true,
caso contrario retorna false. */
int apaga_evento(char descr[MAXSTR]);




/* Funcoes utilizadas directamente pelos comandos */

/* Para o comando 'a'.
   Pede input ao utilizador e insere novo evento se possivel
   de acordo com as especificacoes do enunciado. */
void adiciona_evento(void);


/* Para o comando 'l'.
   Vai listar eventos de todas as salas utilizando imprime_evento */
void lista_eventos(void);


/* Para o comando 's'.
Le o input do utilizador e imprime todos os eventos da sala pedida */
void lista_sala(void);


/* Para o comando 'r'.*/
void remove_evento(void);


/* para o comando 'i'. */
void altera_inicio_evento(void);


/* para o comando 't'. */
void altera_duracao_evento(void);


/* para o comando 'm'. */
void altera_sala_evento(void);


/* para o comando 'A'. */
void adiciona_participante(void);


/* Indica o indice de um participante num evento em ev.part
retorna -1 caso deste participante nao participe no evento */
int encontra_participante(Evento ev, char part[MAXSTR]);


/* Para o comando 'R'. */
void remove_participante(void);