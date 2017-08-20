# QmlTreeView

This repository contains the sources for a QML TreeView Item that works with QtQuickControls2.

## Why this project?

QtQuickControls2 does not provide a TreeView item yet because it focus on mobile application development where 
tree view are not that common. But QtQuickControls2 can also be used to develop nice desktop applications and a tree view
component is often needed. I needed a TreeView for one of my personal (but still private) project so I decided to 
create a one myself.

## Solution

The solution I found is to use a ListView Item as the base qml item of the tree view and use a proxy model that 
flatten a source tree model. 

## Source code organisation

* The core of the library can be found in the ``lib`` folder. This is a header only library that contains the ``TreeViewModel`` 
and the ``TreeItemViewModel``

* The ``imports`` directory contains an example qml implementation of the TreeView and the TreeItemDelegate items. 
The delegate is extensible: you can specify the component to use to to render both the arrow and the display item. 
It handles indentation and expanding/collapsing automatically for you.

* There is also a small ``tests`` suite (using the [catch testing framework]() ) that you can run using ``ctest``.

* The ``example`` create two tree model: a StandardItemModel and QFileSystemModel. It then wraps those two models in a 
FlattenedProxyModel and exposes them via a context property.

## How can I use it ?

License here is "super-open", not even copy-left or copy-right, just use it as you want, as it's the most practical to you :

- if you want to use it as GIT submodule and compile it in your app, do it!
- if you prefer separate project as a shared library, do it!
- if you need to modify the project to be able to integrate in you app (opensource or not), do it!
- if you want to share your work on the library, thanks a lot, but if you don't, no problem!

