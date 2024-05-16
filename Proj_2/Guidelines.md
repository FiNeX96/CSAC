## Introdução



A expansão de Pierce da fração racional a/b, 1<=a<b, b>1, consiste em calcular a sequência de inteiros a(0)=a. a(1)=b%a(0), ..., a(n+1)=b%a(n), e ternima quando se otem um a(n) igual a 0.

Neste trabalho vamos calcular P(b), que é o número máximo de iterações até se chegar a a(n)=0, para um dado b e para todos os valores possíveis de a. O código pierce.c faz isso com código apenas para o CPU. Interess-nos fazer o mesmo numa placa gráfica.


## Ficheiros do professor

O professor disponibilizou os seguintes ficheiros:

```txt
Proj_3/
├─ node_modules/
├─ Professor Material/
│  ├─ elapsed_time.h
│  ├─ linux_performance_counters.h
│  ├─ matrix_multiplication.c
│  ├─ matrix_multiplication_data.tgz
│  ├─ matrix_multiplication_run.bash
│  ├─ matrix_multiplication_with_performance_counters.c
│  ├─ memory_info.c
│  ├─ memory_info_data.tgz
│  ├─ performance_counters_test.c
│  ├─ Web Assembly/
│  │  ├─  a.html
│  │  ├─  a.js
│  │  ├─  a.wasm
├─ src/
│  ├─ index.css
│  ├─ index.js
├─ .gitignore
├─ Guidelines.md
├─ README.md
```