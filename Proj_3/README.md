# Projeto 3 - CSAC
## Expansão de Pierce

### Contexto

A expansão de Pierce da fração racional a/b, com 1<=a<b e b>1, consiste em calcular a sequência de inteiros a[0]=a, a[1]=b%a[0], ..., a[n+1]=b%a[n], e ternima quando se otem um a[n] igual a 0.

Neste trabalho vamos calcular P[b], que é o número máximo de iterações até se chegar a a[n]=0, para um dado b e para todos os valores possíveis de a. O código pierce.c faz isso com código apenas para o CPU. Interessa-nos fazer o mesmo numa placa gráfica. Use como referências o código presente na aplicação hello world (CUDA e OpenCL) fornecida com a conta em banana.ua.pt e o códio fornecido a seguir.

### Como executar o projeto
#### OpenCL made by Pedro Ramos
```
cd Ramos/openCL_Pierce
make openCL_Pierce
./openCL_Pierce <number of the largest expansion>
```

example:
```
./openCL_Pierce 500000
```



#### Get OpenCL Specs
```
cd Ramos/openCL_Pierce
make openCL_List_Specs
./openCL_List_Specs
```

### Guidelines

[Guidelines](Guidelines.md)
