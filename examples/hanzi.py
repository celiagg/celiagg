""" A simple renderer for Hanzi Writer data:
https://github.com/chanind/hanzi-writer-data
"""

from __future__ import division

import argparse
import json
import os.path as op
import re

import numpy as np
from skimage.io import imsave

import celiagg as agg


def get_bounds(paths):
    min_x, min_y, max_x, max_y = None, None, None, None
    for p in paths:
        verts = p.vertices()
        for v in verts:
            if min_x is None:
                min_x = v[0]
            else:
                min_x = min(min_x, v[0])
            if min_y is None:
                min_y = v[1]
            else:
                min_y = min(min_y, v[1])
            if max_x is None:
                max_x = v[0]
            else:
                max_x = max(max_x, v[0])
            if max_y is None:
                max_y = v[1]
            else:
                max_y = max(max_y, v[1])
    return min_x, min_y, max_x, max_y


def parse_path(path_str):
    def extractnum(it, offset):
        return float(next(it).group('num')) + offset

    regex = re.compile(
        '(?P<command>[MmZzLlHhVvCcSsQqTtAa])|'
        '(?P<num>-?(([0-9]*[.][0-9]*)|[0-9]+)([eE][+-]?[0-9]*)?)'
    )
    path = agg.Path()
    dataiter = regex.finditer(path_str)
    currentpt = (0.0, 0.0)
    lastctrl = None
    for chunk in dataiter:
        command = chunk.group('command')
        if command in 'mM':
            offx = currentpt[0] if command.islower() else 0.0
            offy = currentpt[1] if command.islower() else 0.0
            x, y = (extractnum(dataiter, offx), extractnum(dataiter, offy))
            path.move_to(x, y)
            currentpt = (x, y)
            lastctrl = None
        elif command in 'lL':
            offx = currentpt[0] if command.islower() else 0.0
            offy = currentpt[1] if command.islower() else 0.0
            x, y = (extractnum(dataiter, offx), extractnum(dataiter, offy))
            path.line_to(x, y)
            currentpt = (x, y)
            lastctrl = None
        elif command in 'hH':
            offx = currentpt[0] if command.islower() else 0.0
            x, y = (extractnum(dataiter, offx), currentpt[1])
            path.line_to(x, y)
            currentpt = (x, y)
            lastctrl = None
        elif command in 'vV':
            offy = currentpt[1] if command.islower() else 0.0
            x, y = (currentpt[0], extractnum(dataiter, offy))
            path.line_to(x, y)
            currentpt = (x, y)
            lastctrl = None
        elif command in 'cC':
            offx = currentpt[0] if command.islower() else 0.0
            offy = currentpt[1] if command.islower() else 0.0
            x1, y1 = extractnum(dataiter, offx), extractnum(dataiter, offy)
            x2, y2 = extractnum(dataiter, offx), extractnum(dataiter, offy)
            x, y = extractnum(dataiter, offx), extractnum(dataiter, offy)
            path.cubic_to(x1, y1, x2, y2, x, y)
            currentpt = (x, y)
            lastctrl = (x2, y2)
        elif command in 'sS':
            offx = currentpt[0] if command.islower() else 0.0
            offy = currentpt[1] if command.islower() else 0.0
            x1, y1 = currentpt if lastctrl is None else lastctrl
            x2, y2 = extractnum(dataiter, offx), extractnum(dataiter, offy)
            x, y = extractnum(dataiter, offx), extractnum(dataiter, offy)
            path.cubic_to(x1, y1, x2, y2, x, y)
            currentpt = (x, y)
            lastctrl = (x2, y2)
        elif command in 'qQ':
            offx = currentpt[0] if command.islower() else 0.0
            offy = currentpt[1] if command.islower() else 0.0
            x1, y1 = extractnum(dataiter, offx), extractnum(dataiter, offy)
            x, y = extractnum(dataiter, offx), extractnum(dataiter, offy)
            path.quadric_to(x1, y1, x, y)
            currentpt = (x, y)
            lastctrl = (x1, y1)
        elif command in 'tT':
            offx = currentpt[0] if command.islower() else 0.0
            offy = currentpt[1] if command.islower() else 0.0
            x1, y1 = currentpt if lastctrl is None else lastctrl
            x, y = extractnum(dataiter, offx), extractnum(dataiter, offy)
            path.quadric_to(x1, y1, x, y)
            currentpt = (x, y)
            lastctrl = (x1, y1)
        elif command in 'aA':
            # Elliptical arc. ``agg.Path`` does not directly support this.
            next(dataiter)
            next(dataiter)
            next(dataiter)
            next(dataiter)
            next(dataiter)
            next(dataiter)
            next(dataiter)
        elif command in 'zZ':
            path.close()

    return path


def render(paths, scale=1.0):
    min_x, min_y, max_x, max_y = get_bounds(paths)
    width, height = (max_x - min_x) * scale, (max_y - min_y) * scale
    margin_x, margin_y = width * 0.15, height * 0.15
    width += margin_x * 2
    height += margin_y * 2

    off_x = -min_x + margin_x
    off_y = -min_y + margin_y

    canvas = agg.CanvasRGB24(np.empty((int(height), int(width), 3),
                                      dtype=np.uint8), bottom_up=True)
    canvas.clear(1, 1, 1)
    state = agg.GraphicsState(master_alpha=1.0, line_width=1,
                              drawing_mode=agg.DrawingMode.DrawFillStroke)
    transform = agg.Transform()
    transform.scale(scale, scale)
    transform.translate(off_x, off_y)
    black = agg.SolidPaint(0.0, 0.0, 0.0)
    for path in paths:
        canvas.draw_shape(path, transform, state, stroke=black, fill=black)
    return canvas.array


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('file', type=str)
    parser.add_argument('-s', '--scale', type=float, default=1.0)

    args = parser.parse_args()
    with open(op.abspath(args.file), 'r') as fp:
        data = json.load(fp)
    paths = [parse_path(p) for p in data['strokes']]
    buffer = render(paths, scale=args.scale)
    outname = op.splitext(op.basename(args.file))[0] + '.png'
    imsave(outname, buffer)


if __name__ == '__main__':
    main()
