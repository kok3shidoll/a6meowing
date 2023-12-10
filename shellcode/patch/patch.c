#include <stdint.h>
#include <stdbool.h>

typedef void (*write_ttb_t)(uint32_t val);
typedef void (*flush_tlbs_t)(void);
typedef void (*bcopy_t)(uint32_t src, uint32_t dest, uint32_t n);

write_ttb_t arm_write_ttb = (write_ttb_t)0x3f00044c;
flush_tlbs_t arm_flush_tlbs = (flush_tlbs_t)0x3f000698;
bcopy_t my_bcopy = (bcopy_t)0x3f009ac0;

uint32_t gFlag;

#define VROM_BASE_ADDRESS           (0x00000000)
#define LOAD_ADDRESS                (0x10000000)
#define RELOCATE_PAYLOAD_ADDRESS    (0x10010000)
#define RELOCATE_LOAD_ADDRESS       (0x10020000)
#define IMAGE_SIZE                  (0x40000)
#define TTBR0_BASE                  (0x1007c000)
#define JUMP_PAYLOAD_BASE           (0x10079800)
#define PAYLOAD_MAX_SIZE            (0x1000)
#define gDemotionRegister           (0x3f500000)

#define remap_rom_to_sram   (1 << 0)
#define enable_demotion     (1 << 1)
#define use_checkm8_payload (1 << 2)

__attribute__((noinline)) void patchROM(void)
{
    // relocate ROM
    my_bcopy(VROM_BASE_ADDRESS, LOAD_ADDRESS, 0x10000);
    
    uint16_t* patchAddr16;
    uint32_t* patchAddr32;
    
    // patch loadaddr, imagesize
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x09d8);
    patchAddr32[0] = 0xbf004a3d;
    
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x09f0);
    patchAddr32[0] = 0xbf004837;
    
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x0a02);
    patchAddr32[0] = 0xbf004a33;
    
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x0a14);
    patchAddr32[0] = 0xbf00482e;
    
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x0a48);
    patchAddr32[0] = 0xbf004821;
    
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x0a68);
    patchAddr32[0] = 0xbf004819;
    
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x0a82);
    patchAddr32[0] = 0xbf004a13;
    
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x0ab8);
    patchAddr32[0] = IMAGE_SIZE;
    
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x0ad0);
    patchAddr32[0] = RELOCATE_LOAD_ADDRESS;
    
    // security_init
    // security_clear_mem_in_chunks
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x6b18);
    patchAddr32[0] = 0xbf00480d;
    
    patchAddr16 = (uint16_t*)(LOAD_ADDRESS + 0x6b1c);
    patchAddr16[0] = 0x490d;
    
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x6b50);
    patchAddr32[0] = RELOCATE_LOAD_ADDRESS;
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x6b54);
    patchAddr32[0] = IMAGE_SIZE;
    
    // jump shellcode
    patchAddr16 = (uint16_t*)(LOAD_ADDRESS + 0x086e);
    patchAddr16[0] = 0xbf00;
    
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x0870);
    patchAddr32[0] = 0x47004800;
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x0874);
    patchAddr32[0] = JUMP_PAYLOAD_BASE + 1;
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x0878);
    patchAddr32[0] = 0xbf00bf00;
    
    // sigcheck patch
    // verify_shsh
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x499c);
    patchAddr32[0] = 0x60182000;
    
    // shsh
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x49e0); // SDOM
    patchAddr32[0] = 0x20002000;
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x4a02); // PROD
    patchAddr32[0] = 0x20002000;
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x4a4c); // CHIP
    patchAddr32[0] = 0x20002000;
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x4a6e); // TYPE
    patchAddr32[0] = 0x20002000;
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x4a94); // SEPO
    patchAddr32[0] = 0x20002000;
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x4ab4); // CEPO
    patchAddr32[0] = 0x20002000;
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x4ad4); // BORD
    patchAddr32[0] = 0x20002000;
    patchAddr32 = (uint32_t*)(LOAD_ADDRESS + 0x4afa); // ECID
    patchAddr32[0] = 0x20002000;
    
    // write ttbr0
    uint32_t* ttbr0 = (uint32_t*)TTBR0_BASE;
    ttbr0[0]  = 0x10000c1e;
    
    arm_write_ttb(TTBR0_BASE);
    arm_flush_tlbs();
    
}

__attribute__((noinline)) void demote(void)
{
    uint32_t* fuseBase = (uint32_t*)gDemotionRegister;
    fuseBase[0] &= 0xfffffffe;
}

__attribute__((noinline)) void copy_checkm8_payload(void)
{
    // todo
}

__attribute__((noinline)) int main_payload(void)
{
    uint32_t* gOffsets = (uint32_t*)(RELOCATE_PAYLOAD_ADDRESS + 0x300);
    gFlag = gOffsets[0];
    
    if((gFlag & remap_rom_to_sram) && (gFlag & use_checkm8_payload))
    {
        return -1;
    }
    
    
    if(gFlag & remap_rom_to_sram)
    {
        patchROM();
    }
    
    if(gFlag & enable_demotion)
    {
        demote();
    }
    
    if(gFlag & use_checkm8_payload)
    {
        copy_checkm8_payload();
    }
    
    return 0;
}
