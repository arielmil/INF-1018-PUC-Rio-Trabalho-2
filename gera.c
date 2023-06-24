//Thiago Henriques 2211171
//Ariel Mileguir 1811928

#include <stdlib.h>
#include <string.h>
#include "gera.h"

/* Definicões de constantes */

// maximo de linhas do codigo em simples
#define MAX_LINHAS 1000

/* Definições de opcodes */

#define PUSHQRBP "55" // Push %rbp
#define MOVQRSPRBP "48 89 e5" // Move %rsp para %rbp
#define SUBQX20RSP "48 83 ec 20" // Subtrai 20 de %rsp
#define MOVLV10 "41 ba" // Move $var para %r10d
#define MOVLV11 "41 bb" // Move $var para %r11d
#define MOVLM10 "44 8b 55" // Move alguém da memória para %r10d (precisa colocar os 8 bits de offset de %rbp quando usar)
#define MOVLM11 "44 8b 5d" // Move alguém da memória para %r11d (precisa colocar os 8 bits de offset de %rbp quando usar)
#define MOVL11M "44 89 5d" // Move %r11d para a memória (precisa colocar os 8 bits de offset de %rbp quando usar)
#define MOVLMEAX "8b 45" // Move um valor na memoria para o eax (precisa colocar os 8 bits de offset de %rbp quando usar)
#define MOVL "8b"
#define ADDL "45 01 d3" // Soma %r10d com %r11d e coloca o resultado em %r11d
#define SUBL "45 29 d3" // Subtrai %r10d com %r11d e coloca o resultado em %r11d
#define IMULL "45 0f af da" // Multiplica %r10d com %r11d e coloca o resultado em %r11d
#define N1IMULL "45 6b db ff" // Multiplica %r11d com -1 e coloca o resultado em %r11d
#define CMPL010 "41 83 fa 00" // Compara %r10d com $0
#define CMPL011 "41 83 fb 00" // Compara %r11d com $0
#define JLE "7e" // Jump if less or equal (precisiona colocar o offset de 8 bits para a posição de memória que for pular quando usar)
#define RET "c3" // Return
#define LEAVE "c9" // Leave

// Struct para guardar as instruções
typedef struct {
    char *nome;
    char *bytes;
} Instrucao;

Instrucao instrucoes[] = {
    {"PUSHQRBP", PUSHQRBP},
    {"MOVQRSPRBP", MOVQRSPRBP},
    {"SUBQX20RSP", SUBQX20RSP},
    {"MOVLV10", MOVLV10},
    {"MOVLV11", MOVLV11},
    {"MOVLM10", MOVLM10},
    {"MOVLM11", MOVLM11},
    {"MOVL11M", MOVL11M},
    {"MOVLMEAX", MOVLMEAX},
    {"MOVL", MOVL},
    {"ADDL", ADDL},
    {"SUBL", SUBL},
    {"IMULL", IMULL},
    {"N1IMULL", N1IMULL},
    {"CMPL010", CMPL010},
    {"CMPL011", CMPL011},
    {"JLE", JLE},
    {"RET", RET},
    {"LEAVE", LEAVE}

};

#define NUM_INSTRUCOES (sizeof(instrucoes) / sizeof(Instrucao))

// Struct para guardar informações sobre os labels (Posição dos offsets no array codigo, e a linha que é para ele pular para no JLE)
typedef struct IndiceLinha {

  int indiceOffset;
  int linha;

} indiceLinha;

// Funcao para escrever um inteiro em little endian
void escreveLittleEndian(int valor, unsigned char *codigo, int *end) {
  int i;
  for (i = 0; i < 4; i++)
    {
      codigo[*end] = valor >> (i*8);
      (*end)++;
    }
}

// Usada pela funcao adicionarInstrucao abaixo.
char* encontrarInstrucao(char* nome) {
    for (int i = 0; i < NUM_INSTRUCOES; i++) {
        if (strcmp(instrucoes[i].nome, nome) == 0) {
            return instrucoes[i].bytes;
        }
    }
    return NULL; // Se a instrução não for encontrada
}

