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

cimport numpy as cnp

ctypedef _image.Image* img_ptr_t


cdef _get_format_size(PixelFormat pixel_format):
    sizes = {
        1: (PixelFormat.Gray8, PixelFormat.BGR24, PixelFormat.RGB24,
            PixelFormat.BGRA32, PixelFormat.RGBA32, PixelFormat.ARGB32,
            PixelFormat.ABGR32),
        2: (PixelFormat.Gray16, PixelFormat.BGR48, PixelFormat.RGB48,
            PixelFormat.BGRA64, PixelFormat.RGBA64, PixelFormat.ARGB64,
            PixelFormat.ABGR64),
        4: (PixelFormat.Gray32, PixelFormat.BGR96, PixelFormat.RGB96,
            PixelFormat.BGRA128, PixelFormat.RGBA128, PixelFormat.ARGB128,
            PixelFormat.ABGR128),
    }
    format_sizes = {fmt: size for size, fmts in sizes.items() for fmt in fmts}
    return format_sizes[pixel_format]


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


cdef img_ptr_t _get_2d_u8_img(cnp.npy_uint8[:, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_2d_u16_img(cnp.npy_uint16[:, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_2d_f32_img(cnp.npy_float32[:, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_3d_u8_img(cnp.npy_uint8[:, :, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_3d_u16_img(cnp.npy_uint16[:, :, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_3d_f32_img(cnp.npy_float32[:, :, ::1] arr, bool bottom_up):
    return new _image.Image(<unsigned char*>&arr[0][0][0], arr.shape[1], arr.shape[0],
                            -arr.strides[0] if bottom_up else arr.strides[0])


cdef img_ptr_t _get_image(array, pixel_format, bottom_up):
    bytes_per_channel = _get_format_size(pixel_format)

    # Finally build the image
    if array.ndim == 2:
        if bytes_per_channel == 1:
            return _get_2d_u8_img(array, bottom_up)
        elif bytes_per_channel == 2:
            return _get_2d_u16_img(array, bottom_up)
        elif bytes_per_channel == 4:
            return _get_2d_f32_img(array, bottom_up)
    else:
        if bytes_per_channel == 1:
            return _get_3d_u8_img(array, bottom_up)
        elif bytes_per_channel == 2:
            return _get_3d_u16_img(array, bottom_up)
        elif bytes_per_channel == 4:
            return _get_3d_f32_img(array, bottom_up)


cdef class Image:
    """Image(pixel_buf, pixel_format, bottom_up=False)

    :param pixel_buf: A 2D or 3D array containing image data
    :param pixel_format: A PixelFormat describing the image's pixel format
    :param bottom_up: If True, the image data starts at the bottom of the image
    """
    cdef img_ptr_t _this
    cdef PixelFormat pixel_format
    cdef object pixel_buffer
    cdef bool bottom_up

    def __cinit__(self, pixel_buf, PixelFormat pixel_format, bool bottom_up=False):
        expected_size = _get_format_size(pixel_format)
        expected_dim = _get_format_last_dim(pixel_format)

        # Do several sanity checks...
        if pixel_buf.itemsize is not expected_size:
            msg = "The '{}' format requires an array of size {}, but size {} was passed"
            pix_fmt_name = PixelFormat(pixel_format).name
            raise TypeError(msg.format(pix_fmt_name, expected_size, pixel_buf.nbytes))

        if pixel_buf.ndim not in (2, 3):
            raise ValueError("An image array must be 2 or 3 dimensions")

        last_dim = pixel_buf.shape[-1] if pixel_buf.ndim == 3 else 1
        if last_dim != expected_dim:
            msg = "The '{}' format requires an array of dimension MxNx{}"
            pix_fmt_name = PixelFormat(pixel_format).name
            raise ValueError(msg.format(pix_fmt_name, expected_dim))

        self._this = _get_image(pixel_buf, pixel_format, bottom_up)
        self.pixel_format = pixel_format
        self.pixel_buffer = pixel_buf
        self.bottom_up = bottom_up

    def __dealloc__(self):
        del self._this

    def copy(self):
        """Returns a deep copy of the image.
        """
        pixel_buf = self.pixel_buffer.copy()
        return Image(pixel_buf, self.pixel_format, bottom_up=self.bottom_up)

    property format:
        def __get__(self):
            return self.pixel_format

    property pixels:
        def __get__(self):
            if self.pixel_buffer.base is not None:
                self.pixel_buffer.base
            return self.pixel_buffer

    property height:
        def __get__(self):
            return self._this.height()

    property width:
        def __get__(self):
            return self._this.width()
