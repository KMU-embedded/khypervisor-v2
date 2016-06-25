#include <stdio.h>
#include <vdev.h>
#include <core/timer.h>
#include <arch/irq.h>
#include <arch/armv7.h>
#include <core/scheduler.h>
#include <config.h>
#include <irq-chip.h>
#include <drivers/vdev/vdev_timer.h>

// #include <lib/list.h>

// #define DEBUG

/*
 * CHECK LIST
 * [ ] vDev Timer instance init
 * [ ] Timer access handler
 *      [v] Skeleton
 *      [ ] Set SW timer value (tm_set_timer)
 *      [ ] Handle IMASK, Enable/Disable bits
 * [ ] Timer IRQ handler/injector
 *      [v] Skeleton
 *      [ ]
 * [ ] Generic Timer PL1/PL0 access setting
 * [ ]
 *
 */

#define VTIMER_IRQ 30 /* NS PL1 Timer */

extern struct virq_chip *virq_hw;

/*
   CP64(CNTPCT)
   CP64(CNTVCT)
   CP64(CNTP_CVAL)
   CP64(CNTV_CVAL)

   CP32(CNTFRQ)
   CP32(CNTKCTL)
   CP32(CNTP_TVAL)
   CP32(CNTP_CTL)
   CP32(CNTV_TVAL)
   CP32(CNTV_CTL)
   */

int vdev_timer_access32(uint8_t read, uint32_t what, uint32_t *rt)
{
    struct vcpu *vcpu = get_current_vcpu();
    struct vdev_timer *v = &vcpu->vtimer;
    uint32_t *target = NULL;
    uint32_t tmp = 0;

    switch (what) {
        case CP32(CNTFRQ)    :
            target = &v->frq;
            break;
        case CP32(CNTKCTL)   :
            target = &v->k_ctl;
            break;
        case CP32(CNTP_CTL)  :
            target = &v->p_ctl;
            break;
        case CP32(CNTV_CTL)  :
            target = &v->v_ctl;
            break;

        case CP32(CNTP_TVAL) :
            if (read) {
                target = &tmp;
                tmp = (uint32_t)
                    ( v->p_cval - (timer_get_syscounter() - v->p_ct_offset) );
            } else {
                /* FIXME:(igkang) sign extension of *rt needed */
                v->p_cval = (timer_get_syscounter() - v->p_ct_offset + *rt);
                return 0;
            }
        case CP32(CNTV_TVAL) :
            if (read) {
                tmp = (uint32_t)
                    ( v->v_cval - (timer_get_syscounter() - v->p_ct_offset) );
            } else {
                /* FIXME:(igkang) sign extension of *rt needed */
                v->v_cval = (timer_get_syscounter() - v->p_ct_offset + *rt);
                return 0;
            }
            /* do subtraction and set cval */
            break;
        default:
            return 1;
            break;
    }

    if (read) {
        *rt = *target;
    } else { /* write */
        *target = *rt;
    }

    return 0;
}

int vdev_timer_access64(uint8_t read, uint32_t what, uint32_t *rt_low, uint32_t *rt_high)
{
    struct vcpu *vcpu = get_current_vcpu();
    struct vdev_timer *v = &vcpu->vtimer;
    uint64_t *target = NULL;
    uint64_t tmp = 0;

    switch (what) {
        case CP64(CNTPCT)    :
        case CP64(CNTVCT)    :
            if (!read) {
                return 1;
            } else {
                target = &tmp;
                tmp = timer_get_syscounter() + v->p_ct_offset;
            }
            /* do something */
            break;
        case CP64(CNTP_CVAL) :
            target = &v->p_cval;
            break;
        case CP64(CNTV_CVAL) :
            target = &v->v_cval;
            break;
        default:
            return 1;
            break;
    }

    if (read) {
        *rt_low  = (uint32_t) (*target & 0xFFFFFFFF);
        *rt_high = (uint32_t) (*target >> 32);
    } else { /* write */
        *target = ((uint64_t) *rt_high << 32) | (uint64_t) *rt_low;
    }

    return 0;
}

/* FIXME:(igkang) The macro "container_of()" in lib/list.h is not complete.
 * References:
 *      http://fxr.watson.org/fxr/source/contrib/vchiq/interface/compat/list.h
 */
#define container_of2(ptr, type, member)                         \
({                                                              \
        __typeof(((type *)0)->member) *_p = (ptr);              \
        (type *)((char *)_p - offsetof(type, member));          \
})

/* TODO:(igkang) need to rewrite core/timer code to store pdata in each "struct timer" instance */
void vdev_timer_handler(void *pdata, uint64_t *expiration) {
    *expiration = 0;

    /* do IRQ injection to vCPU of vdev_timer instance which we currently handling */
    struct timer *t = container_of2(expiration, struct timer, expiration);
    struct vdev_timer *v = container_of2(t, struct vdev_timer, swtimer);
    struct vcpu *vcpu = container_of2(v, struct vcpu, vtimer);
    virq_hw->forward_irq(vcpu, 29, 29, INJECT_SW);
}

void init_vdev_timer(struct vdev_timer *v)
{
    /* for reset values, see ARMv7 reference manual */
    // reset _CTLs
    // timer interrupt mask?
    // couter offset

    // init and register sw timer
    tm_register_timer(&v->swtimer, vdev_timer_handler);
}

