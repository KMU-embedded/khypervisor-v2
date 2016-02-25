#include <stdio.h>
#include <debug_print.h>

#include <gic.h>
#include <armv7_p15.h>
#include <a15_cp15_sysregs.h>
#include <smp.h>
#include <vcpu.h>
#include <hvmm_trace.h>
#include <gic_regs.h>
#include <hvmm_types.h>

#include <board/rtsm-config.h>

#define CBAR_PERIPHBASE_MSB_MASK    0x000000FF

#define ARM_CPUID_CORTEXA15   0x412fc0f1

#define MIDR_MASK_PPN        (0x0FFF << 4)
#define MIDR_PPN_CORTEXA15    (0xC0F << 4)

#define GIC_INT_PRIORITY_DEFAULT_WORD    ((GIC_INT_PRIORITY_DEFAULT << 24) \
                                         |(GIC_INT_PRIORITY_DEFAULT << 16) \
                                         |(GIC_INT_PRIORITY_DEFAULT << 8) \
                                         |(GIC_INT_PRIORITY_DEFAULT))

#define GIC_SIGNATURE_INITIALIZED   0x5108EAD7
/**
 * @brief Registers for Generic Interrupt Controller(GIC)
 */
struct gic {
    uint32_t base;          /**< GIC base address */
    volatile uint32_t *gicd; /**< Distributor */
    volatile uint32_t *gicc; /**< CPU interface */
    volatile uint32_t *gich; /**< Virtual interface control (common)*/
    volatile uint32_t *gicv;/**< Virtual CPU interface */
    uint32_t lines;             /**< The Maximum number of interrupts */
    uint32_t cpus;              /**< The number of implemented CPU interfaces */
    uint32_t initialized;       /**< Check whether initializing GIC. */
};

static struct gic _gic;

static void dump_gic_regs(void)
{
    uint32_t midr;
    HVMM_TRACE_ENTER();
    midr = read_midr();
    debug_print("midr: 0x%08x\n", midr);
    if ((midr & MIDR_MASK_PPN) == MIDR_PPN_CORTEXA15) {
        uint32_t value;
        debug_print("cbar: 0x%08x\n", _gic.base);
        debug_print("gicd: 0x%08x\n", (uint32_t)_gic.gicd);
        debug_print("gicc: 0x%08x\n", (uint32_t)_gic.gicc);
        debug_print("gich: 0x%08x\n", (uint32_t)_gic.gich);
        debug_print("gicv: 0x%08x\n", (uint32_t)_gic.gicv);
        value = _gic.gicd[GICD_CTLR];
        debug_print("GICD_CTLR: 0x%08x\n", value);
        value = _gic.gicd[GICD_TYPER];
        debug_print("GICD_TYPER: 0x%08x\n", value);
        value = _gic.gicd[GICD_IIDR];
        debug_print("GICD_IIDR: 0x%08x\n", value);
    }
    HVMM_TRACE_EXIT();
}

/**
 * @brief Return base address of GIC.
 *
 * When 40 bit address supports, This function wil use.
 */
static uint64_t periphbase_pa(void)
{
    /* CBAR:   4,  c0,   0 */
    /*
     * MRC p15, 4, <Rt>, c15, c0, 0; Read Configuration Base
     * Address Register
     */
    uint64_t periphbase = (uint64_t) read_cbar();
    uint64_t pbmsb = periphbase & ((uint64_t)CBAR_PERIPHBASE_MSB_MASK);
    if (pbmsb) {
        periphbase &= ~((uint64_t)CBAR_PERIPHBASE_MSB_MASK);
        periphbase |= (pbmsb << 32);
    }
    return periphbase;
}
/**
 * @brief   Return address of GIC memory map to _gic.base.
 * @param   va_base Base address(Physical) of GIC.
 * @return  If target architecture is Cortex-A15 then return success,
 *          otherwise return failed.
 */