// Funcao para adicionar uma instrucao (Pode ter mais de um byte) ao codigo
void adicionarInstrucao(unsigned char* codigo, char* nomeInstrucao, int* posicao) {
    char *instrucao = encontrarInstrucao(nomeInstrucao);
    if (instrucao == NULL) {
        printf("Instrução desconhecida: %s\n", nomeInstrucao);
        return;
    }

    // Copie a instrução para um array de char (um buffet) para que possamos modificá-la com strtok
    char instrucaoBuffer[30];
    strncpy(instrucaoBuffer, instrucao, sizeof(instrucaoBuffer));
    instrucaoBuffer[sizeof(instrucaoBuffer) - 1] = '\0';  // Garante que a string esteja terminada com NULL

    char *token;
    unsigned int numHex;

    /* Obtém o primeiro token */
    token = strtok(instrucaoBuffer, " ");

    /* caminha através de outros tokens */
    while( token != NULL ) {
        sscanf(token, "%x", &numHex);
        codigo[*posicao] = (unsigned char)numHex;
        (*posicao)++;
        token = strtok(NULL, " ");
    }

}

// Para imprimir o codigo em hexadecimal indicando cada linha
void imprimirCodigoMaquina(unsigned char codigo[], int indiceByte[], int numLinhas) {
    int i, j;

    printf("\nImprimindo codigo de maquina:\n\n");

    for(i = 0; i < numLinhas; i++) {
        int inicioLinha = indiceByte[i];
        int fimLinha;

        if(i == numLinhas - 1) { // Última linha
            fimLinha = inicioLinha;
            // percorre o array até encontrar leave (0xc3)
            while(codigo[fimLinha] != 0xc3) {
                fimLinha++;
            }
        } else {
            fimLinha = indiceByte[i + 1] - 2;
        }

        if (i > 0) {
          
          printf("Linha %d:\t", i);

          for(j = inicioLinha - 1; j <= fimLinha; j++) {
            printf("%02x ", codigo[j]);
          }

          printf("\n");
        }
       
    }
}

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

//gera.c

