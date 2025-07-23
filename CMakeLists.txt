cmake_minimum_required(VERSION 3.15)  # FetchContent使うなら3.14+くらい
project(NenePancake VERSION 0.1.0 LANGUAGES CXX)

# ヘッダオンリーターゲット
add_library(NenePancake INTERFACE)

# ヘッダ検索パス（ビルド時）
target_include_directories(NenePancake
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>  # インストール後の相対パス
)

# C++標準（利用側に要求）
target_compile_features(NenePancake INTERFACE cxx_std_17)  # 17でOK

# 名前空間エイリアス（推奨）※ 便利にするための IMPORTED エイリアス
add_library(NenePancake::NenePancake ALIAS NenePancake)

# ここから: テスト用の分岐(直接このリポジトリを開いたときのみ有効)
if( (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) )
    option(NENEPANCAKE_BUILD_TESTS "Build NenePancake tests" ON)
else()
    option(NENEPANCAKE_BUILD_TESTS "Build NenePancake tests" OFF)
endif()

if(NENEPANCAKE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
# ここまで

# ===== インストール設定 =====
include(GNUInstallDirs)

install(
    TARGETS NenePancake
    EXPORT NenePancakeTargets
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# パッケージ構成ファイル生成 (Config + Version)
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NenePancakeConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NenePancakeConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NenePancakeConfig.cmake"
    @ONLY
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/NenePancakeConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/NenePancakeConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NenePancake
)

install(
    EXPORT NenePancakeTargets
    NAMESPACE NenePancake::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NenePancake
)