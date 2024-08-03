# Install script for directory: /mnt/d/work_space/v2_gb/game-base/3rd/openssl

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/ssl" TYPE FILE FILES
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/apps/openssl.cnf"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/apps/ct_log_list.cnf"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  FILE(MAKE_DIRECTORY $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ssl/certs)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  FILE(MAKE_DIRECTORY $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ssl/misc)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  FILE(MAKE_DIRECTORY $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ssl/private)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/c_rehash")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/openssl" TYPE FILE FILES
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/aes.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/asn1.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/asn1_mac.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/asn1err.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/asn1t.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/async.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/asyncerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/bio.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/bioerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/blowfish.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/bn.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/bnerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/buffer.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/buffererr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/camellia.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/cast.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/cmac.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/cms.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/cmserr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/comp.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/comperr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/conf.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/conf_api.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/conferr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/crypto.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/cryptoerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ct.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/cterr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/des.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/dh.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/dherr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/dsa.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/dsaerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/dtls1.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/e_os2.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ebcdic.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ec.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ecdh.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ecdsa.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ecerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/engine.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/engineerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/err.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/evp.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/evperr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/hmac.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/idea.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/kdf.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/kdferr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/lhash.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/md2.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/md4.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/md5.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/mdc2.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/modes.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/obj_mac.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/objects.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/objectserr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ocsp.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ocsperr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/opensslconf.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/opensslv.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ossl_typ.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/pem.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/pem2.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/pemerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/pkcs12.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/pkcs12err.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/pkcs7.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/pkcs7err.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/rand.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/rand_drbg.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/randerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/rc2.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/rc4.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/rc5.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ripemd.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/rsa.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/rsaerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/safestack.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/seed.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/sha.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/srp.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/srtp.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ssl.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ssl2.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ssl3.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/sslerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/stack.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/store.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/storeerr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/symhacks.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/tls1.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ts.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/tserr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/txt_db.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/ui.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/uierr.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/whrlpool.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/x509.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/x509_vfy.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/x509err.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/x509v3.h"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/include/openssl/x509v3err.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openssl" TYPE FILE FILES
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/FAQ"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/LICENSE"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/README"
    "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/README.ENGINE"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share" TYPE DIRECTORY FILES "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/doc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/openssl.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/crypto/cmake_install.cmake")
  include("/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/ssl/cmake_install.cmake")
  include("/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/apps/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/mnt/d/work_space/v2_gb/game-base/3rd/openssl/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
