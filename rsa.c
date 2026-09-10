#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gmp.h>
#include "common.h"

void algoritmo_euclides_estendido(mpz_t d, mpz_t x, mpz_t y, const mpz_t a, const mpz_t b) {
    if (mpz_cmp_ui(b, 0) == 0) {
        mpz_set(d, a);
        mpz_set_ui(x, 1);
        mpz_set_ui(y, 0);
        return;
    }

    mpz_t x2, x1, y2, y1, aa, bb, q, r, xx, yy, temp;
    mpz_inits(x2, x1, y2, y1, aa, bb, q, r, xx, yy, temp, NULL);

    mpz_set_ui(x2, 1); mpz_set_ui(x1, 0);
    mpz_set_ui(y2, 0); mpz_set_ui(y1, 1);
    mpz_set(aa, a);
    mpz_set(bb, b);

    while (mpz_cmp_ui(bb, 0) > 0) {
        mpz_fdiv_q(q, aa, bb);
        mpz_fdiv_r(r, aa, bb);

        mpz_mul(temp, q, x1);
        mpz_sub(xx, x2, temp);

        mpz_mul(temp, q, y1);
        mpz_sub(yy, y2, temp);

        mpz_set(aa, bb);
        mpz_set(bb, r);
        mpz_set(x2, x1); mpz_set(x1, xx);
        mpz_set(y2, y1); mpz_set(y1, yy);
    }

    mpz_set(d, aa);
    mpz_set(x, x2);
    mpz_set(y, y2);

    mpz_clears(x2, x1, y2, y1, aa, bb, q, r, xx, yy, temp, NULL);
}

void potencia_modular(mpz_t result, const mpz_t base_in, const mpz_t exp_in, const mpz_t mod) {
    mpz_t base, exp;
    mpz_inits(base, exp, NULL);

    mpz_set_ui(result, 1);
    mpz_mod(base, base_in, mod);
    mpz_set(exp, exp_in);

    while (mpz_cmp_ui(exp, 0) > 0) {
        if (mpz_odd_p(exp)) {
            mpz_mul(result, result, base);
            mpz_mod(result, result, mod);
        }
        mpz_fdiv_q_2exp(exp, exp, 1);
        mpz_mul(base, base, base);
        mpz_mod(base, base, mod);
    }

    mpz_clears(base, exp, NULL);
}

void gerar_primo(mpz_t primo, gmp_randstate_t state, unsigned long bits) {
    mpz_urandomb(primo, state, bits);
    mpz_setbit(primo, bits - 1);
    mpz_setbit(primo, 0);
    mpz_nextprime(primo, primo);
}

void gerar_chaves(mpz_t e, mpz_t d, mpz_t n, unsigned long bits_modulo, gmp_randstate_t state) {
    mpz_t p, q, phi_n, s1, s2, gcd_result, mdc_ed, x, y;
    mpz_inits(p, q, phi_n, s1, s2, gcd_result, mdc_ed, x, y, NULL);

    gerar_primo(p, state, bits_modulo / 2);
    gerar_primo(q, state, bits_modulo / 2);

    mpz_mul(n, p, q);

    mpz_sub_ui(s1, p, 1);
    mpz_sub_ui(s2, q, 1);
    mpz_mul(phi_n, s1, s2);

    mpz_set_ui(e, 65537);
    mpz_gcd(gcd_result, e, phi_n);
    while (mpz_cmp_ui(gcd_result, 1) != 0) {
        mpz_nextprime(e, e);
        mpz_gcd(gcd_result, e, phi_n);
    }

    algoritmo_euclides_estendido(mdc_ed, x, y, e, phi_n);
    mpz_mod(d, x, phi_n);
    if (mpz_cmp_ui(d, 0) < 0) {
        mpz_add(d, d, phi_n);
    }

    mpz_clears(p, q, phi_n, s1, s2, gcd_result, mdc_ed, x, y, NULL);
}

void bytes_para_mpz(mpz_t resultado, const unsigned char *bytes, size_t tamanho) {
    mpz_import(resultado, tamanho, 1, 1, 0, 0, bytes);
}

void mpz_para_bytes(unsigned char *bytes, size_t tamanho, const mpz_t valor) {
    memset(bytes, 0, tamanho);
    if (mpz_cmp_ui(valor, 0) == 0) return;

    size_t count;
    size_t tamanho_real = (mpz_sizeinbase(valor, 2) + 7) / 8;
    unsigned char *temp = malloc(tamanho_real);
    mpz_export(temp, &count, 1, 1, 0, 0, valor);
    memcpy(bytes + (tamanho - count), temp, count);
    free(temp);
}

size_t calcular_tamanho_bloco(unsigned long bits_modulo) {
    unsigned long margem_bits = 16;
    return (bits_modulo - margem_bits) / 8;
}

