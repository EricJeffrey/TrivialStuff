
# Linux内核的个人笔记

### pagefault

以数据读/写缺页为例的fault流程

```
arch/arm64/kernel/entry.S:          SYM_CODE_START(vectors) =>  异常向量
arch/arm64/kernel/entry.S:          kernel_ventry           =>  处理异常，通过 `b el\el\ht\()_\regsize\()_\label` 跳转到相应处理函数
                                                                此处普通用户数据缺页会分发到 el0t_64_sync_handler
arch/arm64/kernel/entry-common.c:   el0t_64_sync_handler    =>  el0 64位 同步 异常处理函数
arch/arm64/kernel/entry-common.c:   el0_da                  =>  data/instruction abort的处理函数，获取寄存器信息，上下文切换，然后 do_mem_abort
arch/arm64/mm/fault.c:              do_mem_abort            =>  根据 esr_to_fault_info(esr) 得到的结果执行相应的处理函数，此处
    arch/arm64/mm/fault.c:          fault_info              =>  定义了不同类型异常描述及处理函数，此处关注的是 do_page_fault
arch/arm64/mm/fault.c:              __do_page_fault         =>  获取虚拟地址空间vma，地址校验，然后 handle_mm_fault
mm/memory.c:                        handle_mm_fault
mm/memory.c:                        __handle_mm_fault
mm/memory.c:                        handle_pte_fault
mm/memory.c:                        do_anonymous_page       => 对read fault特殊处理
```

内核中页表操作相关的一些缩写

```
pgd -> Page Global Directory
pud -> Page Upper Direcory, 处理4级页表引入的
p4d -> Page Level 4 Directory, 处理5级页表引入的
pmd -> Page Middle Directory
pte -> Page Table Entry
pfn = Page Frame Number
```

### 全局零页 

参考 [CSDN-linux那些事之zero page](https://blog.csdn.net/weixin_42730667/article/details/123121624)

1. 内核中包含一个全局的页`zero_page`，其内容为0
2. 发生 *读* pagefault的时候
   1. 将`zero_page`的地址映射到产生fault的页 -> `mm/memory.c:do_anonymous_page()`
   2. 映射为 *只读* 以确保`zero_page`的内容不会被修改
3. 发生 *写* pagefault的时候
   1. 分配新的物理页并建立映射


### ARM

异常级别

```
el0 -> 普通用户进程
el1 -> 内核
el2 -> hypervisor
el3 -> 保留，low-level firmware and security code
```
