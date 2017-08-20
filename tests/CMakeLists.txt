project(QtQuickControls2.TreeView.Tests)

enable_testing()
find_package(Qt5 5.9 REQUIRED Core Gui Qml Widgets)

add_executable(${PROJECT_NAME} catch.hpp main.cpp TreeViewModelTests.cpp)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Qml Qt5::Widgets)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
