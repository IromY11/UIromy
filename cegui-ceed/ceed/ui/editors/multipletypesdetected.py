# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\Iromys\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\editors\MultipleTypesDetected.ui'
#
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_MultipleTypesDetectedDialog(object):
    def setupUi(self, MultipleTypesDetectedDialog):
        MultipleTypesDetectedDialog.setObjectName("MultipleTypesDetectedDialog")
        MultipleTypesDetectedDialog.resize(400, 300)
        self.verticalLayout = QtGui.QVBoxLayout(MultipleTypesDetectedDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.label = QtGui.QLabel(MultipleTypesDetectedDialog)
        self.label.setWordWrap(True)
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.typeChoice = QtGui.QListWidget(MultipleTypesDetectedDialog)
        self.typeChoice.setAlternatingRowColors(False)
        self.typeChoice.setObjectName("typeChoice")
        self.verticalLayout.addWidget(self.typeChoice)
        self.buttonBox = QtGui.QDialogButtonBox(MultipleTypesDetectedDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(MultipleTypesDetectedDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), MultipleTypesDetectedDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), MultipleTypesDetectedDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(MultipleTypesDetectedDialog)

    def retranslateUi(self, MultipleTypesDetectedDialog):
        MultipleTypesDetectedDialog.setWindowTitle(QtGui.QApplication.translate("MultipleTypesDetectedDialog", "Multiple types detected!", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("MultipleTypesDetectedDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'MS Shell Dlg 2\'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">An effort was made to auto-detect type of the file your are trying to open. Multiple positives turned up. Please decide the type of the data in the file manually.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Bold items are the ones that turned up positive, so preferrably choose from them.</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))

