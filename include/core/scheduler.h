#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <hvmm_types.h>
#include <vcpu_regs.h>
#include <vcpu.h>
//#include <board/rtsm-config.h>

#define GUEST_VERBOSE_ALL       0xFF
#define GUEST_VERBOSE_LEVEL_0   0x01
#define GUEST_VERBOSE_LEVEL_1   0x02
#define GUEST_VERBOSE_LEVEL_2   0x04
#define GUEST_VERBOSE_LEVEL_3   0x08
#define GUEST_VERBOSE_LEVEL_4   0x10
#define GUEST_VERBOSE_LEVEL_5   0x20
#define GUEST_VERBOSE_LEVEL_6   0x40
#define GUEST_VERBOSE_LEVEL_7   0x80

void sched_init();
int sched_vcpu_register(int vcpuid);
int sched_vcpu_unregister();
int sched_vcpu_attach(int vcpuid);
int sched_vcpu_detach();
void do_schedule(void *pdata);

vmid_t sched_policy_determ_next(void);

void guest_sched_start(void);
vmid_t get_current_vcpuid(void);

hvmm_status_t guest_perform_switch(struct core_regs *regs);
hvmm_status_t guest_switchto(vmid_t vmid);

#endif /* __SCHEDULER_H__ */
