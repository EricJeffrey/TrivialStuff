# -*- coding: utf-8 -*-
import uuid
import requests
import hashlib
import time
import json

YOUDAO_URL = 'https://openapi.youdao.com/api'
APP_KEY = 'your_app_key'
APP_SECRET = 'your_app_secret'


def encrypt(signStr):
    hash_algorithm = hashlib.sha256()
    hash_algorithm.update(signStr.encode('utf-8'))
    return hash_algorithm.hexdigest()


def truncate(q):
    if q is None:
        return None
    size = len(q)
    return q if size <= 20 else q[0:10] + str(size) + q[size - 10:size]


def do_request(data):
    headers = {'Content-Type': 'application/x-www-form-urlencoded'}
    return requests.post(YOUDAO_URL, data=data, headers=headers)


def get_word_trans(word, out_path, audio_path):
    q = word

    data = {}
    data['from'] = 'en'
    data['to'] = 'zh-CHS'
    data['signType'] = 'v3'
    curtime = str(int(time.time()))
    data['curtime'] = curtime
    salt = str(uuid.uuid1())
    signStr = APP_KEY + truncate(q) + salt + curtime + APP_SECRET
    sign = encrypt(signStr)
    data['appKey'] = APP_KEY
    data['q'] = q
    data['salt'] = salt
    data['sign'] = sign
    # data['vocabId'] = "您的用户词表ID"

    response = do_request(data)
    contentType = response.headers['Content-Type']
    if contentType == "audio/mp3":
        millis = int(round(time.time() * 1000))
        filePath = audio_path + ".mp3"
        fo = open(filePath, 'wb')
        fo.write(response.content)
        fo.close()
    else:
        with open(out_path, "a", encoding='utf-8') as fp:
            content = response.content
            try:
                fp.write(content.decode()+"\n")
            except:
                fp.write(str(content) + "\n")
                print('gg')
        # print(response.content.decode())


def translate_words(words):
    i = 0
    for word in words:
        get_word_trans(word, "words.txt", word)
        time.sleep(0.05)
        i += 1
        print(i)


def extract_info():
    lines = []
    with open("words.txt", "rt", encoding='utf-8') as fp:
        for line in fp.readlines():
            word_info = json.loads(line)
            tmpline = word_info["query"]+":"
            basic_info = word_info["basic"]
            if "phonetic" in basic_info:
                tmpline += "[%s] " % (basic_info["phonetic"])
            if "explains" in basic_info:
                tmpline += "; ".join(basic_info["explains"])
            lines.append(tmpline + "\n")
    with open("words_extracted.txt", "a", encoding='utf-8') as fp:
        fp.writelines(lines)


if __name__ == '__main__':
    words = ["impulse", "outlet", "contrive", "conceive", "mere", "relate", "aflame", "murmurous", "hemisphere", "weariness", "exquisite", "marvel", "vouchsafed", "steerage", "prow", "borne", "hither", "oppressed", "dweller", "defraud", "tenet", "beacon", "thronging", "heroic", "comparatively", "profoundly", "fuse", "dizzily", "whirlpools", "tyranny", "guise", "benevolence", "interfered", "buckle up", "piston", "locomotive", "friction", "stagger", "in irons", "defy", "amidst", "intricate", "nexus", "contend",
             "resolute", "consecrated", "anew", "aloft", "striving", "utter", "deserts", "diverge", "untrammeled", "salvation", "condescension", "monopoly", "atrophy", "tariffs", "preach", "gird", "proportion", "wept", "philanthropic", "emancipation", "redeem", "sapped", "fertility", "sullenly", "aspiration", "boast", "subside", "slacken", "pulpy", "handicap", "dethroned", "trammel", "sanitary", "broad", "preceded", "patron", "privation", "imperishable", "foothold", "teeming", "cease", "unfettering", "caravel", ]
    # translate_words(words)
    extract_info()
