"""A very simplistic SVG renderer.
"""

from __future__ import division
import argparse
from collections import OrderedDict
import os.path as op
import re
from xml.etree.ElementTree import parse, register_namespace

import numpy as np
from skimage.io import imsave

import celiagg as agg

NS_SVG = '{http://www.w3.org/2000/svg}'
NS_XLINK = '{http://www.w3.org/1999/xlink}'
UNITS = 'ceimnptx'

# Lambdas allow forward declarations
ELEMENT_PARSERS = {
    NS_SVG+'defs': lambda: parse_defs,
    NS_SVG+'g': lambda: parse_graphics,
    NS_SVG+'path': lambda: parse_path,
    NS_SVG+'linearGradient': lambda: parse_gradient,
    NS_SVG+'radialGradient': lambda: parse_gradient,
}


def _parse_generic(elem, context):
    parser = ELEMENT_PARSERS.get(elem.tag, lambda: None)()
    if parser is not None:
        elem_id = elem.get('id')
        context[elem_id] = parser(elem, context)


def interpret_color(value, context):
    color = parse_color(value)
    if color is not None:
        return agg.SolidPaint(*color)
    elif value.lower() == 'none':
        return None
    elif value.startswith('url'):
        match = re.match(r'url\(#(?P<label>\w+)\)', value)
        if match is None:
            return None
        obj = context.get(match.group('label'))
        if obj is None or 'type' not in obj:
            return None
        if obj['type'] == 'linearGradient':
            grad = agg.LinearGradientPaint(
                obj['x1'], obj['y1'], obj['x2'], obj['y2'], obj['stops'],
                obj.get('spreadMethod', agg.GradientSpread.SpreadPad),
                obj.get('gradientUnits', agg.GradientUnits.ObjectBoundingBox)
            )
            grad.transform = obj.get('gradientTransform', agg.Transform())
            return grad
        if obj['type'] == 'radialGradient':
            grad = agg.RadialGradientPaint(
                obj['cx'], obj['cy'], obj['r'], obj['fx'], obj['fy'],
                obj['stops'],
                obj.get('spreadMethod', agg.GradientSpread.SpreadPad),
                obj.get('gradientUnits', agg.GradientUnits.ObjectBoundingBox)
            )
            grad.transform = obj.get('gradientTransform', agg.Transform())
            return grad


def interpret_state(style, stroke, fill):
    state = agg.GraphicsState(
        master_alpha=float(style.get('opacity', 1.0)),
        line_width=parse_united(style.get('stroke-width', '1')),
        miter_limit=float(style.get('stroke-miterlimit', 4.0))
    )

    mode = agg.DrawingMode.DrawFillStroke
    if 'nonzero' == style.get('stroke-fillrule', 'nonzero'):
        if stroke is None and fill is not None:
            mode = agg.DrawingMode.DrawFill
        elif stroke is not None and fill is None:
            mode = agg.DrawingMode.DrawStroke
        elif stroke is not None and fill is not None:
            mode = agg.DrawingMode.DrawFillStroke
    else:
        if stroke is None and fill is not None:
            mode = agg.DrawingMode.EofDrawFill
        elif stroke is not None and fill is None:
            mode = agg.DrawingMode.EofDrawStroke
        elif stroke is not None and fill is not None:
            mode = agg.DrawingMode.EofDrawFillStroke
    state.drawing_mode = mode

    return state


def interpret_style(style, context):
    fill = interpret_color(style.get('fill', 'none'), context)
    stroke = interpret_color(style.get('stroke', 'none'), context)
    state = interpret_state(style, stroke, fill)
    return state, stroke, fill


def parse_color(value):
    def unhex(c):
        return int(c, base=16)

    HTML_COLORS = {
        'black': (0, 0, 0), 'green': (0, 128, 0),
        'silver': (192, 192, 192), 'lime': (0, 255, 0),
        'gray': (128, 128, 128), 'olive': (128, 128, 0),
        'white': (255, 255, 255), 'yellow': (255, 255, 0),
        'maroon': (128, 0, 0), 'navy': (0, 0, 128),
        'red': (255, 0, 0), 'blue': (0, 0, 255),
        'purple': (128, 0, 128), 'teal': (0, 128, 128),
        'fuchsia': (255, 0, 255), 'aqua': (0, 255, 255),
    }
    if value.startswith('#'):
        value = value[1:]
        if len(value) == 3:
            r, g, b = value
            return (unhex(r*2)/255, unhex(g*2)/255, unhex(b*2)/255)
        elif len(value) == 6:
            r, g, b = value[:2], value[2:4], value[4:]
            return (unhex(r)/255, unhex(g)/255, unhex(b)/255)
    elif value.startswith('rgb'):
        match = re.match(r'rgb\((?P<args>[0-9.,-]+)\)', value)
        if match is None:
            return None
        args = match.group('args').split(',')
        if len(args) != 3:
            return None
        if all(a.endswith('%') for a in args):
            return tuple([float(a[:-1])/100 for a in args])
        else:
            return tuple([int(a)/255 for a in args])
    elif value in HTML_COLORS:
        value = HTML_COLORS[value]
        return tuple([a/255 for a in value])
    return None


def parse_defs(elem, context):
    for child in elem:
        _parse_generic(child, context)


