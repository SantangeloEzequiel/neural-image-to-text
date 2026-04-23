#!/usr/bin/python3

from PIL import ImageFont
from PIL import Image
from PIL import ImageDraw

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

import sys

if len(sys.argv) != 2:
    print("Uso: %s <archivo>" % sys.argv[0])
    sys.exit(1)

s = "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789"
f = sys.argv[1]

fo = ImageFont.truetype('LicensePlate.ttf', 30)
im = Image.new('1', (1, 1))
dr = ImageDraw.Draw(im)
w, h = dr.textsize('A', font=fo)

im = Image.new('1', (w * len(s), h))
dr = ImageDraw.Draw(im)

dr.rectangle((0, 0, w * len(s), h), outline=1, fill=1)
for i in range(len(s)):
    dr.text((i * w, -2), s[i], font=fo, fill=0, outline=0)
im.save('%s.bmp' % f)

fo = open('%s.txt' % f, 'wt')
fo.write('%d %d %d\n' % (len(s), w, h))
for i in range(len(s)):
    fo.write('%d\n' % i)
for c in s:
    fo.write(c + '\n')
fo.close()
