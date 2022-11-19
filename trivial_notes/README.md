
# 平常使用各种软件遇到的问题/配置

### Linux

网络流量/磁盘IO查看: `nethogs/iostat`

虚拟网卡 - tun/tap/can
- tun与tap是操作系统内核中的虚拟网络设备。tun是网络层的虚拟网络设备，可以收发第三层数据报文包，如IP封包，因此常用于一些点对点IP隧道，例如OpenVPN，IPSec等。tap是链路层的虚拟网络设备，等同于一个以太网设备，它可以收发第二层数据报文包，如以太网数据帧。Tap最常见的用途就是做为虚拟机的网卡，因为它和普通的物理网卡更加相近，也经常用作普通机器的虚拟网卡。
- can(Controller Area Network)是一个面向车辆总线的接口协议，允许微控制器和设备在没有主机的情况下相互通信。


#### RaspberryPi

使用vscode-remote-ssh连接的时候一直连不上，起初以为是aarch64架构不支持，捣鼓了半天，后来发现不是不支持，而是系统崩了，然后
```sh
sudo journalctl -xe
# ... Sep 11 15:34:11 raspberrypi kernel: hwmon hwmon1: Undervoltage detected!
```

`Undervoltage detected`: 电压过低

### VIM

使用Windows Terminal遇到的问题，在进入vim后，光标无法闪烁了，并且退出vim仍然无法闪烁，搜索后发现是vim默认会使用DEC控制字符关闭光标: [mintty](https://github.com/mintty/wsltty/issues/133), [alacritty](https://github.com/alacritty/alacritty/issues/4609), [WindowsTerminal](https://github.com/microsoft/terminal/issues/4335)

解决方法为在vimrc中添加如下代码，其中`autocmd VimEnter`设置进入vim的命令，`\e[5 q`是控制序列，`t_SI/t_EI`分别是插入模式和正常模式下终端光标的样式

```
if &term =~ '^xterm'
  autocmd VimEnter * silent !echo -ne "\e[5 q"
  " solid underscore
  let &t_SI .= "\<Esc>[5 q"
  " solid block
  let &t_EI .= "\<Esc>[5 q"

  " 1 or 0 -> blinking block
  " 3 -> blinking underscore
  " Recent versions of xterm (282 or above) also support
  " 5 -> blinking vertical bar
  " 6 -> solid vertical bar
endif
```

### bash

Ctrl+箭头向前/后移动一个word的配置
```sh
#.inputrc
"\e[1;5C": forward-word
"\e[1;5D": backward-word
"\e[5C": forward-word
"\e[5D": backward-word
"\e\e[C": forward-word
"\e\e[D": backward-word
```
