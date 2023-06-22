/*
 * 内存相关
 * */

#include "../include/asm/system.h"
#include "../include/linux/mm.h"
#include "../include/linux/kernel.h"
#include "../include/string.h"

#define ARDS_ADDR   0x1100
// 把1M以下内存称为无效内存，1M以下的物理内存给内核用
#define VALID_MEMORY_FROM           0x100000

#define ZONE_VALID 1        // ards 可用内存区域
#define ZONE_RESERVED 2     // ards 不可用区域

//用于接管BIOS的内存统计
physics_memory_info_t g_physics_memory;
//
physics_memory_map_t g_physics_memory_map;

//内存初始化
void memory_init() {
    check_memory_info_t* p = (check_memory_info_t*)ARDS_ADDR;
    check_memmory_item_t* p_data = (check_memmory_item_t*)(ARDS_ADDR + 2);

    //将BIOS检测的可用内存读取出来
    for (int i = 0; i < p->times; ++i) {
        check_memmory_item_t* tmp = p_data + i;

        if (tmp->base_addr_low > 0 && tmp->type == ZONE_VALID) {
            //这里就定义了一块连续，会不会有可用内存不连续的情况？也就有两块可用内存，不会
            g_physics_memory.addr_start = tmp->base_addr_low;       //这里因为我们就不超过4G，所以也就没管高地址
            g_physics_memory.valid_mem_size = tmp->length_low;
            g_physics_memory.addr_end = tmp->base_addr_low + tmp->length_low;
        }
    }

    // 如果没有可用内存
    // BIOS和其他硬件设备使用的地址空间范围是0xA0000到0xFFFFF，这部分被称为"高端内存"（High Memory）。
    // 因此，在BIOS检测可用内存时，它从0x100000开始扫描，以查找可用的高端内存空间。
    if (VALID_MEMORY_FROM != g_physics_memory.addr_start) {
        printk("[%s:%d] no valid physics memory\n", __FILE__, __LINE__);
        return;
    }

    g_physics_memory.pages_total = g_physics_memory.addr_end >> 12;
    g_physics_memory.pages_used = 0;
    g_physics_memory.pages_free = g_physics_memory.pages_total - g_physics_memory.pages_used;
}

//
void memory_map_int() {
    // 验证
    if (VALID_MEMORY_FROM != g_physics_memory.addr_start) {
        printk("[%s:%d] no valid physics memory\n", __FILE__, __LINE__);
        return;
    }

    g_physics_memory_map.addr_base = (uint)VALID_MEMORY_FROM;
//    g_physics_memory_map.map = (uchar*)VALID_MEMORY_FROM;       //将初始的内存作为内存map位管理，1B映射一个内存页（4096B）
    g_physics_memory_map.map = (uchar*)0x10000;       //将bitmap放到BIOS的0x10000的地址

    // 共有这么多物理页可用
    g_physics_memory_map.pages_total = g_physics_memory.pages_total;

    // 清零
    memset(g_physics_memory_map.map, 0, g_physics_memory_map.pages_total);

/*
 * 下面是将bitmap放到BIOS以外的代码，上面将bitmap放到了BIOS的地址0x10000
 * */
        // 1B映射一个page，共需要这么多page，如8196个page即需要8196B，一个page为4096B，共需要2个page
        /*    g_physics_memory_map.bitmap_item_used = g_physics_memory_map.pages_total / PAGE_SIZE;
            if (0 != g_physics_memory_map.pages_total % PAGE_SIZE) {
                g_physics_memory_map.bitmap_item_used += 1;
            }

            //存储Bitmap需要多少个页表，这部分在map中要标记已经使用了
            for (int i = 0; i < g_physics_memory_map.bitmap_item_used; ++i) {
                g_physics_memory_map.map[i] = 1;
            }

            printk("physics memory map position: 0x%X(%dM) - 0x%X(%dM)\n",
                   g_physics_memory_map.map,
                   ((int)g_physics_memory_map.map) / 1024 / 1024,
                   g_physics_memory_map.addr_base,
                   g_physics_memory_map.addr_base / 1024 / 1024);

            printk("physical memory starts here: 0x%X(%dM), used: %d pages\n",
                   g_physics_memory_map.addr_base, g_physics_memory_map.addr_base / 1024 / 1024,
                   g_physics_memory_map.bitmap_item_used);
        */
}


//打印BIOS内存检测结果
void print_check_memory_info()
{
    check_memory_info_t* p = (check_memory_info_t*)ARDS_ADDR;               //获取检查信息的结构体
    check_memmory_item_t* p_data = (check_memmory_item_t*)(ARDS_ADDR + 2);  //获取ards数组的首地址

    unsigned short times = p->times;                                        //获取一共有多少块内存块

    printk("====== memory check info =====\n");
    //遍历ards数组，将内存信息打印出来
    for (int i = 0; i < times; ++i) {
        check_memmory_item_t* tmp = p_data + i;

        printk("\t %x, %x, %x, %x, %d\n", tmp->base_addr_high, tmp->base_addr_low,
               tmp->length_high, tmp->length_low, tmp->type);
    }
    printk("====== memory check info =====\n");
}

//获得空闲页
void* get_free_page()
{
    bool find = false;

    int i = g_physics_memory_map.bitmap_item_used;
    for (; i < g_physics_memory.pages_total; ++i)
    {
        if (0 == g_physics_memory_map.map[i])
        {
            find = true;
            break;
        }
    }

    if (!find)
    {
        printk("memory used up!");
        return NULL;
    }

    g_physics_memory_map.map[i] = 1;
    g_physics_memory_map.bitmap_item_used++;

    void* ret = (void*)(g_physics_memory_map.addr_base + (i << 12));

    printk("[%s]return: 0x%X, used: %d pages\n", __FUNCTION__, ret, g_physics_memory_map.bitmap_item_used);

    return ret;
}

void free_page(void* p) {
    if (p < g_physics_memory.addr_start || p > g_physics_memory.addr_end) {
        printk("invalid address!");
        return;
    }

    int index = (int)(p - g_physics_memory_map.addr_base) >> 12;

    g_physics_memory_map.map[index] = 0;
    g_physics_memory_map.bitmap_item_used--;

    printk("[%s]return: 0x%X, used: %d pages\n", __FUNCTION__, p, g_physics_memory_map.bitmap_item_used);
}