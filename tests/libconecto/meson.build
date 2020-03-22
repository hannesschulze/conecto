libconecto_tests = [
]

foreach test : libconecto_tests
  test(
    test.get(1) + '_test',
    executable(
      test.get(1) + '_exe',
      [ test.get(0), test_files ],
      dependencies: [ libconecto_dep, gtest_dep ]
    )
  )
endforeach