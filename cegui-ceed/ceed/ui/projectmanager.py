# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\Iromys\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\ProjectManager.ui'
#
<<<<<<< HEAD
# Created: Fri Apr 03 20:18:18 2015
=======
# Created: Fri Apr 03 17:53:35 2015
>>>>>>> c571f43318051bacc0fbf7da05af203cfa97782a
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_ProjectManager(object):
    def setupUi(self, ProjectManager):
        ProjectManager.setObjectName("ProjectManager")
        ProjectManager.resize(274, 348)
        self.contents = QtGui.QWidget()
        self.contents.setObjectName("contents")
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.contents)
        self.verticalLayout_2.setContentsMargins(-1, 0, -1, -1)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.view = QtGui.QTreeView(self.contents)
        self.view.setEditTriggers(QtGui.QAbstractItemView.NoEditTriggers)
        self.view.setDragDropMode(QtGui.QAbstractItemView.InternalMove)
        self.view.setDefaultDropAction(QtCore.Qt.MoveAction)
        self.view.setAlternatingRowColors(True)
        self.view.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection)
        self.view.setSelectionBehavior(QtGui.QAbstractItemView.SelectItems)
        self.view.setIconSize(QtCore.QSize(16, 16))
        self.view.setSortingEnabled(True)
        self.view.setAnimated(True)
        self.view.setObjectName("view")
        self.verticalLayout_2.addWidget(self.view)
        ProjectManager.setWidget(self.contents)

        self.retranslateUi(ProjectManager)
        QtCore.QMetaObject.connectSlotsByName(ProjectManager)

    def retranslateUi(self, ProjectManager):
        ProjectManager.setWindowTitle(QtGui.QApplication.translate("ProjectManager", "Project Manager", None, QtGui.QApplication.UnicodeUTF8))

