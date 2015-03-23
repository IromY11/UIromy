# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\NewProjectDialog.ui'
#
# Created: Mon Mar 23 15:36:24 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_NewProjectDialog(object):
    def setupUi(self, NewProjectDialog):
        NewProjectDialog.setObjectName("NewProjectDialog")
        NewProjectDialog.resize(411, 231)
        self.verticalLayout = QtGui.QVBoxLayout(NewProjectDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.helpText = QtGui.QLabel(NewProjectDialog)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.MinimumExpanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.helpText.sizePolicy().hasHeightForWidth())
        self.helpText.setSizePolicy(sizePolicy)
        self.helpText.setWordWrap(True)
        self.helpText.setObjectName("helpText")
        self.verticalLayout.addWidget(self.helpText)
        self.groupBox = QtGui.QGroupBox(NewProjectDialog)
        self.groupBox.setFlat(False)
        self.groupBox.setCheckable(False)
        self.groupBox.setObjectName("groupBox")
        self.formLayout = QtGui.QFormLayout(self.groupBox)
        self.formLayout.setFieldGrowthPolicy(QtGui.QFormLayout.ExpandingFieldsGrow)
        self.formLayout.setObjectName("formLayout")
        self.projectFilePathLabel = QtGui.QLabel(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.projectFilePathLabel.sizePolicy().hasHeightForWidth())
        self.projectFilePathLabel.setSizePolicy(sizePolicy)
        self.projectFilePathLabel.setMinimumSize(QtCore.QSize(0, 25))
        self.projectFilePathLabel.setObjectName("projectFilePathLabel")
        self.formLayout.setWidget(0, QtGui.QFormLayout.LabelRole, self.projectFilePathLabel)
        self.projectFilePath = FileLineEdit(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.projectFilePath.sizePolicy().hasHeightForWidth())
        self.projectFilePath.setSizePolicy(sizePolicy)
        self.projectFilePath.setMinimumSize(QtCore.QSize(0, 25))
        self.projectFilePath.setObjectName("projectFilePath")
        self.formLayout.setWidget(0, QtGui.QFormLayout.FieldRole, self.projectFilePath)
        self.createResourceDirs = QtGui.QCheckBox(self.groupBox)
        self.createResourceDirs.setEnabled(True)
        self.createResourceDirs.setAutoFillBackground(False)
        self.createResourceDirs.setChecked(True)
        self.createResourceDirs.setObjectName("createResourceDirs")
        self.formLayout.setWidget(1, QtGui.QFormLayout.FieldRole, self.createResourceDirs)
        self.verticalLayout.addWidget(self.groupBox)
        self.buttonBox = QtGui.QDialogButtonBox(NewProjectDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(NewProjectDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), NewProjectDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), NewProjectDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(NewProjectDialog)

    def retranslateUi(self, NewProjectDialog):
        NewProjectDialog.setWindowTitle(QtGui.QApplication.translate("NewProjectDialog", "Create a new project file", None, QtGui.QApplication.UnicodeUTF8))
        self.helpText.setText(QtGui.QApplication.translate("NewProjectDialog", "You can tinker with more settings later in the project settings window.", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox.setTitle(QtGui.QApplication.translate("NewProjectDialog", "Basic project settings", None, QtGui.QApplication.UnicodeUTF8))
        self.projectFilePathLabel.setText(QtGui.QApplication.translate("NewProjectDialog", "Project file location", None, QtGui.QApplication.UnicodeUTF8))
        self.createResourceDirs.setText(QtGui.QApplication.translate("NewProjectDialog", "Create resource directories", None, QtGui.QApplication.UnicodeUTF8))

from ceed.qtwidgets import FileLineEdit
