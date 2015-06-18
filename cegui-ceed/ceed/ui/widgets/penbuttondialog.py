# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\widgets\PenButtonDialog.ui'
#
# Created: Fri Jun 12 15:50:36 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_PenButtonDialog(object):
    def setupUi(self, PenButtonDialog):
        PenButtonDialog.setObjectName("PenButtonDialog")
        PenButtonDialog.resize(400, 300)
        self.gridLayout = QtGui.QGridLayout(PenButtonDialog)
        self.gridLayout.setObjectName("gridLayout")
        self.lineWidth = QtGui.QDoubleSpinBox(PenButtonDialog)
        self.lineWidth.setMaximum(100.0)
        self.lineWidth.setSingleStep(0.5)
        self.lineWidth.setProperty("value", 1.0)
        self.lineWidth.setObjectName("lineWidth")
        self.gridLayout.addWidget(self.lineWidth, 2, 1, 1, 1)
        self.colourLabel = QtGui.QLabel(PenButtonDialog)
        self.colourLabel.setObjectName("colourLabel")
        self.gridLayout.addWidget(self.colourLabel, 0, 0, 1, 1)
        self.buttonBox = QtGui.QDialogButtonBox(PenButtonDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridLayout.addWidget(self.buttonBox, 5, 0, 1, 2)
        spacerItem = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.gridLayout.addItem(spacerItem, 3, 1, 1, 1)
        self.lineStyleLabel = QtGui.QLabel(PenButtonDialog)
        self.lineStyleLabel.setObjectName("lineStyleLabel")
        self.gridLayout.addWidget(self.lineStyleLabel, 1, 0, 1, 1)
        self.lineStyle = QtGui.QComboBox(PenButtonDialog)
        self.lineStyle.setObjectName("lineStyle")
        self.lineStyle.addItem("")
        self.lineStyle.addItem("")
        self.lineStyle.addItem("")
        self.lineStyle.addItem("")
        self.lineStyle.addItem("")
        self.gridLayout.addWidget(self.lineStyle, 1, 1, 1, 1)
        self.lineWidthLabel = QtGui.QLabel(PenButtonDialog)
        self.lineWidthLabel.setObjectName("lineWidthLabel")
        self.gridLayout.addWidget(self.lineWidthLabel, 2, 0, 1, 1)
        self.preview = QtGui.QGraphicsView(PenButtonDialog)
        self.preview.setObjectName("preview")
        self.gridLayout.addWidget(self.preview, 4, 0, 1, 2)
        self.colour = ColourButton(PenButtonDialog)
        self.colour.setObjectName("colour")
        self.gridLayout.addWidget(self.colour, 0, 1, 1, 1)

        self.retranslateUi(PenButtonDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), PenButtonDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), PenButtonDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(PenButtonDialog)

    def retranslateUi(self, PenButtonDialog):
        PenButtonDialog.setWindowTitle(QtGui.QApplication.translate("PenButtonDialog", "Choose pen style", None, QtGui.QApplication.UnicodeUTF8))
        self.colourLabel.setText(QtGui.QApplication.translate("PenButtonDialog", "Colour", None, QtGui.QApplication.UnicodeUTF8))
        self.lineStyleLabel.setText(QtGui.QApplication.translate("PenButtonDialog", "Line style", None, QtGui.QApplication.UnicodeUTF8))
        self.lineStyle.setItemText(0, QtGui.QApplication.translate("PenButtonDialog", "Solid", None, QtGui.QApplication.UnicodeUTF8))
        self.lineStyle.setItemText(1, QtGui.QApplication.translate("PenButtonDialog", "Dash", None, QtGui.QApplication.UnicodeUTF8))
        self.lineStyle.setItemText(2, QtGui.QApplication.translate("PenButtonDialog", "Dot", None, QtGui.QApplication.UnicodeUTF8))
        self.lineStyle.setItemText(3, QtGui.QApplication.translate("PenButtonDialog", "Dash Dot", None, QtGui.QApplication.UnicodeUTF8))
        self.lineStyle.setItemText(4, QtGui.QApplication.translate("PenButtonDialog", "Dash Dot Dot", None, QtGui.QApplication.UnicodeUTF8))
        self.lineWidthLabel.setText(QtGui.QApplication.translate("PenButtonDialog", "Line width", None, QtGui.QApplication.UnicodeUTF8))
        self.colour.setText(QtGui.QApplication.translate("PenButtonDialog", "PushButton", None, QtGui.QApplication.UnicodeUTF8))

from ceed.qtwidgets import ColourButton
