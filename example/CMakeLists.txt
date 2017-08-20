project(QtQuickControls2.TreeView.Example)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 5.9 REQUIRED Core Gui Qml Widgets)

add_executable(${PROJECT_NAME} main.cpp main.qml qml.qrc qtquickcontrols2.conf
        # the below files are not necessary, they are here only so that they appear in QtCreator/CLion
        ../lib/TreeViewModel.h ../lib/TreeItemViewModel.h
        ../imports/TreeView.qml ../imports/TreeItemView.qml)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Qml Qt5::Widgets)
