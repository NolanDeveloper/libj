project('libj', 'c',
  version: '0.1',
  default_options: ['warning_level=3', 'werror=true', 'c_std=c11'])

include_directories = include_directories('include')

install_headers('include/libj.h')

libis_dep = dependency('is', required: false)
if not libis_dep.found()
  libis_proj = subproject('libis')
  libis_dep = libis_proj.get_variable('libis_dep')
endif

libgb_dep = dependency('gb', required: false)
if not libgb_dep.found()
  libgb_proj = subproject('libgb')
  libgb_dep = libgb_proj.get_variable('libgb_dep')
endif

libsb_dep = dependency('sb', required: false)
if not libsb_dep.found()
  libsb_proj = subproject('libsb')
  libsb_dep = libsb_proj.get_variable('libsb_dep')
endif

libutf_dep = dependency('utf', required: false)
if not libutf_dep.found()
  libutf_proj = subproject('libutf')
  libutf_dep = libutf_proj.get_variable('libutf_dep')
endif

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
