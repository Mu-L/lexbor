/*
 * Copyright (C) 2019-2024 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/encoding/encoding.h"
#include "lexbor/encoding/encode.h"
#include "lexbor/encoding/multi.h"


#define append_to_file(fc, data, len, codepoint)                               \
    do {                                                                       \
        lxb_char_t *data_to = data;                                            \
                                                                               \
        for (size_t j = 0; j < len; j++) {                                     \
            fprintf(fc, "\\x%02X", (unsigned) data_to[j]);                     \
        }                                                                      \
                                                                               \
        fprintf(fc, " 0x%04X\n", (unsigned) codepoint);                        \
    }                                                                          \
    while (0)

#define encode_to_file(fc, _cp)                                                \
    do {                                                                       \
        const lxb_codepoint_t *cps = &_cp;                                     \
                                                                               \
        lxb_encoding_encode_init(&ctx, enc_data, data, sizeof(data));          \
                                                                               \
        status = enc_data->encode(&ctx, &cps, (cps + 1));                      \
        if (status != LXB_STATUS_OK) {                                         \
            printf("Failed to encoding code point: %u\n", _cp);                \
            return EXIT_FAILURE;                                               \
        }                                                                      \
                                                                               \
        status = lxb_encoding_encode_finish(&ctx);                             \
        if (status != LXB_STATUS_OK) {                                         \
            printf("Failed to encoding code point: %u\n", _cp);                \
            return EXIT_FAILURE;                                               \
        }                                                                      \
                                                                               \
        append_to_file(fc, ctx.buffer_out, ctx.buffer_used, _cp);              \
    }                                                                          \
    while (0)


int main(int argc, const char * argv[])
{
    size_t size;
    lxb_char_t data[12];
    lxb_status_t status;
    lxb_codepoint_t cp;
    lxb_encoding_encode_t ctx;
    const lxb_encoding_data_t *enc_data;

    const char *filepath = "./iso_2022_jp_map_decode.txt";

    enc_data = lxb_encoding_data(LXB_ENCODING_ISO_2022_JP);

    FILE *fc = fopen(filepath, "w");
    if (fc == NULL) {
        printf("Failed to opening file: %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    fprintf(fc, "#\n"
            "# Copyright (C) 2019-2024 Alexander Borisov\n"
            "#\n"
            "# Author: Alexander Borisov <borisov@lexbor.com>\n"
            "#\n\n");

    fprintf(fc, "#\n"
            "# This file generated by the program\n"
            "# \"utils/lexbor/encoding/iso_2022_jp_map_decode.c\"\n"
            "#\n\n");

    for (lxb_codepoint_t i = 0x00; i < 0x80; i++) {
        if (i == 0x0E || i == 0x0F || i == 0x1B) {
            continue;
        }

        encode_to_file(fc, i);
    }

    /* Single index */
    size = sizeof(lxb_encoding_multi_iso_2022_jp_katakana_map)
           / sizeof(lxb_codepoint_t);

    for (size_t i = 0; i < size; i++) {
        cp = lxb_encoding_multi_iso_2022_jp_katakana_map[i];

        if (cp == LXB_ENCODING_ERROR_CODEPOINT) {
            continue;
        }

        encode_to_file(fc, cp);
    }

    size = sizeof(lxb_encoding_multi_jis0208_map) / sizeof(lxb_codepoint_t);

    for (size_t i = 0; i < size; i++) {
        cp = lxb_encoding_multi_jis0208_map[i];

        if (cp == LXB_ENCODING_ERROR_CODEPOINT) {
            continue;
        }

        encode_to_file(fc, cp);
    }

    cp = 0xFF0D; encode_to_file(fc, cp);
    cp = 0x203E; encode_to_file(fc, cp);
    cp = 0x00A5; encode_to_file(fc, cp);

    fprintf(fc, "\n# END\n");

    fclose(fc);

    return EXIT_SUCCESS;
}
