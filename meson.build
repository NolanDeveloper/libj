project('libj', 'c',
  version: '0.1',
  default_options: ['warning_level=3', 'werror=true', 'c_std=c11'])

include_directories = include_directories('include')

install_headers('include/libj.h')

libis_dep = dependency('is', fallback: ['libis', 'libis_dep'])
libgb_dep = dependency('gb', fallback: ['libgb', 'libgb_dep'])
libsb_dep = dependency('sb', fallback: ['libsb', 'libsb_dep'])
libutf_dep = dependency('utf', fallback: ['libutf', 'libutf_dep'])

dependencies = [
  libis_dep,
  libgb_dep,
  libsb_dep,
  libutf_dep,
]

src = [
  'src/libj.c',
  'src/libj_from_string.c',
  'src/libj_to_string.c',
  'src/libj_utils.c',
]

libj = library(
  'j',
  src,
  install: true,
  dependencies: dependencies,
  include_directories: include_directories
)

pkg = import('pkgconfig')
pkg.generate(libj)

libj_dep = declare_dependency(
  include_directories: include_directories,
  link_with: libj
)

libj_test_unit = executable(
  'libj_test_unit',
  ['test-unit/main.c', 'test-unit/sanity.c'],
  link_with: libj,
  dependencies: [libis_dep],
  include_directories: include_directories
)

test('unit test', libj_test_unit)

libj_test_json_test_suite = executable(
  'libj_test_json_test_suite',
  ['test-json-test-suite/main.c'],
  link_with: libj,
  dependencies: [libis_dep, libsb_dep],
  include_directories: include_directories
)

test(
  'JSONTestSuite test',
  libj_test_json_test_suite,
  args: meson.current_source_dir() / 'test-json-test-suite/JSONTestSuite'
)
