# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\editors\animation_list\AnimationListDockWidget.ui'
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

class Ui_AnimationListDockWidget(object):
    def setupUi(self, AnimationListDockWidget):
        AnimationListDockWidget.setObjectName("AnimationListDockWidget")
        AnimationListDockWidget.setEnabled(True)
        AnimationListDockWidget.resize(290, 300)
        self.dockWidgetContents = QtGui.QWidget()
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.verticalLayout = QtGui.QVBoxLayout(self.dockWidgetContents)
        self.verticalLayout.setContentsMargins(-1, 0, -1, -1)
        self.verticalLayout.setObjectName("verticalLayout")
        self.list = QtGui.QListWidget(self.dockWidgetContents)
        self.list.setObjectName("list")
        self.verticalLayout.addWidget(self.list)
        AnimationListDockWidget.setWidget(self.dockWidgetContents)

        self.retranslateUi(AnimationListDockWidget)
        QtCore.QMetaObject.connectSlotsByName(AnimationListDockWidget)

    def retranslateUi(self, AnimationListDockWidget):
        AnimationListDockWidget.setWindowTitle(QtGui.QApplication.translate("AnimationListDockWidget", "List of animations", None, QtGui.QApplication.UnicodeUTF8))

