#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "common.h"

#define NB 4
#define NK 4
#define NR 10
#define TAM_CHAVE 16
#define TAM_BLOCO 16

static const unsigned char sbox[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static const unsigned char inv_sbox[256] = {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static const unsigned char rcon[11] = {
    0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1B,0x36
};

typedef unsigned char estado_t[4][NB];

unsigned char gmul(unsigned char a, unsigned char b) {
    unsigned char p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        unsigned char hi_bit_set = (a & 0x80);
        a <<= 1;
        if (hi_bit_set) a ^= 0x1B;
        b >>= 1;
    }
    return p;
}

void sub_bytes(estado_t estado) {
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < NB; c++)
            estado[r][c] = sbox[estado[r][c]];
}

void inv_sub_bytes(estado_t estado) {
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < NB; c++)
            estado[r][c] = inv_sbox[estado[r][c]];
}

void shift_rows(estado_t estado) {
    unsigned char temp;
    temp = estado[1][0];
    estado[1][0] = estado[1][1];
    estado[1][1] = estado[1][2];
    estado[1][2] = estado[1][3];
    estado[1][3] = temp;

    unsigned char t0 = estado[2][0], t1 = estado[2][1];
    estado[2][0] = estado[2][2]; estado[2][1] = estado[2][3];
    estado[2][2] = t0; estado[2][3] = t1;

    temp = estado[3][3];
    estado[3][3] = estado[3][2];
    estado[3][2] = estado[3][1];
    estado[3][1] = estado[3][0];
    estado[3][0] = temp;
}

void inv_shift_rows(estado_t estado) {
    unsigned char temp;

    temp = estado[1][3];
    estado[1][3] = estado[1][2];
    estado[1][2] = estado[1][1];
    estado[1][1] = estado[1][0];
    estado[1][0] = temp;

    unsigned char t0 = estado[2][0], t1 = estado[2][1];
    estado[2][0] = estado[2][2]; estado[2][1] = estado[2][3];
    estado[2][2] = t0; estado[2][3] = t1;

    temp = estado[3][0];
    estado[3][0] = estado[3][1];
    estado[3][1] = estado[3][2];
    estado[3][2] = estado[3][3];
    estado[3][3] = temp;
}

void mix_columns(estado_t estado) {
    for (int c = 0; c < NB; c++) {
        unsigned char a0 = estado[0][c], a1 = estado[1][c];
        unsigned char a2 = estado[2][c], a3 = estado[3][c];

        estado[0][c] = (unsigned char)(gmul(a0,2) ^ gmul(a1,3) ^ a2 ^ a3);
        estado[1][c] = (unsigned char)(a0 ^ gmul(a1,2) ^ gmul(a2,3) ^ a3);
        estado[2][c] = (unsigned char)(a0 ^ a1 ^ gmul(a2,2) ^ gmul(a3,3));
        estado[3][c] = (unsigned char)(gmul(a0,3) ^ a1 ^ a2 ^ gmul(a3,2));
    }
}

void inv_mix_columns(estado_t estado) {
    for (int c = 0; c < NB; c++) {
        unsigned char a0 = estado[0][c], a1 = estado[1][c];
        unsigned char a2 = estado[2][c], a3 = estado[3][c];

        estado[0][c] = (unsigned char)(gmul(a0,14) ^ gmul(a1,11) ^ gmul(a2,13) ^ gmul(a3,9));
        estado[1][c] = (unsigned char)(gmul(a0,9)  ^ gmul(a1,14) ^ gmul(a2,11) ^ gmul(a3,13));
        estado[2][c] = (unsigned char)(gmul(a0,13) ^ gmul(a1,9)  ^ gmul(a2,14) ^ gmul(a3,11));
        estado[3][c] = (unsigned char)(gmul(a0,11) ^ gmul(a1,13) ^ gmul(a2,9)  ^ gmul(a3,14));
    }
}

