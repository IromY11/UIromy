# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\BitmapEditor.ui'
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

class Ui_BitmapEditor(object):
    def setupUi(self, BitmapEditor):
        BitmapEditor.setObjectName("BitmapEditor")
        BitmapEditor.resize(489, 384)
        self.horizontalLayout = QtGui.QHBoxLayout(BitmapEditor)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.preview = QtGui.QLabel(BitmapEditor)
        self.preview.setText("")
        self.preview.setObjectName("preview")
        self.horizontalLayout.addWidget(self.preview)
        self.buttons = QtGui.QWidget(BitmapEditor)
        self.buttons.setObjectName("buttons")
        self.verticalLayout = QtGui.QVBoxLayout(self.buttons)
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.launchExternal = QtGui.QPushButton(self.buttons)
        self.launchExternal.setFlat(False)
        self.launchExternal.setObjectName("launchExternal")
        self.verticalLayout.addWidget(self.launchExternal)
        self.horizontalLayout.addWidget(self.buttons)

        self.retranslateUi(BitmapEditor)
        QtCore.QMetaObject.connectSlotsByName(BitmapEditor)

    def retranslateUi(self, BitmapEditor):
        BitmapEditor.setWindowTitle(QtGui.QApplication.translate("BitmapEditor", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.launchExternal.setText(QtGui.QApplication.translate("BitmapEditor", "Launch external editor", None, QtGui.QApplication.UnicodeUTF8))

