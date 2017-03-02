#include <vm_map.h>
#include <size.h>

struct memdesc_t vm_device_md[] = {
    {"sram a1", 0x00000000, 0x00000000, SZ_16K, MEMATTR_DEVICE_MEMORY, 1},
    {"sram a2", 0x00004000, 0x00004000, SZ_16K, MEMATTR_DEVICE_MEMORY, 1},
    {"sram a3-4", 0x00008000, 0x00008000, SZ_16K, MEMATTR_DEVICE_MEMORY, 1},
    {"sram d", 0x00010000, 0x00010000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"sram b", 0x00020000, 0x00020000, SZ_64K, MEMATTR_DEVICE_MEMORY, 1},
    {"sram controller", 0x01c00000, 0x01c00000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"dram controller", 0x01c01000, 0x01c01000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"dma", 0x01c02000, 0x01c02000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"nand flash", 0x01c03000, 0x01c03000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"transport stream", 0x01c04000, 0x01c04000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"spi0", 0x01c05000, 0x01c05000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"spi1", 0x01c06000, 0x01c06000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"memory stick", 0x01c07000, 0x01c07000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"tvd", 0x01c08000, 0x01c08000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"csi0", 0x01c09000, 0x01c09000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"tve0", 0x01c0a000, 0x01c0a000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"emac", 0x01c0b000, 0x01c0b000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"lcd0", 0x01c0c000, 0x01c0c000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"lcd1", 0x01c0d000, 0x01c0d000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"video engine", 0x01c0e000, 0x01c0e000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"sd/mmc0", 0x01c0f000, 0x01c0f000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"sd/mmc1", 0x01c10000, 0x01c10000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"sd/mmc2", 0x01c11000, 0x01c11000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"sd/mmc3", 0x01c12000, 0x01c12000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"usb0", 0x01c13000, 0x01c13000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"usb1", 0x01c14000, 0x01c14000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"security system", 0x01c15000, 0x01c15000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"hdmi", 0x01c16000, 0x01c16000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"spi2", 0x01c17000, 0x01c17000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"sata", 0x01c18000, 0x01c18000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"pata", 0x01c19000, 0x01c19000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"ace", 0x01c1a000, 0x01c1a000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"tve1", 0x01c1b000, 0x01c1b000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"usb2", 0x01c1c000, 0x01c1c000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"csi1", 0x01c1d000, 0x01c1d000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c1e000, 0x01c1e000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"spi3", 0x01c1f000, 0x01c1f000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"ccu", 0x01c20000, 0x01c20000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"interrupt", 0x01c20400, 0x01c20400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"pio", 0x01c20800, 0x01c20800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"timer", 0x01c20c00, 0x01c20c00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"owa", 0x01c21000, 0x01c21000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"ac97", 0x01c21400, 0x01c21400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"ir0", 0x01c21800, 0x01c21800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"ir1", 0x01c21c00, 0x01c21c00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"iis-1", 0x01c22000, 0x01c22000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"iis-0", 0x01c22400, 0x01c22400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"lradc0/1", 0x01c22800, 0x01c22800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"ad/da", 0x01c22c00, 0x01c22c00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"keypad", 0x01c23000, 0x01c23000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c23400, 0x01c23400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"sid", 0x01c23800, 0x01c23800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"sjtag", 0x01c23c00, 0x01c23c00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c24000, 0x01c24000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"iis-2", 0x01c24400, 0x01c24400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c24800, 0x01c24800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c24c00, 0x01c24c00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"tp", 0x01c25000, 0x01c25000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"pmu", 0x01c25400, 0x01c25400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c25800, 0x01c25800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"cpu configuration", 0x01c25c00, 0x01c25c00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c26000, 0x01c26000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c26400, 0x01c26400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c26800, 0x01c26800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c26c00, 0x01c26c00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c27000, 0x01c27000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c27400, 0x01c27400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c27800, 0x01c27800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"unk", 0x01c27c00, 0x01c27c00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"uart0", 0x01c28000, 0x01c28000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"uart1", 0x01c28400, 0x01c28400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"uart2", 0x01c28800, 0x01c28800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"uart3", 0x01c28c00, 0x01c28c00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"uart4", 0x01c29000, 0x01c29000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"uart5", 0x01c29400, 0x01c29400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"uart6", 0x01c29800, 0x01c29800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"uart7", 0x01c29c00, 0x01c29c00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"ps2-0", 0x01c2a000, 0x01c2a000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"ps2-1", 0x01c2a400, 0x01c2a400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"/", 0x01c2a800, 0x01c2a800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"twi0", 0x01c2ac00, 0x01c2ac00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"twi1", 0x01c2b000, 0x01c2b000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"twi2", 0x01c2b400, 0x01c2b400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"twi3", 0x01c2b800, 0x01c2b800, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"can", 0x01c2bc00, 0x01c2bc00, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"twi4", 0x01c2c000, 0x01c2c000, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"smart card reader", 0x01c2c400, 0x01c2c400, SZ_1K, MEMATTR_DEVICE_MEMORY, 1},
    {"gps", 0x01c30000, 0x01c30000, SZ_64K, MEMATTR_DEVICE_MEMORY, 1},
    {"mali400", 0x01c40000, 0x01c40000, SZ_64K, MEMATTR_DEVICE_MEMORY, 1},
    {"gmac", 0x01c50000, 0x01c50000, SZ_64K, MEMATTR_DEVICE_MEMORY, 1},
    {"hstimer", 0x01c60000, 0x01c60000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"hdmi1", 0x01ce0000, 0x01ce0000, SZ_128K, MEMATTR_DEVICE_MEMORY, 1},
    {"cpubist", 0x3f501000, 0x3f501000, SZ_4K, MEMATTR_DEVICE_MEMORY, 1},
    {"sram c", 0x01d00000, 0x01d00000, SZ_1M, MEMATTR_DEVICE_MEMORY, 1},
    {"de_fe0", 0x01e00000, 0x01e00000, SZ_128K, MEMATTR_DEVICE_MEMORY, 1},
    {"de_fe1", 0x01e20000, 0x01e20000, SZ_128K, MEMATTR_DEVICE_MEMORY, 1},
    {"de_be0", 0x01e60000, 0x01e60000, SZ_128K, MEMATTR_DEVICE_MEMORY, 1},
    {"de_be1", 0x01e40000, 0x01e40000, SZ_128K, MEMATTR_DEVICE_MEMORY, 1},
    {"mp", 0x01e80000, 0x01e80000, SZ_128K, MEMATTR_DEVICE_MEMORY, 1},
    {"avg", 0x01ea0000, 0x01ea0000, SZ_128K, MEMATTR_DEVICE_MEMORY, 1},
    {"coresight debug module", 0x3f500000, 0x3f500000, SZ_64K, MEMATTR_DEVICE_MEMORY, 1},
    {"brom", 0xffff0000, 0xffff0000, SZ_32K, MEMATTR_DEVICE_MEMORY, 1},

    { "gicd", CFG_GIC_BASE_PA | 0x1000, CFG_GIC_BASE_PA | 0x1000, SZ_4K, MEMATTR_DEVICE_MEMORY, 0 },
    { "gicc", CFG_GIC_BASE_PA | 0x2000, CFG_GIC_BASE_PA | 0x6000, SZ_8K, MEMATTR_DEVICE_MEMORY, 1 },

    {0, 0, 0, 0, 0, 0}
};
