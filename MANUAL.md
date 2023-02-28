# Tabela de Conteúdos
1. [Geração do Gerador de Código](#geração-do-gerador-de-código)
   1. [Header](#Header)
   2. [Regras](#Regras)
   3. [Registradores](#Registradores)
   4. [Código](#Código)
2. [Formato do Arquivo .md](#formato-do-arquivo-md)
3. [Alocador de Registradores](#alocador-de-registradores)
   1. [Setup para o Alocador](#setup-para-o-alocador)
   2. [Interface do Alocador](#interface-do-alocador)
      1. [Nova Instrução](#nova-instrução)
      2. [Alocar](#alocar)
      3. [Emitir Código](#emitir-código)
4. [Sugestões Sobre Implementação](#sugestões-sobre-implementação) 
   1. [Classe Tree](#classe-tree) 
   2. [Designar Registradores Alvo](#designar-registradores-alvo)


# Geração do Gerador de Código
_Nota: assume-se que todos os comandos estão sendo rodados a partir da root do projeto._

Primeiramente é necessário compilar o gerador de código, que pode ser feito através do comando `$ make generator`, que irá gerar todos os arquivos objeto (.o) e o arquivo executável do gerador na pasta `./output/src/`;

Para que o gerador leia seu arquivo de entrada e crie o arquivo final, há duas opções:
- Inserir seu arquivo na pasta `./tests/grammars/`, e executar o script `./tests/test.sh`, que compilará todos os arquivos na pasta das gramáticas;
- Criar seu arquivo e compilálo através do comando `./output/src/generator seu_arquivo.md -o sua_saida.o [./output/src/RegAlloc.o] -std=c++17`;
  -  O arquivo `RegAlloc.o` deve ser incluso apenas caso seu arquivo utilize o alocador de registradores, caso contrário não deve ser incluso.

Agora é só executar seu arquivo de saída normalmente. Caso tenha utilizado o primeiro método, ele se encontra em `./output/grammars/`


# Formato do Arquivo .md
O arquivo que será lido pelo gerador é um arquivo .md (machine description) com três ou quatro seções, separadas dois sinais de porcentagem (%%):
- A primeira seção é o header, a declaração dos símbolos terminais e não terminais da sua gramática, tal como o seu código C++ que será inserido no topo do arquivo gerado;
- A segunda seção é a declaração das regras da gramática, que serão explicados à frente; 
- A terceira seção serve para declarar os registradores que serão utilizados na alocação, caso haja opte por fazê-la;
  - A alocação de registradores é opcional, e caso essa seção seja omitida, não ocorrerá alocação;
- A última seção é seu código C++ que será incluso ao final do arquivo. Caso o arquivo final deva ter uma função main, geralmente ela é inserida aqui.

_Nota: todas as seções estão melhor detalhadas no artigo de [TCC](), esse é só um guia rápido de como usar a ferramenta._

### Header
O header deve obrigatoriamente conter duas coisas:

Os símbolos da gramática, declarados através das diretivas `%term {IDENTIFIER}` e `%nonterm {IDENTIFIER}`, que serão inclusos em um enum chamado `User_symbols`;

Uma classe chamada `Tree` que estende a classe `YamgTree`, tendo a si própria como template (`class Tree: public YamgTree<Tree>`).

Essa classe deve obrigatoriamente implementar o método `void readTree()`; esse método deve fazer uma leitura da AST e inserir a árvore lida em `this`.
A maneira como essa leitura é feita fica a critério do usuário, e nas gramáticas exemplo há uma possível maneira de fazer isso, lendo o _standard input_.

Ademais, há duas funções extras que podem opcionalmente ser implementadas, que são `User_Symbols readUserSymbol()` e `Node_type readNodeType()`.
Seu propósito é apenas facilitar a criação da árvore, porém elas não são necessárias ao programa.

`User_symbols` é um enum contendo os terminais e não-terminais definidos por você.
Há quatro variáveis `TERMINALS_START`, `TERMINALS_END`, `NON_TERMINALS_START`, `NON_TERMINALS_END`, que retornam o intervalo inclusivo dos símbolos.

`Node_type` é um enum com os valores `operacao`, `registrador`, `constante`, e `especifico`.
Esses dois enums são utilizados pelo programa para determinar o casamento de padrões, portanto é importante que eles sejam informados corretamente para o programa.

### Regras
Aqui definem-se as regras que o gerador irá utilizar para criar o casador de padrões. Elas tem o seguinte formato:

```
regra  -> nome_regra <- não-terminal: árvore custo = ação ;
árvore -> terminal ( [ação] lista_árvore )
        | terminal
        | não-terminal
lista_árvore -> lista_árvore, árvore
              | árvore
ação  -> { Codigo-C++ }
custo -> { Codigo-C++ }
```

- nome_regra: uma string que é o nome único da regra, utilizado para casar as regras;
- árvore: a árvore que representa o padrão a ser casado;
  - cada nó da árvore pode opcionalmente conter uma ação, que será executada no momento que o nó for casado;
  - Ex.: `CONST`, `ADD(reg,CONST)`, `MUL( {foo(bar);} reg, reg)`
- lista_árvore: recursão para da árvore, possibilitando a inserção de _n_ elementos, e subárvores;
  - Ex.: `ADD(reg, reg, CONST)`, `SUB(reg, ADD(reg, CONST))`;
- ação: código C++ envolto por chaves, que será executado quando o padrão for casado;
  - Caso esteja utilizando o alocador de registradores, você deve querer criar uma nova instrução aqui;
  - Senão, esse deve ser o lugar onde seu código é emitido, por exemplo printando a instrução para algum arquivo;
  - Diretiva: a diretiva `$[n]`, onde _n_ é um número inteiro, retorna o nome do n-ésimo nó na árvore;
    - Para a instrução `ADD(reg,CONST)`, `$[0]` retornará o nome de `ADD`, `$[1]` será o nome de `reg`, e `$[2]` o valor de `CONST`;
    - Utilizado principalmente para passar o nome da variável para uma nova instrução;
    - Ex.: `RegAlloc::newInstruction({ "addi, %o, %o, %c" }, { {$[0], YAMG_WRITEABLE_OPERAND}, $[1] }, { $[2] });` (esse código será explicado na seção do alocador);
- custo: código C++ envolto por chaves, deve obrigatoriamente retornar um valor inteiro, caso contrário não será possível casar o padrão;
  - Diretiva: a diretiva `$cost[n]`, sendo _n_ um inteiro, retorna o custo do n-ésimo nó na árvore, seguindo a mesma lógica da diretiva `$[n]`;
    - Exemplo de possível custo para a instrução `ADD(reg,CONST)`: `{ return 1 + $cost[1]; }`.

### Registradores
Essa seção é opcional, deve apenas ser declarada caso você vá utilizar o alocador de registradores, caso contrário ela não deve ser inserida no arquivo,
de tal modo que ele tenha apenas três seções, como abaixo:
```
{ código c++ }
%term ...

%%

regra1 -> ...
regra2 -> ...

%%

{ código c++ }
```

Aqui são declarados os registradores de sua máquina para onde variáveis podem ser alocadas, registradores reservados para spilling, e as instruções de _load/store_ da máquina, caso hajam.

Um exemplo na arquitetura MIPS seria:
```
%register $s0
%register $s1
%register $s2
%register $s3
%register $s4
%register $s5
%register $s6
%register $s7
%spill_reg $t0
%spill_reg $t1
%spill_reg $t2
%set_read  "rw %o, %o($sp)"
%set_write "lw %o, %o($sp)"
```

A diretiva `%register` insere o registrador especificado na lista de registradores disponíveis para alocação;

A diretiva `%spill_reg` insere o registrador em uma lista de registradores para _spilling_.
Devem haver tantos registradores para _spill_ quanto a quantidade máxima de operadores na sua gramática.

Na arquitetura MIPS, uma operação tem no máximo três operadores, portanto são declarados três registradores para _spilling_.

As diretivas `%set_read` e `%set_write` definem, respectivamente, as instruções de _load/store_ da arquitetura.
Elas serão usadas no caso de _spilling_, chamando um _load_ para registradores que foram para memória, e um _store_ para aqueles que foram alterados.

### Código
Essa é uma seção onde código C++ pode ser inserido livremente.

Caso o casador de padrões seja utilizado por si só, sua função `main()` deverá aparecer aqui.

Se ele for utilizado como biblioteca de outro programa, essa seção provavelmente ficará vazia, ou então possuirá algumas funções para integração com o resto de seu programa.


# Alocador de Registradores
Este programa possui um alocador de registradores opcional, que você pode utilizar para facilmente prototipar suas gramáticas.

### Setup para o Alocador
Primeiramente é necessário incluir a seção de registradores e seguir os passos explicados [anteriormente](#registradores).

É necessário também compilar seu programa incluindo o objeto `./output/src/RegAlloc.o`, para que o programa rode corretamente.

### Interface do Alocador
Embora hajam mais funções na declaração da classe, apenas três delas devem ser usadas por você.

#### Nova Instrução
`RegAlloc::newInstruction(Instruction)` ou `RegAlloc::newInstruction(std::string template_instruction, std::vector< Instruction::OperandType > operands, std::vector< std::string > constants={})`:
  - Essa função serve para criar uma nova instrução. As instruções são guardadas na classe `RegAlloc`, e são inseridas na ordem em que são chamadas, que também é a ordem que os padrões são casados;
  - A string `template_instruction` é o esqueleto de uma instrução, podendo conter qualquer combinação das diretivas `%o` e `%c` que serão substituídos por registradores e constantes;
    - Por exemplo, a instrução MIPS `addi $s0, $zero, 9`, representada pela árvore `ADDI(reg,CONST)`, teria a seguinte `template_instruction`: `"addi %o, $zero, %c"`;
    - O primeiro `%o` receberá um registrador que o alocador decidir para ele, caso haja algum disponível. Caso contrário, um registrador de _spill_ será temporariamente utilizado;
    - O `%c` será substituído pelo valor DE `CONST`;
  - O parâmetro `operands` representa os operandos que substituirão os `%o` da `template_instruction`;
    - É um vetor do tipo `OperandType`, que é composto por uma string e um especificador de variável de escrita;
      - A string representa o nome da variável atrelada àquela operação, o mesmo nome que aparece na AST;
      - O outro valor só precisa ser especificado quando o operando é o alvo da operação, ou seja, recebe um novo valor;
        - Para a instrução MIPS acima, `$s0` recebe o valor da soma de `$zero` com a constante 9, e portanto o valor deve ser especificado para esse operando;
        - Assim sendo, o vetor `operandos` para essa instrução deve ser da forma `{ { "resultadoSoma", YAMG_WRITEABLE_OPERAND } }`, sendo `resultadoSoma` o nome da variável, e o valor `YAMG_WRITEABLE_OPERAND` indica que ele é um operando de escrita;
  - O último parâmetro, `constants` é um vetor opcional com o valor das constantes da operação, que substituirão os `%c` na `template_instruction`.

Para melhor entender a criação de instruções, considere o código e as regras abaixo, que fazem uso das diretivas descritas na seção [regras](#regras):
```
1. int valor1 = 3;
2. int soma = valor1 + valor1;
```
```
addConst -> reg: ADD(reg,CONST) { return 1 + $cost[1]; } = {
  RegAlloc::newInstruction({"addi %o, $zero, %c"}, { {$[1], YAMG_WRITEABLE_OPERAND} }, { $[2] });
};
addRegs -> reg: ADD(reg,reg,reg) { return 1 + $cost[1] + $cost[2] + $cost[3] } = {
  RegAlloc::newInstruction({"add %o, %o, %o"}, { {$[1], YAMG_WRITEABLE_OPERAND}, {$[2]}, {$[3]} });
};
```

A primeira linha de código é casada pelo padrão `addConst`, enquanto a linha dois é casada por `addRegs`.
O vetor de instruções estará preenchido da seguinte maneira (os `%o` e `%c` foram substituídos por seus respectivos valores para facilitar a visualização):
```
RegAlloc::instructions[0] = "addi valor1, $zero, 3"
RegAlloc::instructions[1] = "add  soma, valor1, valor1" 
```

Note que, quando os registradores forem alocados, ao invés dos nomes das variáveis, estarão nas instruções os registradores declarados em sua seção respectiva.
No exemplo de MIPS construído acima, eles são `$s0`, `$s1`, etc.

#### Alocar
A função `allocate()` é a parte principal do alocador.
Nela está contido todo o algoritmo de _linear scan_.

Essa função não retorna nada, ao invés disso gera listas internas nas instruções, que podem ser usadas posteriormente pela emissão de código.

#### Emitir Código
Por fim, temos a função `emitCode(bool use_registers=true)`, que emite as instruções para a saída padrão, já substituindo os `%o` pelos devidos registradores, e `%c` pelas constantes.
O código só pode ser impresso depois da alocação, ou seja, é necessário chamar a função `allocate()` primeiro.

Se o valor `false` for passada para ela, apenas as `template_instructions` serão emitidas, sem realizar nenhuma substituição.
Isso é para propósitos de teste e verificação de corretude, e pode ser utilizado mesmo sem ter chamado `allocate()` anteriormente.


# Sugestões Sobre Implementação

### Classe Tree
Na construção dos nós da árvore, na função `readTree()`, é necessário _setar_ em cada nó os atributos `name`, `User_symbols`, e `Node_type`.
- name: O nome do registrador ou valor da constante correspondente na AST. Essa variável é acessada quando a diretiva `$[n]` é chamada, necessária para alocação de registradores;
- User_symbols: O símbolo que aquele nó representa, independente de seu nome. Um nó pode ter o nome "soma", mas possuir o símbolo `User_symbols::reg`; 
- Node_type: Um valor que representa, de maneira mais genérica, o que aquele nó representa, podendo ser um de:
  - `operacao`;
  - `registrador`;
  - `constante`;
  - `especifico`;

### Designar Registradores Alvo
Imagine que você utilizará a alocação de registradores, e tem uma regra `ADD(reg, reg)`, e a instrução que ela emitirá é da forma `newInstruction({"add %o, %o, %o"}, { { $[0], YAMG_WRITEABLE_OPERAND}, {$[1]}, {$[2]} })`.

É possível observar que `$[0]` pegaria o nome armazenado no nó `ADD`, portanto até mesmo nós de operação deveriam possuir o nome do registrador destino.

Alternativamente, é possível alterar sua gramática de tal modo que todos os registradores apareçam explicitamente nas regras, de modo que a regra acima ficaria `ADD(reg, reg, reg)`,
e a instrução seria `newInstruction({"add %o, %o, %o"}, { { $[1], YAMG_WRITEABLE_OPERAND}, {$[2]}, {$[3]} })`, deixando o nó `ADD` sem a necessidade de um nome.

_Nota: essas sugestões são com base nas experiências e ideias da autora para resolver dificuldades relaciodas às limitações do modelo do projeto.
       Caso você tenha alguma outra ideia para desenvolver sua gramática, não é necessário se prender à essas sugestões._
