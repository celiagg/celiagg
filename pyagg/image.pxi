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

ctypedef _image.Image* img_ptr_t

cdef _get_format_dtype(PixelFormat pixel_format):
    dtypes = {
        numpy.uint8: (PixelFormat.Gray8, PixelFormat.BGR24,
                      PixelFormat.RGB24, PixelFormat.BGRA32,
                      PixelFormat.RGBA32, PixelFormat.ARGB32,
                      PixelFormat.ABGR32),
        numpy.uint16: (PixelFormat.Gray16, PixelFormat.BGR48,
                       PixelFormat.RGB48, PixelFormat.BGRA64,
                       PixelFormat.RGBA64, PixelFormat.ARGB64,
                       PixelFormat.ABGR64),
        numpy.float32: (PixelFormat.Gray32, PixelFormat.BGR96,
                        PixelFormat.RGB96, PixelFormat.BGRA128,
                        PixelFormat.RGBA128, PixelFormat.ARGB128,
                        PixelFormat.ABGR128),
    }
    format_dtypes = {fmt: dt for dt, fmts in dtypes.items() for fmt in fmts}
    return format_dtypes[pixel_format]


cdef _get_format_last_dim(PixelFormat pixel_format):
    dims = {
        1: (PixelFormat.Gray8, PixelFormat.Gray16, PixelFormat.Gray32),
        3: (PixelFormat.BGR24, PixelFormat.RGB24, PixelFormat.BGR48,
            PixelFormat.RGB48, PixelFormat.BGR96, PixelFormat.RGB96),
        4: (PixelFormat.BGRA32, PixelFormat.RGBA32, PixelFormat.ARGB32,
            PixelFormat.ABGR32, PixelFormat.BGRA64, PixelFormat.RGBA64,
            PixelFormat.ARGB64, PixelFormat.ABGR64, PixelFormat.BGRA128,
            PixelFormat.RGBA128, PixelFormat.ARGB128, PixelFormat.ABGR128)
    }
    format_dims = {fmt: dim for dim, fmts in dims.items() for fmt in fmts}
    return format_dims[pixel_format]


cdef img_ptr_t _get_2d_u8_img(uint8_t[:, ::1] arr, PixelFormat pixel_format):
    return new _image.Image(<uint8_t*>&arr[0][0], arr.shape[1], arr.shape[0],
                                     arr.strides[0], pixel_format)


cdef img_ptr_t _get_2d_u16_img(uint16_t[:, ::1] arr, PixelFormat pixel_format):
    return new _image.Image(<uint8_t*>&arr[0][0], arr.shape[1], arr.shape[0],
                                     arr.strides[0], pixel_format)


cdef img_ptr_t _get_2d_f32_img(float[:, ::1] arr, PixelFormat pixel_format):
    return new _image.Image(<uint8_t*>&arr[0][0], arr.shape[1], arr.shape[0],
                                     arr.strides[0], pixel_format)


cdef img_ptr_t _get_3d_u8_img(uint8_t[:, :, ::1] arr, PixelFormat pixel_format):
    return new _image.Image(<uint8_t*>&arr[0][0][0], arr.shape[1], arr.shape[0],
                                     arr.strides[0], pixel_format)


cdef img_ptr_t _get_3d_u16_img(uint16_t[:, :, ::1] arr, PixelFormat pixel_format):
    return new _image.Image(<uint8_t*>&arr[0][0][0], arr.shape[1], arr.shape[0],
                                     arr.strides[0], pixel_format)


cdef img_ptr_t _get_3d_f32_img(float[:, :, ::1] arr, PixelFormat pixel_format):
    return new _image.Image(<uint8_t*>&arr[0][0][0], arr.shape[1], arr.shape[0],
                                     arr.strides[0], pixel_format)


cdef img_ptr_t _get_image(array, pixel_format):
    dtype = _get_format_dtype(pixel_format)

    # Finally build the image
    if array.ndim == 2:
        if dtype is numpy.uint8:
            return _get_2d_u8_img(array, pixel_format)
        elif dtype is numpy.uint16:
            return _get_2d_u16_img(array, pixel_format)
        elif dtype is numpy.float32:
            return _get_2d_f32_img(array, pixel_format)
    else:
        if dtype is numpy.uint8:
            return _get_3d_u8_img(array, pixel_format)
        elif dtype is numpy.uint16:
            return _get_3d_u16_img(array, pixel_format)
        elif dtype is numpy.float32:
            return _get_3d_f32_img(array, pixel_format)


cdef class Image:
    """Image(array, pixel_format)
        image: A 2D or 3D numpy array containing image data
        pixel_format: A PixelFormat describing the image's pixel format
    """
    cdef _image.Image* _this
    cdef PixelFormat pixel_format
    cdef object pixel_array

    def __cinit__(self, array, PixelFormat pixel_format):
        expected_dtype = _get_format_dtype(pixel_format)
        expected_dim = _get_format_last_dim(pixel_format)

        # Do several sanity checks...
        if not isinstance(array, numpy.ndarray):
            raise TypeError("An image must be provided in a numpy array")

        if array.dtype.type is not expected_dtype:
            msg = "The '{}' format requires an array of type {}, but {} was passed"
            pix_fmt_name = PixelFormat(pixel_format).name
            ex_dtype_name = numpy.dtype(expected_dtype).name
            dtype_name = array.dtype.name
            raise TypeError(msg.format(pix_fmt_name, ex_dtype_name, dtype_name))

        if array.ndim not in (2, 3):
            raise ValueError("An image array must be 2 or 3 dimensions")

        last_dim = array.shape[-1] if array.ndim == 3 else 1
        if last_dim != expected_dim:
            msg = "The '{}' format requires an array of dimension MxNx{}"
            pix_fmt_name = PixelFormat(pixel_format).name
            raise ValueError(msg.format(pix_fmt_name, expected_dim))

        self._this = _get_image(array, pixel_format)
        self.pixel_format = pixel_format
        self.pixel_array = array

    def __dealloc__(self):
        del self._this

    property format:
        def __get__(self):
            return self.pixel_format

    property pixels:
        def __get__(self):
            if self.pixel_array.base is not None:
                self.pixel_array.base
            return self.pixel_array
