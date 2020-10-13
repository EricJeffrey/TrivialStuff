# coding=utf-8
import pyperclip
import os
import json
import time


sno2nameStr = '''{"SAXXXX":"XX旺","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX轩","SAXXXX":"XX轩","SAXXXX":"XX","SAXXXX":"XX雅","SAXXXX":"XX楠","SAXXXX":"XX峻","SAXXXX":"XX迪","SAXXXX":"XX晨","SAXXXX":"XX帆","SAXXXX":"XX岩","SAXXXX":"XX","SAXXXX":"XX伟","SAXXXX":"XX航","SAXXXX":"XX辛","SAXXXX":"XX亮","SAXXXX":"XX","SAXXXX":"XX扬","SAXXXX":"XX兰","SAXXXX":"XX霞","SAXXXX":"XX","SAXXXX":"XX昕","SAXXXX":"XX宁","SAXXXX":"XX","SAXXXX":"XX文","SAXXXX":"XX帆","SAXXXX":"XX","SAXXXX":"XX宇","SAXXXX":"XX涵","SAXXXX":"XX杰","SAXXXX":"XX祥","SAXXXX":"XX江","SAXXXX":"XX儒","SAXXXX":"XX悠","SAXXXX":"XX奎","SAXXXX":"XX燃","SAXXXX":"XX","SAXXXX":"XX人","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX猛","SAXXXX":"XX兴","SAXXXX":"XX梓","SAXXXX":"XX","SAXXXX":"XX鑫","SAXXXX":"XX为","SAXXXX":"XX航","SAXXXX":"XX凯","SAXXXX":"XX滢","SAXXXX":"XX","SAXXXX":"XX辕","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX鹏","SAXXXX":"XX磊","SAXXXX":"XX","SAXXXX":"XX阳","SAXXXX":"XX辉","SAXXXX":"XX程","SAXXXX":"XX坤","SAXXXX":"XX","SAXXXX":"XX凯","SAXXXX":"XX飞","SAXXXX":"XX红","SAXXXX":"XX坤","SAXXXX":"XX慧","SAXXXX":"XX玲","SAXXXX":"XX毅","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX翔","SAXXXX":"XX威","SAXXXX":"XX强","SAXXXX":"XX凯","SAXXXX":"XX扬","SAXXXX":"XX文","SAXXXX":"XX毅","SAXXXX":"XX鑫","SAXXXX":"XX","SAXXXX":"XX然","SAXXXX":"XX雨","SAXXXX":"XX","SAXXXX":"XX成","SAXXXX":"XX","SAXXXX":"XX科","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX韬","SAXXXX":"XX东","SAXXXX":"XX瑞","SAXXXX":"XX","SAXXXX":"XX冰","SAXXXX":"XX同","SAXXXX":"XX梅","SAXXXX":"XX","SAXXXX":"XX远","SAXXXX":"XX聪","SAXXXX":"XX博","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX冉","SAXXXX":"XX豪","SAXXXX":"XX月","SAXXXX":"XX楠","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX怡","SAXXXX":"XX坤","SAXXXX":"XX晶","SAXXXX":"XX杰","SAXXXX":"XX","SAXXXX":"XX博","SAXXXX":"XX瑞","SAXXXX":"XX杰","SAXXXX":"XX炜","SAXXXX":"XX熠","SAXXXX":"XX","SAXXXX":"XX宇","SAXXXX":"XX源","SAXXXX":"XX超","SAXXXX":"XX删","SAXXXX":"XX深","SAXXXX":"XX深","SAXXXX":"XX刘","SAXXXX":"XX辉","SAXXXX":"XX枫","SAXXXX":"XX","SAXXXX":"XX轩","SAXXXX":"XX伦","SAXXXX":"XX伟","SAXXXX":"XX杰","SAXXXX":"XX亚","SAXXXX":"XX鑫","SAXXXX":"XX宇","SAXXXX":"XX强","SAXXXX":"XX","SAXXXX":"XX拓","SAXXXX":"XX君","SAXXXX":"XX尚","SAXXXX":"XX强","SAXXXX":"XX","SAXXXX":"XX婷","SAXXXX":"XX","SAXXXX":"XX宁","SAXXXX":"XX楠","SAXXXX":"XX嘉","SAXXXX":"XX","SAXXXX":"XX斌","SAXXXX":"XX苇","SAXXXX":"XX钧","SAXXXX":"XX展","SAXXXX":"XX","SAXXXX":"XX豪","SAXXXX":"XX君","SAXXXX":"XX龙","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX佳","SAXXXX":"XX超","SAXXXX":"XX箭","SAXXXX":"XX明","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX红","SAXXXX":"XX颖","SAXXXX":"XX峰","SAXXXX":"XX","SAXXXX":"XX","SAXXXX":"XX叶","SAXXXX":"XX","SAXXXX":"XX磊","SAXXXX":"XX","SAXXXX":"XX燕","SAXXXX":"XXHAMMAD_HAMZA"}'''


def work():
    print("监控剪贴板的内容，自动打开文件")
    sno2name = json.loads(sno2nameStr)
    allfiles = os.listdir()
    lastsno = ""
    cnt = 0
    while True:
        s = pyperclip.paste()
        if s != lastsno and s in sno2name:
            for f in allfiles:
                if s in f:
                    cnt += 1
                    print("%d %s %s" % (cnt, s, sno2name[s]))
                    os.system('\"%s\"' % (f))
            lastsno = s
        time.sleep(0.1)


if __name__ == "__main__":
    print("请将该脚本移动到包含所有学生文件的文件夹中")
    work()
