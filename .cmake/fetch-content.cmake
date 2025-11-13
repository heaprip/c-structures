include(FetchContent)
FetchContent_Declare(
    cmocka
    URL      https://cmocka.org/files/1.1/cmocka-1.1.8.tar.xz
    URL_HASH MD5=976fe5a166f9729c350119d997256d2f
)
set(WITH_STATIC_LIB OFF CACHE BOOL "CMocka: Build with a static library" FORCE)
set(WITH_CMOCKERY_SUPPORT OFF CACHE BOOL "CMocka: Install a cmockery header" FORCE)
set(WITH_EXAMPLES OFF CACHE BOOL "CMocka: Build examples" FORCE)
set(UNIT_TESTING OFF CACHE BOOL "CMocka: Build with unit testing" FORCE)
set(PICKY_DEVELOPER OFF CACHE BOOL "CMocka: Build with picky developer flags" FORCE)
FetchContent_MakeAvailable(cmocka)
