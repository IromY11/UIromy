# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\Iromys\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\AboutDialog.ui'
#
# Created: Mon May 18 01:37:27 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_AboutDialog(object):
    def setupUi(self, AboutDialog):
        AboutDialog.setObjectName("AboutDialog")
        AboutDialog.setWindowModality(QtCore.Qt.ApplicationModal)
        AboutDialog.resize(253, 212)
        AboutDialog.setMinimumSize(QtCore.QSize(0, 0))
        AboutDialog.setModal(False)
        self.verticalLayout = QtGui.QVBoxLayout(AboutDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.aboutImage = QtGui.QLabel(AboutDialog)
        self.aboutImage.setFrameShape(QtGui.QFrame.StyledPanel)
        self.aboutImage.setText("")
        self.aboutImage.setObjectName("aboutImage")
        self.verticalLayout.addWidget(self.aboutImage)
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.gridLayout.setObjectName("gridLayout")
        self.CEEDDescription = QtGui.QLabel(AboutDialog)
        self.CEEDDescription.setAlignment(QtCore.Qt.AlignCenter)
        self.CEEDDescription.setTextInteractionFlags(QtCore.Qt.NoTextInteraction)
        self.CEEDDescription.setObjectName("CEEDDescription")
        self.gridLayout.addWidget(self.CEEDDescription, 0, 0, 1, 1)
        self.CEEDVersion = QtGui.QLabel(AboutDialog)
        self.CEEDVersion.setTextInteractionFlags(QtCore.Qt.NoTextInteraction)
        self.CEEDVersion.setObjectName("CEEDVersion")
        self.gridLayout.addWidget(self.CEEDVersion, 2, 0, 1, 1)
        self.PySideVersion = QtGui.QLabel(AboutDialog)
        self.PySideVersion.setTextInteractionFlags(QtCore.Qt.NoTextInteraction)
        self.PySideVersion.setObjectName("PySideVersion")
        self.gridLayout.addWidget(self.PySideVersion, 3, 0, 1, 1)
        self.QtVersion = QtGui.QLabel(AboutDialog)
        self.QtVersion.setTextInteractionFlags(QtCore.Qt.NoTextInteraction)
        self.QtVersion.setObjectName("QtVersion")
        self.gridLayout.addWidget(self.QtVersion, 4, 0, 1, 1)
        self.PyCEGUIVersion = QtGui.QLabel(AboutDialog)
        self.PyCEGUIVersion.setTextInteractionFlags(QtCore.Qt.NoTextInteraction)
        self.PyCEGUIVersion.setObjectName("PyCEGUIVersion")
        self.gridLayout.addWidget(self.PyCEGUIVersion, 5, 0, 1, 1)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem, 1, 0, 1, 1)
        self.verticalLayout.addLayout(self.gridLayout)
        self.buttonBox = QtGui.QDialogButtonBox(AboutDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(AboutDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), AboutDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), AboutDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(AboutDialog)

    def retranslateUi(self, AboutDialog):
        AboutDialog.setWindowTitle(QtGui.QApplication.translate("AboutDialog", "About", None, QtGui.QApplication.UnicodeUTF8))
        self.CEEDDescription.setText(QtGui.QApplication.translate("AboutDialog", "This is a bug; fix me.", None, QtGui.QApplication.UnicodeUTF8))
        self.CEEDVersion.setText(QtGui.QApplication.translate("AboutDialog", "This is a bug; fix me.", None, QtGui.QApplication.UnicodeUTF8))
        self.PySideVersion.setText(QtGui.QApplication.translate("AboutDialog", "This is a bug; fix me.", None, QtGui.QApplication.UnicodeUTF8))
        self.QtVersion.setText(QtGui.QApplication.translate("AboutDialog", "This is a bug; fix me.", None, QtGui.QApplication.UnicodeUTF8))
        self.PyCEGUIVersion.setText(QtGui.QApplication.translate("AboutDialog", "This is a bug; fix me.", None, QtGui.QApplication.UnicodeUTF8))

