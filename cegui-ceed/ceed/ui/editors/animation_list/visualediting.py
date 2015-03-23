# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\editors\animation_list\VisualEditing.ui'
#
# Created: Mon Mar 23 15:36:26 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_VisualEditing(object):
    def setupUi(self, VisualEditing):
        VisualEditing.setObjectName("VisualEditing")
        VisualEditing.resize(400, 300)
        self.verticalLayout_2 = QtGui.QVBoxLayout(VisualEditing)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.toolPanel = QtGui.QWidget(VisualEditing)
        self.toolPanel.setObjectName("toolPanel")
        self.horizontalLayout = QtGui.QHBoxLayout(self.toolPanel)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label = QtGui.QLabel(self.toolPanel)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.previewWidgetSelector = QtGui.QComboBox(self.toolPanel)
        self.previewWidgetSelector.setObjectName("previewWidgetSelector")
        self.horizontalLayout.addWidget(self.previewWidgetSelector)
        self.verticalLayout_2.addWidget(self.toolPanel)
        self.ceguiPreview = QtGui.QWidget(VisualEditing)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.ceguiPreview.sizePolicy().hasHeightForWidth())
        self.ceguiPreview.setSizePolicy(sizePolicy)
        self.ceguiPreview.setObjectName("ceguiPreview")
        self.verticalLayout_2.addWidget(self.ceguiPreview)

        self.retranslateUi(VisualEditing)
        QtCore.QMetaObject.connectSlotsByName(VisualEditing)

    def retranslateUi(self, VisualEditing):
        VisualEditing.setWindowTitle(QtGui.QApplication.translate("VisualEditing", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("VisualEditing", "Preview widget", None, QtGui.QApplication.UnicodeUTF8))

