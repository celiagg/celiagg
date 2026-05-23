project(
    'celiagg',
    'c', 'cpp', 'cython',
    default_options: [
        'cpp_std=c++11',
        'warning_level=1',
        'b_ndebug=if-release',
    ],
)

py = import('python').find_installation(pure: false)
py_dep = py.dependency()
np_dep = dependency('numpy')

cpp = meson.get_compiler('cpp')

extra_cpp_args = [
    '-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION',
]
if cpp.get_id() != 'msvc'
    extra_cpp_args += [
        '-Wfatal-errors',
        '-Wno-unused-function',
    ]
endif

# AGG library: vendored sources compiled into the extension
agg_root = 'agg-svn' / 'agg-2.4'
agg_inc = include_directories(
    agg_root / 'include',
    agg_root,
)

agg_sources = files(
    agg_root / 'src' / 'agg_arc.cpp',
    agg_root / 'src' / 'agg_arrowhead.cpp',
    agg_root / 'src' / 'agg_bezier_arc.cpp',
    agg_root / 'src' / 'agg_bspline.cpp',
    agg_root / 'src' / 'agg_color_rgba.cpp',
    agg_root / 'src' / 'agg_curves.cpp',
    agg_root / 'src' / 'agg_embedded_raster_fonts.cpp',
    agg_root / 'src' / 'agg_gsv_text.cpp',
    agg_root / 'src' / 'agg_image_filters.cpp',
    agg_root / 'src' / 'agg_line_aa_basics.cpp',
    agg_root / 'src' / 'agg_line_profile_aa.cpp',
    agg_root / 'src' / 'agg_rounded_rect.cpp',
    agg_root / 'src' / 'agg_sqrt_tables.cpp',
    agg_root / 'src' / 'agg_trans_affine.cpp',
    agg_root / 'src' / 'agg_trans_double_path.cpp',
    agg_root / 'src' / 'agg_trans_single_path.cpp',
    agg_root / 'src' / 'agg_trans_warp_magnifier.cpp',
    agg_root / 'src' / 'agg_vcgen_bspline.cpp',
    agg_root / 'src' / 'agg_vcgen_contour.cpp',
    agg_root / 'src' / 'agg_vcgen_dash.cpp',
    agg_root / 'src' / 'agg_vcgen_markers_term.cpp',
    agg_root / 'src' / 'agg_vcgen_smooth_poly1.cpp',
    agg_root / 'src' / 'agg_vcgen_stroke.cpp',
    agg_root / 'src' / 'agg_vpgen_clip_polygon.cpp',
    agg_root / 'src' / 'agg_vpgen_clip_polyline.cpp',
    agg_root / 'src' / 'agg_vpgen_segmentator.cpp',
)

# Optional text rendering. On POSIX this requires FreeType2 and optionally
# enables HarfBuzz; on Windows it uses GDI directly.
text_rendering = get_option('text_rendering')
font_sources = []
font_inc = []
font_deps = []
text_defines = []

if text_rendering
    message('Text rendering enabled.')
    text_defines += '-D_ENABLE_TEXT_RENDERING'

    if host_machine.system() == 'windows'
        # Use find_library so meson emits the right linker syntax for the
        # active toolchain (Gdi32.lib for MSVC, -lgdi32 for MinGW).
        font_deps += [
            cpp.find_library('gdi32'),
            cpp.find_library('user32'),
        ]
        font_inc += include_directories(agg_root / 'font_win32_tt')
        font_sources += files(
            agg_root / 'font_win32_tt' / 'agg_font_win32_tt.cpp',
        )
        # XXX: HarfBuzz on Windows is not yet wired up.
    else
        freetype_dep = dependency('freetype2', required: true)
        font_deps += freetype_dep

        harfbuzz_dep = dependency('harfbuzz', required: false)
        if harfbuzz_dep.found()
            font_deps += harfbuzz_dep
            text_defines += '-D_USE_HARFBUZZ'
        endif

        text_defines += '-D_USE_FREETYPE'
        font_inc += include_directories(agg_root / 'font_freetype')
        font_sources += files(
            agg_root / 'font_freetype' / 'agg_font_freetype.cpp',
        )
    endif
else
    message('Text rendering disabled.')
endif

subdir('celiagg')
