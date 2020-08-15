

def cria_celula(v):
    """cria_celula: valor --> celula (lista com apenas valor). Tipo mutavel"""

    if type(v) is not int or v not in range(-1, 2):
        raise ValueError("cria_celula: argumento invalido.")

    return [v]


def eh_celula(arg):
    """eh_celula: universal --> booleano"""
    return isinstance(arg, list) and len(arg) == 1 and type(arg[0]) is int\
        and arg[0] in range(-1, 2)


def obter_valor(v):
    """obter_valor: celula --> valor (int)"""
    if not eh_celula(v):
        return False

    return v[0]


def inverte_estado(v):
    """inverte_estado: celula --> celula com valor invertido.
       Modificador destrutivo"""
    if not eh_celula(v):
        return False

    v[0] = (lambda x: (x + 1) % 2 if x >= 0 else x)(v[0])
    return v


def celulas_iguais(c1, c2):
    """celulas_iguais: celula x celula --> booleano"""

    return eh_celula(c1) and eh_celula(c2) and\
        obter_valor(c1) == obter_valor(c2)


def celula_para_str(c):
    """celula_para_str: celula --> cad. caracteres"""

    if not eh_celula(c):
        return False

    return (lambda x: str(x) if x >= 0 else 'x')(obter_valor(c))


def cria_coordenada(l, c):
    """cria_coodenada: int x int --> tuplo
    Este contrutor gera um TAD imutavel pois nao sao requeridos
    modificadores destrutivos"""

    if type(l) == type(c) == int and l in range(3) and c in range(3):
        return (l, c)

    else:
        raise ValueError("cria_coordenada: argumentos invalidos.")


def eh_coordenada(arg):
    """eh_coordenada: universal --> booleano"""

    return isinstance(arg, tuple) and len(arg) == 2\
        and all(isinstance(i, int) and i in range(3) for i in arg)


def coordenada_linha(coor):
    """coordenada_linha: coordenada --> inteiro"""

    return coor[0] if eh_coordenada(coor) else False


def coordenada_coluna(coor):
    """coordenada_coluna: coordenada --> inteiro"""
    return coor[1] if eh_coordenada(coor) else False


def coordenadas_iguais(coor1, coor2):
    """coordenadas_iguais: coordenada x coordenada --> booleano

    Verifica igualdade acedendo 'a interface de coordenada_linha
    e coordenada_coluna."""

    return eh_coordenada(coor1) and eh_coordenada(coor2)\
        and coordenada_linha(coor1) == coordenada_linha(coor2)\
        and coordenada_coluna(coor1) == coordenada_coluna(coor2)


def coordenada_para_str(coor):
    """coordenada_para_str: coordenada --> cad. caracteres"""
    return str(coor) if eh_coordenada(coor) else False


def tabuleiro_inicial():
    """tabuleiro_inicial:   --> tabuleiro"""
    return cria_tabuleiro(((-1, -1, -1), (0, 0, -1), (0, -1)))


def cria_tabuleiro(repr_antiga):
    """cria_tabuleiro: tuplo de tuplos com inteiros na representacao antiga-->
                    --> tabuleiro
                    """

    if not (isinstance(repr_antiga, tuple) and len(repr_antiga) == 3
            and all(isinstance(x, tuple) for x in repr_antiga)
            and len(repr_antiga[0]) == 3
            and len(repr_antiga[1]) == 3
            and len(repr_antiga[2]) == 2
            and all(type(c) is int and c in range(-1, 2)
                    for linha in repr_antiga for c in linha)):

        raise ValueError("cria_tabuleiro: argumento invalido.")

    tab_tmp = [[cria_celula(celula) for celula in linha]
               for linha in repr_antiga]

    tab_tmp[2] = [None] + tab_tmp[2]  # placeholder para emparelhar indices
    return tab_tmp


