/*
 * common.h - Funcoes compartilhadas entre rsa.c e aes.c para o
 * experimento de analise empirica (Estrutura de Dados Basicas II).
 *
 * Centralizar essas funcoes aqui garante que:
 *   1. O mesmo valor de n gera EXATAMENTE a mesma mensagem nos dois
 *      algoritmos (comparacao justa: mesma entrada para os dois).
 *   2. A forma de medir tempo e salvar resultados eh identica para
 *      RSA e AES, evitando qualquer vies na metodologia.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

char *gerar_mensagem_deterministica(size_t tamanho);

double cronometro_agora(void);

void salvar_resultado_csv(const char *arquivo, const char *algoritmo,
                           size_t n, double tempo_cifra_ms, double tempo_decifra_ms);

#endif 