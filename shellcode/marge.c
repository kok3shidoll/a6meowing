#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int openFile(char *file, size_t *sz, unsigned char **buf) {
    FILE *fd = fopen(file, "r");
    if (!fd) {
        printf("error opening %s\n", file);
        return -1;
    }
    
    fseek(fd, 0, SEEK_END);
    *sz = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    
    *buf = malloc(*sz);
    if (!*buf) {
        printf("error allocating file buffer\n");
        fclose(fd);
        return -1;
    }
    
    fread(*buf, *sz, 1, fd);
    fclose(fd);
    
    return 0;
}

int main(int argc, char **argv) {
    
    if(argc != 4) {
        printf("%s <in1> <in2> <out>\n", argv[0]);
        return 0;
    }
    
    char *entryPath = argv[1];
    char *payloadPath = argv[2];
    char *outFilePath = argv[3];
    
    unsigned char *entryBuf = NULL;
    unsigned char *payloadBuf = NULL;
    size_t entrySize = 0;
    size_t payloadSize = 0;
    
    
    openFile(entryPath, &entrySize, &entryBuf);
    assert((entrySize < 0x300) && entryBuf);
    
    openFile(payloadPath, &payloadSize, &payloadBuf);
    assert((payloadSize > 0x300) && (payloadSize < 0x1400) && payloadBuf);
    
    size_t outSize = payloadSize;
    assert((outSize > 0) && (outSize < 0x1400));
    
    unsigned char* outBuf = malloc(outSize);
    if (!outBuf) {
        printf("error allocating file buffer\n");
        return -1;
    }
    assert(outBuf);
    
    memset(outBuf, '\0', outSize);
    memcpy(outBuf, entryBuf, entrySize);
    memcpy(outBuf+0x300, payloadBuf+0x300, payloadSize);
    
    FILE *out = fopen(outFilePath, "w");
    if (!out) {
        printf("error opening %s\n", outFilePath);
        return -1;
    }
    
    fwrite(outBuf, outSize, 1, out);
    fflush(out);
    fclose(out);
    
    free(entryBuf);
    free(payloadBuf);
    free(outBuf);
    
    return 0;
}
