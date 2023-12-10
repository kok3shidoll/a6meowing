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
    
    if(argc != 3) {
        printf("%s <in> <out>\n", argv[0]);
        return 0;
    }
    
    char *inFilePath = argv[1];
    char *outFilePath = argv[2];
    
    unsigned char *fileBuf = NULL;
    size_t fileSize = 0;
    
    openFile(inFilePath, &fileSize, &fileBuf);
    assert((fileSize > 0x1800) && fileBuf);
    
    size_t outSize = fileSize - 0x1800;
    assert((outSize > 0) && (outSize < 0x100));
    
    unsigned char* outBuf = malloc(outSize);
    if (!outBuf) {
        printf("error allocating file buffer\n");
        return -1;
    }
    
    assert(outBuf);
    
    memcpy(outBuf, fileBuf + 0x1800, outSize);
    
    FILE *out = fopen(outFilePath, "w");
    if (!out) {
        printf("error opening %s\n", outFilePath);
        return -1;
    }
    
    fwrite(outBuf, outSize, 1, out);
    fflush(out);
    fclose(out);
    
    free(fileBuf);
    free(outBuf);
    
    return 0;
}
