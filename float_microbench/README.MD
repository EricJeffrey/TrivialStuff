
# 浮点数性能运算

参考:
- [compiler explorer](https://godbolt.org/)
- [GCC内联汇编格式](https://blog.csdn.net/weixin_42258222/article/details/116501867)
- [Intel I5-12600KF信息](https://www.intel.com/content/www/us/en/products/sku/134590/intel-core-i512600kf-processor-20m-cache-up-to-4-90-ghz/specifications.html)
- [Intel I5-12600KF指标](https://nanoreview.net/en/cpu/intel-core-i5-12600kf)
- [VFMADD132SS指令家族格式](https://www.felixcloutier.com/x86/vfmadd132ss:vfmadd213ss:vfmadd231ss)


## 主要方法：

- 使用`VFMADD213SS`汇编指令进行 `t += x * y` 操作
- 使用`gcc -masm=intel -mavx512f`参数编译，保证支持相应指令
- 使用内联汇编直接对`xmm1,xmm2,xmm3`寄存器赋值，并内联`vfmadd231ss %%xmm1, %%xmm2, %%xmm3`执行
- 使用宏展开在一次循环中增加执行次数，消减循环变量读取的影响
- 使用`register`指示符指定循环变量为寄存器

## 待完成
- 绑CPU核心+锁定CPU频率？
- float累加精读丢失？
