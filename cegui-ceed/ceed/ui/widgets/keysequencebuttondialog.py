# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\Iromys\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\widgets\KeySequenceButtonDialog.ui'
#
# Created: Sat Mar 21 23:56:15 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_KeySequenceButtonDialog(object):
    def setupUi(self, KeySequenceButtonDialog):
        KeySequenceButtonDialog.setObjectName("KeySequenceButtonDialog")
        KeySequenceButtonDialog.resize(427, 154)
        KeySequenceButtonDialog.setSizeGripEnabled(False)
        KeySequenceButtonDialog.setModal(True)
        self.verticalLayout = QtGui.QVBoxLayout(KeySequenceButtonDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.label = QtGui.QLabel(KeySequenceButtonDialog)
        self.label.setWordWrap(True)
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.keyCombination = QtGui.QLineEdit(KeySequenceButtonDialog)
        self.keyCombination.setFocusPolicy(QtCore.Qt.NoFocus)
        self.keyCombination.setReadOnly(True)
        self.keyCombination.setObjectName("keyCombination")
        self.verticalLayout.addWidget(self.keyCombination)
        self.buttonBox = QtGui.QDialogButtonBox(KeySequenceButtonDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(KeySequenceButtonDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), KeySequenceButtonDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), KeySequenceButtonDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(KeySequenceButtonDialog)

    def retranslateUi(self, KeySequenceButtonDialog):
        KeySequenceButtonDialog.setWindowTitle(QtGui.QApplication.translate("KeySequenceButtonDialog", "Change a key sequence", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("KeySequenceButtonDialog", "This is a modal dialog, meaning it intercepts all keyboard input. Press the desired combination you want (modifier + key or just key) and press OK to confirm your choice. Cancel leaves the value alone.", None, QtGui.QApplication.UnicodeUTF8))

