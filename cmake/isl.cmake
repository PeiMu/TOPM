include(ExternalProject)

set(ISL_ROOT        ${CMAKE_BINARY_DIR}/3rd_party/ISL)
set(SPDLOG_GIT_TAG  master)  # 指定版本
set(ISL_GIT_URL     https://repo.or.cz/isl.git)  # 指定git仓库地址
set(ISL_CONFIGURE   cd ${ISL_ROOT}/src/ISL && ./autogen.sh && ./configure --prefix=${ISL_ROOT})  # 指定配置指令（注意此处修改了安装目录，否则默认情况下安装到系统目录）
set(ISL_MAKE        cd ${ISL_ROOT}/src/ISL && make)  # 指定编译指令（需要覆盖默认指令，进入我们指定的ISL_ROOT目录下）
set(ISL_INSTALL     cd ${ISL_ROOT}/src/ISL && make install)  # 指定安装指令（需要覆盖默认指令，进入我们指定的ISL_ROOT目录下）

# 指定编译好的静态库文件的路径
set(ISL_LIB       ${ISL_ROOT}/lib/libisl.a)
# 指定头文件所在的目录
set(ISL_INCLUDE_DIR   ${ISL_ROOT}/include)

if(NOT ISL_LIB)
    ExternalProject_Add(ISL
        PREFIX            ${ISL_ROOT}
        GIT_REPOSITORY    ${ISL_GIT_URL}
        GIT_TAG           ${ISL_GIT_TAG}
        CONFIGURE_COMMAND ${ISL_CONFIGURE}
        BUILD_COMMAND     ${ISL_MAKE}
        INSTALL_COMMAND   ${ISL_INSTALL}
        )
endif()