@cython.boundscheck(False)
@cython.wraparound(False)
cpdef hist_min_max_uint16(numpy.uint16_t[:, :] arr, numpy.uint32_t[:] hist, numpy.uint16_t[:] min_max):
    cdef numpy.uint16_t v
    assert hist.shape[0] == 1024
    cdef Py_ssize_t i, j
    with nogil:
        hist[:] = 0
        min_max[0] = arr[0,0]
        min_max[1] = arr[0,0]
        for i in range(arr.shape[0]):
            for j in range(arr.shape[1]):
                v = arr[i, j]
                hist[v >> 6] += 1
                if v < min_max[0]:
                    min_max[0] = v
                elif v > min_max[1]:
                    min_max[1] = v


@cython.boundscheck(False)
@cython.wraparound(False)
cpdef hist_min_max_uint12(numpy.uint16_t[:, :] arr, numpy.uint32_t[:] hist, numpy.uint16_t[:] min_max):
    cdef numpy.uint16_t v
    assert hist.shape[0] == 1024
    cdef Py_ssize_t i, j
    with nogil:
        hist[:] = 0
        min_max[0] = arr[0,0]
        min_max[1] = arr[0,0]
        for i in range(arr.shape[0]):
            for j in range(arr.shape[1]):
                v = arr[i, j]
                hist[v >> 2] += 1
                if v < min_max[0]:
                    min_max[0] = v
                elif v > min_max[1]:
                    min_max[1] = v

@cython.boundscheck(False)
@cython.wraparound(False)
cpdef hist_min_max_uint8(numpy.uint8_t[:, :] arr, numpy.uint32_t[:] hist, numpy.uint8_t[:] min_max):
    cdef numpy.uint8_t v
    assert hist.shape[0] == 256
    cdef Py_ssize_t i, j
    with nogil:
        hist[:] = 0
        min_max[0] = arr[0,0]
        min_max[1] = arr[0,0]
        for i in range(arr.shape[0]):
            for j in range(arr.shape[1]):
                v = arr[i, j]
                hist[v] += 1
                if v < min_max[0]:
                    min_max[0] = v
                elif v > min_max[1]:
                    min_max[1] = v

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.cdivision(True)
cpdef hist_min_max_float32(numpy.float32_t[:, :] arr, numpy.uint32_t[:] hist, numpy.float32_t[:] min_max, numpy.float32_t hist_min, numpy.float32_t hist_max):
    cdef numpy.float32_t v
    cdef numpy.uint32_t n_bins = hist.shape[0]
    cdef Py_ssize_t i, j, bin
    cdef numpy.float32_t bin_factor = (n_bins - 1) / (hist_max - hist_min)
    with nogil:
        hist[:] = 0
        min_max[0] = arr[0,0]
        min_max[1] = arr[0,0]
        for i in range(arr.shape[0]):
            for j in range(arr.shape[1]):
                v = arr[i, j]
                bin = <Py_ssize_t> (bin_factor * (v - hist_min))
                hist[bin] += 1
                if v < min_max[0]:
                    min_max[0] = v
                elif v > min_max[1]:
                    min_max[1] = v
