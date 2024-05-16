## Introdução



A expansão de Pierce da fração racional a/b, 1<=a<b, b>1, consiste em calcular a sequência de inteiros a(0)=a. a(1)=b%a(0), ..., a(n+1)=b%a(n), e ternima quando se otem um a(n) igual a 0.

Neste trabalho vamos calcular P(b), que é o número máximo de iterações até se chegar a a(n)=0, para um dado b e para todos os valores possíveis de a. O código pierce.c faz isso com código apenas para o CPU. Interess-nos fazer o mesmo numa placa gráfica.


## Ficheiros do professor

O professor disponibilizou os seguintes ficheiros:

```txt
Proj_3/
├─ node_modules/
├─ Professor Material/
│  ├─ # All provided Material
│  ├─ pierce.c
├─ src/
│  ├─ # All source files
├─ .gitignore
├─ Guidelines.md
├─ README.md
```