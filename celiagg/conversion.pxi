# The MIT License (MIT)
#
# Copyright (c) 2016 WUSTL ZPLAB
# Copyright (c) 2016-2023 Celiagg Contributors
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

from cython.view cimport array

cdef size_t _get_multiple(pixel_format):
    multiples = {
        1: (PixelFormat.Gray8,),
        2: (PixelFormat.Gray16,),
        3: (PixelFormat.BGR24, PixelFormat.RGB24),
        4: (PixelFormat.Gray32, PixelFormat.BGRA32, PixelFormat.RGBA32,
            PixelFormat.ARGB32, PixelFormat.ABGR32),
        6: (PixelFormat.BGR48, PixelFormat.RGB48),
        8: (PixelFormat.BGRA64, PixelFormat.RGBA64, PixelFormat.ARGB64,
            PixelFormat.ABGR64),
        12: (PixelFormat.BGR96, PixelFormat.RGB96),
        16: (PixelFormat.BGRA128, PixelFormat.RGBA128, PixelFormat.ARGB128,
             PixelFormat.ABGR128),
    }
    format_mults = {fmt: m for m, fmts in multiples.items() for fmt in fmts}
    return format_mults[pixel_format]


def convert_image(src, to_format, bottom_up=False):
    """convert_image(src, to_format, bottom_up=False)
    Create a new image with a desired pixel format and orientation.

    :param image: An Image instance
    :param to_format: A PixelFormat describing the desired output format
    :param bottom_up: If True, the image data is flipped in the y axis
    """
    if not isinstance(src, Image):
        raise TypeError("src must an Image instance")
    if not isinstance(to_format, (int, PixelFormat)):
        raise TypeError("to_format must be a PixelFormat value")

    cdef:
        PixelFormat dst_format = PixelFormat(to_format)
        PixelFormat src_format = src.format
        size_t count = src.width * src.height
        size_t multiple = _get_multiple(dst_format)
        unsigned char [:] srcbuf = src.pixels
        unsigned char [:] dstbuf = array(size=count * multiple,
                                         itemsize=sizeof(unsigned char),
                                         format='B', mode='c',
                                         allocate_buffer=True)

    _conversion.convert_pixels(<unsigned char*>&srcbuf[0],
                               <unsigned char*>&dstbuf[0],
                               count, src_format, dst_format)

    return Image(dstbuf, to_format, bottom_up=bottom_up)