static hvmm_status_t init_gic_base(uint32_t *va_base)
{
    /* MIDR[15:4], CRn:c0, Op1:0, CRm:c0, Op2:0  == 0xC0F (Cortex-A15) */
    /* Cortex-A15 C15 System Control, C15 Registers */
    /* Name: Op1, CRm, Op2 */
    uint32_t midr;
    hvmm_status_t result = HVMM_STATUS_UNKNOWN_ERROR;
    HVMM_TRACE_ENTER();
    midr = read_midr();
    debug_print("midr: 0x%08x\n", midr);
    /*
     * Note:
     * We currently support GICv2 with Cortex-A15 only.
     * Other architectures with GICv2 support will be further
     * listed and added for support later
     */
    if ((midr & MIDR_MASK_PPN) == MIDR_PPN_CORTEXA15) {
        /* fall-back to periphbase addr from cbar */
        if (va_base == 0) {
            va_base = (uint32_t *)(uint32_t)(periphbase_pa() & \
                    0x00000000FFFFFFFFULL);
        }
        _gic.base = (uint32_t) va_base;
        debug_print("cbar: 0x%08x\n", _gic.base);
        _gic.gicd = (uint32_t *)(_gic.base + GIC_OFFSET_GICD);
        _gic.gicc = (uint32_t *)(_gic.base + GIC_OFFSET_GICC);
        _gic.gich = (uint32_t *)(_gic.base + GIC_OFFSET_GICH);
        _gic.gicv = (uint32_t *)(_gic.base + GIC_OFFSET_GICV);
        result = HVMM_STATUS_SUCCESS;
    } else {
        debug_print("GICv2 Unsupported\n\r");
        debug_print("midr.ppn:0x%08x\n", midr & MIDR_MASK_PPN);
        result = HVMM_STATUS_UNSUPPORTED_FEATURE;
    }
    HVMM_TRACE_EXIT();
    return result;
}
/**
 * @brief Initializes and enables GIC Distributor
 * <pre>
 * Initialization sequence
 * 1. Set Default SPI's polarity.
 * 2. Set Default priority.
 * 3. Diable all interrupts.
 * 4. Route all IRQs to all target processors.
 * 5. Enable Distributor.
 * </pre>
 * @return Always return success.
 */
static hvmm_status_t gicd_init(void)
{
    uint32_t type;
    int i;
    uint32_t cpumask;
    HVMM_TRACE_ENTER();
    /* Disable Distributor */
    _gic.gicd[GICD_CTLR] = 0;
    type = _gic.gicd[GICD_TYPER];
    _gic.lines = 32 * ((type & GICD_TYPE_LINES_MASK) + 1);
    _gic.cpus = 1 + ((type & GICD_TYPE_CPUS_MASK) >> GICD_TYPE_CPUS_SHIFT);
    debug_print("GIC: lines: 0x%08x\n", _gic.lines);
    debug_print(" cpus: 0x%08x\n", _gic.cpus);
    debug_print(" IID: 0x%08x\n", _gic.gicd[GICD_IIDR]);
    /* Interrupt polarity for SPIs (Global Interrupts) active-low */
    for (i = 32; i < _gic.lines; i += 16)
        _gic.gicd[GICD_ICFGR + i / 16] = 0x0;

    /* Default Priority for all Interrupts
     * Private/Banked interrupts will be configured separately
     */
    for (i = 32; i < _gic.lines; i += 4)
        _gic.gicd[GICD_IPRIORITYR + i / 4] = GIC_INT_PRIORITY_DEFAULT_WORD;

    /* Disable all global interrupts.
     * Private/Banked interrupts will be configured separately
     */
    for (i = 32; i < _gic.lines; i += 32)
        _gic.gicd[GICD_ICENABLER + i / 32] = 0xFFFFFFFF;

    /* Route all global IRQs to this CPU */
    cpumask = 1 << smp_processor_id();
    cpumask |= cpumask << 8;
    cpumask |= cpumask << 16;
    for (i = 32; i < _gic.lines; i += 4)
        _gic.gicd[GICD_ITARGETSR + i / 4] = cpumask;

    /* Enable Distributor */
    _gic.gicd[GICD_CTLR] = GICD_CTLR_ENABLE;
    HVMM_TRACE_EXIT();
    return HVMM_STATUS_SUCCESS;
}

/**
 * @brief Initializes GICv2 CPU Interface
 * <pre>
 * Initialization sequence
 * 1. Diable all PPI interrupts, ensure all SGI interrupts are enabled.
 * 2. Set priority on PPI and SGI interrupts.
 * 3. Set priority threshold(Priority Masking),
 *    Finest granularity of priority
 * 4. Enable signaling of interrupts.
 * </pre>
 * @return Always return success.
 */
static hvmm_status_t gicc_init(void)
{
    hvmm_status_t result = HVMM_STATUS_UNKNOWN_ERROR;
    int i;
    /* Disable forwarding PPIs(ID16~31) */
    _gic.gicd[GICD_ICENABLER] = 0xFFFF0000;
    /* Enable forwarding SGIs(ID0~15) */
    _gic.gicd[GICD_ISENABLER] = 0x0000FFFF;
    /* Default priority for SGIs and PPIs */
    for (i = 0; i < 32; i += 4)
        _gic.gicd[GICD_IPRIORITYR + i / 4] = GIC_INT_PRIORITY_DEFAULT_WORD;

    /* No Priority Masking: the lowest value as the threshold : 255 */
    _gic.gicc[GICC_PMR] = 0xFF;
    _gic.gicc[GICC_BPR] = 0x0;
    /* Enable signaling of interrupts and GICC_EOIR only drops priority */
    _gic.gicc[GICC_CTLR] = GICC_CTL_ENABLE | GICC_CTL_EOI;
    result = HVMM_STATUS_SUCCESS;
    return result;
}

