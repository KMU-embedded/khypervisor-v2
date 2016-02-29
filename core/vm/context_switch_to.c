#include <hvmm_types.h>
#include <context_switch_to.h>
#include <vm.h>
#include <vcpu.h>
#include <interrupt.h>

hvmm_status_t context_switch_to(vcpuid_t current_id, vcpuid_t next_id, struct core_regs *current_regs)
{
    struct vcpu *current = vcpu_find(current_id);
    struct vcpu *next = vcpu_find(next_id);

    if (current_id == VCPUID_INVALID) {
        vm_restore(next->vmid);
        vcpu_restore(next, current_regs);
        return HVMM_STATUS_SUCCESS;
    }

    vcpu_save(current, current_regs);
    if (current->vmid != next->vmid) {
        vm_save(current->vmid);
        // TODO(casionwoo) : vm_save, vm_restore two functions will be merged later.
        vm_restore(next->vmid);
    }
    vcpu_restore(next, current_regs);
    return HVMM_STATUS_SUCCESS;
}
