# Projecto 2 - IAED 2018/19

Este projecto visa implementar um tipo de dados e operações básicas para 
armazenar e procurar dados eficientemente.

Para isso foi utilizada uma tabela de dispersão que tem ponteiros
que garantem que a ordem de inserção é preservada quando queremos
aceder a todos os elementos.

Este tipo abstracto de dados é genérico na medida em que aceita
outro tipo de dados. Para tal foi convencionado que cada tipo deve ter
um destructor (pontiero para função) que garante que toda a memória
relacionada com esse tipo seja libertada.