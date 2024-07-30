project('msl', ['cpp', 'c'],
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++20'])

meson_source_lister = executable('msl', 'main.cpp', install : true)

test('test', meson_source_lister)