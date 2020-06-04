project('lua', 'c', version: '5.3.5',
  default_options: [ ])

compiler = meson.get_compiler('c')

add_global_arguments('-DLUA_USE_LINUX', language: 'c')

incdirs = [ include_directories('src') ]
srcs = [ 
  'src/lfunc.c',
  'src/lopcodes.c',
  'src/lauxlib.c',
  #  'src/luac.c',
  'src/ltm.c',
  'src/lbitlib.c',
  'src/ldo.c',
  'src/lgc.c',
  'src/liolib.c',
  'src/lobject.c',
  'src/lmathlib.c',
  'src/lstate.c',
  'src/ltable.c',
  'src/ldump.c',
  'src/lcorolib.c',
  'src/lua.c',
  'src/llex.c',
  'src/lundump.c',
  'src/lmem.c',
  'src/lbaselib.c',
  'src/ldblib.c',
  'src/lctype.c',
  'src/lutf8lib.c',
  'src/lvm.c',
  'src/lzio.c',
  'src/lstrlib.c',
  'src/lapi.c',
  'src/lstring.c',
  'src/loslib.c',
  'src/lcode.c',
  'src/ltablib.c',
  'src/lparser.c',
  'src/loadlib.c',
  'src/linit.c',
  'src/ldebug.c',
]

deps = [
  compiler.find_library('m', required: true),
  compiler.find_library('dl', required: true),
  dependency('readline', required: true),
]

library('lua', srcs, include_directories: 'src', dependencies: deps)