void add_round_key(estado_t estado, const unsigned char *round_key) {
    for (int c = 0; c < NB; c++)
        for (int r = 0; r < 4; r++)
            estado[r][c] ^= round_key[c * 4 + r];
}

void key_expansion(const unsigned char *chave, unsigned char *chaves_expandidas) {
    unsigned char temp[4];
    memcpy(chaves_expandidas, chave, TAM_CHAVE);

    int bytes_gerados = TAM_CHAVE;
    int rcon_idx = 1;

    while (bytes_gerados < TAM_BLOCO * (NR + 1)) {
        memcpy(temp, chaves_expandidas + bytes_gerados - 4, 4);

        if (bytes_gerados % TAM_CHAVE == 0) {
            /* RotWord: rotaciona os 4 bytes */
            unsigned char t = temp[0];
            temp[0] = temp[1]; temp[1] = temp[2]; temp[2] = temp[3]; temp[3] = t;
            /* SubWord: aplica S-box em cada byte */
            for (int i = 0; i < 4; i++) temp[i] = sbox[temp[i]];
            /* XOR com a constante Rcon do round */
            temp[0] ^= rcon[rcon_idx++];
        }

        for (int i = 0; i < 4; i++)
            chaves_expandidas[bytes_gerados + i] =
                chaves_expandidas[bytes_gerados - TAM_CHAVE + i] ^ temp[i];

        bytes_gerados += 4;
    }
}

void bytes_para_estado(const unsigned char *in, estado_t estado) {
    for (int c = 0; c < NB; c++)
        for (int r = 0; r < 4; r++)
            estado[r][c] = in[c * 4 + r];
}

void estado_para_bytes(const estado_t estado, unsigned char *out) {
    for (int c = 0; c < NB; c++)
        for (int r = 0; r < 4; r++)
            out[c * 4 + r] = estado[r][c];
}

void aes_cifrar_bloco(const unsigned char *entrada, unsigned char *saida,
                       const unsigned char *chaves_expandidas) {
    estado_t estado;
    bytes_para_estado(entrada, estado);

    add_round_key(estado, chaves_expandidas + 0);

    for (int round = 1; round < NR; round++) {
        sub_bytes(estado);
        shift_rows(estado);
        mix_columns(estado);
        add_round_key(estado, chaves_expandidas + round * TAM_BLOCO);
    }

    sub_bytes(estado);
    shift_rows(estado);
    add_round_key(estado, chaves_expandidas + NR * TAM_BLOCO);

    estado_para_bytes(estado, saida);
}

void aes_decifrar_bloco(const unsigned char *entrada, unsigned char *saida,
                         const unsigned char *chaves_expandidas) {
    estado_t estado;
    bytes_para_estado(entrada, estado);

    add_round_key(estado, chaves_expandidas + NR * TAM_BLOCO);

    for (int round = NR - 1; round >= 1; round--) {
        inv_shift_rows(estado);
        inv_sub_bytes(estado);
        add_round_key(estado, chaves_expandidas + round * TAM_BLOCO);
        inv_mix_columns(estado);
    }

    inv_shift_rows(estado);
    inv_sub_bytes(estado);
    add_round_key(estado, chaves_expandidas + 0);

    estado_para_bytes(estado, saida);
}

void aes_cifrar_mensagem(const unsigned char *msg, size_t tamanho,
                          const unsigned char *chaves_expandidas,
                          unsigned char *saida, size_t *tamanho_saida) {
    size_t num_blocos = (tamanho + TAM_BLOCO - 1) / TAM_BLOCO;
    if (num_blocos == 0) num_blocos = 1;
    *tamanho_saida = num_blocos * TAM_BLOCO;

    unsigned char bloco_in[TAM_BLOCO];
    for (size_t b = 0; b < num_blocos; b++) {
        memset(bloco_in, 0, TAM_BLOCO);
        size_t offset = b * TAM_BLOCO;
        size_t restante = tamanho - offset;
        size_t copiar = restante < TAM_BLOCO ? restante : TAM_BLOCO;
        memcpy(bloco_in, msg + offset, copiar);

        aes_cifrar_bloco(bloco_in, saida + offset, chaves_expandidas);
    }
}