funcp gera (FILE *f, unsigned char codigo[])
{
  int line = 1;
  int  c;

  int qntLinhas; // usada para a função imprimirCodigoMaquina

  // calcula o endereço de cada linha
  int end = 0;
  int i = 0;

  char indiceOffset; // Usado no ultimo for antes do final do return. Serve para saber qual o indice dos offsets no array codigo que tem o JLE
  char indiceByteLinha; // byte que indica o começo de uma linha no array codigo

  char offsetMem; // Offset de memória para as váriaveis e pulos do JLE

  int indiceByte[MAX_LINHAS]; // array que relaciona (i = numero da linha - 1) no arquivo em simples com o indice do primeiro byte da linha no array codigo para cada i.
  indiceLinha labelPulaLinha[MAX_LINHAS]; // array que guarda a posição no array código de cada label (offset para o JLE), assim como a linha em que é para ele pular para (Com espaço o suficiente para todos os labels)
  
  int sizeLabelPulaLinha = 0; // quantidade de iflez's que tem no código

  //inicio:

  //pushq, %rbp
  adicionarInstrucao(codigo, "PUSHQRBP", &end);

  //movq, %rsp, %rbp
  adicionarInstrucao(codigo, "MOVQRSPRBP", &end);

  //subq, $32, %rsp //Aloca espaço para as 5 variáveis
  adicionarInstrucao(codigo, "SUBQX20RSP", &end);

  // le o arquivo:

  indiceByte[0] = 8; // mapeia a linha 0 para o byte 8 (inicio do codigo / byte de inicio da primeira linha)

  printf("\n\nPrintando codigo em simples:\n\n");

  while ((c = fgetc(f)) != EOF) {

    switch (c) {
      case 'r': { /* retorno funcionando*/
        char var0;
        int idx0;
        if (fscanf(f, "et %c%d", &var0, &idx0) != 2)
          error("comando invalido", line);

        printf("%d ret %c%d\n", line, var0, idx0);

        switch (var0) {
          case('v'):
            adicionarInstrucao(codigo, "MOVLMEAX", &end);

            offsetMem = -4 * idx0;
            codigo[end] = offsetMem;
            end++;

            break;
          
          case('$'):
            break;
          
          default:
            error("comando invalido", line);
        }

        // coloca o fim do assmebly no vetor codigo:
        
        //leave
        adicionarInstrucao(codigo, "LEAVE", &end);

        //ret
        adicionarInstrucao(codigo, "RET", &end);

        break;
      }
      
      case 'v': { /* atribuiÃ§Ã£o e op. aritmetica a fazer: atrib var e par / op arit de td*/
        int idx0, idx1;
        char var0 = c, c0, var1;
        if (fscanf(f, "%d %c", &idx0, &c0) != 2)
          error("comando invalido", line);
        
        if (c0 == '<') { /* atribuicao */
        
          if (fscanf(f, " %c%d", &var1, &idx1) != 2)
            error("comando invalido", line);
          
          printf("%d %c%d < %c%d\n", line, var0, idx0, var1, idx1);
          //se for constante na direita (funcionando)
          if (var1 == '$')
          {
            // se for v1 na esquerda
            if (idx0 == 1)
            {
              // escreve no codigo
              codigo[end] = 0xc7;
              end++;
              codigo[end] = 0x45;
              end++;
              codigo[end] = 0xfc;
              end++;
              // escreve o valor da constante em ate 4 bytes com for
              for (i = 0; i < 4; i++)
              {
                codigo[end] = idx1 >> (i*8);
                // idx1= 87 65 43 21 >> (0*2) = 21
                // idx1= 87 65 43 21 >> (1*2) = 00 87 65 43 || 21
                // idx1= 87 65 43 21 >> (2*2) = 00 00 87 65 || 43 21
                // idx1= 87 65 43 21 >> (3*2) = 00 00 00 87 || 65 43 21
                end++;
              }
            }
            // se for v2 na esquerda
            if (idx0 == 2)
            {
              // escreve no codigo
              codigo[end] = 0xc7;
              end++;
              codigo[end] = 0x45;
              end++;
              codigo[end] = 0xf8;
              end++;
              // escreve o valor da constante em ate 4 bytes com for
              for (i = 0; i < 4; i++)
              {
                codigo[end] = idx1 >> (i*8);
                end++;
              }
            }
            // se for v3 na esquerda
            if (idx0 == 3)
            {
              // escreve no codigo
              codigo[end] = 0xc7;
              end++;
              codigo[end] = 0x45;
              end++;
              codigo[end] = 0xf4;
              end++;
              // escreve o valor da constante em ate 4 bytes com for
              for (i = 0; i < 4; i++)
              {
                codigo[end] = idx1 >> (i*8);
                end++;
              }
            }
            // se for v4 na esquerda
            if (idx0 == 4)
            {
              // escreve no codigo
              codigo[end] = 0xc7;
              end++;
              codigo[end] = 0x45;
              end++;
              codigo[end] = 0xf0;
              end++;
              // escreve o valor da constante em ate 4 bytes com for
              for (i = 0; i < 4; i++)
              {
                codigo[end] = idx1 >> (i*8);
                end++;
              }
            }
            // se for v5 na esquerda
            if (idx0 == 5)
            {
              // escreve no codigo
              codigo[end] = 0xc7;
              end++;
              codigo[end] = 0x45;
              end++;
              codigo[end] = 0xec;
              end++;
              // escreve o valor da constante em ate 4 bytes com for
              for (i = 0; i < 4; i++)
              {
                codigo[end] = idx1 >> (i*8);
                end++;
              }
            }
          }


          // se for variavel na direita (funcionando)
          if (var1 == 'v')
          {
            // se for v1 na esquerda
            if (idx0 == 1)
            {
              // se for v1 na direita
              if (idx1 == 1)
              {
                // v1 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xfc;
                end++;
              }
              // se for v2 na direita
              if (idx1 == 2)
              {
                // v2 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf8;
                end++;
              }
              // se for v3 na direita
              if (idx1 == 3)
              {
                // v3 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf4;
                end++;
              }
              // se for v4 na direita
              if (idx1 == 4)
              {
                // v4 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf0;
                end++;
              }
              // se for v5 na direita
              if (idx1 == 5)
              {
                // v5 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xec;
                end++;
              }
              // eax para v1 (acontece em todos os casos com mesmo codigo)
                codigo[end] = 0x89;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xfc;
                end++;
            }

            // se for v2 na esquerda
            if (idx0 == 2)
            {
              // se for v1 na direita
              if (idx1 == 1)
              {
                // v1 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xfc;
                end++;
              }
              // se for v2 na direita
              if (idx1 == 2)
              {
                  // v2 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf8;
                end++;

              }
              // se for v3 na direita
              if (idx1 == 3)
              {
                // v3 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf4;
                end++;
              }
              // se for v4 na direita
              if (idx1 == 4)
              {
                // v4 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf0;
                end++;
              }
              // se for v5 na direita
              if (idx1 == 5)
              {
                // v5 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xec;
                end++;
              }
              // eax para v2 (acontece em todos os casos com mesmo codigo)
                codigo[end] = 0x89;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf8;
                end++;
            }
            
            // se for v3 na esquerda
            if (idx0 == 3)
            {
              // se for v1 na direita
              if (idx1 == 1)
              {
                // v1 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xfc;
                end++;
              }
              // se for v2 na direita
              if (idx1 == 2)
              {
                // v2 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf8;
                end++;
              }
              // se for v3 na direita
              if (idx1 == 3)
              {
                // v3 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf4;
                end++;
              }
              // se for v4 na direita
              if (idx1 == 4)
              {
                // v4 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf0;
                end++;
              }
              // se for v5 na direita
              if (idx1 == 5)
              {
                // v5 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xec;
                end++;
              }
              // eax para v3 (acontece em todos os casos com mesmo codigo)
                codigo[end] = 0x89;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf4;
                end++;
            }

            // se for v4 na esquerda
            if (idx0 == 4)
            {
              // se for v1 na direita
              if (idx1 == 1)
              {
                // v1 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xfc;
                end++;
              }
              // se for v2 na direita
              if (idx1 == 2)
              {
                // v2 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf8;
                end++;
              }
              // se for v3 na direita
              if (idx1 == 3)
              {
                // v3 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf4;
                end++;
              }
              // se for v4 na direita
              if (idx1 == 4)
              {
                // v4 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf0;
                end++;
              }
              // se for v5 na direita
              if (idx1 == 5)
              {
                // v5 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xec;
                end++;
              }
              // eax para v4 (acontece em todos os casos com mesmo codigo)
                codigo[end] = 0x89;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf0;
                end++;
            }
            // se for v5 na esquerda
            if (idx0 == 5)
            {
              // se for v1 na direita
              if (idx1 == 1)
              {
                // v1 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xfc;
                end++;
              }
              // se for v2 na direita
              if (idx1 == 2)
              {
                // v2 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf8;
                end++;
              }
              // se for v3 na direita
              if (idx1 == 3)
              {
                // v3 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf4;
                end++;
              }
              // se for v4 na direita
              if (idx1 == 4)
              {
                // v4 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xf0;
                end++;
              }
              // se for v5 na direita
              if (idx1 == 5)
              {
                // v5 para eax
                codigo[end] = 0x8b;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xec;
                end++;
              }
              // eax para v5 (acontece em todos os casos com mesmo codigo)
                codigo[end] = 0x89;
                end++;
                codigo[end] = 0x45;
                end++;
                codigo[end] = 0xec;
                end++;
            }
          
        }

        // se for parametro na direita (funcionando)
        if (var1 == 'p')
        {
          // se for v1 na esquerda
          if (idx0 == 1)
          {
            // se for p1 na direita
            if (idx1 == 1)
            {
              // p1 para v1
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x7d;
              end++;
              codigo[end] = 0xfc;
              end++;
            }
            // se for p2 na direita
            if (idx1 == 2)
            {
              // p2 para v1
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x75;
              end++;
              codigo[end] = 0xfc;
              end++;
            }
            // se for p3 na direita
            if (idx1 == 3)
            {
              // p3 para v1
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x55;
              end++;
              codigo[end] = 0xfc;
              end++;
            }
          }

          // se for v2 na esquerda
          if (idx0 == 2)
          {
            // se for p1 na direita
            if (idx1 == 1)
            {
              // p1 para v2
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x7d;
              end++;
              codigo[end] = 0xf8;
              end++;
            }
            // se for p2 na direita
            if (idx1 == 2)
            {
              // p2 para v2
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x75;
              end++;
              codigo[end] = 0xf8;
              end++;
            }
            // se for p3 na direita
            if (idx1 == 3)
            {
              // p3 para v2
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x55;
              end++;
              codigo[end] = 0xf8;
              end++;
            }
          }
          
          // se for v3 na esquerda
          if (idx0 == 3)
          {
            // se for p1 na direita
            if (idx1 == 1)
            {
              // p1 para v3
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x7d;
              end++;
              codigo[end] = 0xf4;
              end++;
            }
            // se for p2 na direita
            if (idx1 == 2)
            {
              // p2 para v3
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x75;
              end++;
              codigo[end] = 0xf4;
              end++;
            }
            // se for p3 na direita
            if (idx1 == 3)
            {
              // p3 para v3
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x55;
              end++;
              codigo[end] = 0xf4;
              end++;
            }
          }
          // se for v4 na esquerda
          if (idx0 == 4)
          {
            // se for p1 na direita
            if (idx1 == 1)
            {
              // p1 para v4
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x7d;
              end++;
              codigo[end] = 0xf0;
              end++;
            }
            // se for p2 na direita
            if (idx1 == 2)
            {
              // p2 para v4
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x75;
              end++;
              codigo[end] = 0xf0;
              end++;
            }
            //se for p3 na direita
            if (idx1 == 3)
            {
              // p3 para v4
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x55;
              end++;
              codigo[end] = 0xf0;
              end++;
            }
          }

        // se for v5 na esquerda
          if (idx0 == 5)
          {
            // se for p1 na direita
            if (idx1 == 1)
            {
              // p1 para v5
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x7d;
              end++;
              codigo[end] = 0xec;
              end++;
            }
            // se for p2 na direita
            if (idx1 == 2)
            {
              // p2 para v5
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x75;
              end++;
              codigo[end] = 0xec;
              end++;
            }
            // se for p3 na direita
            if (idx1 == 3)
            {
              // p3 para v5
              codigo[end] = 0x89;
              end++;
              codigo[end] = 0x55;
              end++;
              codigo[end] = 0xec;
              end++;
            }
          }
        }
      }
        else { /* operaÃ§Ã£o aritmÃ©tica */ // ariel implemente do seu jeito aqui
          char var2, op;
          int idx2;
          if (c0 != '=')
            error("comando invalido", line);
          if (fscanf(f, " %c%d %c %c%d", &var1, &idx1, &op, &var2, &idx2) != 5)
            error("comando invalido", line);
          printf("%d %c%d = %c%d %c %c%d\n", line, var0, idx0, var1, idx1, op, var2, idx2);
          
          /* 4 posibilidades: var1 = v e var2 = v, var1 = $ e var2 = v, var1 = v e var2 = $, var1 = $ e var2 = $. tal que $ = constante e v = variavel. */

          if (var1 == 'v' && var2 == 'v') { // se for variavel e variavel

            // Copia o valor da variavel 1 (Na memóriaaaa thcetcherereee) para o registrador %r10d
            offsetMem = -4 * idx1;
            adicionarInstrucao(codigo, "MOVLM10", &end);
            codigo[end] = offsetMem;
            end++;

            // Copia o valor da variavel 2 (Na memóriaaaa thcetcherereee) para o registrador %r11d
            offsetMem = -4 * idx2;
            adicionarInstrucao(codigo, "MOVLM11", &end);
            codigo[end] = offsetMem;
            end++;

          }

          else if (var1 == 'v' && var2 == '$') {
            // Copia o valor da variavel 1 (Na memóriaaaa thcetcherereee) para o registrador %r10d
            offsetMem = -4 * idx1;
            adicionarInstrucao(codigo, "MOVLM10", &end);
            codigo[end] = offsetMem;
            end++;

            // Copia o valor da constante 2 para o registrador %r11d  
            adicionarInstrucao(codigo, "MOVLV11", &end);
            escreveLittleEndian(idx2, codigo, &end);
          }

          else if (var1 == '$' && var2 == 'v') {
            // Copia o valor da constante 2 para o registrador %r11d  
            adicionarInstrucao(codigo, "MOVLV10", &end);
            escreveLittleEndian(idx1, codigo, &end);

            // Copia o valor da constante 2 para o registrador %r11d
            offsetMem = -4 * idx2;
            adicionarInstrucao(codigo, "MOVLM11", &end);
            codigo[end] = offsetMem;
            end++;
          }

          else if (var1 == '$' && var2 == '$') {
            // Copia o valor da constante 2 para o registrador %r11d  
            adicionarInstrucao(codigo, "MOVLV10", &end);
            escreveLittleEndian(idx1, codigo, &end);

            // Copia o valor da constante 2 para o registrador %r11d  
            adicionarInstrucao(codigo, "MOVLV11", &end);
            escreveLittleEndian(idx2, codigo, &end);
          }

          else {
            error("comando invalido", line);
          }

           // Faz a operação aritmética
          switch (op) {
            case '+': {
              adicionarInstrucao(codigo, "ADDL", &end);
              break;
            }

            case '-': {
              adicionarInstrucao(codigo, "SUBL", &end);
              
              adicionarInstrucao(codigo, "N1IMULL", &end); //Primeiro faz a subtração de r10d por r11d (guarda o resultado em r11d), depois multiplica r11d por -1 para ajeitar o sinal do resultado
              break;
            }

            case '*': {
              adicionarInstrucao(codigo, "IMULL", &end);
              break;
            }

            default: error("comando desconhecido", line);
            }

            // Copia o valor do registrador %r11d para a variavel 0 (Na memóriaaaa thcetcherereee) (Salva %r11d na memória)
            offsetMem = -4 * idx0;
            adicionarInstrucao(codigo, "MOVL11M", &end);
            codigo[end] = offsetMem;
            end++;
        }
        
        break;
      }
      case 'i': { /* desvio condicional (a fazer, ariel)*/ // thiago vai adiantando essa parte aqui
        char var0;
        int idx0, n;
        if (fscanf(f, "flez %c%d %d", &var0, &idx0, &n) != 3) {
            error("comando invalido", line);
        }
        
        printf("%d iflez %c%d %d\n", line, var0, idx0, n);
        //printf("\nPrintando valores:\tvar0: %c, idx0: %d, n: %d, line:\n", var0, idx0, n, line);

        // Copia o valor da variavel (ou constante) para o registrador %r10d
        switch(var0) {
          case 'v': {
            offsetMem = -4 * idx0;
            adicionarInstrucao(codigo, "MOVLM10", &end);
            codigo[end] = offsetMem;
            end++;
            break;
          }
          case '$': {
            adicionarInstrucao(codigo, "MOVLV10", &end);
            escreveLittleEndian(idx0, codigo, &end);
            break;
          }

          default: error("comando desconhecido", line);
        }

        adicionarInstrucao(codigo, "CMPL010", &end); // Compara o valor de r10d com 0
        adicionarInstrucao(codigo, "JLE", &end); // Se r10d <= 0, pula para a linha n

        labelPulaLinha[sizeLabelPulaLinha].linha = n;
        labelPulaLinha[sizeLabelPulaLinha].indiceOffset = end;

        sizeLabelPulaLinha++;
        end++; // Pula o byte do offset

        break;
      }
      default: error("comando desconhecido", line);
    }

    line++;

    indiceByte[line] = end + 1; // Coloca a posição do byte de ínicio da próxima linha do array codigo no array indiceByte

    fscanf(f, " ");
  }//fim do arquivo
  
  qntLinhas = line; // Guarda a quantidade de linhas do arquivo (informação em que estava em line e que vai se perder em seguida)
  
  

  line = 0;
  
  // Preenche o array codigo com as labels corretas
  for (i = 0; i < sizeLabelPulaLinha; i++) {

    line = labelPulaLinha[i].linha; //O valor n lido de cada linha com iflez
    indiceOffset = labelPulaLinha[i].indiceOffset; //O indice do byte de offset da linha com iflez

    indiceByteLinha = indiceByte[line]; //O indice do byte de inicio da linha que o iflez vai desviar para

    offsetMem = indiceByteLinha - indiceOffset - 2; //Calcula o indice do offset fazendo indiceoffset - indiceByteLinha

    codigo[indiceOffset] = offsetMem; // Coloca o offset no array codigo

  }

  imprimirCodigoMaquina(codigo, indiceByte, qntLinhas);
  // retornar para funcaoSimples do tipo funcp
  return (funcp)codigo;
}