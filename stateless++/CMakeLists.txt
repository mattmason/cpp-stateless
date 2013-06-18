# Install stateless++ header files.

file(GLOB files *.hpp)
install(FILES ${files} DESTINATION "${PROJECT_BINARY_DIR}/stateless++")
