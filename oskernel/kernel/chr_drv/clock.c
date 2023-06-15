#include "../../include/linux/kernel.h"
#include "../../include/linux/traps.h"


#define PIT_CHAN0_REG 0X40
#define PIT_CHAN2_REG 0X42
#define PIT_CTRL_REG 0X43

#define HZ 100
#define OSCILLATOR 1193182                  //即每秒钟产生1193182个脉冲，这个是固定的，不能改
#define CLOCK_COUNTER (OSCILLATOR / HZ)     //tick多少次后发生一次中断

// 10ms触发一次中断
//#define JIFFY (1000 / HZ)

//int jiffy = JIFFY;
//int cpu_tickes = 0;

void clock_init() {
    out_byte(PIT_CTRL_REG, 0b00110100);
    /*
     * 设置tick多少次后发生一次中断
     * 由于只能发送两个字节，最大为0xFFFF，即65535次，故中断间的最大间隔为1193182/65535 = 18
     * 即最慢能每秒18个中断
     * */
    out_byte(PIT_CHAN0_REG, CLOCK_COUNTER & 0xff);          //发送低位
    out_byte(PIT_CHAN0_REG, (CLOCK_COUNTER >> 8) & 0xff);   //发送高位
}


static int val;
//时钟中断处理函数
void clock_handler(int idt_index) {
    //send_eoi(idt_index);

    printf("0x%x\n", idt_index + val);
    val += 1;
}