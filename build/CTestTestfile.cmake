# CMake generated Testfile for 
# Source directory: /mnt/d/downtheload/skull/ece309/project 3/P3_starter
# Build directory: /mnt/d/downtheload/skull/ece309/project 3/P3_starter/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(p2_regression "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/build/test_p2")
set_tests_properties(p2_regression PROPERTIES  WORKING_DIRECTORY "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/build" _BACKTRACE_TRIPLES "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/CMakeLists.txt;82;add_test;/mnt/d/downtheload/skull/ece309/project 3/P3_starter/CMakeLists.txt;0;")
add_test(p3 "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/build/test_p3")
set_tests_properties(p3 PROPERTIES  WORKING_DIRECTORY "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/build" _BACKTRACE_TRIPLES "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/CMakeLists.txt;86;add_test;/mnt/d/downtheload/skull/ece309/project 3/P3_starter/CMakeLists.txt;0;")
add_test(tool_loop "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/build/test_loop")
set_tests_properties(tool_loop PROPERTIES  WORKING_DIRECTORY "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/build" _BACKTRACE_TRIPLES "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/CMakeLists.txt;90;add_test;/mnt/d/downtheload/skull/ece309/project 3/P3_starter/CMakeLists.txt;0;")
add_test(e2e "bash" "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/tests/e2e/run_e2e.sh" "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/build/miniharness")
set_tests_properties(e2e PROPERTIES  WORKING_DIRECTORY "/mnt/d/downtheload/skull/ece309/project 3/P3_starter" _BACKTRACE_TRIPLES "/mnt/d/downtheload/skull/ece309/project 3/P3_starter/CMakeLists.txt;99;add_test;/mnt/d/downtheload/skull/ece309/project 3/P3_starter/CMakeLists.txt;0;")
