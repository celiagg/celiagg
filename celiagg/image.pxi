# The MIT License (MIT)
#
# Copyright (c) 2016 WUSTL ZPLAB
# Copyright (c) 2016-2021 Celiagg Contributors
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

cimport numpy as np

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


cdef img_ptr_t _get_2d_u8_img(np.npy_uint8[:, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_2d_u16_img(np.npy_uint16[:, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_2d_f32_img(float[:, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_3d_u8_img(np.npy_uint8[:, :, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_3d_u16_img(np.npy_uint16[:, :, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_3d_f32_img(float[:, :, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_image(array, pixel_format, bottom_up):
    dtype = _get_format_dtype(pixel_format)

    # Finally build the image
    if array.ndim == 2:
        if dtype is numpy.uint8:
            return _get_2d_u8_img(array, bottom_up)
        elif dtype is numpy.uint16:
            return _get_2d_u16_img(array, bottom_up)
        elif dtype is numpy.float32:
            return _get_2d_f32_img(array, bottom_up)
    else:
        if dtype is numpy.uint8:
            return _get_3d_u8_img(array, bottom_up)
        elif dtype is numpy.uint16:
            return _get_3d_u16_img(array, bottom_up)
        elif dtype is numpy.float32:
            return _get_3d_f32_img(array, bottom_up)


cdef class Image:
    """Image(array, pixel_format, bottom_up=False)

    :param image: A 2D or 3D numpy array containing image data
    :param pixel_format: A PixelFormat describing the image's pixel format
    :param bottom_up: If True, the image data starts at the bottom of the image
    """
    cdef img_ptr_t _this
    cdef PixelFormat pixel_format
    cdef object pixel_array
    cdef bool bottom_up

    def __cinit__(self, array, PixelFormat pixel_format, bool bottom_up=False):
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

        self._this = _get_image(array, pixel_format, bottom_up)
        self.pixel_format = pixel_format
        self.pixel_array = array
        self.bottom_up = bottom_up

    def __dealloc__(self):
        del self._this

    def copy(self):
        """Returns a deep copy of the image.
        """
        array = self.pixel_array.copy()
        return Image(array, self.pixel_format, bottom_up=self.bottom_up)

    property format:
        def __get__(self):
            return self.pixel_format

    property pixels:
        def __get__(self):
            if self.pixel_array.base is not None:
                self.pixel_array.base
            return self.pixel_array

    property height:
        def __get__(self):
            return self._this.height()

    property width:
        def __get__(self):
            return self._this.width()
