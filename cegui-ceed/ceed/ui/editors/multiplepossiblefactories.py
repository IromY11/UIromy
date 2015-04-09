# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\editors\MultiplePossibleFactories.ui'
#
# Created: Thu Apr 09 19:00:21 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_MultiplePossibleFactoriesDialog(object):
    def setupUi(self, MultiplePossibleFactoriesDialog):
        MultiplePossibleFactoriesDialog.setObjectName("MultiplePossibleFactoriesDialog")
        MultiplePossibleFactoriesDialog.setWindowModality(QtCore.Qt.ApplicationModal)
        MultiplePossibleFactoriesDialog.resize(400, 300)
        self.verticalLayout = QtGui.QVBoxLayout(MultiplePossibleFactoriesDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.label = QtGui.QLabel(MultiplePossibleFactoriesDialog)
        self.label.setWordWrap(True)
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.factoryChoice = QtGui.QListWidget(MultiplePossibleFactoriesDialog)
        self.factoryChoice.setObjectName("factoryChoice")
        self.verticalLayout.addWidget(self.factoryChoice)
        self.buttonBox = QtGui.QDialogButtonBox(MultiplePossibleFactoriesDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(MultiplePossibleFactoriesDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), MultiplePossibleFactoriesDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), MultiplePossibleFactoriesDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(MultiplePossibleFactoriesDialog)

    def retranslateUi(self, MultiplePossibleFactoriesDialog):
        MultiplePossibleFactoriesDialog.setWindowTitle(QtGui.QApplication.translate("MultiplePossibleFactoriesDialog", "Multiple possible editors found!", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("MultiplePossibleFactoriesDialog", "The file you are trying to open checked possible in multiple editor factories. That means that it\'s extension is ambiguous. Please choose which editor you want to open this file in.", None, QtGui.QApplication.UnicodeUTF8))

