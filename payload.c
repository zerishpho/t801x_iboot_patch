#include <stdint.h>
#include <stdbool.h>

#include <common.h>
#include <offsetfinder.h>

#include <payload.h>

#define DEVELOPMENT 1

#define INSN_MOV_X0_0           0xd2800000
#define INSN_MOV_X0_1           0xd2800020
#define INSN_RET                0xd65f03c0
#define INSN_NOP                0xd503201f
#define INSN_CHECKOUT_FUSELOCK  0x1400000A

typedef int (*jump_t)(void* arg0, void* arg1);

int patch(void* arg0, void* arg1)
{
    
    // t8015, TODO: offsetfinder
    jump_t ret = (jump_t)0x180018004;
    uint64_t iboot_base = 0x18001c000;
    
    void* idata = (void*)(iboot_base);
    uint64_t isize = *(uint64_t*)(iboot_base + 0x308) - iboot_base; // ... why 0x308??
    
    
    // OF64
    uint64_t _check_bootmode = find_check_bootmode(iboot_base, idata, isize);
    if(!_check_bootmode)
        return -1;
    _check_bootmode += iboot_base;
    
    uint64_t _debug_enabled = find_debug_enabled(iboot_base, idata, isize);
    if(!_debug_enabled)
        return -1;
    _debug_enabled += iboot_base;
    
#ifdef DEVELOPMENT
    uint64_t _fuse_lock = find_fuse_lock(iboot_base, idata, isize);
    if(!_fuse_lock)
        return -1;
    _fuse_lock += iboot_base;
    
    uint64_t _get_fuse_lock = find_get_fuse_lock(iboot_base, idata, isize);
    if(!_get_fuse_lock)
        return -1;
    _get_fuse_lock += iboot_base;
#endif
    
    uint64_t _jumpto_bl = find_jumpto_bl(iboot_base, idata, isize);
    if(!_jumpto_bl)
        return -1;
    _jumpto_bl += iboot_base;
    
    /* TODO: bootargs injection
    uint64_t _bootargs_nop = find_bootargs_nop(iboot_base, idata, isize);
    if(!_bootargs_nop)
        return -1;
    _bootargs_nop += iboot_base;
    
    uint64_t _bootargs_adr = find_bootargs_adr(iboot_base, idata, isize);
    if(!_bootargs_adr)
        return -1;
    _bootargs_adr += iboot_base;
     */
    
    uint64_t _bootx_str = find_bootx_str(iboot_base, idata, isize);
    if(!_bootx_str)
        return -1;
    _bootx_str += iboot_base;
    
    uint64_t _bootx_cmd_handler = find_bootx_cmd_handler(iboot_base, idata, isize);
    if(!_bootx_cmd_handler)
        return -1;
    _bootx_cmd_handler += iboot_base;
    
    uint64_t _go_cmd_handler = find_go_cmd_handler(iboot_base, idata, isize);
    if(!_go_cmd_handler)
        return -1;
    _go_cmd_handler += iboot_base;
    
    uint64_t _zeroBuf = find_zero(iboot_base, idata, isize);
    if(!_zeroBuf)
        return -1;
    _zeroBuf += iboot_base;
    
    // patch
    uint32_t* patch_check_bootmode = (uint32_t*)_check_bootmode;
    patch_check_bootmode[0] = INSN_MOV_X0_1;
    patch_check_bootmode[1] = INSN_RET;
    
    uint32_t* patch_debug_enabled = (uint32_t*)_debug_enabled;
    patch_debug_enabled[0] = INSN_MOV_X0_1;
  
    uint32_t* patch_bootx_str = (uint32_t*)_bootx_str;
    patch_bootx_str[0] = 0x77726F64; // 'bootx' -> 'dorwx'
    
    uint64_t* patch_bootx_cmd_handler = (uint64_t*)_bootx_cmd_handler;
    uint64_t* patch_go_cmd_handler = (uint64_t*)_go_cmd_handler;
    
    patch_bootx_cmd_handler[0] = _zeroBuf;
    patch_go_cmd_handler[0] = _zeroBuf + a11rxw_len;
    
    memcpy((void*)(_zeroBuf), a11rxw, a11rxw_len);
    memcpy((void*)(_zeroBuf + a11rxw_len), go_cmd_hook, go_cmd_hook_len);
    memcpy((void*)(_zeroBuf + a11rxw_len + go_cmd_hook_len), tram, tram_len);
    
    uint64_t jumpto_hook_addr = _zeroBuf + a11rxw_len + go_cmd_hook_len;
    uint32_t opcode = make_branch(_jumpto_bl, jumpto_hook_addr);
    uint32_t* patch_jumpto_bl = (uint32_t*)_jumpto_bl;
    patch_jumpto_bl[0] = opcode;
    
#ifdef DEVELOPMENT
    uint32_t* patch_get_fuse_lock = (uint32_t*)_get_fuse_lock;
    patch_get_fuse_lock[0] = INSN_MOV_X0_1;
    patch_get_fuse_lock[1] = INSN_RET;
    
    uint32_t* patch_fuse_lock = (uint32_t*)_fuse_lock;
    patch_fuse_lock[0] = INSN_CHECKOUT_FUSELOCK;
#endif
    
    // TODO: offsetfinder
    {
        // - image4_validate_property_callback_interposer
        uint32_t* sigcheck1 = (uint32_t*)0x180030ad0; // D22, 15.7.1
        sigcheck1[0] = INSN_MOV_X0_0;
        
        // - boot_manifest_hash
        // ...
        // if (memcmp((const void *)env_props.boot_manifest_hash, (const void *)img_props.manifest_hash, HASH_SIZE) != 0)  <-- if(0)
        //    goto untrusted;
        // img_props.manifest_hash_verified = true;
        // ...
        uint32_t* sigcheck2 = (uint32_t*)0x18003128c; // D22, 15.7.1 
        sigcheck2[0] = INSN_MOV_X0_0;
    }
    
    return ret(arg0, arg1);
}
