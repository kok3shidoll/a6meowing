#include <stdint.h>
#include <stdbool.h>

#include "jump/jump.h"

typedef void (*bcopy_t)(uint32_t src, uint32_t dest, uint32_t n);

bcopy_t my_bcopy = (bcopy_t)0x3f009ac0;

uint32_t gFlag;

#define LOAD_ADDRESS                (0x10000000)
#define RELOCATE_PAYLOAD_ADDRESS    (0x10010000)
#define JUMP_PAYLOAD_BASE           (0x10079800)
#define PAYLOAD_MAX_SIZE            (0x1000)

__attribute__((noinline)) void prepare(void)
{
    // relocate shellcode
    my_bcopy(LOAD_ADDRESS, RELOCATE_PAYLOAD_ADDRESS, PAYLOAD_MAX_SIZE);
    my_bcopy((uint32_t)jump_bin, JUMP_PAYLOAD_BASE, jump_bin_len);
}
