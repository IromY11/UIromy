# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\FileSystemBrowser.ui'
#
<<<<<<< HEAD
# Created: Wed Apr 29 10:56:49 2015
=======
# Created: Mon May 18 01:37:27 2015
>>>>>>> 8d71f48158ce6577737b930a8dc24036e2e85210
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_FileSystemBrowser(object):
    def setupUi(self, FileSystemBrowser):
        FileSystemBrowser.setObjectName("FileSystemBrowser")
        FileSystemBrowser.resize(274, 843)
        self.contents = QtGui.QWidget()
        self.contents.setObjectName("contents")
        self.verticalLayout = QtGui.QVBoxLayout(self.contents)
        self.verticalLayout.setContentsMargins(-1, 0, -1, -1)
        self.verticalLayout.setObjectName("verticalLayout")
        self.buttons = QtGui.QWidget(self.contents)
        self.buttons.setObjectName("buttons")
        self.horizontalLayout = QtGui.QHBoxLayout(self.buttons)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.parentDirectoryButton = QtGui.QToolButton(self.buttons)
        self.parentDirectoryButton.setText("")
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap("icons/actions/filesystem_parent_directory.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.parentDirectoryButton.setIcon(icon)
        self.parentDirectoryButton.setIconSize(QtCore.QSize(22, 22))
        self.parentDirectoryButton.setAutoRaise(True)
        self.parentDirectoryButton.setObjectName("parentDirectoryButton")
        self.horizontalLayout.addWidget(self.parentDirectoryButton)
        self.homeDirectoryButton = QtGui.QToolButton(self.buttons)
        self.homeDirectoryButton.setText("")
        icon1 = QtGui.QIcon()
        icon1.addPixmap(QtGui.QPixmap("icons/actions/filesystem_home_directory.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.homeDirectoryButton.setIcon(icon1)
        self.homeDirectoryButton.setIconSize(QtCore.QSize(22, 22))
        self.homeDirectoryButton.setAutoRaise(True)
        self.homeDirectoryButton.setObjectName("homeDirectoryButton")
        self.horizontalLayout.addWidget(self.homeDirectoryButton)
        self.projectDirectoryButton = QtGui.QToolButton(self.buttons)
        self.projectDirectoryButton.setEnabled(False)
        self.projectDirectoryButton.setText("")
        icon2 = QtGui.QIcon()
        icon2.addPixmap(QtGui.QPixmap("icons/actions/filesystem_project_directory.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.projectDirectoryButton.setIcon(icon2)
        self.projectDirectoryButton.setIconSize(QtCore.QSize(22, 22))
        self.projectDirectoryButton.setAutoRaise(True)
        self.projectDirectoryButton.setObjectName("projectDirectoryButton")
        self.horizontalLayout.addWidget(self.projectDirectoryButton)
        self.activeFileDirectoryButton = QtGui.QToolButton(self.buttons)
        self.activeFileDirectoryButton.setEnabled(False)
        self.activeFileDirectoryButton.setText("")
        icon3 = QtGui.QIcon()
        icon3.addPixmap(QtGui.QPixmap("icons/actions/filesystem_active_file_directory.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.activeFileDirectoryButton.setIcon(icon3)
        self.activeFileDirectoryButton.setIconSize(QtCore.QSize(22, 22))
        self.activeFileDirectoryButton.setAutoRaise(True)
        self.activeFileDirectoryButton.setObjectName("activeFileDirectoryButton")
        self.horizontalLayout.addWidget(self.activeFileDirectoryButton)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.verticalLayout.addWidget(self.buttons)
        self.pathBox = QtGui.QComboBox(self.contents)
        self.pathBox.setEditable(True)
        self.pathBox.setObjectName("pathBox")
        self.verticalLayout.addWidget(self.pathBox)
        self.view = QtGui.QListView(self.contents)
        self.view.setEnabled(True)
        self.view.setObjectName("view")
        self.verticalLayout.addWidget(self.view)
        FileSystemBrowser.setWidget(self.contents)

        self.retranslateUi(FileSystemBrowser)
        QtCore.QMetaObject.connectSlotsByName(FileSystemBrowser)
        FileSystemBrowser.setTabOrder(self.view, self.parentDirectoryButton)
        FileSystemBrowser.setTabOrder(self.parentDirectoryButton, self.homeDirectoryButton)
        FileSystemBrowser.setTabOrder(self.homeDirectoryButton, self.projectDirectoryButton)
        FileSystemBrowser.setTabOrder(self.projectDirectoryButton, self.activeFileDirectoryButton)
        FileSystemBrowser.setTabOrder(self.activeFileDirectoryButton, self.pathBox)

    def retranslateUi(self, FileSystemBrowser):
        FileSystemBrowser.setWindowTitle(QtGui.QApplication.translate("FileSystemBrowser", "File System Browser", None, QtGui.QApplication.UnicodeUTF8))
        self.parentDirectoryButton.setToolTip(QtGui.QApplication.translate("FileSystemBrowser", "Up", None, QtGui.QApplication.UnicodeUTF8))
        self.homeDirectoryButton.setToolTip(QtGui.QApplication.translate("FileSystemBrowser", "Home", None, QtGui.QApplication.UnicodeUTF8))
        self.projectDirectoryButton.setToolTip(QtGui.QApplication.translate("FileSystemBrowser", "Project Directory", None, QtGui.QApplication.UnicodeUTF8))
        self.activeFileDirectoryButton.setToolTip(QtGui.QApplication.translate("FileSystemBrowser", "Locate active file", None, QtGui.QApplication.UnicodeUTF8))