def parse_gradient_stop(elem):
    offset = float(elem.get('offset', 0))
    style = parse_style(elem.get('style', ''))
    a = float(style.get('stop-opacity', 1.0))
    r, g, b = parse_color(style.get('stop-color', '#000000'))
    return (offset, r, g, b, a)


def parse_graphics(elem, context):
    graphics = OrderedDict()
    for child in elem:
        _parse_generic(child, graphics)
    context.setdefault('graphics', []).append(graphics)


def parse_gradient(elem, context):
    GRAD_UNITS = {'userSpaceOnUse': agg.GradientUnits.UserSpace,
                  'objectBoundingBox': agg.GradientUnits.ObjectBoundingBox}
    GRAD_SPREAD = {'pad': agg.GradientSpread.SpreadPad,
                   'reflect': agg.GradientSpread.SpreadReflect,
                   'repeat': agg.GradientSpread.SpreadRepeat}
    CONVERTERS = {
        'x1': float, 'x2': float, 'y1': float, 'y2': float,
        'cx': float, 'cy': float, 'r': float, 'fx': float, 'fy': float,
        'gradientTransform': parse_transform,
        'gradientUnits': lambda x: GRAD_UNITS[x],
        'spreadMethod': lambda x: GRAD_SPREAD[x],
    }
    grad = {}
    if NS_XLINK+'href' in elem.attrib:
        link = elem.get(NS_XLINK+'href')
        if link.startswith('#') and link[1:] in context:
            del elem.attrib[NS_XLINK+'href']
            grad = context[link[1:]].copy()
    grad['type'] = elem.tag[len(NS_SVG):]
    for name, value in elem.attrib.items():
        if name in CONVERTERS:
            value = CONVERTERS[name](value)
        grad[name] = value
    for stop in elem.findall(NS_SVG+'stop'):
        grad.setdefault('stops', []).append(parse_gradient_stop(stop))
    return grad


def parse_path(elem, context):
    def extractnum(it, offset):
        return float(next(it).group('num')) + offset

    regex = re.compile(
        '(?P<command>[MmZzLlHhVvCcSsQqTtAa])|'
        '(?P<num>-?(([0-9]*[.][0-9]*)|[0-9]+)([eE][+-]?[0-9]*)?)'
    )
    path = agg.Path()
    dataiter = regex.finditer(elem.get('d', ''))
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

    return {'style': parse_style(elem.get('style', '')),
            'transform': parse_transform(elem.get('transform', '')),
            'data': path}


def parse_style(style):
    ret = {}
    for p in style.split(';'):
        k, v = p.split(':')
        ret[k] = v
    return ret


def parse_svg(path):
    context = {}
    root = parse(path).getroot()
    context['__document__'] = root.attrib.copy()
    for child in root:
        _parse_generic(child, context)
    return context


def parse_transform(value):
    def rotate(deg):
        t = agg.Transform()
        t.rotate(deg * np.pi / 180)
        return t

    def scale(*s):
        sx = s[0]
        sy = sx if len(s) == 1 else s[1]
        return agg.Transform(sx, 0.0, 0.0, sy, 0.0, 0.0)

    def translate(*t):
        tx = t[0]
        ty = 0.0 if len(t) == 1 else t[1]
        return agg.Transform(1.0, 0.0, 0.0, 1.0, tx, ty)

    FUNCTIONS = {
        'matrix': agg.Transform,
        'translate': translate,
        'rotate': rotate,
        'scale': scale,
        'skewX': lambda s: agg.Transform(1.0, 0.0, s, 1.0, 0.0, 0.0),
        'skewY': lambda s: agg.Transform(1.0, s, 0.0, 1.0, 0.0, 0.0),
    }
    match = re.match(r'(?P<func>\w+)\((?P<args>[0-9.,-e]+)\)', value)
    if match is None:
        return None

    func = FUNCTIONS.get(match.group('func'), lambda *a: None)
    args = match.group('args')
    args = [float(a) for a in args.split(',')]
    return func(*args)


def parse_united(value):
    units = {'px': 1, 'pt': 1.25, 'pc': 15, 'mm': 3.543307,
             'cm': 35.43307, 'in': 90}
    scale = units.get(value[-2:])
    if scale is not None:
        return scale * float(value[:-2])
    else:
        return float(value)


def render(context, scale=1.0):
    docattrs = context['__document__']
    width = parse_united(docattrs.get('width', '100')) * scale
    height = parse_united(docattrs.get('height', '100')) * scale
    canvas = agg.CanvasRGB24(np.empty((int(height), int(width), 3),
                                      dtype=np.uint8))
    canvas.clear(1, 1, 1)
    for group in context['graphics']:
        for elem in group.values():
            transform = elem.get('transform') or agg.Transform()
            shape = elem['data']
            gs, stroke, fill = interpret_style(elem['style'], context)
            transform.scale(scale, scale)
            canvas.draw_shape(shape, transform, gs, stroke=stroke, fill=fill)
    return canvas.array


def main():
    register_namespace('svg', NS_SVG[1:-1])
    register_namespace('xlink', NS_XLINK[1:-1])

    parser = argparse.ArgumentParser()
    parser.add_argument('file', type=str)
    parser.add_argument('-s', '--scale', type=float, default=1.0)

    args = parser.parse_args()
    context = parse_svg(op.abspath(args.file))
    buffer = render(context, scale=args.scale)
    outname = op.splitext(op.basename(args.file))[0] + '.png'
    imsave(outname, buffer)


if __name__ == '__main__':
    main()
