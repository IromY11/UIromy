# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\editors\looknfeel\LookNFeelHierarchyDockWidget.ui'
#
# Created: Mon Mar 23 15:36:26 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_LookNFeelHierarchyDockWidget(object):
    def setupUi(self, LookNFeelHierarchyDockWidget):
        LookNFeelHierarchyDockWidget.setObjectName("LookNFeelHierarchyDockWidget")
        LookNFeelHierarchyDockWidget.setEnabled(False)
        LookNFeelHierarchyDockWidget.resize(409, 668)
        LookNFeelHierarchyDockWidget.setMinimumSize(QtCore.QSize(336, 250))
        self.dockWidgetContents = QtGui.QWidget()
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.dockWidgetContents.sizePolicy().hasHeightForWidth())
        self.dockWidgetContents.setSizePolicy(sizePolicy)
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.verticalLayout = QtGui.QVBoxLayout(self.dockWidgetContents)
        self.verticalLayout.setContentsMargins(-1, 0, -1, -1)
        self.verticalLayout.setObjectName("verticalLayout")
        self.headerLayout = QtGui.QHBoxLayout()
        self.headerLayout.setObjectName("headerLayout")
        self.widgetLookNameLabel = QtGui.QLabel(self.dockWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.widgetLookNameLabel.sizePolicy().hasHeightForWidth())
        self.widgetLookNameLabel.setSizePolicy(sizePolicy)
        self.widgetLookNameLabel.setObjectName("widgetLookNameLabel")
        self.headerLayout.addWidget(self.widgetLookNameLabel)
        self.widgetLookName = QtGui.QLabel(self.dockWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.widgetLookName.sizePolicy().hasHeightForWidth())
        self.widgetLookName.setSizePolicy(sizePolicy)
        self.widgetLookName.setText("")
        self.widgetLookName.setObjectName("widgetLookName")
        self.headerLayout.addWidget(self.widgetLookName)
        self.verticalLayout.addLayout(self.headerLayout)
        self.stateSelectionLayout = QtGui.QHBoxLayout()
        self.stateSelectionLayout.setObjectName("stateSelectionLayout")
        self.stateLabel = QtGui.QLabel(self.dockWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.stateLabel.sizePolicy().hasHeightForWidth())
        self.stateLabel.setSizePolicy(sizePolicy)
        self.stateLabel.setObjectName("stateLabel")
        self.stateSelectionLayout.addWidget(self.stateLabel)
        self.displayStateCombobox = QtGui.QComboBox(self.dockWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.displayStateCombobox.sizePolicy().hasHeightForWidth())
        self.displayStateCombobox.setSizePolicy(sizePolicy)
        self.displayStateCombobox.setObjectName("displayStateCombobox")
        self.stateSelectionLayout.addWidget(self.displayStateCombobox)
        self.verticalLayout.addLayout(self.stateSelectionLayout)
        self.treeView = LookNFeelHierarchyTreeView(self.dockWidgetContents)
        self.treeView.setProperty("showDropIndicator", False)
        self.treeView.setAlternatingRowColors(True)
        self.treeView.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.treeView.setRootIsDecorated(False)
        self.treeView.setAnimated(True)
        self.treeView.setWordWrap(True)
        self.treeView.setHeaderHidden(True)
        self.treeView.setObjectName("treeView")
        self.treeView.header().setSortIndicatorShown(False)
        self.treeView.header().setStretchLastSection(True)
        self.verticalLayout.addWidget(self.treeView)
        LookNFeelHierarchyDockWidget.setWidget(self.dockWidgetContents)

        self.retranslateUi(LookNFeelHierarchyDockWidget)
        QtCore.QMetaObject.connectSlotsByName(LookNFeelHierarchyDockWidget)

    def retranslateUi(self, LookNFeelHierarchyDockWidget):
        LookNFeelHierarchyDockWidget.setWindowTitle(QtGui.QApplication.translate("LookNFeelHierarchyDockWidget", "Look n\' Feel Hierarchy", None, QtGui.QApplication.UnicodeUTF8))
        self.widgetLookNameLabel.setText(QtGui.QApplication.translate("LookNFeelHierarchyDockWidget", "WidgetLookFeel:", None, QtGui.QApplication.UnicodeUTF8))
        self.stateLabel.setText(QtGui.QApplication.translate("LookNFeelHierarchyDockWidget", "Display (StateImagery):", None, QtGui.QApplication.UnicodeUTF8))

from ceed.editors.looknfeel.hierarchy_tree_view import LookNFeelHierarchyTreeView