def eh_tabuleiro(arg):
    """eh_tabuleiro: universal --> booleano
    Indica se o argumento eh um tabuleiro verificando se eh uma
    lista de listas com celulas"""

    return isinstance(arg, list) and len(arg) == 3\
        and all(isinstance(subli, list) and len(subli) == 3 for subli in arg)\
        and arg[2][0] is None\
        and all(eh_celula(arg[l][c]) for l in range(3) for c in range(3)
                if (l, c) != (2, 0))


def str_para_tabuleiro(s):
    """str_para_tabuleiro: cad. caracteres --> tabuleiro

    Tenta gerar um tabuleiro a partir de uma cadeia de caracteres
    correspondente 'a representacao antiga"""

    try:
        return cria_tabuleiro(eval(s))

    except:  # apanha *qualquer* erro em caso de argumento invalido
        raise ValueError("str_para_tabuleiro: argumento invalido.")


def tabuleiro_celula(tab, coor):
    """tabuleiro_celula: tab x coor --> celula

    Devolve celula na coordenada coor do tabuleiro tab.
    Se os argumentos forem invalidos devolve False"""

    if not eh_tabuleiro(tab) or not eh_coordenada(coor):
        return False

    return tab[coordenada_linha(coor)][coordenada_coluna(coor)]


def tabuleiro_substitui_celula(tab, cel, coor):
    """tabuleiro_substitui_celula: tabuleiro x celula x coordenada -->
                                 --> tabuleiro


    Permuta a celula em coor no tabuleiro tab pelo argumento cel.
    Altera o tabuleiro no global namespace. Devolve tabuleiro modificado"""

    if not eh_tabuleiro(tab) or not eh_celula(cel) or not eh_coordenada(coor):
        raise ValueError("tabuleiro_substitui_celula: argumentos invalidos")

    linha, coluna = coordenada_linha(coor), coordenada_coluna(coor)
    tab[linha][coluna] = cel  # modificador destrutivo
    return tab


def tabuleiro_inverte_estado(tab, coor):
    """tabuleiro_inverte_estado: tabuleiro x coordenada --> tabuleiro

    inverte celula do tabuleiro tab em coor.
    Eh uma composicao das funcoes tabuleiro_substitui_celula,
    tabuleiro_celula, e inverte_estado. Acede ao tabuleiro
    apenas via esta interface"""

    if not eh_tabuleiro(tab) or not eh_coordenada(coor):
        raise ValueError("tabuleiro_inverte_estado: argumentos invalidos.")

    return tabuleiro_substitui_celula(tab,
            inverte_estado(tabuleiro_celula(tab, coor)), coor)


def tabuleiro_dimensao(tab):
    """tabuleiro_dimensao: tabuleiro --> inteiro

    Devolve a dimensao da matriz do tabuleiro.
    Seria util para generalizar o TAD do tabuleiro"""

    return 3


def tabuleiro_para_str(tab):
    """tabuleiro_para_str: tabuleiro --> cad. caracteres

    Obtem todas as celulas do tabuleiro recorrendo 'a interface
    e formata no formato indicado"""

    if not eh_tabuleiro(tab):
        return False

    return """+-------+
|...{2}...|
|..{1}.{5}..|
|.{0}.{4}.{7}.|
|..{3}.{6}..|
+-------+""".format(*[celula_para_str(tabuleiro_celula(tab,
                    cria_coordenada(l, c)))
                    for l in range(3) for c in range(3) if (l, c) != (2, 0)])


"""lista de todas as coordenadas possiveis no tabuleiro.
   Nao inclui a coordenada (2, 0)"""

coordenadas_validas = [
                        cria_coordenada(l, c) for l in range(3)
                        for c in range(3) if (l, c) != (2, 0)
                       ]


def tabuleiros_iguais(t1, t2):
    """tabuleiros_iguais: tabuleiro x tabuleiro --> booleano

    verifica se todas as celulas de dois tabuleiros sao iguais"""

    if not eh_tabuleiro(t1) or not eh_tabuleiro(t2):
        return False

    return all(celulas_iguais(tabuleiro_celula(t1, coor),
                              tabuleiro_celula(t2, coor))
               for coor in coordenadas_validas)


