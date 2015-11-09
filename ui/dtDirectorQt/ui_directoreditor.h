/********************************************************************************
** Form generated from reading UI file 'directoreditor.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIRECTOREDITOR_H
#define UI_DIRECTOREDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
#include <dtDirectorQt/graphbrowser.h>
#include <dtDirectorQt/graphtabs.h>
#include <dtDirectorQt/nodetabs.h>
#include <dtDirectorQt/propertyeditor.h>
#include <dtDirectorQt/searchbrowser.h>
#include <dtDirectorQt/threadbrowser.h>

QT_BEGIN_NAMESPACE

class Ui_DirectorEditor
{
public:
    QAction *action_New;
    QAction *action_Load;
    QAction *action_Save;
    QAction *action_Save_as;
    QAction *action_Step_Out_Of_Graph;
    QAction *action_Smart_Grid_snap;
    QAction *action_Undo;
    QAction *action_Redo;
    QAction *action_Delete;
    QAction *action_Cut;
    QAction *action_Copy;
    QAction *action_Paste;
    QAction *action_Manage_Libraries;
    QAction *action_Property_Editor;
    QAction *action_Graph_Browser;
    QAction *action_Show_Links;
    QAction *action_Hide_Links;
    QAction *action_Refresh;
    QAction *action_Exit;
    QAction *actionDirector_Help;
    QAction *action_Search_Browser;
    QAction *actionPause;
    QAction *actionContinue;
    QAction *actionStep_Next;
    QAction *actionPaste_with_Links;
    QAction *action_Thread_Browser;
    QAction *action_Node_Palette;
    QAction *actionToggle_Break_Point;
    QAction *action_Manage_Plugins;
    QAction *actionNone;
    QAction *action_Manage_Imported_Scripts;
    QWidget *centralwidget;
    QGridLayout *gridLayout_9;
    dtDirector::GraphTabs *graphTab;
    QWidget *tab;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuRecent_Files;
    QMenu *menu_Edit;
    QMenu *menu_View;
    QMenu *menuHelp;
    QMenu *menuDebug;
    QStatusBar *statusbar;
    QToolBar *FileToolbar;
    QToolBar *EditToolbar;
    dtDirector::PropertyEditor *propertyEditor;
    dtDirector::GraphBrowser *graphBrowser;
    dtDirector::SearchBrowser *searchBrowser;
    dtDirector::ThreadBrowser *threadBrowser;
    QDockWidget *nodePalette;
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    QLineEdit *nodeSearchEdit;
    QTabWidget *nodeTabs;
    QWidget *eventNodeTab;
    QGridLayout *gridLayout_2;
    dtDirector::NodeTabs *eventNodeTabWidget;
    QWidget *eventNodeTabWidgetPage1;
    QWidget *actionNodeTab;
    QGridLayout *gridLayout_3;
    dtDirector::NodeTabs *actionNodeTabWidget;
    QWidget *actionNodeTabWidgetPage1;
    QWidget *mutatorNodeTab;
    QGridLayout *gridLayout_8;
    dtDirector::NodeTabs *mutatorNodeTabWidget;
    QWidget *mutatorNodeTabWidgetPage1;
    QWidget *variableNodeTab;
    QGridLayout *gridLayout_4;
    dtDirector::NodeTabs *variableNodeTabWidget;
    QWidget *variableNodeTabWidgetPage1;
    QWidget *macroNodeTab;
    QGridLayout *gridLayout_5;
    dtDirector::NodeTabs *macroNodeTabWidget;
    QWidget *macroNodeTabWidgetPage1;
    QWidget *linkNodeTab;
    QGridLayout *gridLayout_6;
    dtDirector::NodeTabs *linkNodeTabWidget;
    QWidget *linkNodeTabWidgetPage1;
    QWidget *miscNodeTab;
    QGridLayout *gridLayout_7;
    dtDirector::NodeTabs *miscNodeTabWidget;
    QWidget *miscNodeTabWidgetPage1;
    QWidget *referenceNodeTabs;
    QGridLayout *gridLayout_11;
    dtDirector::NodeTabs *referenceNodeTabWidget;
    QWidget *referenceNodeTabWidgetPage1;
    QWidget *searchNodeTab;
    QGridLayout *gridLayout_10;
    dtDirector::NodeTabs *searchNodeTabWidget;
    QWidget *searchNodeTabWidgetPage1;

    void setupUi(QMainWindow *DirectorEditor)
    {
        if (DirectorEditor->objectName().isEmpty())
            DirectorEditor->setObjectName(QString::fromUtf8("DirectorEditor"));
        DirectorEditor->resize(800, 729);
        DirectorEditor->setAnimated(true);
        DirectorEditor->setDocumentMode(false);
        DirectorEditor->setDockNestingEnabled(false);
        DirectorEditor->setDockOptions(QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks|QMainWindow::VerticalTabs);
        action_New = new QAction(DirectorEditor);
        action_New->setObjectName(QString::fromUtf8("action_New"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/new.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_New->setIcon(icon);
        action_Load = new QAction(DirectorEditor);
        action_Load->setObjectName(QString::fromUtf8("action_Load"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Load->setIcon(icon1);
        action_Save = new QAction(DirectorEditor);
        action_Save->setObjectName(QString::fromUtf8("action_Save"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Save->setIcon(icon2);
        action_Save_as = new QAction(DirectorEditor);
        action_Save_as->setObjectName(QString::fromUtf8("action_Save_as"));
        action_Save_as->setIcon(icon2);
        action_Step_Out_Of_Graph = new QAction(DirectorEditor);
        action_Step_Out_Of_Graph->setObjectName(QString::fromUtf8("action_Step_Out_Of_Graph"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/parent.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Step_Out_Of_Graph->setIcon(icon3);
        action_Smart_Grid_snap = new QAction(DirectorEditor);
        action_Smart_Grid_snap->setObjectName(QString::fromUtf8("action_Smart_Grid_snap"));
        action_Smart_Grid_snap->setCheckable(true);
        action_Smart_Grid_snap->setChecked(true);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/snapgrid.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Smart_Grid_snap->setIcon(icon4);
        action_Undo = new QAction(DirectorEditor);
        action_Undo->setObjectName(QString::fromUtf8("action_Undo"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/undo.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Undo->setIcon(icon5);
        action_Redo = new QAction(DirectorEditor);
        action_Redo->setObjectName(QString::fromUtf8("action_Redo"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icons/redo.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Redo->setIcon(icon6);
        action_Delete = new QAction(DirectorEditor);
        action_Delete->setObjectName(QString::fromUtf8("action_Delete"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/icons/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Delete->setIcon(icon7);
        action_Cut = new QAction(DirectorEditor);
        action_Cut->setObjectName(QString::fromUtf8("action_Cut"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/cut.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Cut->setIcon(icon8);
        action_Copy = new QAction(DirectorEditor);
        action_Copy->setObjectName(QString::fromUtf8("action_Copy"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/duplicate.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Copy->setIcon(icon9);
        action_Paste = new QAction(DirectorEditor);
        action_Paste->setObjectName(QString::fromUtf8("action_Paste"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/icons/paste.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Paste->setIcon(icon10);
        action_Manage_Libraries = new QAction(DirectorEditor);
        action_Manage_Libraries->setObjectName(QString::fromUtf8("action_Manage_Libraries"));
        action_Property_Editor = new QAction(DirectorEditor);
        action_Property_Editor->setObjectName(QString::fromUtf8("action_Property_Editor"));
        action_Property_Editor->setCheckable(true);
        action_Property_Editor->setChecked(true);
        action_Graph_Browser = new QAction(DirectorEditor);
        action_Graph_Browser->setObjectName(QString::fromUtf8("action_Graph_Browser"));
        action_Graph_Browser->setCheckable(true);
        action_Graph_Browser->setChecked(true);
        action_Show_Links = new QAction(DirectorEditor);
        action_Show_Links->setObjectName(QString::fromUtf8("action_Show_Links"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/icons/showlinks.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Show_Links->setIcon(icon11);
        action_Hide_Links = new QAction(DirectorEditor);
        action_Hide_Links->setObjectName(QString::fromUtf8("action_Hide_Links"));
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/icons/hidelinks.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Hide_Links->setIcon(icon12);
        action_Refresh = new QAction(DirectorEditor);
        action_Refresh->setObjectName(QString::fromUtf8("action_Refresh"));
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/icons/refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Refresh->setIcon(icon13);
        action_Exit = new QAction(DirectorEditor);
        action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
        actionDirector_Help = new QAction(DirectorEditor);
        actionDirector_Help->setObjectName(QString::fromUtf8("actionDirector_Help"));
        action_Search_Browser = new QAction(DirectorEditor);
        action_Search_Browser->setObjectName(QString::fromUtf8("action_Search_Browser"));
        action_Search_Browser->setCheckable(true);
        action_Search_Browser->setEnabled(true);
        action_Search_Browser->setShortcutContext(Qt::WidgetShortcut);
        actionPause = new QAction(DirectorEditor);
        actionPause->setObjectName(QString::fromUtf8("actionPause"));
        actionPause->setEnabled(true);
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/icons/debug_pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPause->setIcon(icon14);
        actionContinue = new QAction(DirectorEditor);
        actionContinue->setObjectName(QString::fromUtf8("actionContinue"));
        actionContinue->setEnabled(true);
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/icons/debug_continue.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionContinue->setIcon(icon15);
        actionStep_Next = new QAction(DirectorEditor);
        actionStep_Next->setObjectName(QString::fromUtf8("actionStep_Next"));
        actionStep_Next->setEnabled(true);
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/icons/debug_step.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStep_Next->setIcon(icon16);
        actionPaste_with_Links = new QAction(DirectorEditor);
        actionPaste_with_Links->setObjectName(QString::fromUtf8("actionPaste_with_Links"));
        actionPaste_with_Links->setIcon(icon10);
        action_Thread_Browser = new QAction(DirectorEditor);
        action_Thread_Browser->setObjectName(QString::fromUtf8("action_Thread_Browser"));
        action_Thread_Browser->setCheckable(true);
        action_Thread_Browser->setEnabled(false);
        action_Node_Palette = new QAction(DirectorEditor);
        action_Node_Palette->setObjectName(QString::fromUtf8("action_Node_Palette"));
        action_Node_Palette->setCheckable(true);
        action_Node_Palette->setChecked(true);
        actionToggle_Break_Point = new QAction(DirectorEditor);
        actionToggle_Break_Point->setObjectName(QString::fromUtf8("actionToggle_Break_Point"));
        actionToggle_Break_Point->setCheckable(true);
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/icons/debug_break.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToggle_Break_Point->setIcon(icon17);
        action_Manage_Plugins = new QAction(DirectorEditor);
        action_Manage_Plugins->setObjectName(QString::fromUtf8("action_Manage_Plugins"));
        actionNone = new QAction(DirectorEditor);
        actionNone->setObjectName(QString::fromUtf8("actionNone"));
        action_Manage_Imported_Scripts = new QAction(DirectorEditor);
        action_Manage_Imported_Scripts->setObjectName(QString::fromUtf8("action_Manage_Imported_Scripts"));
        action_Manage_Imported_Scripts->setEnabled(true);
        centralwidget = new QWidget(DirectorEditor);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_9 = new QGridLayout(centralwidget);
        gridLayout_9->setSpacing(0);
        gridLayout_9->setContentsMargins(0, 0, 0, 0);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        graphTab = new dtDirector::GraphTabs(centralwidget);
        graphTab->setObjectName(QString::fromUtf8("graphTab"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(graphTab->sizePolicy().hasHeightForWidth());
        graphTab->setSizePolicy(sizePolicy);
        graphTab->setTabsClosable(true);
        graphTab->setMovable(true);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        graphTab->addTab(tab, QString());

        gridLayout_9->addWidget(graphTab, 0, 0, 1, 1);

        DirectorEditor->setCentralWidget(centralwidget);
        menubar = new QMenuBar(DirectorEditor);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuRecent_Files = new QMenu(menuFile);
        menuRecent_Files->setObjectName(QString::fromUtf8("menuRecent_Files"));
        menu_Edit = new QMenu(menubar);
        menu_Edit->setObjectName(QString::fromUtf8("menu_Edit"));
        menu_View = new QMenu(menubar);
        menu_View->setObjectName(QString::fromUtf8("menu_View"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuDebug = new QMenu(menubar);
        menuDebug->setObjectName(QString::fromUtf8("menuDebug"));
        menuDebug->setEnabled(true);
        DirectorEditor->setMenuBar(menubar);
        statusbar = new QStatusBar(DirectorEditor);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        DirectorEditor->setStatusBar(statusbar);
        FileToolbar = new QToolBar(DirectorEditor);
        FileToolbar->setObjectName(QString::fromUtf8("FileToolbar"));
        DirectorEditor->addToolBar(Qt::TopToolBarArea, FileToolbar);
        EditToolbar = new QToolBar(DirectorEditor);
        EditToolbar->setObjectName(QString::fromUtf8("EditToolbar"));
        DirectorEditor->addToolBar(Qt::TopToolBarArea, EditToolbar);
        propertyEditor = new dtDirector::PropertyEditor(DirectorEditor);
        propertyEditor->setObjectName(QString::fromUtf8("propertyEditor"));
        propertyEditor->setMinimumSize(QSize(80, 150));
        propertyEditor->setFeatures(QDockWidget::AllDockWidgetFeatures);
        propertyEditor->setAllowedAreas(Qt::AllDockWidgetAreas);
        DirectorEditor->addDockWidget(static_cast<Qt::DockWidgetArea>(8), propertyEditor);
        graphBrowser = new dtDirector::GraphBrowser(DirectorEditor);
        graphBrowser->setObjectName(QString::fromUtf8("graphBrowser"));
        graphBrowser->setMinimumSize(QSize(80, 150));
        graphBrowser->setFeatures(QDockWidget::AllDockWidgetFeatures);
        DirectorEditor->addDockWidget(static_cast<Qt::DockWidgetArea>(8), graphBrowser);
        searchBrowser = new dtDirector::SearchBrowser(DirectorEditor);
        searchBrowser->setObjectName(QString::fromUtf8("searchBrowser"));
        DirectorEditor->addDockWidget(static_cast<Qt::DockWidgetArea>(2), searchBrowser);
        threadBrowser = new dtDirector::ThreadBrowser(DirectorEditor);
        threadBrowser->setObjectName(QString::fromUtf8("threadBrowser"));
        DirectorEditor->addDockWidget(static_cast<Qt::DockWidgetArea>(2), threadBrowser);
        nodePalette = new QDockWidget(DirectorEditor);
        nodePalette->setObjectName(QString::fromUtf8("nodePalette"));
        nodePalette->setFeatures(QDockWidget::AllDockWidgetFeatures);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        nodeSearchEdit = new QLineEdit(dockWidgetContents);
        nodeSearchEdit->setObjectName(QString::fromUtf8("nodeSearchEdit"));
        nodeSearchEdit->setEnabled(true);

        gridLayout->addWidget(nodeSearchEdit, 0, 0, 1, 2);

        nodeTabs = new QTabWidget(dockWidgetContents);
        nodeTabs->setObjectName(QString::fromUtf8("nodeTabs"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(nodeTabs->sizePolicy().hasHeightForWidth());
        nodeTabs->setSizePolicy(sizePolicy1);
        nodeTabs->setAutoFillBackground(false);
        nodeTabs->setTabPosition(QTabWidget::West);
        nodeTabs->setUsesScrollButtons(true);
        nodeTabs->setMovable(false);
        eventNodeTab = new QWidget();
        eventNodeTab->setObjectName(QString::fromUtf8("eventNodeTab"));
        gridLayout_2 = new QGridLayout(eventNodeTab);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        eventNodeTabWidget = new dtDirector::NodeTabs(eventNodeTab);
        eventNodeTabWidget->setObjectName(QString::fromUtf8("eventNodeTabWidget"));
        eventNodeTabWidget->setAutoFillBackground(false);
        eventNodeTabWidgetPage1 = new QWidget();
        eventNodeTabWidgetPage1->setObjectName(QString::fromUtf8("eventNodeTabWidgetPage1"));
        eventNodeTabWidgetPage1->setGeometry(QRect(0, 0, 115, 379));
        eventNodeTabWidget->addItem(eventNodeTabWidgetPage1, QString::fromUtf8(""));

        gridLayout_2->addWidget(eventNodeTabWidget, 0, 0, 1, 1);

        nodeTabs->addTab(eventNodeTab, QString());
        actionNodeTab = new QWidget();
        actionNodeTab->setObjectName(QString::fromUtf8("actionNodeTab"));
        gridLayout_3 = new QGridLayout(actionNodeTab);
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        actionNodeTabWidget = new dtDirector::NodeTabs(actionNodeTab);
        actionNodeTabWidget->setObjectName(QString::fromUtf8("actionNodeTabWidget"));
        actionNodeTabWidgetPage1 = new QWidget();
        actionNodeTabWidgetPage1->setObjectName(QString::fromUtf8("actionNodeTabWidgetPage1"));
        actionNodeTabWidgetPage1->setGeometry(QRect(0, 0, 115, 379));
        actionNodeTabWidget->addItem(actionNodeTabWidgetPage1, QString::fromUtf8(""));

        gridLayout_3->addWidget(actionNodeTabWidget, 0, 0, 1, 1);

        nodeTabs->addTab(actionNodeTab, QString());
        mutatorNodeTab = new QWidget();
        mutatorNodeTab->setObjectName(QString::fromUtf8("mutatorNodeTab"));
        gridLayout_8 = new QGridLayout(mutatorNodeTab);
        gridLayout_8->setContentsMargins(0, 0, 0, 0);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        mutatorNodeTabWidget = new dtDirector::NodeTabs(mutatorNodeTab);
        mutatorNodeTabWidget->setObjectName(QString::fromUtf8("mutatorNodeTabWidget"));
        mutatorNodeTabWidgetPage1 = new QWidget();
        mutatorNodeTabWidgetPage1->setObjectName(QString::fromUtf8("mutatorNodeTabWidgetPage1"));
        mutatorNodeTabWidgetPage1->setGeometry(QRect(0, 0, 115, 379));
        mutatorNodeTabWidget->addItem(mutatorNodeTabWidgetPage1, QString::fromUtf8(""));

        gridLayout_8->addWidget(mutatorNodeTabWidget, 0, 0, 1, 1);

        nodeTabs->addTab(mutatorNodeTab, QString());
        variableNodeTab = new QWidget();
        variableNodeTab->setObjectName(QString::fromUtf8("variableNodeTab"));
        gridLayout_4 = new QGridLayout(variableNodeTab);
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        variableNodeTabWidget = new dtDirector::NodeTabs(variableNodeTab);
        variableNodeTabWidget->setObjectName(QString::fromUtf8("variableNodeTabWidget"));
        variableNodeTabWidgetPage1 = new QWidget();
        variableNodeTabWidgetPage1->setObjectName(QString::fromUtf8("variableNodeTabWidgetPage1"));
        variableNodeTabWidgetPage1->setGeometry(QRect(0, 0, 115, 379));
        variableNodeTabWidget->addItem(variableNodeTabWidgetPage1, QString::fromUtf8(""));

        gridLayout_4->addWidget(variableNodeTabWidget, 0, 0, 1, 1);

        nodeTabs->addTab(variableNodeTab, QString());
        macroNodeTab = new QWidget();
        macroNodeTab->setObjectName(QString::fromUtf8("macroNodeTab"));
        gridLayout_5 = new QGridLayout(macroNodeTab);
        gridLayout_5->setContentsMargins(0, 0, 0, 0);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        macroNodeTabWidget = new dtDirector::NodeTabs(macroNodeTab);
        macroNodeTabWidget->setObjectName(QString::fromUtf8("macroNodeTabWidget"));
        macroNodeTabWidgetPage1 = new QWidget();
        macroNodeTabWidgetPage1->setObjectName(QString::fromUtf8("macroNodeTabWidgetPage1"));
        macroNodeTabWidgetPage1->setGeometry(QRect(0, 0, 115, 379));
        macroNodeTabWidget->addItem(macroNodeTabWidgetPage1, QString::fromUtf8(""));

        gridLayout_5->addWidget(macroNodeTabWidget, 0, 0, 1, 1);

        nodeTabs->addTab(macroNodeTab, QString());
        linkNodeTab = new QWidget();
        linkNodeTab->setObjectName(QString::fromUtf8("linkNodeTab"));
        gridLayout_6 = new QGridLayout(linkNodeTab);
        gridLayout_6->setContentsMargins(0, 0, 0, 0);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        linkNodeTabWidget = new dtDirector::NodeTabs(linkNodeTab);
        linkNodeTabWidget->setObjectName(QString::fromUtf8("linkNodeTabWidget"));
        linkNodeTabWidgetPage1 = new QWidget();
        linkNodeTabWidgetPage1->setObjectName(QString::fromUtf8("linkNodeTabWidgetPage1"));
        linkNodeTabWidgetPage1->setGeometry(QRect(0, 0, 115, 379));
        linkNodeTabWidget->addItem(linkNodeTabWidgetPage1, QString::fromUtf8(""));

        gridLayout_6->addWidget(linkNodeTabWidget, 0, 0, 1, 1);

        nodeTabs->addTab(linkNodeTab, QString());
        miscNodeTab = new QWidget();
        miscNodeTab->setObjectName(QString::fromUtf8("miscNodeTab"));
        gridLayout_7 = new QGridLayout(miscNodeTab);
        gridLayout_7->setContentsMargins(0, 0, 0, 0);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        miscNodeTabWidget = new dtDirector::NodeTabs(miscNodeTab);
        miscNodeTabWidget->setObjectName(QString::fromUtf8("miscNodeTabWidget"));
        miscNodeTabWidgetPage1 = new QWidget();
        miscNodeTabWidgetPage1->setObjectName(QString::fromUtf8("miscNodeTabWidgetPage1"));
        miscNodeTabWidgetPage1->setGeometry(QRect(0, 0, 115, 379));
        miscNodeTabWidget->addItem(miscNodeTabWidgetPage1, QString::fromUtf8(""));

        gridLayout_7->addWidget(miscNodeTabWidget, 0, 0, 1, 1);

        nodeTabs->addTab(miscNodeTab, QString());
        referenceNodeTabs = new QWidget();
        referenceNodeTabs->setObjectName(QString::fromUtf8("referenceNodeTabs"));
        gridLayout_11 = new QGridLayout(referenceNodeTabs);
        gridLayout_11->setSpacing(0);
        gridLayout_11->setContentsMargins(0, 0, 0, 0);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        referenceNodeTabWidget = new dtDirector::NodeTabs(referenceNodeTabs);
        referenceNodeTabWidget->setObjectName(QString::fromUtf8("referenceNodeTabWidget"));
        referenceNodeTabWidgetPage1 = new QWidget();
        referenceNodeTabWidgetPage1->setObjectName(QString::fromUtf8("referenceNodeTabWidgetPage1"));
        referenceNodeTabWidgetPage1->setGeometry(QRect(0, 0, 115, 379));
        referenceNodeTabWidget->addItem(referenceNodeTabWidgetPage1, QString::fromUtf8(""));

        gridLayout_11->addWidget(referenceNodeTabWidget, 0, 0, 1, 1);

        nodeTabs->addTab(referenceNodeTabs, QString());
        searchNodeTab = new QWidget();
        searchNodeTab->setObjectName(QString::fromUtf8("searchNodeTab"));
        gridLayout_10 = new QGridLayout(searchNodeTab);
        gridLayout_10->setContentsMargins(0, 0, 0, 0);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        searchNodeTabWidget = new dtDirector::NodeTabs(searchNodeTab);
        searchNodeTabWidget->setObjectName(QString::fromUtf8("searchNodeTabWidget"));
        searchNodeTabWidget->setEnabled(true);
        sizePolicy1.setHeightForWidth(searchNodeTabWidget->sizePolicy().hasHeightForWidth());
        searchNodeTabWidget->setSizePolicy(sizePolicy1);
        searchNodeTabWidgetPage1 = new QWidget();
        searchNodeTabWidgetPage1->setObjectName(QString::fromUtf8("searchNodeTabWidgetPage1"));
        searchNodeTabWidgetPage1->setGeometry(QRect(0, 0, 115, 379));
        searchNodeTabWidget->addItem(searchNodeTabWidgetPage1, QString::fromUtf8(""));

        gridLayout_10->addWidget(searchNodeTabWidget, 0, 0, 1, 1);

        nodeTabs->addTab(searchNodeTab, QString());

        gridLayout->addWidget(nodeTabs, 1, 0, 1, 2);

        nodePalette->setWidget(dockWidgetContents);
        DirectorEditor->addDockWidget(static_cast<Qt::DockWidgetArea>(1), nodePalette);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menu_Edit->menuAction());
        menubar->addAction(menu_View->menuAction());
        menubar->addAction(menuDebug->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(action_New);
        menuFile->addAction(action_Load);
        menuFile->addAction(action_Save);
        menuFile->addAction(action_Save_as);
        menuFile->addSeparator();
        menuFile->addAction(menuRecent_Files->menuAction());
        menuFile->addSeparator();
        menuFile->addAction(action_Exit);
        menuRecent_Files->addAction(actionNone);
        menu_Edit->addAction(action_Step_Out_Of_Graph);
        menu_Edit->addSeparator();
        menu_Edit->addAction(action_Smart_Grid_snap);
        menu_Edit->addSeparator();
        menu_Edit->addAction(action_Undo);
        menu_Edit->addAction(action_Redo);
        menu_Edit->addSeparator();
        menu_Edit->addAction(action_Cut);
        menu_Edit->addAction(action_Copy);
        menu_Edit->addAction(action_Paste);
        menu_Edit->addAction(actionPaste_with_Links);
        menu_Edit->addSeparator();
        menu_Edit->addAction(action_Delete);
        menu_Edit->addSeparator();
        menu_Edit->addAction(action_Manage_Plugins);
        menu_Edit->addAction(action_Manage_Libraries);
        menu_Edit->addAction(action_Manage_Imported_Scripts);
        menu_View->addAction(action_Node_Palette);
        menu_View->addAction(action_Search_Browser);
        menu_View->addAction(action_Property_Editor);
        menu_View->addAction(action_Graph_Browser);
        menu_View->addAction(action_Thread_Browser);
        menu_View->addSeparator();
        menu_View->addAction(action_Show_Links);
        menu_View->addAction(action_Hide_Links);
        menu_View->addSeparator();
        menu_View->addAction(action_Refresh);
        menuHelp->addAction(actionDirector_Help);
        menuDebug->addAction(actionPause);
        menuDebug->addAction(actionContinue);
        menuDebug->addAction(actionStep_Next);
        menuDebug->addSeparator();
        menuDebug->addAction(actionToggle_Break_Point);
        FileToolbar->addAction(action_New);
        FileToolbar->addAction(action_Load);
        FileToolbar->addAction(action_Save);
        EditToolbar->addAction(action_Step_Out_Of_Graph);
        EditToolbar->addSeparator();
        EditToolbar->addAction(action_Smart_Grid_snap);
        EditToolbar->addSeparator();
        EditToolbar->addAction(action_Undo);
        EditToolbar->addAction(action_Redo);
        EditToolbar->addSeparator();
        EditToolbar->addAction(action_Cut);
        EditToolbar->addAction(action_Copy);
        EditToolbar->addAction(action_Paste);
        EditToolbar->addSeparator();
        EditToolbar->addAction(action_Delete);
        EditToolbar->addSeparator();
        EditToolbar->addAction(action_Show_Links);
        EditToolbar->addAction(action_Hide_Links);
        EditToolbar->addSeparator();
        EditToolbar->addAction(action_Refresh);
        EditToolbar->addSeparator();
        EditToolbar->addAction(actionPause);
        EditToolbar->addAction(actionContinue);
        EditToolbar->addAction(actionStep_Next);
        EditToolbar->addAction(actionToggle_Break_Point);

        retranslateUi(DirectorEditor);

        nodeTabs->setCurrentIndex(0);
        eventNodeTabWidget->layout()->setSpacing(0);
        actionNodeTabWidget->layout()->setSpacing(0);
        mutatorNodeTabWidget->setCurrentIndex(0);
        mutatorNodeTabWidget->layout()->setSpacing(0);
        variableNodeTabWidget->layout()->setSpacing(0);
        macroNodeTabWidget->layout()->setSpacing(0);
        linkNodeTabWidget->layout()->setSpacing(0);
        miscNodeTabWidget->layout()->setSpacing(0);
        referenceNodeTabWidget->setCurrentIndex(0);
        referenceNodeTabWidget->layout()->setSpacing(0);
        searchNodeTabWidget->layout()->setSpacing(0);


        QMetaObject::connectSlotsByName(DirectorEditor);
    } // setupUi

    void retranslateUi(QMainWindow *DirectorEditor)
    {
        DirectorEditor->setWindowTitle(QApplication::translate("DirectorEditor", "No Director Graph Loaded", 0, QApplication::UnicodeUTF8));
        action_New->setText(QApplication::translate("DirectorEditor", "&New", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_New->setToolTip(QApplication::translate("DirectorEditor", "Begins a new Director script (Ctrl+N).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_New->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+N", 0, QApplication::UnicodeUTF8));
        action_Load->setText(QApplication::translate("DirectorEditor", "&Load", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Load->setToolTip(QApplication::translate("DirectorEditor", "Loads a Director script from a file (Ctrl+L).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Load->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+L", 0, QApplication::UnicodeUTF8));
        action_Save->setText(QApplication::translate("DirectorEditor", "&Save", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Save->setToolTip(QApplication::translate("DirectorEditor", "Saves the current Director script (Ctrl+S).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Save->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        action_Save_as->setText(QApplication::translate("DirectorEditor", "&Save as...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Save_as->setToolTip(QApplication::translate("DirectorEditor", "Saves the current Director script.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Step_Out_Of_Graph->setText(QApplication::translate("DirectorEditor", "Step &out of Graph", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Step_Out_Of_Graph->setToolTip(QApplication::translate("DirectorEditor", "Returns to the parent graph (Ctrl+Shift+U).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Step_Out_Of_Graph->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+Shift+U", 0, QApplication::UnicodeUTF8));
        action_Smart_Grid_snap->setText(QApplication::translate("DirectorEditor", "Smart &Grid Snap", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Smart_Grid_snap->setToolTip(QApplication::translate("DirectorEditor", "Snaps nodes to a smart grid determined by the placement of other nodes (Ctrl+G).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Smart_Grid_snap->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+G", 0, QApplication::UnicodeUTF8));
        action_Undo->setText(QApplication::translate("DirectorEditor", "&Undo", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Undo->setToolTip(QApplication::translate("DirectorEditor", "Reverts to your last action (Ctrl+Z).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Undo->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+Z", 0, QApplication::UnicodeUTF8));
        action_Redo->setText(QApplication::translate("DirectorEditor", "&Redo", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Redo->setToolTip(QApplication::translate("DirectorEditor", "Reverts your last undo action (Ctrl+Y).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Redo->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+Y", 0, QApplication::UnicodeUTF8));
        action_Delete->setText(QApplication::translate("DirectorEditor", "&Delete", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Delete->setToolTip(QApplication::translate("DirectorEditor", "Reverts your last undo action (Delete).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Delete->setShortcut(QApplication::translate("DirectorEditor", "Del", 0, QApplication::UnicodeUTF8));
        action_Cut->setText(QApplication::translate("DirectorEditor", "&Cut", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Cut->setToolTip(QApplication::translate("DirectorEditor", "Cuts the currently selected nodes to the clipboard (Ctrl+X).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Cut->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+X", 0, QApplication::UnicodeUTF8));
        action_Copy->setText(QApplication::translate("DirectorEditor", "&Copy", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Copy->setToolTip(QApplication::translate("DirectorEditor", "Copies the currently selected nodes to the clipboard (Ctrl+C).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Copy->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+C", 0, QApplication::UnicodeUTF8));
        action_Paste->setText(QApplication::translate("DirectorEditor", "&Paste", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Paste->setToolTip(QApplication::translate("DirectorEditor", "Pastes the nodes saved in the clipboard to the current graph (Ctrl+V).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Paste->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+V", 0, QApplication::UnicodeUTF8));
        action_Manage_Libraries->setText(QApplication::translate("DirectorEditor", "&Manage Libraries...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Manage_Libraries->setToolTip(QApplication::translate("DirectorEditor", "Manages the Node Libraries for the script.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Property_Editor->setText(QApplication::translate("DirectorEditor", "&Property Editor", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Property_Editor->setToolTip(QApplication::translate("DirectorEditor", "Shows the Property Editor (Ctrl+P).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Property_Editor->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        action_Graph_Browser->setText(QApplication::translate("DirectorEditor", "&Macro Browser", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Graph_Browser->setToolTip(QApplication::translate("DirectorEditor", "Shows the Graph Browser (Ctrl+B).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Graph_Browser->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+B", 0, QApplication::UnicodeUTF8));
        action_Show_Links->setText(QApplication::translate("DirectorEditor", "&Show All Links", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Show_Links->setToolTip(QApplication::translate("DirectorEditor", "Shows all hidden links on selected nodes (Ctrl+U).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Show_Links->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+U", 0, QApplication::UnicodeUTF8));
        action_Hide_Links->setText(QApplication::translate("DirectorEditor", "&Hide All Links", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Hide_Links->setToolTip(QApplication::translate("DirectorEditor", "Hides all unused links on selected nodes (Ctrl+H).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Hide_Links->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+H", 0, QApplication::UnicodeUTF8));
        action_Refresh->setText(QApplication::translate("DirectorEditor", "&Refresh", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Refresh->setToolTip(QApplication::translate("DirectorEditor", "Refresh the current view (F5).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Refresh->setShortcut(QApplication::translate("DirectorEditor", "F5", 0, QApplication::UnicodeUTF8));
        action_Exit->setText(QApplication::translate("DirectorEditor", "Exit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Exit->setToolTip(QApplication::translate("DirectorEditor", "Exit the editor", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Exit->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionDirector_Help->setText(QApplication::translate("DirectorEditor", "Director Help...", 0, QApplication::UnicodeUTF8));
        action_Search_Browser->setText(QApplication::translate("DirectorEditor", "&Search Browser", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Search_Browser->setToolTip(QApplication::translate("DirectorEditor", "Shows the Search Browser (Ctrl+F).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Search_Browser->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+F", 0, QApplication::UnicodeUTF8));
        actionPause->setText(QApplication::translate("DirectorEditor", "Pause", 0, QApplication::UnicodeUTF8));
        actionContinue->setText(QApplication::translate("DirectorEditor", "Continue", 0, QApplication::UnicodeUTF8));
        actionContinue->setShortcut(QApplication::translate("DirectorEditor", "F5", 0, QApplication::UnicodeUTF8));
        actionStep_Next->setText(QApplication::translate("DirectorEditor", "Step Next", 0, QApplication::UnicodeUTF8));
        actionStep_Next->setShortcut(QApplication::translate("DirectorEditor", "F10", 0, QApplication::UnicodeUTF8));
        actionPaste_with_Links->setText(QApplication::translate("DirectorEditor", "Paste with Links", 0, QApplication::UnicodeUTF8));
        actionPaste_with_Links->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+Shift+V", 0, QApplication::UnicodeUTF8));
        action_Thread_Browser->setText(QApplication::translate("DirectorEditor", "&Thread Browser", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Thread_Browser->setToolTip(QApplication::translate("DirectorEditor", "Shows the Thread Browser (Ctrl+T).", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Thread_Browser->setShortcut(QApplication::translate("DirectorEditor", "Ctrl+T", 0, QApplication::UnicodeUTF8));
        action_Node_Palette->setText(QApplication::translate("DirectorEditor", "Node Palette", 0, QApplication::UnicodeUTF8));
        actionToggle_Break_Point->setText(QApplication::translate("DirectorEditor", "Toggle Break Point", 0, QApplication::UnicodeUTF8));
        actionToggle_Break_Point->setShortcut(QApplication::translate("DirectorEditor", "F9", 0, QApplication::UnicodeUTF8));
        action_Manage_Plugins->setText(QApplication::translate("DirectorEditor", "Manage Plugins...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Manage_Plugins->setToolTip(QApplication::translate("DirectorEditor", "Manage the Plugins for the script.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionNone->setText(QApplication::translate("DirectorEditor", "<None>", 0, QApplication::UnicodeUTF8));
        action_Manage_Imported_Scripts->setText(QApplication::translate("DirectorEditor", "Manage Imported Scripts...", 0, QApplication::UnicodeUTF8));
        graphTab->setTabText(graphTab->indexOf(tab), QApplication::translate("DirectorEditor", "Tab 1", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("DirectorEditor", "&File", 0, QApplication::UnicodeUTF8));
        menuRecent_Files->setTitle(QApplication::translate("DirectorEditor", "Recent Files", 0, QApplication::UnicodeUTF8));
        menu_Edit->setTitle(QApplication::translate("DirectorEditor", "&Edit", 0, QApplication::UnicodeUTF8));
        menu_View->setTitle(QApplication::translate("DirectorEditor", "&View", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("DirectorEditor", "Help", 0, QApplication::UnicodeUTF8));
        menuDebug->setTitle(QApplication::translate("DirectorEditor", "Debug", 0, QApplication::UnicodeUTF8));
        FileToolbar->setWindowTitle(QApplication::translate("DirectorEditor", "File Toolbar", 0, QApplication::UnicodeUTF8));
        EditToolbar->setWindowTitle(QApplication::translate("DirectorEditor", "Edit Toolbar", 0, QApplication::UnicodeUTF8));
        nodePalette->setWindowTitle(QApplication::translate("DirectorEditor", "Node Palette", 0, QApplication::UnicodeUTF8));
        eventNodeTabWidget->setItemText(eventNodeTabWidget->indexOf(eventNodeTabWidgetPage1), QString());
        nodeTabs->setTabText(nodeTabs->indexOf(eventNodeTab), QApplication::translate("DirectorEditor", "Events", 0, QApplication::UnicodeUTF8));
        actionNodeTabWidget->setItemText(actionNodeTabWidget->indexOf(actionNodeTabWidgetPage1), QString());
        nodeTabs->setTabText(nodeTabs->indexOf(actionNodeTab), QApplication::translate("DirectorEditor", "Actions", 0, QApplication::UnicodeUTF8));
        mutatorNodeTabWidget->setItemText(mutatorNodeTabWidget->indexOf(mutatorNodeTabWidgetPage1), QString());
        nodeTabs->setTabText(nodeTabs->indexOf(mutatorNodeTab), QApplication::translate("DirectorEditor", "Mutators", 0, QApplication::UnicodeUTF8));
        variableNodeTabWidget->setItemText(variableNodeTabWidget->indexOf(variableNodeTabWidgetPage1), QString());
        nodeTabs->setTabText(nodeTabs->indexOf(variableNodeTab), QApplication::translate("DirectorEditor", "Variables", 0, QApplication::UnicodeUTF8));
        macroNodeTabWidget->setItemText(macroNodeTabWidget->indexOf(macroNodeTabWidgetPage1), QString());
        nodeTabs->setTabText(nodeTabs->indexOf(macroNodeTab), QApplication::translate("DirectorEditor", "Macros", 0, QApplication::UnicodeUTF8));
        linkNodeTabWidget->setItemText(linkNodeTabWidget->indexOf(linkNodeTabWidgetPage1), QString());
        nodeTabs->setTabText(nodeTabs->indexOf(linkNodeTab), QApplication::translate("DirectorEditor", "Links", 0, QApplication::UnicodeUTF8));
        miscNodeTabWidget->setItemText(miscNodeTabWidget->indexOf(miscNodeTabWidgetPage1), QString());
        nodeTabs->setTabText(nodeTabs->indexOf(miscNodeTab), QApplication::translate("DirectorEditor", "Misc", 0, QApplication::UnicodeUTF8));
        referenceNodeTabWidget->setItemText(referenceNodeTabWidget->indexOf(referenceNodeTabWidgetPage1), QString());
        nodeTabs->setTabText(nodeTabs->indexOf(referenceNodeTabs), QApplication::translate("DirectorEditor", "References", 0, QApplication::UnicodeUTF8));
        searchNodeTabWidget->setItemText(searchNodeTabWidget->indexOf(searchNodeTabWidgetPage1), QString());
        nodeTabs->setTabText(nodeTabs->indexOf(searchNodeTab), QApplication::translate("DirectorEditor", "Search", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DirectorEditor: public Ui_DirectorEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIRECTOREDITOR_H
