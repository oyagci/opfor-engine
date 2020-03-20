test_srcs = [
  'tests.cpp',
  'entity.cpp',
]

# Dependencies
test_deps = []
test_deps += dependency('gtest', required : true) # Google Test Suite

testexe = executable(
  'ecs-test',
  test_srcs,
  dependencies : test_deps,
  include_directories: incdirs,
)

test('ecstest', testexe)
