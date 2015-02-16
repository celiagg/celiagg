import numpy
from . import pyagg
import concurrent.futures as futures
from collections import namedtuple

HistogramReturn = namedtuple('HistogramReturn', ('histogram', 'max_bin', 'min_intensity', 'max_intensity'))

pool = futures.ThreadPoolExecutor(max_workers=16)

def histogram(array, twelve_bit=False, n_bins=1024, hist_max=None, hist_min=None, n_threads=8):
    array = numpy.asarray(array)
    extra_args = ()
    if array.dtype == numpy.uint8:
        function = pyagg.hist_min_max_uint8
        n_bins = 256
    elif array.dtype == numpy.uint16:
        n_bins = 1024
        if twelve_bit:
            function = pyagg.hist_min_max_uint12
        else:
            function = pyagg.hist_min_max_uint16
    elif array.dtype == numpy.float32:
        function = pyagg.hist_min_max_float32
        if hist_max is None:
            hist_max = array.max()
        if hist_min is None:
            hist_min = array.min()
        extra_args = (hist_min, hist_max)
    else:
        raise TypeError('array argument type must be uint8, uint16, or float32')
        
    slices = [array[i::n_threads] for i in range(n_threads)]
    histograms = numpy.empty((n_threads, n_bins), dtype=numpy.uint32)
    min_maxs = numpy.empty((n_threads, 2), dtype=array.dtype)
    futures = [pool.submit(function, arr_slice, hist_slice, min_max, *extra_args) for
               arr_slice, hist_slice, min_max in zip(slices, histograms, min_maxs)]
    for future in futures:
        future.result()

    histogram = histograms.sum(axis=0, dtype=numpy.uint32)
    max_bin = histogram.argmax()

    return HistogramReturn(histogram, max_bin, min_maxs[:,0].min(), min_maxs[:,1].max())
