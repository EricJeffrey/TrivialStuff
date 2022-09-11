
# 平常使用各种软件遇到的问题/配置

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
