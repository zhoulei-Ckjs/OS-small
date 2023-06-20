#include "../include/linux/mm.h"
#include "../include/asm/system.h"

/*
 * 桶链表
 * 16 bytes
 */
struct bucket_desc {
    void			    *page;          // 管理的物理页
    struct bucket_desc	*next;          // 下一个bucket地址
    void			    *freeptr;       // 下一个可供分配的
    unsigned short		refcnt;         // 引用计数，释放物理页时要用
    unsigned short		bucket_size;    // 每个桶的大小
};


/*
 * 桶
 * 8 bytes
 */
struct _bucket_dir {
    int			size;
    struct bucket_desc	*chain;
};

/*
 * 桶目录，包含了各个大小的内存桶
 */
struct _bucket_dir bucket_dir[] = {
        { 16,	(struct bucket_desc *) 0},
        { 32,	(struct bucket_desc *) 0},
        { 64,	(struct bucket_desc *) 0},
        { 128,	(struct bucket_desc *) 0},
        { 256,	(struct bucket_desc *) 0},
        { 512,	(struct bucket_desc *) 0},
        { 1024,	(struct bucket_desc *) 0},
        { 2048, (struct bucket_desc *) 0},
        { 4096, (struct bucket_desc *) 0},
        { 0,    (struct bucket_desc *) 0}
};   /* End of list marker */

/*
 * 桶描述符列表头指针，流程会申请一个页表，用来初始化为 4096 / 16 = 256个桶描述符
 * 如果一个桶的内存用光了，那么就要申请一块内存作为一个新桶
 * 我们用 桶组 -> 桶描述符 -> 真正管理的物理页这种模式
 */
struct bucket_desc *free_bucket_desc = (struct bucket_desc *) 0;

/*
 * 初始化桶的描述符，将一个页4096个字节专门存储桶描述符
 */
static inline void init_bucket_desc()
{
    struct bucket_desc *bdesc, *first;
    int	i;

    first = bdesc = (struct bucket_desc *) get_free_page();     //获取一个新的物理页
    if (!bdesc)                                                 //如果获取的内存为空，get_free_page已经打印了内存不足
        return NULL;
    for (i = PAGE_SIZE/sizeof(struct bucket_desc); i > 1; i--) {        //对获取到的物理页链表化
        bdesc->next = bdesc+1;
        bdesc++;
    }
    /*
     * This is done last, to avoid race conditions in case
     * get_free_page() sleeps and this routine gets called again....
     */
    bdesc->next = free_bucket_desc;     //将当前桶的最后一个链表置空
    free_bucket_desc = first;
}

void* malloc(size_t len)
{
    struct _bucket_dir	*bdir;      //桶目录
    struct bucket_desc	*bdesc;
    void			*retval;

    //搜索对应这个空间的bucket
    for (bdir = bucket_dir; bdir->size; bdir++)
        if (bdir->size >= len)
            break;
    // 找到了最后一个都没找到大于这个空间的内存，说明申请的内存 大于 4096 byte
    if (!bdir->size) {
        printk("malloc called with impossibly large argument (%d)\n", len);
        return NULL;
    }

    // 找到了能够刚好容纳这个size的空间
    CLI	                                                        //可重入，避免竞争，关中断
    for (bdesc = bdir->chain; bdesc; bdesc = bdesc->next)       // 往后找可用空间，如果一直没有可用空间，则就让bdesc为空
        if (bdesc->freeptr)             // 如果当前这个内存页还有剩余空间（如4096 被分成 按16字节一份 的 256份）
            break;
    /*
     * 如果没有找到有空闲空间的桶，那就新开辟一个桶加入桶链表
     */
    if (!bdesc) {
        char		*cp;
        int		i;

        if (!free_bucket_desc)              //如果所有的桶描述符都用完了（空闲桶描述符专门申请了一个页来管理）
            init_bucket_desc();             //申请一个页来重新初始化一群桶
        bdesc = free_bucket_desc;           //空闲桶描述符首地址
        free_bucket_desc = bdesc->next;
        bdesc->refcnt = 0;
        bdesc->bucket_size = bdir->size;
        bdesc->page = bdesc->freeptr = (void *) (cp = (char *) get_free_page());        //给桶描述符分配一个物理页用来分配真正的内存
        if (!cp)
            return NULL;
        // 将真正代分配的物理页划分为一块块的 相应桶size的 物理页，将它们用链表串联起来，这里涉及到一个名字：嵌入式指针
        for (i = PAGE_SIZE / bdir->size; i > 1; i--) {
            *((char **) cp) = cp + bdir->size;
            cp += bdir->size;
        }
        *((char **) cp) = 0;            //将最后一个size的待分配空间指向的下一个地址置空
        bdesc->next = bdir->chain; /* 将桶描述符加入到桶组中，我们通过桶组找到桶描述符再找可用空间，这里采用头插法 */
        bdir->chain = bdesc;
    }
    //获得了可用空间，根据桶描述符找到真正可分配的物理页，再分配空间
    retval = (void *) bdesc->freeptr;
    bdesc->freeptr = *((void **) retval);
    bdesc->refcnt++;
    STI	/* OK, we're safe again */
    return(retval);
}

/*
 * 为了实现free自动识别要释放的内存大小，我们要增加一个标记，在我们malloc的时候都是以一个page作为一个桶的一部分
 * 回想get_free_page的时候，我们以1B作为一个页是否被用的标记，这个1B完全可以存储这个页属于哪个桶
 * 另一种思想是在待分配的物理页首地址处写上大小，但是这样的话4096大小和2048等就不好弄了
 * 有待去实现自动识别的free...
 */
void free_s(void *obj, int size)
{
    void		*page;
    struct _bucket_dir	*bdir;
    struct bucket_desc	*bdesc, *prev;
    bdesc = prev = 0;
    //计算这个页属于哪个页
    page = (void *)  ((unsigned long) obj & 0xfffff000);
    /* Now search the buckets looking for that page */
    for (bdir = bucket_dir; bdir->size; bdir++) {
        prev = 0;
        /* If size is zero then this conditional is always false */
        if (bdir->size < size)
            continue;
        for (bdesc = bdir->chain; bdesc; bdesc = bdesc->next) {
            if (bdesc->page == page)
                goto found;
            prev = bdesc;
        }
    }
    return;
found:
    CLI
    //头插法，将待释放的内存放到空闲链表头部
    *((void **)obj) = bdesc->freeptr;
    bdesc->freeptr = obj;
    bdesc->refcnt--;
    if (bdesc->refcnt == 0) {
        /*
         * 要再进行判断一下，确保找到的bucket_descriptor是正确的
         */
        if ((prev && (prev->next != bdesc)) || (!prev && (bdir->chain != bdesc)))
            for (prev = bdir->chain; prev; prev = prev->next)
                if (prev->next == bdesc)
                    break;
        if (prev)   //也是头插法，将空闲的bucket_descriptor插到空闲描述符头
            prev->next = bdesc->next;
        else {
            if (bdir->chain != bdesc)   //这不可能，我们prev是空，但是p不是桶链表的头
                return;
            bdir->chain = bdesc->next;
        }
        //要释放内存物理页，因为refcnt(引用计数)已经为0了
        free_page((unsigned long) bdesc->page);
        bdesc->next = free_bucket_desc;     //这个描述符也要放到空闲描述符表
        free_bucket_desc = bdesc;
    }
    STI
    return;
}