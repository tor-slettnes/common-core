## -*- cmake -*-
#===============================================================================
## @file CMakeLists.txt
## @brief CMake rules to build Common Core static/shared library
## @author Tor Slettnes
#===============================================================================

target_sources(${TARGET} PRIVATE
  init.c++
  host.c++
  logsink.c++
  path.c++
  process.c++
  runtime.c++
  serialport.c++
  symbols.c++
  systemservice.c++
  timezone.c++
  user.c++
  dns-sd.c++
)

if(BUILD_CRYPTO)
  find_package("OpenSSL" REQUIRED)
  target_link_libraries(${TARGET} PUBLIC
    OpenSSL::SSL
    OpenSSL::Crypto
  )

  target_compile_definitions(${TARGET} PRIVATE
    SALT_1="aa3c8c30fe854b03aa77f6e336ac44fd"
    SALT_2="9264531dc07449a483af01f1ffea1bd7"
    SALT_3="d5f156dd64ad4b10aa3748b08650304b"
    SALT_4="46d4230bceca42898cfa28d2dfacf618"
    SALT_5="8f38e6231dc94b2e8b4a1719bbe5b677"
    SALT_6="ce5fa090fb7b48d9b8164e6477d0ca8e"
    SALT_7="1fe9dc4aeff04298a5b570121cf7c07d"
    SALT_8="bf328c7590a649fd8864833ea5f57c83"
    SALT_9="c3dd03738533404aa233c00bd4e21140"
    SALT_10="0bf250625a6141a6b6b9e2c9ee5c8150"
    SALT_11="02b2d7c888824c20a8f09ad6eba33996"
    SALT_12="27cd5e77814945fea143c80975a89963"
    SALT_13="0c6acda0258c425b95398b52f568584f"
    SALT_14="00fbeb8f7e524c34bbe71716ce1f6c14"
    SALT_15="51602d02fe96473d9c419d843f255a9d"
    SALT_16="c4dba577c6af418e9cb779f0a1e83b27"
  )

  target_sources(${TARGET} PRIVATE
    aes.c++
  )
endif()
