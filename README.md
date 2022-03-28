# Descrição
Repositório destinado à ferramenta de gerador de gerador de código para TCC
Universidade Estadual de Londrina, 2021
Aluna: Sophie Nascimento
Orientador: Wesley Attrot

# Requerimentos para rodar o programa
 - make
 - bison 3.7.4
 - flex  2.6.4
 - c++17 (utilizado g++ 7.5.0)

# Casos de Teste
Para realizar os casos de teste padrões:
 - $ make
 - $ ./tests/test.sh

A saída das gramáticas estará na pasta ./tests/grammar_outputs.
É possível criar outros casos de teste e colocá-los na pasta ./tests/grammars, o script rodará todos os arquivos na pasta.