/* API functions */
hvmm_status_t gic_enable_irq(uint32_t irq)
{
    _gic.gicd[GICD_ISENABLER + irq / 32] = (1u << (irq % 32));
    return HVMM_STATUS_SUCCESS;
}

hvmm_status_t gic_disable_irq(uint32_t irq)
{
    _gic.gicd[GICD_ICENABLER + irq / 32] = (1u << (irq % 32));
    return HVMM_STATUS_SUCCESS;
}

hvmm_status_t gic_completion_irq(uint32_t irq)
{
    _gic.gicc[GICC_EOIR] = irq;
    return HVMM_STATUS_SUCCESS;
}

hvmm_status_t gic_deactivate_irq(uint32_t irq)
{
    _gic.gicc[GICC_DIR] = irq;
    return HVMM_STATUS_SUCCESS;
}

volatile uint32_t *gic_vgic_baseaddr(void)
{
    if (_gic.initialized != GIC_SIGNATURE_INITIALIZED) {
        HVMM_TRACE_ENTER();
        debug_print("gic: ERROR - not initialized\n\r");
        HVMM_TRACE_EXIT();
    }
    return _gic.gich;
}

hvmm_status_t gic_init(void)
{
    uint32_t cpu = smp_processor_id();
    hvmm_status_t result = HVMM_STATUS_UNKNOWN_ERROR;

    HVMM_TRACE_ENTER();
    /*
     * Determining VA of GIC base adddress has not been defined yet.
     * Let is use the PA for the time being
     */
    if (!cpu) {
        result = init_gic_base((void *)0);
        if (result == HVMM_STATUS_SUCCESS)
            dump_gic_regs();
         /*
         * Initialize and Enable GIC Distributor
         */
        if (result == HVMM_STATUS_SUCCESS)
            result = gicd_init();
    }
    /*
     * Initialize and Enable GIC CPU Interface for this CPU
     * For test it
     */
    if (cpu)
        result = HVMM_STATUS_SUCCESS;

    if (result == HVMM_STATUS_SUCCESS)
        result = gicc_init();

    if (result == HVMM_STATUS_SUCCESS)
        _gic.initialized = GIC_SIGNATURE_INITIALIZED;

    HVMM_TRACE_EXIT();
    return result;
}

hvmm_status_t gic_configure_irq(uint32_t irq,
                enum gic_int_polarity polarity,  uint8_t cpumask,
                uint8_t priority)
{
    hvmm_status_t result = HVMM_STATUS_UNKNOWN_ERROR;
    HVMM_TRACE_ENTER();
    if (irq < _gic.lines) {
        uint32_t icfg;
        volatile uint8_t *reg8;
        /* disable forwarding */
        result = gic_disable_irq(irq);
        if (result == HVMM_STATUS_SUCCESS) {
            /* polarity: level or edge */
            icfg = _gic.gicd[GICD_ICFGR + irq / 16];
            if (polarity == GIC_INT_POLARITY_LEVEL)
                icfg &= ~(2u << (2 * (irq % 16)));
            else
                icfg |= (2u << (2 * (irq % 16)));

            _gic.gicd[GICD_ICFGR + irq / 16] = icfg;
            /* routing */
            reg8 = (uint8_t *) &(_gic.gicd[GICD_ITARGETSR]);
            reg8[irq] = cpumask;
            /* priority */
            reg8 = (uint8_t *) &(_gic.gicd[GICD_IPRIORITYR]);
            reg8[irq] = priority;
            /* enable forwarding */
            result = gic_enable_irq(irq);
        }
    } else {
        debug_print("invalid irq: 0x%08x\n", irq);
        result = HVMM_STATUS_UNSUPPORTED_FEATURE;
    }
    HVMM_TRACE_EXIT();
    return result;
}


uint32_t gic_get_irq_number(void)
{
    /*
     * 1. ACK - CPU Interface - GICC_IAR read
     * 2. Completion - CPU Interface - GICC_EOIR
     * 2.1 Deactivation - CPU Interface - GICC_DIR
     */
    uint32_t iar;
    uint32_t irq;
    /* ACK */
    iar = _gic.gicc[GICC_IAR];
    irq = iar & GICC_IAR_INTID_MASK;

    return irq;
}

hvmm_status_t gic_set_sgi(const uint32_t target, uint32_t sgi)
{
    if(!(sgi < 16))
        return HVMM_STATUS_BAD_ACCESS;

    _gic.gicd[GICD_SGIR] = GICD_SGIR_TARGET_LIST |
        (target << GICD_SGIR_CPU_TARGET_LIST_OFFSET) |
        (sgi & GICD_SGIR_SGI_INT_ID_MASK);
    return HVMM_STATUS_SUCCESS;
}
