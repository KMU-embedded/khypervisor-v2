#include <stdio.h>
#include <stdlib.h>
#include <vdev.h>
#include <core/scheduler.h>
#include <core/vm/vm.h>
#include <arch/armv7.h>

#define MAX_VDEV    256

static struct list_head vdev_list;

#define decode_wnr(iss)             (iss & (1 << 6))
#define decode_srt(iss)             ((iss & 0xF0000) >> 16)

void vdev_handler(struct core_regs *regs, uint32_t iss)
{
    struct vmcb *vm = get_current_vm();
    uint32_t fipa = 0;
    uint8_t wnr = decode_wnr(iss);
    uint8_t srt = decode_srt(iss);

    fipa = read_cp32(HPFAR) << 8;
    fipa |= (read_cp32(HDFAR) & PAGE_MASK);

    struct vdev_instance *instance;
    list_for_each_entry(struct vdev_instance, instance, &vm->vdevs.head, head) {
        uint32_t vdev_base = instance->module->base;
        uint32_t vdev_size = instance->module->size;

        if (vdev_base <= fipa && fipa <= vdev_base + vdev_size) {
            uint32_t offset = fipa - vdev_base;
            if (wnr) {
                instance->module->write(instance->pdata, offset, &(regs->gpr[srt]));
            } else {
                regs->gpr[srt] = instance->module->read(instance->pdata, offset);
            }

            break;
        }
    }
}
void vdev_register(struct vdev_module *module)
{
    if (module == NULL) {
        printf("Failed register vdev %s\n", module->name);
        return;
    }

    LIST_ADDTAIL(&module->head, &vdev_list);
}

void vdev_create(struct vdev_instance *vdevs, vmid_t vmid)
{
    LIST_INITHEAD(&vdevs->head);

    struct vdev_module *module;
    list_for_each_entry(struct vdev_module, module, &vdev_list, head) {
        struct vdev_instance *vdev = malloc(sizeof(struct vdev_instance));

        vdev->owner = vmid;
        vdev->module = module;
        vdev->module->create(&vdev->pdata);

        LIST_ADDTAIL(&vdev->head, &vdevs->head);
    }
}

static uint32_t vdev_module_initcall(initcall_t fn)
{
    return	fn();
}

#include <asm/macro.h>
void SECTION(".init.vdev") vdev_init(void)
{
    LIST_INITHEAD(&vdev_list);

    initcall_t *fn;
    for (fn = __vdev_module_start; fn < __vdev_module_end; fn++) {
        vdev_module_initcall(*fn);
    }
}

