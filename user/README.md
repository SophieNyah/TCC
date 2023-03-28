# Considerações sobre o código de usuário
Esse código utiliza o Analisador Semântico escrito por mim mesma na matéria de Compiladores.

Graças a um planejamento não tão bom, uma compreensão incompleta do problema, e não haver perspectiva para ele ser reutilizado no futuro,
tal como um foco em demonstrar o funcionamento da ferramenta sendo desenvolvida, ao invés de criar um _back-end_ completo,
haverão algumas limitações tanto na análise do código C, quanto na gramática gerada.

Limitações:
- ~~Só pode haver uma definição/declaração de variável por sentença. As seguintes sentenças _poderão_ gerar problemas:~~
  - ~~`int a=0, b=0;`;~~
  - ~~`int a, b=0`;~~
  - ~~`int a=0; b`;~~
  - Então, esquece completamente isso aqui em cima, só não atribui valor quando declara a variável, as vezes dá algum bug bizarro;
- Não poderão haver mais de 5 (cinco) operações por sentença:
  - Atribuição não é considerada nesse limite;
  - A existência de uma constante _pode_ ser considerada nesse limite;
  - `resultado = ((a + b) * c * d) / e;` okay, possui 4 operações;
  - `resultado = ((a + b) * 6 * d) / 5;` não okay, possui 4 operações e 2 constantes;
  - Esse limite existe pois 3 registradores temporários ($t0-$t2), então sobram apenas 5 temporários para as operações, e esse código de usuário não lida com o caso de mais operandos do que registradores;
- Funções não poderão ter mais de 4 (quatro) parâmetros;
  - Os parâmetros serão passados apenas pelos registradores $a0-$a3;
- As funções `printf` e `scanf` só podem ter um único parâmetro depois da string de formatação, e esse parâmetro _deve_ ser uma única variável, e não uma expressão;
  - Para `scanf`, esse parâmetro deve obrigatoriamente o endereço de uma variável (`scanf("%d", &var);`); 
  - Isso acontece porque eu to com preguiça demais pra lidar com essa merda agora;
- Chamadas de funções não podem conter expressões, apenas variáveis;
- `return`s na função main só podem retornar valores inteiros;