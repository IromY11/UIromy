# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\Iromys\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\MainWindow.ui'
#
<<<<<<< HEAD
# Created: Fri Apr 03 20:18:18 2015
=======
# Created: Fri Apr 03 17:53:35 2015
>>>>>>> c571f43318051bacc0fbf7da05af203cfa97782a
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(813, 864)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Ignored, QtGui.QSizePolicy.Ignored)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MainWindow.sizePolicy().hasHeightForWidth())
        MainWindow.setSizePolicy(sizePolicy)
        MainWindow.setMinimumSize(QtCore.QSize(800, 600))
        MainWindow.setAcceptDrops(False)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap("../../data/icons/ceed.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        MainWindow.setWindowIcon(icon)
        MainWindow.setWindowOpacity(1.0)
        MainWindow.setDocumentMode(True)
        MainWindow.setDockNestingEnabled(False)
        MainWindow.setUnifiedTitleAndToolBarOnMac(False)
        self.centralwidget = QtGui.QWidget(MainWindow)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.centralwidget.sizePolicy().hasHeightForWidth())
        self.centralwidget.setSizePolicy(sizePolicy)
        self.centralwidget.setObjectName("centralwidget")
        self.verticalLayout_3 = QtGui.QVBoxLayout(self.centralwidget)
        self.verticalLayout_3.setObjectName("verticalLayout_3")
        self.tabs = QtGui.QTabWidget(self.centralwidget)
        self.tabs.setTabPosition(QtGui.QTabWidget.North)
        self.tabs.setTabShape(QtGui.QTabWidget.Rounded)
        self.tabs.setIconSize(QtCore.QSize(16, 16))
        self.tabs.setUsesScrollButtons(True)
        self.tabs.setDocumentMode(True)
        self.tabs.setTabsClosable(True)
        self.tabs.setMovable(True)
        self.tabs.setObjectName("tabs")
        self.verticalLayout_3.addWidget(self.tabs)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 813, 21))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.actionAbout = QtGui.QAction(MainWindow)
        self.actionAbout.setObjectName("actionAbout")
        self.actionLicense = QtGui.QAction(MainWindow)
        self.actionLicense.setObjectName("actionLicense")
        self.actionCloseTab = QtGui.QAction(MainWindow)
        icon1 = QtGui.QIcon()
        icon1.addPixmap(QtGui.QPixmap("icons/actions/close_tab.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionCloseTab.setIcon(icon1)
        self.actionCloseTab.setObjectName("actionCloseTab")
        self.actionCloseOtherTabs = QtGui.QAction(MainWindow)
        icon2 = QtGui.QIcon()
        icon2.addPixmap(QtGui.QPixmap("icons/actions/close_other_tabs.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionCloseOtherTabs.setIcon(icon2)
        self.actionCloseOtherTabs.setObjectName("actionCloseOtherTabs")
        self.actionCloseAllTabs = QtGui.QAction(MainWindow)
        icon3 = QtGui.QIcon()
        icon3.addPixmap(QtGui.QPixmap("icons/actions/close_all_tabs.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionCloseAllTabs.setIcon(icon3)
        self.actionCloseAllTabs.setObjectName("actionCloseAllTabs")
        self.actionQt = QtGui.QAction(MainWindow)
        self.actionQt.setObjectName("actionQt")

        self.retranslateUi(MainWindow)
        self.tabs.setCurrentIndex(-1)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "UIromy Editor", None, QtGui.QApplication.UnicodeUTF8))
        self.actionAbout.setText(QtGui.QApplication.translate("MainWindow", "About", None, QtGui.QApplication.UnicodeUTF8))
        self.actionLicense.setText(QtGui.QApplication.translate("MainWindow", "License", None, QtGui.QApplication.UnicodeUTF8))
        self.actionCloseTab.setText(QtGui.QApplication.translate("MainWindow", "Close this tab", None, QtGui.QApplication.UnicodeUTF8))
        self.actionCloseOtherTabs.setText(QtGui.QApplication.translate("MainWindow", "Close other tabs", None, QtGui.QApplication.UnicodeUTF8))
        self.actionCloseAllTabs.setText(QtGui.QApplication.translate("MainWindow", "Close all tabs", None, QtGui.QApplication.UnicodeUTF8))
        self.actionQt.setText(QtGui.QApplication.translate("MainWindow", "Qt", None, QtGui.QApplication.UnicodeUTF8))

