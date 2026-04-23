#!/usr/bin/python3

from PIL import ImageFont
from PIL import Image
from PIL import ImageDraw

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

import sys
import random

ns = "0123456789"
ls = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

f = None

if len(sys.argv) == 2 and sys.argv[1] == '-h':
    print("Uso: %s <archivo>" % sys.argv[0])
    sys.exit(1)
elif len(sys.argv) == 2:
    f = sys.argv[1]

fo = ImageFont.truetype('GlNummernschildEng-XgWd.ttf', 60)
im = Image.new('1', (1, 1))
dr = ImageDraw.Draw(im)
w, h = dr.textsize('A', font=fo)

s = ''.join([random.choice(ls) for i in range(2)] + [random.choice(ns) for n in range(4)] + [random.choice(ls) for n in range(2)])
print(s)

if not f:
    f = s.replace(' ', '')

im = Image.new('1', (w * len(s), h))
dr = ImageDraw.Draw(im)


dr.rectangle((0, 0, w * len(s), h), outline=1, fill=1)
for i in range(len(s)):
    dr.text((i * w, -3), s[i], font=fo, fill=0, outline=0)
im.save('%s.bmp' % f)
