add_subdirectory(QR-Code-scanner)
add_subdirectory(openpgp)
add_subdirectory(zxcvbn-c)

qt5_add_resources(RESOURCES ../qml.qrc)

# Compile source files (.h/.cpp)
file(GLOB SOURCE_FILES
	"*.h"
	"*.cpp"
    "main/*.h"
    "main/*.cpp"
    "libwalletqt/Haproxy.cpp"
    "libwalletqt/lthnvpnc.cpp"
    "libwalletqt/lthnvpncLogReaderThread.cpp"
    "libwalletqt/ThreadVerifyHaproxy.cpp"
    "libwalletqt/SignatureValidation.cpp"
    "libwalletqt/HTTPResponse.cpp"
    "libwalletqt/WalletManager.cpp"
    "libwalletqt/WalletListenerImpl.cpp"
    "libwalletqt/Wallet.cpp"
    "libwalletqt/PassphraseHelper.cpp"
    "libwalletqt/PendingTransaction.cpp"
    "libwalletqt/TransactionHistory.cpp"
    "libwalletqt/TransactionInfo.cpp"
    "libwalletqt/QRCodeImageProvider.cpp"
    "libwalletqt/AddressBook.cpp"
    "libwalletqt/Subaddress.cpp"
    "libwalletqt/SubaddressAccount.cpp"
    "libwalletqt/UnsignedTransaction.cpp"
    "libwalletqt/lthnvpnc.h"
    "libwalletqt/HTTPResponse.h"
    "libwalletqt/ThreadVerifyHaproxy.h"
    "libwalletqt/SignatureValidation.h"
    "ed25519-donna/ed25519.c"
    "ed25519-donna/ed25519.h"
    "libwalletqt/lthnvpncLogReaderThread.h"
    "libwalletqt/Haproxy.h"
    "libwalletqt/WalletManager.h"
    "libwalletqt/Wallet.h"
    "libwalletqt/PassphraseHelper.h"
    "libwalletqt/PendingTransaction.h"
    "libwalletqt/TransactionHistory.h"
    "libwalletqt/TransactionInfo.h"
    "libwalletqt/QRCodeImageProvider.h"
    "libwalletqt/Transfer.h"
    "libwalletqt/AddressBook.h"
    "libwalletqt/Subaddress.h"
    "libwalletqt/SubaddressAccount.h"
    "libwalletqt/UnsignedTransaction.h"
    "daemon/*.h"
    "daemon/*.cpp"
    "p2pool/*.h"
    "p2pool/*.cpp"
    "model/*.h"
    "model/*.cpp"
    "qt/*.h"
    "qt/*.cpp"
)
if(APPLE)
    list(APPEND SOURCE_FILES "qt/macoshelper.mm")
endif()

set(EXECUTABLE_FLAG)
if(MINGW)
    set(EXECUTABLE_FLAG WIN32)

    set(ICON ${PROJECT_SOURCE_DIR}/images/appicon.ico)
    set(ICON_RC ${CMAKE_CURRENT_BINARY_DIR}/icon.rc)
    set(ICON_RES ${CMAKE_CURRENT_BINARY_DIR}/icon.o)
    file(WRITE ${ICON_RC} "IDI_ICON1 ICON DISCARDABLE \"${ICON}\"")
    find_program(Qt5_WINDRES_EXECUTABLE NAMES windres x86_64-w64-mingw32-windres REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
    add_custom_command(OUTPUT ${ICON_RES} COMMAND ${Qt5_WINDRES_EXECUTABLE} ${ICON_RC} ${ICON_RES} MAIN_DEPENDENCY ${ICON_RC})
    list(APPEND RESOURCES ${ICON_RES})
endif()

if(APPLE)
    set(ICON ${PROJECT_SOURCE_DIR}/images/appicon.icns)
    set_source_files_properties(${ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    list(APPEND RESOURCES ${ICON})
endif()

set(monero_wallet_gui_sources
    ${SOURCE_FILES}
    ${RESOURCES}
)

if(NOT ANDROID)
    add_executable(lethean-wallet-gui ${EXECUTABLE_FLAG} ${monero_wallet_gui_sources})
else()
    add_library(lethean-wallet-gui SHARED ${monero_wallet_gui_sources})
    set_target_properties(lethean-wallet-gui PROPERTIES COMPILE_DEFINITIONS "ANDROID")
endif()

set_target_properties(lethean-wallet-gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/share/Info.plist"
)

# OpenGL
target_include_directories(lethean-wallet-gui PUBLIC ${OPENGL_INCLUDE_DIR})
message(STATUS "OpenGL: include dir at ${OPENGL_INCLUDE_DIR}")
message(STATUS "OpenGL: libraries at ${OPENGL_LIBRARIES}")

target_include_directories(lethean-wallet-gui PUBLIC ${Qt5Gui_PRIVATE_INCLUDE_DIRS})

target_include_directories(lethean-wallet-gui PUBLIC
	${CMAKE_SOURCE_DIR}/lethean/include
    ${CMAKE_SOURCE_DIR}/lethean/src
    ${CMAKE_SOURCE_DIR}/lethean/external/easylogging++
    ${CMAKE_SOURCE_DIR}/lethean/contrib/epee/include
    ${CMAKE_SOURCE_DIR}/lethean/external/qrcodegen
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/daemon
    ${CMAKE_CURRENT_SOURCE_DIR}/p2pool
	${CMAKE_CURRENT_SOURCE_DIR}/libwalletqt
	${CMAKE_CURRENT_SOURCE_DIR}/model
	${CMAKE_CURRENT_SOURCE_DIR}/QR-Code-scanner
	${CMAKE_CURRENT_SOURCE_DIR}/zxcvbn-c
    ${X11_INCLUDE_DIR}
)

target_compile_definitions(lethean-wallet-gui
	PUBLIC
	${Qt5Widgets_DEFINITIONS}
    ${Qt5Qml_DEFINITIONS}
)

if(APPLE)
    if(NOT ICU_ROOT)
        execute_process(COMMAND brew --prefix icu4c OUTPUT_VARIABLE ICU_ROOT OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
    find_package(ICU REQUIRED COMPONENTS data i18n uc)
    target_link_directories(lethean-wallet-gui PRIVATE ${ICU_ROOT}/lib)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

target_link_libraries(lethean-wallet-gui
    epee
    common
    wallet
    qrcodegen
    easylogging
    ${QT5_LIBRARIES}
    ${EXTRA_LIBRARIES}
    openpgp
    qrdecoder
    translations
    zxcvbn
)

if(X11_FOUND)
    target_link_libraries(lethean-wallet-gui ${X11_LIBRARIES})
endif()

if(WITH_SCANNER)
    target_link_libraries(lethean-wallet-gui qrscanner)
    if(LINUX AND NOT ANDROID)
        target_link_libraries(lethean-wallet-gui
            jpeg
            v4l2
            v4lconvert
            rt
        )
    endif()
endif()

add_custom_command(TARGET lethean-wallet-gui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:daemon> $<TARGET_FILE_DIR:lethean-wallet-gui>)

include(Deploy)

install(TARGETS lethean-wallet-gui
    DESTINATION bin
)
