# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\editors\layout\HierarchyDockWidget.ui'
#
# Created: Wed Mar 25 17:43:03 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_HierarchyDockWidget(object):
    def setupUi(self, HierarchyDockWidget):
        HierarchyDockWidget.setObjectName("HierarchyDockWidget")
        HierarchyDockWidget.setEnabled(False)
        HierarchyDockWidget.resize(266, 412)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(HierarchyDockWidget.sizePolicy().hasHeightForWidth())
        HierarchyDockWidget.setSizePolicy(sizePolicy)
        HierarchyDockWidget.setMinimumSize(QtCore.QSize(180, 250))
        self.dockWidgetContents = QtGui.QWidget()
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.verticalLayout = QtGui.QVBoxLayout(self.dockWidgetContents)
        self.verticalLayout.setContentsMargins(-1, 0, -1, -1)
        self.verticalLayout.setObjectName("verticalLayout")
        self.treeView = WidgetHierarchyTreeView(self.dockWidgetContents)
        self.treeView.setStyleSheet("QTreeView::indicator:checked\n"
"{\n"
"    image: url(\"icons/layout_editing/manipulator_locked.png\");\n"
"}\n"
"\n"
"QTreeView::indicator:unchecked\n"
"{\n"
"    image: url(\"icons/layout_editing/manipulator_unlocked.png\");\n"
"}")
        self.treeView.setDragEnabled(True)
        self.treeView.setDragDropMode(QtGui.QAbstractItemView.DragDrop)
        self.treeView.setDefaultDropAction(QtCore.Qt.MoveAction)
        self.treeView.setAlternatingRowColors(True)
        self.treeView.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection)
        self.treeView.setSortingEnabled(True)
        self.treeView.setAnimated(True)
        self.treeView.setHeaderHidden(True)
        self.treeView.setObjectName("treeView")
        self.treeView.header().setSortIndicatorShown(False)
        self.treeView.header().setStretchLastSection(True)
        self.verticalLayout.addWidget(self.treeView)
        HierarchyDockWidget.setWidget(self.dockWidgetContents)

        self.retranslateUi(HierarchyDockWidget)
        QtCore.QMetaObject.connectSlotsByName(HierarchyDockWidget)

    def retranslateUi(self, HierarchyDockWidget):
        HierarchyDockWidget.setWindowTitle(QtGui.QApplication.translate("HierarchyDockWidget", "Widget Hierarchy", None, QtGui.QApplication.UnicodeUTF8))

from ceed.editors.layout.visual import WidgetHierarchyTreeView
