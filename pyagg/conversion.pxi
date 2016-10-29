# The MIT License (MIT)
#
# Copyright (c) 2016 WUSTL ZPLAB
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Authors: John Wiggins

cdef _get_component_indices(fmt):
    order_map = {
        (0, 0, 0, 0): (PixelFormat.Gray8, PixelFormat.Gray16, PixelFormat.Gray32),
        (0, 1, 2, 0): (PixelFormat.RGB24, PixelFormat.RGB48, PixelFormat.RGB96),
        (2, 1, 0, 0): (PixelFormat.BGR24, PixelFormat.BGR48, PixelFormat.BGR96),
        (0, 1, 2, 3): (PixelFormat.RGBA32, PixelFormat.RGBA64, PixelFormat.RGBA128),
        (2, 1, 0, 3): (PixelFormat.BGRA32, PixelFormat.BGRA64, PixelFormat.BGRA128),
        (3, 0, 1, 2): (PixelFormat.ARGB32, PixelFormat.ARGB64, PixelFormat.ARGB128),
        (3, 2, 1, 0): (PixelFormat.ABGR32, PixelFormat.ABGR64, PixelFormat.ABGR128),
    }
    rev_order_map = {f: o for o, fs in order_map.items() for f in fs}
    return rev_order_map[fmt]


cdef bool _has_alpha(fmt):
    return fmt not in (PixelFormat.Gray8, PixelFormat.Gray16, PixelFormat.Gray32,
                       PixelFormat.BGR24, PixelFormat.RGB24, PixelFormat.BGR48,
                       PixelFormat.RGB48,  PixelFormat.BGR96, PixelFormat.RGB96)


cdef bool _is_gray(fmt):
    return fmt in (PixelFormat.Gray8, PixelFormat.Gray16, PixelFormat.Gray32)


cdef bool _is_rgb(fmt):
    return fmt not in (PixelFormat.Gray8, PixelFormat.Gray16, PixelFormat.Gray32)


cdef float _get_max_value(dtype):
    if dtype.kind == 'f':
        return 1.0
    return float(numpy.iinfo(dtype).max)


cdef gray2rgb(src, dst_order, dtype=None, alpha=False):
    """ Convert gray pixels to RGB[A].
    """
    last_dim = (4,) if alpha else (3,)
    dst = numpy.empty(src.shape + last_dim, dtype=dtype)

    max_from = _get_max_value(src.dtype)
    max_to = _get_max_value(dst.dtype)

    scaled_src = src[:] * (max_to / max_from)
    dst[..., dst_order[0]] = scaled_src
    dst[..., dst_order[1]] = scaled_src
    dst[..., dst_order[2]] = scaled_src
    if alpha:
        dst[..., dst_order[3]] = max_to

    return dst


cdef rgb2rgb(src, src_order, dst_order, dtype=None, alpha=False):
    """ Convert RGB[A] pixels to RGB[A] pixels.
    """
    last_dim = (4,) if alpha else (3,)
    dst = numpy.empty(src.shape[:2] + last_dim, dtype=dtype)

    max_from = _get_max_value(src.dtype)
    max_to = _get_max_value(dst.dtype)
    value_scale = (max_to / max_from)

    dst[..., dst_order[0]] = src[..., src_order[0]] * value_scale
    dst[..., dst_order[1]] = src[..., src_order[1]] * value_scale
    dst[..., dst_order[2]] = src[..., src_order[2]] * value_scale
    if alpha:
        if src.shape[2] == 4:
            dst[..., dst_order[3]] = src[..., src_order[3]] * value_scale
        else:
            dst[..., dst_order[3]] = max_to

    return dst


cdef gray2gray(src, dtype=None):
    """ Convert gray pixels to gray pixels.
    """
    dst = numpy.empty(src.shape, dtype=dtype)
    max_from = _get_max_value(src.dtype)
    max_to = _get_max_value(dst.dtype)
    dst[:] = src[:] * (max_to / max_from)
    return dst


cdef rgb2gray(src, src_order, dtype=None):
    """ Convert RGB[A] pixels to gray.
    """
    dst = numpy.empty(src.shape[:2], dtype=dtype)

    max_from = _get_max_value(dst.dtype)
    max_to = _get_max_value(src.dtype)

    dst[:] = (src[..., src_order[0]] + src[..., src_order[1]] +
              src[..., src_order[2]]) * (max_to / (3.0 * max_from))
    return dst


def convert_image(Image src, PixelFormat to_format, bottom_up=False):
    """ Create a new image from input ``src`` with desired pixel format
    ``to_format`` and orientation ``bottom_up``.
    """
    from_format = src.pixel_format
    dst_dtype = _get_format_dtype(to_format)
    cdef bool dst_alpha = _has_alpha(to_format)
    cdef bool src_gray = _is_gray(from_format)
    cdef bool dst_gray = _is_gray(to_format)
    cdef bool src_rgb = _is_rgb(from_format)
    cdef bool dst_rgb = _is_rgb(to_format)

    src_order = _get_component_indices(from_format)
    dst_order = _get_component_indices(to_format)

    if src_gray:
        if dst_gray:
            dst = gray2gray(src.pixels, dtype=dst_dtype)
        else:
            dst = gray2rgb(src.pixels, dst_order, dtype=dst_dtype,
                           alpha=dst_alpha)
    else:
        if dst_gray:
            dst = rgb2gray(src.pixels, src_order, dtype=dst_dtype)
        else:
            dst = rgb2rgb(src.pixels, src_order, dst_order, dtype=dst_dtype,
                          alpha=dst_alpha)

    return Image(dst, to_format, bottom_up=bottom_up)
