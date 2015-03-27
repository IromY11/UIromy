# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\CEGUIContainerWidget.ui'
#
# Created: Fri Mar 27 17:41:52 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_CEGUIContainerWidget(object):
    def setupUi(self, CEGUIContainerWidget):
        CEGUIContainerWidget.setObjectName("CEGUIContainerWidget")
        CEGUIContainerWidget.resize(752, 687)
        self.verticalLayout = QtGui.QVBoxLayout(CEGUIContainerWidget)
        self.verticalLayout.setSpacing(0)
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.view = GraphicsView(CEGUIContainerWidget)
        self.view.setFrameShape(QtGui.QFrame.NoFrame)
        self.view.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.view.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.view.setViewportUpdateMode(QtGui.QGraphicsView.MinimalViewportUpdate)
        self.view.setProperty("widgetResizable", False)
        self.view.setObjectName("view")
        self.verticalLayout.addWidget(self.view)
        self.widget_2 = QtGui.QWidget(CEGUIContainerWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.widget_2.sizePolicy().hasHeightForWidth())
        self.widget_2.setSizePolicy(sizePolicy)
        self.widget_2.setObjectName("widget_2")
        self.horizontalLayout = QtGui.QHBoxLayout(self.widget_2)
        self.horizontalLayout.setContentsMargins(0, 4, 9, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label = QtGui.QLabel(self.widget_2)
        font = QtGui.QFont()
        font.setPointSize(6)
        font.setItalic(True)
        self.label.setFont(font)
        self.label.setMargin(6)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.resolutionBox = QtGui.QComboBox(self.widget_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.resolutionBox.sizePolicy().hasHeightForWidth())
        self.resolutionBox.setSizePolicy(sizePolicy)
        self.resolutionBox.setMinimumSize(QtCore.QSize(150, 0))
        self.resolutionBox.setEditable(True)
        self.resolutionBox.setObjectName("resolutionBox")
        self.resolutionBox.addItem("")
        self.resolutionBox.addItem("")
        self.resolutionBox.addItem("")
        self.resolutionBox.addItem("")
        self.resolutionBox.addItem("")
        self.resolutionBox.addItem("")
        self.resolutionBox.addItem("")
        self.horizontalLayout.addWidget(self.resolutionBox)
        self.debugInfoButton = QtGui.QPushButton(self.widget_2)
        self.debugInfoButton.setText("")
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap("icons/cegui/debug_info.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.debugInfoButton.setIcon(icon)
        self.debugInfoButton.setAutoDefault(False)
        self.debugInfoButton.setDefault(False)
        self.debugInfoButton.setFlat(True)
        self.debugInfoButton.setObjectName("debugInfoButton")
        self.horizontalLayout.addWidget(self.debugInfoButton)
        self.verticalLayout.addWidget(self.widget_2)

        self.retranslateUi(CEGUIContainerWidget)
        QtCore.QMetaObject.connectSlotsByName(CEGUIContainerWidget)

    def retranslateUi(self, CEGUIContainerWidget):
        CEGUIContainerWidget.setWindowTitle(QtGui.QApplication.translate("CEGUIContainerWidget", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("CEGUIContainerWidget", "Embedded CEGUI", None, QtGui.QApplication.UnicodeUTF8))
        self.resolutionBox.setItemText(0, QtGui.QApplication.translate("CEGUIContainerWidget", "Project default", None, QtGui.QApplication.UnicodeUTF8))
        self.resolutionBox.setItemText(1, QtGui.QApplication.translate("CEGUIContainerWidget", "800x600", None, QtGui.QApplication.UnicodeUTF8))
        self.resolutionBox.setItemText(2, QtGui.QApplication.translate("CEGUIContainerWidget", "1024x768", None, QtGui.QApplication.UnicodeUTF8))
        self.resolutionBox.setItemText(3, QtGui.QApplication.translate("CEGUIContainerWidget", "1280x720", None, QtGui.QApplication.UnicodeUTF8))
        self.resolutionBox.setItemText(4, QtGui.QApplication.translate("CEGUIContainerWidget", "1280x1024", None, QtGui.QApplication.UnicodeUTF8))
        self.resolutionBox.setItemText(5, QtGui.QApplication.translate("CEGUIContainerWidget", "1650x1080", None, QtGui.QApplication.UnicodeUTF8))
        self.resolutionBox.setItemText(6, QtGui.QApplication.translate("CEGUIContainerWidget", "1920x1080", None, QtGui.QApplication.UnicodeUTF8))

from ceed.cegui.qtgraphics import GraphicsView
