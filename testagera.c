/* Thiago_Henriques 2211171 3WB */
/* Ariel_ Matricula 3WB */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gera.h"

#define MAX 1000

int main() {
  FILE *myfp;
  unsigned char codigo[MAX];
  funcp funcaoSimples;
  int res;
  int p1 = 1;
  int p2 = 2;
  int p3 = 3;

  /* Abre o arquivo fonte */
  if ((myfp = fopen("programa", "r")) == NULL) {
    perror("Falha na abertura do arquivo fonte");
    exit(1);
  }
  /* compila a função Simples */
  funcaoSimples = gera(myfp, codigo);
  fclose(myfp);

  /* chama a função */
  res = (*funcaoSimples) (p1,p2,p3);  /* passando argumentos apropriados */
  printf("\nRetorno: %d\n\n", res);

  // printf("Teste 1 (ret $1): %s\n", res == 1 ? "sucesso" : "falha");

  //testar para ret $-1 tbm

  // printf("v1 < $1 \n ret v1 \nTeste 2: %s\n", res == 1 ? "sucesso" : "falha");

  // printf("Teste 3:\nv5 < $5 \nv1 < v5 \nret v1 \nResultado: %s\n", res == 5 ? "sucesso" : "falha");

  // printf("Teste 4:\nv5 < $5 \nv2 < v5 \nret v1 \nResultado: %s\n", res == 5 ? "sucesso" : "falha");

  printf("Teste 5: atribuição com parametro\nResultado: %s\n", res == 2 ? "sucesso" : "falha");

  return 0;
}