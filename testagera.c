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
  
  printf("\nTeste 1: soma simples, parametros de entrada: (5, 10, 0), Resultado esperado: 15.\n");
  if ((myfp = fopen("arquivosTestes/programaTeste1.txt", "r")) == NULL) {
    perror("Falha na abertura do arquivo fonte");
    exit(1);
  }
  funcaoSimples = gera(myfp, codigo);
  fclose(myfp);
  res = (*funcaoSimples) (5,10,0);  
  printf("\nRetorno: %d\n", res);
  printf("Resultado: %s\n", res == 15 ? "sucesso" : "falha");

  printf("\nTeste 2: operação de identidade, parametros de entrada: (7, 0, 0), Resultado esperado: 7.\n");
  if ((myfp = fopen("arquivosTestes/programaTeste2.txt", "r")) == NULL) {
    perror("Falha na abertura do arquivo fonte");
    exit(1);
  }
  funcaoSimples = gera(myfp, codigo);
  fclose(myfp);
  res = (*funcaoSimples) (7,0,0);  
  printf("\nRetorno: %d\n", res);
  printf("Resultado: %s\n", res == 7 ? "sucesso" : "falha");

  printf("\nTeste 3: multiplicação simples, parametros de entrada: (8, 2, 0), Resultado esperado: 16.\n");
  if ((myfp = fopen("arquivosTestes/programaTeste3.txt", "r")) == NULL) {
    perror("Falha na abertura do arquivo fonte");
    exit(1);
  }
  funcaoSimples = gera(myfp, codigo);
  fclose(myfp);
  res = (*funcaoSimples) (8,2,0);  
  printf("\nRetorno: %d\n", res);
  printf("Resultado: %s\n", res == 16 ? "sucesso" : "falha");

  printf("\nTeste 4: operação de valor absoluto, parametros de entrada: (-5, 0, 0), Resultado esperado: 5.\n");
  if ((myfp = fopen("arquivosTestes/programaTeste4.txt", "r")) == NULL) {
    perror("Falha na abertura do arquivo fonte");
    exit(1);
  }
  funcaoSimples = gera(myfp, codigo);
  fclose(myfp);
  res = (*funcaoSimples) (-5,0,0);  
  printf("\nRetorno: %d\n", res);
  printf("Resultado: %s\n", res == 5 ? "sucesso" : "falha");

  printf("\nTeste 5: cálculo de fatorial, parametros de entrada: (5, 0, 0), Resultado esperado: 120.\n");
  if ((myfp = fopen("arquivosTestes/programaTeste5.txt", "r")) == NULL) {
    perror("Falha na abertura do arquivo fonte");
    exit(1);
  }
  funcaoSimples = gera(myfp, codigo);
  fclose(myfp);
  res = (*funcaoSimples) (5,0,0);  
  printf("\nRetorno: %d\n", res);
  printf("Resultado: %s\n", res == 120 ? "sucesso" : "falha");

  return 0;
}
