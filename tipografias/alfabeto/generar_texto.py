#!/usr/bin/python3

from PIL import ImageFont
from PIL import Image
from PIL import ImageDraw

import textwrap

import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

import sys

if len(sys.argv) != 4:
    print("Uso: %s <texto> <archivo> <ancho>" % sys.argv[0])
    sys.exit(1)

s = sys.argv[1]
f = sys.argv[2]
ww = int(sys.argv[3])

s = textwrap.fill(s, width=ww)

fo = ImageFont.load('10x20.pil')
im = Image.new('1', (1, 1))
dr = ImageDraw.Draw(im)
w, h = dr.textsize('A', font=fo)

im = Image.new('1', (w * ww, h * len(s.split('\n'))))
dr = ImageDraw.Draw(im)

dr.rectangle((0, 0, w * ww, h * len(s.split('\n'))), outline=1, fill=1)
dr.multiline_text((0, 0), s, font=fo, fill=0, spacing=0)
im.save('%s.bmp' % f)

