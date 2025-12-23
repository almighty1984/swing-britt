add_library(core)
target_compile_features(core PRIVATE cxx_std_23)

file(GLOB_RECURSE CORE_IXX_FILES "*.ixx")

target_sources(core PUBLIC FILE_SET CXX_MODULES FILES ${CORE_IXX_FILES})
target_link_libraries(core PRIVATE SFML::Audio SFML::Graphics SFML::Window)