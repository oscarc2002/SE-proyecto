#include "paquete.h"

uint32_t crc32_le(paquete_t *pkt, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    char *data = (char *) pkt->init_pkt;

    const uint32_t poly = 0x04C11DB7;

    for (size_t i = 0; i < sizeof(paquete_t)/8 -4; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ ((crc & 1) ? poly : 0);
        }
    }

    return crc ^ 0xFFFFFFFF;
}