def tabuleiro_troca_celulas(t, coor1, coor2):
    """tabuleiro_troca_celulas: tabuleiro x coordenada x coordenada
                             --> tabuleiro

    Construtor destrutivo

    Composicao de tabuleiro_celula e tabuleiro_substitui_celula.
    Le dois valores do tabuleiro e escreve-os cambiando as suas posicoes"""

    cel1 = tabuleiro_celula(t, coor1)
    cel2 = tabuleiro_celula(t, coor2)
    t = tabuleiro_substitui_celula(t, cel2, coor1)
    t = tabuleiro_substitui_celula(t, cel1, coor2)

    return t


def porta_x(t, p):
    """porta_x: tabuleiro, direcao (caracter p) --> tabuleiro"""

    if not eh_tabuleiro(t) or p not in ("E", "D"):
        raise ValueError("porta_x: argumentos invalidos.")

    if p == 'E':
        for i in range(3):
            t = tabuleiro_inverte_estado(t, cria_coordenada(1, i))

    if p == 'D':
        for i in range(3):
            t = tabuleiro_inverte_estado(t, cria_coordenada(i, 1))

    return t


def porta_z(t, p):
    """porta_z: tabuleiro, direcao (caracter p) --> tabuleiro"""

    if not eh_tabuleiro(t) or p not in ("E", "D"):
        raise ValueError("porta_z: argumentos invalidos.")

    if p == 'E':
        for i in range(3):
            tabuleiro_inverte_estado(t, cria_coordenada(0, i))

    if p == 'D':
        for i in range(3):
            tabuleiro_inverte_estado(t, cria_coordenada(i, 2))

    return t


def porta_h(t, p):
    """porta_h: tabuleiro, direcao (caracter p) --> tabuleiro"""
    if not eh_tabuleiro(t) or p not in ("E", "D"):
        raise ValueError("porta_h: argumentos invalidos.")

    if p == 'E':
        for i in range(3):
            tabuleiro_troca_celulas(t, cria_coordenada(0, i), cria_coordenada(1, i))

    if p == 'D':
        for i in range(3):
            tabuleiro_troca_celulas(t, cria_coordenada(i, 2), cria_coordenada(i, 1))

    return t


def hello_quantum(s):
    """hello_quantum: cad. caracteres (e stdin) --> booleano (e stdio)

    Implementacao do jogo Hello Quantum desenvolvido pela IBM
    Recebe configuracao inicial via string em argumento.
    Interacao com o jogador eh feita via stdio"""

    tabuleiro_final, score_max = s.split(':')
    tabuleiro_final = str_para_tabuleiro(tabuleiro_final)

    score_max = int(score_max)
    tab_jogador = tabuleiro_inicial()
    score = 0

    converte_portas = {'X': porta_x, 'Z': porta_z, 'H': porta_h}

    print("Bem-vindo ao Hello Quantum!")
    print("O seu objetivo e chegar ao tabuleiro:")
    print(tabuleiro_para_str(tabuleiro_final))

    print("Comecando com o tabuleiro que se segue:")
    print(tabuleiro_para_str(tab_jogador))

    while score < score_max:
        score += 1

        porta = input("Escolha uma porta para aplicar (X, Z ou H): ")
        qubit = input("Escolha um qubit para analisar (E ou D): ")
        tab_jogador = converte_portas[porta](tab_jogador, qubit)
        print(tabuleiro_para_str(tab_jogador))

        if tabuleiros_iguais(tab_jogador, tabuleiro_final):
            print("Parabens, conseguiu converter o tabuleiro em {} jogadas!".
                  format(score))

            return True  # Ganho o jogo

    return False  # (You lost the game bit.ly/1oYSePF)

