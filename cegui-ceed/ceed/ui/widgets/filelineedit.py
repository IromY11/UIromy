# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\Iromys\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\widgets\FileLineEdit.ui'
#
# Created: Sat Mar 21 23:56:15 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_FileLineEdit(object):
    def setupUi(self, FileLineEdit):
        FileLineEdit.setObjectName("FileLineEdit")
        FileLineEdit.resize(400, 41)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(FileLineEdit.sizePolicy().hasHeightForWidth())
        FileLineEdit.setSizePolicy(sizePolicy)
        self.horizontalLayout = QtGui.QHBoxLayout(FileLineEdit)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.lineEdit = QtGui.QLineEdit(FileLineEdit)
        self.lineEdit.setObjectName("lineEdit")
        self.horizontalLayout.addWidget(self.lineEdit)
        self.browseButton = QtGui.QToolButton(FileLineEdit)
        self.browseButton.setObjectName("browseButton")
        self.horizontalLayout.addWidget(self.browseButton)

        self.retranslateUi(FileLineEdit)
        QtCore.QMetaObject.connectSlotsByName(FileLineEdit)

    def retranslateUi(self, FileLineEdit):
        FileLineEdit.setWindowTitle(QtGui.QApplication.translate("FileLineEdit", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.lineEdit.setPlaceholderText(QtGui.QApplication.translate("FileLineEdit", "Click the button to browse filesystem", None, QtGui.QApplication.UnicodeUTF8))
        self.browseButton.setToolTip(QtGui.QApplication.translate("FileLineEdit", "Browse file system", None, QtGui.QApplication.UnicodeUTF8))
        self.browseButton.setText(QtGui.QApplication.translate("FileLineEdit", "...", None, QtGui.QApplication.UnicodeUTF8))

