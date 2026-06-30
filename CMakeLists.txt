cmake_minimum_required(VERSION 3.16)
project(CBTExam VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    Gui
)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/pages/onboardpage.cpp
    src/pages/biopage.cpp
    src/pages/exampage.cpp
    src/pages/resultpage.cpp
    src/widgets/timerwidget.cpp
    src/widgets/questionwidget.cpp
    src/widgets/notificationpopup.cpp
    src/widgets/supervisormodal.cpp
    src/network/apiclient.cpp
    src/lockdown/lockdownmanager.cpp
    src/models/examsession.cpp
)

set(HEADERS
    src/mainwindow.h
    src/pages/onboardpage.h
    src/pages/biopage.h
    src/pages/exampage.h
    src/pages/resultpage.h
    src/widgets/timerwidget.h
    src/widgets/questionwidget.h
    src/widgets/notificationpopup.h
    src/widgets/supervisormodal.h
    src/network/apiclient.h
    src/lockdown/lockdownmanager.h
    src/models/examsession.h
    src/models/question.h
)

add_executable(CBTExam ${SOURCES} ${HEADERS})

target_link_libraries(CBTExam PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Gui
)

target_include_directories(CBTExam PRIVATE src)

if(WIN32)
    target_compile_definitions(CBTExam PRIVATE PLATFORM_WINDOWS)
    target_link_libraries(CBTExam PRIVATE user32 gdi32)
    set_target_properties(CBTExam PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(CBTExam PRIVATE PLATFORM_LINUX)
    find_package(X11)
    if(X11_FOUND)
        target_link_libraries(CBTExam PRIVATE ${X11_LIBRARIES})
        target_include_directories(CBTExam PRIVATE ${X11_INCLUDE_DIR})
        target_compile_definitions(CBTExam PRIVATE HAVE_X11)
    endif()
endif()
