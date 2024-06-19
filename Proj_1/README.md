# Projeto 1 - CSAC
## Sum-Free Sets em diferentes linguagens e máquinas

### Contexto

No primeiro trabalho, vamos resolver um problema brinquedo (toy problem em Inglês) de várias maneiras. O objetivo principal será tentar tornar o nosso programa o mais rápido possível, de modo a ir o mais longe possível (de preferência, batendo o recorde do mundo de cálculos sobre este problema). Um objetivo secundário será o de comparar a velocidade de processamento quando

    se muda de linguagem de programação,
    se mudam as opções de compilação (em linguagens compiladas), e
    se  muda o ambiente de execução (em particular, quando se muda o processador e quando se paraleliza a execução do programa).

O problema que vamos resolver consiste em contar todos os subconjuntos do conjunto { 1,2,...,n } que são livres de somas (sum-free sets). Um conjunto é livre de somas quando a soma de quaisquer dois elementos do conjunto não pode pertencer ao conjunto.

Na On-Line Encyclopedia of Integer Sequences as contagens que pretendemos estão registadas na sequência A007865.

    Os ficheiros incluem algum código fonte e também dados sobre tempos de execução do código fonte fornecido  em alguns modelos de processador. Nesses ficheiros (do tipo .txt) na primeira coluna mostra-se o valor de n, na segunda mostra-se o número de sub-conjuntos livre de somas, na terceira mostra-se o tempo de execução (somando os tempos gastos em todos os cores), e na quarta mostra-se o tempo de execução real (o chamado wall-clock time).

    Web assembly compila-se, depois de instalar as ferramentas de desenvolvimento, com algo do género "emcc -Wall -O2 a.c -o a.html"; são gerados três ficheiros: a.html, a.js e a.wasm. É possível testar localmente arrancando no web server local, com algo do género "busybox httpd -p 127.0.0.1:8080 -h /run/user/1000/tmp/" (neste exemplo os ficheiros foram guardados em /run/user/1000/tmp).

### Como executar cada linguagem

#### C
Version 1 (all from 1 to n):
```
cd C/version1
make clean v1_plain_all
./v1_plain_all
```

Version 2, 3 and threads:
```
cd C/versionX
make clean vX
./vX
```

#### Bash
```
cd Bash
chmod u+x count_sum_free_sets_v1_plain.sh
./count_sum_free_sets_v1_plain.sh
```

#### Crystal
```
cd Crystal
crystal count_sum_free_sets_v1_plain.cr
```

#### GO
```
cd GO
go run ./sum_free_sets.go
```

#### Java
```
cd Java
javac Main/*.java
java Main/Main
```

#### Javascript
```
cd JS
node count_sum_free_sets.js
```

#### Python
```
cd Python
python count_sum_free_sets.py
```

#### WebAssembly
```
cd WebAssembly

```

#### Scratch
```
import the .sb3 into the scratch web environment and execute it
```