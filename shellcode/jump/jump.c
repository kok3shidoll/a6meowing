#include <stdint.h>
#include <stdbool.h>

enum boot_target {
    BOOT_UNKNOWN = 0,
    BOOT_HALT,
    BOOT_IBOOT,
    BOOT_DARWIN,
    // ...
};

typedef void (*write_ttb_t)(uint32_t val);
typedef void (*flush_tlbs_t)(void);
typedef int (*jump_t)(uint32_t boot, uint32_t ptr, uint32_t arg);
typedef uint32_t (*trampoline_t)(void);
typedef void (*bcopy_t)(uint32_t src, uint32_t dest, uint32_t n);

write_ttb_t arm_write_ttb = (write_ttb_t)0x3f00044c;
flush_tlbs_t arm_flush_tlbs = (flush_tlbs_t)0x3f000698;
jump_t prepare_and_jump = (jump_t)0x3f005f81;
trampoline_t get_boot_trampoline = (trampoline_t)0x3f006e85;
bcopy_t my_bcopy = (bcopy_t)0x3f009ac0;

#define TTBR0_BASE          (0x1007c000)
#define LOAD_ADDRESS        (0x10000000)
#define RELOCATE_ADDRESS    (0x10020000)
#define IMAGE_SIZE          (0x40000)

uint32_t jumpto(void) // 0x10079801
{
    uint32_t* ttbr0 = (uint32_t *)TTBR0_BASE;
    ttbr0[0] = 0x3f000c1e;
    
    arm_write_ttb((uint32_t)ttbr0);
    arm_flush_tlbs();
    
    my_bcopy(RELOCATE_ADDRESS, LOAD_ADDRESS, IMAGE_SIZE);
    
    return prepare_and_jump(BOOT_UNKNOWN, get_boot_trampoline(), LOAD_ADDRESS);
}

