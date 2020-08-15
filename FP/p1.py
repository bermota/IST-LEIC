def destroi(tab):
    """destroi: tabuleiro --> lista
    Converte tuplo de tuplos (tuplos) para lista.
    Similar 'a funcao alisa
    """
    return [qubit for celula in tab for qubit in celula]


def mutavel(tab):
    """mutavel: tabuleiro (tuplo com 3 tuplos)
    --> tabuleiro (lista com 3 listas)

    Facilita manipulacoes diretas de elementos do tabuleiro
    """
    return [list(celula) for celula in tab]


def imutavel(tab):
    """Torna o tabuleiro imutavel. Funcao inversa de mutavel

    imutavel: 'tabuleiro' (lista de listas) --> tabuleiro (tuplo de tuplos)
    """
    return tuple([tuple(celula) for celula in tab])


def eh_tabuleiro(tab):
    """eh_tabuleiro: universal --> booleano
    Verifica se tab e' tabuleiro.

    Apenas funciona porque python eh lazy a avaliar condicoes
    e avalia-as da esquerda para a direita e de cima para baixo
    """
    return isinstance(tab, tuple)\
           and len(tab) == 3\
           and all(isinstance(celula, tuple) for celula in tab)\
           and len(tab[0]) == len(tab[1]) == 3\
           and len(tab[2]) == 2\
           and all(qubit in (-1, 0, 1) for qubit in destroi(tab))


def representa_qubit(qubit):
    """representa_qubit: inteiro --> caracter (string)

    Converte qubit para a sua representacao grafica,
    Funcao invocada por tabuleiro_str
    """
    if qubit == -1:
        return "x"

    return qubit


def tabuleiro_str(tabuleiro):
    """tabuleiro_str: tabuleiro --> cad. caracteres
    Representacao grafica do tabuleiro.

    Essencialmente substitui '.' por representacoes graficas
    de pontos do tabuleiro"""

    if not eh_tabuleiro(tabuleiro):
        raise ValueError("tabuleiro_str: argumento invalido")

    qubits = tuple(map(representa_qubit, destroi(tabuleiro)))

    return """+-------+
|...{2}...|
|..{1}.{5}..|
|.{0}.{4}.{7}.|
|..{3}.{6}..|
+-------+""".format(*qubits)  # * faz unpacking do tuplo com qubits


def tabuleiros_iguais(tab1, tab2):
    """tabuleiros_iguais: tabuleiro x tabuleiro --> booleano

    Indica se os dois tabuleiros dos argumentos sao iguais,
    validando o seu input
    """
    if not eh_tabuleiro(tab1) or not eh_tabuleiro(tab2):
        raise ValueError("tabuleiros_iguais: um dos argumentos nao e tabuleiro")

    else:
        return tab1 == tab2  # nao precisamos de validar elemento a elemento


def simetrico(x):
    """simetrico: inteiro --> inteiro
    Nega um qubit

    i.e. inverte o seu valor se o qubit for determinado.
    Se o qubit for indeterminado mantem-o
    Eh uma dependencia da funcao portas
    """
    if x == -1:  # preserva valores indefinidos
        return -1

    else:
        return (x + 1) % 2  # Inverte valores definidos


def portas(tab, porta, direcao):
    """portas: tabuleiro, caracter, caracter --> tabuleiro
    Funcao generica para realizar operacoes sobre portas.

    Esta funcao eh generica para evitar repeticao de codigo.
    Segue o seguinte algoritmo:
    1. Cria uma representacao interna mutavel do tabuleiro
    2. Faz operacoes sobre essa representacao (inversao ou troca)
    3. Converte a representacao interna para tuplos de tuplos com o tabuleiro
    """

    if not eh_tabuleiro(tab) or direcao not in ("E", "D"):
        raise ValueError("porta_{}: um dos argumentos e invalido".format(porta))

    tab = mutavel(tab)  # O tabuleiro passa a ser lista de listas, podemos alterar os seus elementos
    for i in range(3):  # Todas as portas realizam operacoes sobre 3 [pares de] elementos do tabuleiro

        if porta == "x":
            if direcao == "E":
                tab[1][i] = simetrico(tab[1][i])

            elif direcao == "D":
                tab[i][-2] = simetrico(tab[i][-2])  # porque len(tab[2]) == 2

        elif porta == "z":
            if direcao == "E":
                tab[0][i] = simetrico(tab[0][i])

            elif direcao == "D":
                tab[i][-1] = simetrico(tab[i][-1])

        elif porta == "h":  # faz 3 swaps de elementos do tabuleiro
            if direcao == "E":
                tab[0][i], tab[1][i] = tab[1][i], tab[0][i]

            elif direcao == "D":
                tab[i][-1], tab[i][-2] = tab[i][-2], tab[i][-1]


    return imutavel(tab)  # Formata corretamente o tabuleiro, imutabilizando-o



def porta_x(tabuleiro, direcao):
    """porta_x: tabuleiro, caracter --> tabuleiro"""
    return portas(tabuleiro, 'x', direcao)

def porta_z(tabuleiro, direcao):
    """porta_z: tabuleiro, caracter --> tabuleiro"""
    return portas(tabuleiro, 'z', direcao)

def porta_h(tabuleiro, direcao):
    """porta_h: tabuleiro, caracter --> tabuleiro"""
    return portas(tabuleiro, 'h', direcao)
