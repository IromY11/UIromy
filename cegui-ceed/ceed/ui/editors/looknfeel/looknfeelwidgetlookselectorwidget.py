# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\editors\looknfeel\LookNFeelWidgetLookSelectorWidget.ui'
#
# Created: Thu Apr 02 18:07:13 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_LookNFeelWidgetLookSelector(object):
    def setupUi(self, LookNFeelWidgetLookSelector):
        LookNFeelWidgetLookSelector.setObjectName("LookNFeelWidgetLookSelector")
        LookNFeelWidgetLookSelector.setEnabled(False)
        LookNFeelWidgetLookSelector.resize(386, 90)
        LookNFeelWidgetLookSelector.setMinimumSize(QtCore.QSize(336, 90))
        self.dockWidgetContents = QtGui.QWidget()
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.dockWidgetContents.sizePolicy().hasHeightForWidth())
        self.dockWidgetContents.setSizePolicy(sizePolicy)
        self.dockWidgetContents.setMinimumSize(QtCore.QSize(336, 70))
        self.dockWidgetContents.setMaximumSize(QtCore.QSize(524287, 70))
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.verticalLayout = QtGui.QVBoxLayout(self.dockWidgetContents)
        self.verticalLayout.setContentsMargins(-1, 0, -1, -1)
        self.verticalLayout.setObjectName("verticalLayout")
        self.vertLayout = QtGui.QVBoxLayout()
        self.vertLayout.setSpacing(4)
        self.vertLayout.setObjectName("vertLayout")
        spacerItem = QtGui.QSpacerItem(20, 2, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.vertLayout.addItem(spacerItem)
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setSpacing(0)
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.label = QtGui.QLabel(self.dockWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setMargin(1)
        self.label.setObjectName("label")
        self.horizontalLayout_3.addWidget(self.label)
        self.fileNameLabel = QtGui.QLabel(self.dockWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.fileNameLabel.sizePolicy().hasHeightForWidth())
        self.fileNameLabel.setSizePolicy(sizePolicy)
        self.fileNameLabel.setMinimumSize(QtCore.QSize(230, 23))
        self.fileNameLabel.setStyleSheet("QLabel\n"
"{\n"
"    background-color: rgb(217, 217, 217);\n"
"    border-color: rgb(166, 166, 166);\n"
"    border-style: inset;\n"
"    border-width: 1px;\n"
"}\n"
"\n"
"QToolTip\n"
"{\n"
"    color: rgb(0, 0, 0);\n"
"    background-color: rgb(194, 255, 237);\n"
"    border: 1px solid black;\n"
"    border-radius: 1px;\n"
" }\n"
"\n"
"\n"
"")
        self.fileNameLabel.setObjectName("fileNameLabel")
        self.horizontalLayout_3.addWidget(self.fileNameLabel)
        self.vertLayout.addLayout(self.horizontalLayout_3)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.nameLabel = QtGui.QLabel(self.dockWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.nameLabel.sizePolicy().hasHeightForWidth())
        self.nameLabel.setSizePolicy(sizePolicy)
        self.nameLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.nameLabel.setMargin(1)
        self.nameLabel.setObjectName("nameLabel")
        self.horizontalLayout.addWidget(self.nameLabel)
        self.widgetLookNameBox = QtGui.QComboBox(self.dockWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.widgetLookNameBox.sizePolicy().hasHeightForWidth())
        self.widgetLookNameBox.setSizePolicy(sizePolicy)
        self.widgetLookNameBox.setObjectName("widgetLookNameBox")
        self.horizontalLayout.addWidget(self.widgetLookNameBox)
        self.editWidgetLookButton = QtGui.QPushButton(self.dockWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.editWidgetLookButton.sizePolicy().hasHeightForWidth())
        self.editWidgetLookButton.setSizePolicy(sizePolicy)
        self.editWidgetLookButton.setMinimumSize(QtCore.QSize(40, 23))
        self.editWidgetLookButton.setMaximumSize(QtCore.QSize(40, 23))
        self.editWidgetLookButton.setObjectName("editWidgetLookButton")
        self.horizontalLayout.addWidget(self.editWidgetLookButton)
        self.vertLayout.addLayout(self.horizontalLayout)
        spacerItem1 = QtGui.QSpacerItem(20, 2, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.vertLayout.addItem(spacerItem1)
        self.verticalLayout.addLayout(self.vertLayout)
        LookNFeelWidgetLookSelector.setWidget(self.dockWidgetContents)

        self.retranslateUi(LookNFeelWidgetLookSelector)
        QtCore.QMetaObject.connectSlotsByName(LookNFeelWidgetLookSelector)

    def retranslateUi(self, LookNFeelWidgetLookSelector):
        LookNFeelWidgetLookSelector.setWindowTitle(QtGui.QApplication.translate("LookNFeelWidgetLookSelector", "WidgetLook Selector", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("LookNFeelWidgetLookSelector", "Look n\' Feel file:", None, QtGui.QApplication.UnicodeUTF8))
        self.fileNameLabel.setText(QtGui.QApplication.translate("LookNFeelWidgetLookSelector", "File path", None, QtGui.QApplication.UnicodeUTF8))
        self.nameLabel.setText(QtGui.QApplication.translate("LookNFeelWidgetLookSelector", "Select a WidgetLook:", None, QtGui.QApplication.UnicodeUTF8))
        self.editWidgetLookButton.setText(QtGui.QApplication.translate("LookNFeelWidgetLookSelector", "Edit", None, QtGui.QApplication.UnicodeUTF8))

