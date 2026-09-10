/*
 * common.c - Implementacao das funcoes compartilhadas entre rsa.c e aes.c.
 * Ver common.h para a documentacao de cada funcao.
 */

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *gerar_mensagem_deterministica(size_t tamanho) {
    static const char alfabeto[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char *msg = malloc(tamanho + 1);
 
    unsigned int semente = (unsigned int)tamanho;
 
    for (size_t i = 0; i < tamanho; i++) {
        msg[i] = alfabeto[rand_r(&semente) % (sizeof(alfabeto) - 1)];
    }
    msg[tamanho] = '\0';
    return msg;
}


double cronometro_agora(void) {
    return (double)clock() / (double)CLOCKS_PER_SEC;
}

void salvar_resultado_csv(const char *arquivo, const char *algoritmo,
                           size_t n, double tempo_cifra_ms, double tempo_decifra_ms) {
    /* Verifica se o arquivo ja existe e tem conteudo, para decidir se
       precisa escrever o cabecalho */
    FILE *teste = fopen(arquivo, "r");
    int precisa_cabecalho = 1;
    if (teste != NULL) {
        fseek(teste, 0, SEEK_END);
        if (ftell(teste) > 0) precisa_cabecalho = 0;
        fclose(teste);
    }

    FILE *f = fopen(arquivo, "a");
    if (!f) {
        fprintf(stderr, "Aviso: nao foi possivel abrir '%s' para escrita.\n", arquivo);
        return;
    }

    if (precisa_cabecalho) {
        fprintf(f, "algoritmo,n,tempo_cifra_ms,tempo_decifra_ms\n");
    }
    fprintf(f, "%s,%zu,%.6f,%.6f\n", algoritmo, n, tempo_cifra_ms, tempo_decifra_ms);
    fclose(f);
}