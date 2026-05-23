celiagg_inc = include_directories('.')

celiagg_cpp_sources = files(
    'canvas_impl.cpp',
    'font_cache.cpp',
    'font.cpp',
    'image.cpp',
    'paint.cpp',
    'vertex_source.cpp',
)

py.extension_module(
    '_celiagg',
    sources: [
        '_celiagg.pyx',
        celiagg_cpp_sources,
        agg_sources,
        font_sources,
    ],
    include_directories: [
        agg_inc,
        celiagg_inc,
        font_inc,
    ],
    cpp_args: extra_cpp_args + text_defines,
    dependencies: [py_dep, np_dep, font_deps],
    cython_args: ['-I', meson.current_source_dir()],
    override_options: ['cython_language=cpp'],
    install: true,
    subdir: 'celiagg',
)

py.install_sources(
    ['__init__.py'],
    subdir: 'celiagg',
)

py.install_sources(
    [
        'tests/__init__.py',
        'tests/test_canvas.py',
        'tests/test_drawing.py',
        'tests/test_font.py',
        'tests/test_no_text.py',
        'tests/test_paint.py',
        'tests/test_path.py',
        'tests/test_state.py',
        'tests/test_text.py',
        'tests/test_transform.py',
    ],
    subdir: 'celiagg/tests',
)

py.install_sources(
    ['data/__init__.py'],
    subdir: 'celiagg/data',
)

install_data(
    [
        'data/Montserrat-Regular.ttf',
        'data/OFL.txt',
    ],
    install_dir: py.get_install_dir() / 'celiagg' / 'data',
)
