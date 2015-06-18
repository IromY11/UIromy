# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\editors\looknfeel\LookNFeelPropertyEditorDockWidget.ui'
#
# Created: Fri Jun 12 15:50:36 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_LookNFeelPropertyEditorDockWidget(object):
    def setupUi(self, LookNFeelPropertyEditorDockWidget):
        LookNFeelPropertyEditorDockWidget.setObjectName("LookNFeelPropertyEditorDockWidget")
        LookNFeelPropertyEditorDockWidget.setEnabled(False)
        LookNFeelPropertyEditorDockWidget.resize(550, 773)
        LookNFeelPropertyEditorDockWidget.setMinimumSize(QtCore.QSize(336, 112))
        self.dockWidgetContents = QtGui.QWidget()
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.dockWidgetContents.sizePolicy().hasHeightForWidth())
        self.dockWidgetContents.setSizePolicy(sizePolicy)
        self.dockWidgetContents.setMinimumSize(QtCore.QSize(336, 70))
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.dockWidgetContents_2 = QtGui.QVBoxLayout(self.dockWidgetContents)
        self.dockWidgetContents_2.setContentsMargins(-1, 0, -1, -1)
        self.dockWidgetContents_2.setObjectName("dockWidgetContents_2")
        self.verticalLayout = QtGui.QVBoxLayout()
        self.verticalLayout.setObjectName("verticalLayout")
        self.dockWidgetContents_2.addLayout(self.verticalLayout)
        LookNFeelPropertyEditorDockWidget.setWidget(self.dockWidgetContents)

        self.retranslateUi(LookNFeelPropertyEditorDockWidget)
        QtCore.QMetaObject.connectSlotsByName(LookNFeelPropertyEditorDockWidget)

    def retranslateUi(self, LookNFeelPropertyEditorDockWidget):
        LookNFeelPropertyEditorDockWidget.setWindowTitle(QtGui.QApplication.translate("LookNFeelPropertyEditorDockWidget", "WidgetLook Property Editor", None, QtGui.QApplication.UnicodeUTF8))