void aes_decifrar_mensagem(const unsigned char *cifrado, size_t tamanho,
                            const unsigned char *chaves_expandidas,
                            unsigned char *saida) {
    size_t num_blocos = tamanho / TAM_BLOCO;
    for (size_t b = 0; b < num_blocos; b++) {
        size_t offset = b * TAM_BLOCO;
        aes_decifrar_bloco(cifrado + offset, saida + offset, chaves_expandidas);
    }
}

int main(int argc, char *argv[]) {

    unsigned char chave[TAM_CHAVE] = {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
    };

    unsigned char chaves_expandidas[TAM_BLOCO * (NR + 1)];
    key_expansion(chave, chaves_expandidas);

    char *mensagem_gerada = NULL;
    const char *mensagem;
    int modo_silencioso = 0;
    const char *arquivo_csv = NULL;

    if (argc >= 3 && strcmp(argv[1], "-n") == 0) {
        size_t tamanho_pedido = (size_t)atol(argv[2]);
        mensagem_gerada = gerar_mensagem_deterministica(tamanho_pedido);
        mensagem = mensagem_gerada;
        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-q") == 0) modo_silencioso = 1;
            if (strcmp(argv[i], "--csv") == 0 && i + 1 < argc) arquivo_csv = argv[i + 1];
        }
    } else if (argc >= 2 && argv[1][0] != '-') {
        mensagem = argv[1];
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--csv") == 0 && i + 1 < argc) arquivo_csv = argv[i + 1];
        }
    } else {
        mensagem = "This is a test...";
    }

    size_t tamanho = strlen(mensagem);

    size_t tamanho_max = ((tamanho / TAM_BLOCO) + 2) * TAM_BLOCO;
    unsigned char *cifrado = malloc(tamanho_max);
    unsigned char *decifrado = malloc(tamanho_max);
    size_t tamanho_cifrado;

    if (!modo_silencioso)
        printf("Mensagem original : \"%s\" (%zu bytes)\n", mensagem, tamanho);
    else
        printf("Mensagem original : %zu bytes\n", tamanho);

    double t0 = cronometro_agora();
    aes_cifrar_mensagem((const unsigned char *)mensagem, tamanho,
                         chaves_expandidas, cifrado, &tamanho_cifrado);
    double tempo_cifra_ms = (cronometro_agora() - t0) * 1000.0;

    if (!modo_silencioso) {
        printf("Mensagem cifrada  : ");
        for (size_t i = 0; i < tamanho_cifrado; i++) printf("%02x ", cifrado[i]);
        printf("\n");
    }

    double t1 = cronometro_agora();
    aes_decifrar_mensagem(cifrado, tamanho_cifrado, chaves_expandidas, decifrado);
    double tempo_decifra_ms = (cronometro_agora() - t1) * 1000.0;

    decifrado[tamanho] = '\0';

    if (!modo_silencioso)
        printf("Mensagem decifrada: \"%s\"\n", (char *)decifrado);

    printf(memcmp(mensagem, decifrado, tamanho) == 0
               ? "OK: mensagem recuperada corretamente.\n"
               : "ERRO: mensagem nao confere!\n");

    printf("\nTempo de cifragem : %.3f ms\n", tempo_cifra_ms);
    printf("Tempo de decifragem: %.3f ms\n", tempo_decifra_ms);

    if (arquivo_csv != NULL) {
        salvar_resultado_csv(arquivo_csv, "AES", tamanho, tempo_cifra_ms, tempo_decifra_ms);
        printf("\nResultado salvo em '%s'\n", arquivo_csv);
    }

    free(cifrado);
    free(decifrado);
    if (mensagem_gerada) free(mensagem_gerada);

    return 0;
}
