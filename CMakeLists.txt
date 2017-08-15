cmake_minimum_required(VERSION 3.8)
project(TreeView)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 5.9 REQUIRED Core Gui Qml Widgets)

add_executable(TreeView main.cpp qml.qrc FlattenedProxyModel.h FlattenedProxyModel.cpp main.qml TreeView.qml TreeItemDelegate.qml TreeNode.cpp TreeNode.h qtquickcontrols2.conf)
target_link_libraries(TreeView Qt5::Core Qt5::Gui Qt5::Qml Qt5::Widgets)
