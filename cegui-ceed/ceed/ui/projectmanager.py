# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\ProjectManager.ui'
#
<<<<<<< HEAD
<<<<<<< HEAD
# Created: Wed Apr 29 10:56:49 2015
=======
# Created: Mon May 18 01:37:27 2015
>>>>>>> 8d71f48158ce6577737b930a8dc24036e2e85210
=======
# Created: Sat May 23 15:53:32 2015
>>>>>>> 5c99a2e5037babbf80457f6614ef5105ed6e4b86
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