void rsa_cifrar_mensagem(const unsigned char *msg, size_t tamanho, size_t tamanho_bloco,
                          const mpz_t e, const mpz_t n, mpz_t *cifrado, size_t num_blocos) {
    mpz_t m;
    mpz_init(m);
    for (size_t b = 0; b < num_blocos; b++) {
        size_t offset = b * tamanho_bloco;
        size_t restante = tamanho - offset;
        size_t tam_atual = restante < tamanho_bloco ? restante : tamanho_bloco;

        bytes_para_mpz(m, msg + offset, tam_atual);
        potencia_modular(cifrado[b], m, e, n);
    }
    mpz_clear(m);
}
void rsa_decifrar_mensagem(const mpz_t *cifrado, size_t num_blocos, size_t tamanho_bloco,
                            size_t tamanho_total, const mpz_t d, const mpz_t n,
                            unsigned char *saida) {
    mpz_t m;
    mpz_init(m);
    for (size_t b = 0; b < num_blocos; b++) {
        size_t offset = b * tamanho_bloco;
        size_t restante = tamanho_total - offset;
        size_t tam_atual = restante < tamanho_bloco ? restante : tamanho_bloco;

        potencia_modular(m, cifrado[b], d, n);
        mpz_para_bytes(saida + offset, tam_atual, m);
    }
    mpz_clear(m);
}

int main(int argc, char *argv[]) {
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, (unsigned long)time(NULL));

    unsigned long bits_modulo = 1024; // valor testado no artigo

    mpz_t e, d, n;
    mpz_inits(e, d, n, NULL);

    printf("Gerando chaves RSA de %lu bits...\n", bits_modulo);
    gerar_chaves(e, d, n, bits_modulo, state);

    gmp_printf("Chave publica  (e, n) = (%Zd, %Zd)\n\n", e, n);
    gmp_printf("Chave privada  (d, n) = (%Zd, %Zd)\n\n", d, n);

    char *mensagem_gerada = NULL;
    const char *mensagem;
    int modo_silencioso = 0;
    const char *arquivo_csv = NULL;

    // captura dos argumentos da linha de comando
    if (argc >= 3 && strcmp(argv[1], "-n") == 0) {
        size_t tamanho_pedido = (size_t)atol(argv[2]);
        mensagem_gerada = gerar_mensagem_deterministica(tamanho_pedido);
        mensagem = mensagem_gerada;
        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-q") == 0) modo_silencioso = 1;
            if (strcmp(argv[i], "--csv") == 0 && i + 1 < argc) arquivo_csv = argv[i + 1];
        }
        printf("Mensagem deterministica gerada com %zu caracteres.\n", tamanho_pedido);
    } else if (argc >= 2 && argv[1][0] != '-') {
        mensagem = argv[1];
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--csv") == 0 && i + 1 < argc) arquivo_csv = argv[i + 1];
        }
    } else {
        mensagem = "RSA"; //valor padrão para testes apenas
    }

    size_t tamanho = strlen(mensagem);

    size_t tamanho_bloco = calcular_tamanho_bloco(bits_modulo);
    size_t num_blocos = (tamanho + tamanho_bloco - 1) / tamanho_bloco;
    if (num_blocos == 0) num_blocos = 1;

    printf("Tamanho do bloco calculado: %zu bytes (modulo de %lu bits)\n", tamanho_bloco, bits_modulo);
    printf("Mensagem de %zu bytes sera dividida em %zu bloco(s)\n\n", tamanho, num_blocos);

    mpz_t *cifrado = malloc(num_blocos * sizeof(mpz_t));
    for (size_t i = 0; i < num_blocos; i++) mpz_init(cifrado[i]);

    if (!modo_silencioso) printf("Cifrando mensagem \"%s\"...\n", mensagem);
    else printf("Cifrando mensagem de %zu caracteres...\n", tamanho);

    double t0 = cronometro_agora();
    rsa_cifrar_mensagem((const unsigned char *)mensagem, tamanho, tamanho_bloco, e, n, cifrado, num_blocos);
    double tempo_cifra_ms = (cronometro_agora() - t0) * 1000.0;

    if (!modo_silencioso) {
        for (size_t i = 0; i < num_blocos; i++) {
            gmp_printf("  bloco %zu -> c = %Zd\n", i, cifrado[i]);
        }
    }

    unsigned char *resultado = malloc(tamanho + 1);
    if (!modo_silencioso) printf("\nDecifrando...\n");

    double t1 = cronometro_agora();
    rsa_decifrar_mensagem(cifrado, num_blocos, tamanho_bloco, tamanho, d, n, resultado);
    double tempo_decifra_ms = (cronometro_agora() - t1) * 1000.0;

    resultado[tamanho] = '\0';

    if (!modo_silencioso) printf("\nMensagem recuperada: \"%s\"\n", resultado);
    printf(strcmp((char *)resultado, mensagem) == 0 ? "OK: mensagem recuperada corretamente.\n"
                                                      : "ERRO: mensagem nao confere!\n");

    printf("\nTempo de cifragem : %.3f ms\n", tempo_cifra_ms);
    printf("Tempo de decifragem: %.3f ms\n", tempo_decifra_ms);

    if (arquivo_csv != NULL) {
        salvar_resultado_csv(arquivo_csv, "RSA", tamanho, tempo_cifra_ms, tempo_decifra_ms);
        printf("\nResultado salvo em '%s'\n", arquivo_csv);
    }

    for (size_t i = 0; i < num_blocos; i++) mpz_clear(cifrado[i]);
    free(cifrado);
    free(resultado);
    if (mensagem_gerada) free(mensagem_gerada);
    mpz_clears(e, d, n, NULL);
    gmp_randclear(state);

    return 0;
}
