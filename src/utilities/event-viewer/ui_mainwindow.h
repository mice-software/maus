/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *line_inputFile;
    QToolButton *btn_inputFile;
    QFrame *line_3;
    QPushButton *btn_settings;
    QTabWidget *tabs_changePlot;
    QWidget *tab_position;
    QVBoxLayout *verticalLayout;
    QCustomPlot *plot_position_xz;
    QCustomPlot *plot_position_yz;
    QWidget *tab_momentum;
    QVBoxLayout *verticalLayout_3;
    QCustomPlot *plot_momentum_t;
    QCustomPlot *plot_momentum_z;
    QWidget *tab_time;
    QVBoxLayout *verticalLayout_4;
    QCustomPlot *plot_tof0_to_tof1;
    QCustomPlot *plot_tof0_to_tof2;
    QCustomPlot *plot_tof1_to_tof2;
    QWidget *tab_tracker;
    QVBoxLayout *verticalLayout_5;
    QCustomPlot *plot_scifi_cluster_position_az;
    QTabWidget *tabWidget;
    QWidget *tab_Navigation;
    QGridLayout *gridLayout_3;
    QGroupBox *gb_Navigation;
    QGridLayout *gridLayout;
    QLabel *label_spillNumber;
    QLabel *label_spill;
    QLabel *label_GoToSpill;
    QSpinBox *int_goToSpill;
    QPushButton *btn_previousEvent;
    QSpinBox *int_goToEvent;
    QPushButton *btn_previousSpill;
    QLabel *label_GoToEvent;
    QPushButton *btn_nextSpill;
    QPushButton *btn_nextEvent;
    QLabel *label_eventNumber;
    QLabel *label_event;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *horizontalSpacer_5;
    QGroupBox *gb_EventSelection;
    QHBoxLayout *horizontalLayout_9;
    QCheckBox *chk_NoHits;
    QCheckBox *chk_TOF0;
    QCheckBox *chk_TOF1;
    QCheckBox *chk_UST;
    QCheckBox *chk_DST;
    QCheckBox *chk_TOF2;
    QWidget *tab_HeprepOptions;
    QGridLayout *gridLayout_4;
    QGroupBox *gb_HepRepSection;
    QGridLayout *gridLayout_2;
    QLineEdit *line_OutputDir;
    QLabel *lbl_HeprappDir;
    QCheckBox *chk_DisplayHepRApp;
    QLabel *lbl_GeometryFile;
    QCheckBox *chk_HepRepExport;
    QToolButton *btn_HeprappDir;
    QLabel *lbl_JavaDir;
    QLabel *lbl_OutputDir;
    QToolButton *btn_GeometryFile;
    QLineEdit *line_GeometryFile;
    QToolButton *btn_OutputDir;
    QToolButton *btn_JavaDir;
    QLineEdit *line_JavaDir;
    QLineEdit *line_HeprappDir;
    QMenuBar *menuBar;
    QMenu *menuSettings;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1023, 700);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout_2 = new QVBoxLayout(centralWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        line_inputFile = new QLineEdit(centralWidget);
        line_inputFile->setObjectName(QStringLiteral("line_inputFile"));

        horizontalLayout->addWidget(line_inputFile);

        btn_inputFile = new QToolButton(centralWidget);
        btn_inputFile->setObjectName(QStringLiteral("btn_inputFile"));

        horizontalLayout->addWidget(btn_inputFile);

        line_3 = new QFrame(centralWidget);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(line_3);

        btn_settings = new QPushButton(centralWidget);
        btn_settings->setObjectName(QStringLiteral("btn_settings"));

        horizontalLayout->addWidget(btn_settings);


        verticalLayout_2->addLayout(horizontalLayout);

        tabs_changePlot = new QTabWidget(centralWidget);
        tabs_changePlot->setObjectName(QStringLiteral("tabs_changePlot"));
        tab_position = new QWidget();
        tab_position->setObjectName(QStringLiteral("tab_position"));
        verticalLayout = new QVBoxLayout(tab_position);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        plot_position_xz = new QCustomPlot(tab_position);
        plot_position_xz->setObjectName(QStringLiteral("plot_position_xz"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(plot_position_xz->sizePolicy().hasHeightForWidth());
        plot_position_xz->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(plot_position_xz);

        plot_position_yz = new QCustomPlot(tab_position);
        plot_position_yz->setObjectName(QStringLiteral("plot_position_yz"));
        sizePolicy.setHeightForWidth(plot_position_yz->sizePolicy().hasHeightForWidth());
        plot_position_yz->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(plot_position_yz);

        tabs_changePlot->addTab(tab_position, QString());
        tab_momentum = new QWidget();
        tab_momentum->setObjectName(QStringLiteral("tab_momentum"));
        verticalLayout_3 = new QVBoxLayout(tab_momentum);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        plot_momentum_t = new QCustomPlot(tab_momentum);
        plot_momentum_t->setObjectName(QStringLiteral("plot_momentum_t"));
        sizePolicy.setHeightForWidth(plot_momentum_t->sizePolicy().hasHeightForWidth());
        plot_momentum_t->setSizePolicy(sizePolicy);

        verticalLayout_3->addWidget(plot_momentum_t);

        plot_momentum_z = new QCustomPlot(tab_momentum);
        plot_momentum_z->setObjectName(QStringLiteral("plot_momentum_z"));
        sizePolicy.setHeightForWidth(plot_momentum_z->sizePolicy().hasHeightForWidth());
        plot_momentum_z->setSizePolicy(sizePolicy);

        verticalLayout_3->addWidget(plot_momentum_z);

        tabs_changePlot->addTab(tab_momentum, QString());
        tab_time = new QWidget();
        tab_time->setObjectName(QStringLiteral("tab_time"));
        verticalLayout_4 = new QVBoxLayout(tab_time);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        plot_tof0_to_tof1 = new QCustomPlot(tab_time);
        plot_tof0_to_tof1->setObjectName(QStringLiteral("plot_tof0_to_tof1"));
        sizePolicy.setHeightForWidth(plot_tof0_to_tof1->sizePolicy().hasHeightForWidth());
        plot_tof0_to_tof1->setSizePolicy(sizePolicy);

        verticalLayout_4->addWidget(plot_tof0_to_tof1);

        plot_tof0_to_tof2 = new QCustomPlot(tab_time);
        plot_tof0_to_tof2->setObjectName(QStringLiteral("plot_tof0_to_tof2"));
        sizePolicy.setHeightForWidth(plot_tof0_to_tof2->sizePolicy().hasHeightForWidth());
        plot_tof0_to_tof2->setSizePolicy(sizePolicy);

        verticalLayout_4->addWidget(plot_tof0_to_tof2);

        plot_tof1_to_tof2 = new QCustomPlot(tab_time);
        plot_tof1_to_tof2->setObjectName(QStringLiteral("plot_tof1_to_tof2"));
        sizePolicy.setHeightForWidth(plot_tof1_to_tof2->sizePolicy().hasHeightForWidth());
        plot_tof1_to_tof2->setSizePolicy(sizePolicy);

        verticalLayout_4->addWidget(plot_tof1_to_tof2);

        tabs_changePlot->addTab(tab_time, QString());
        tab_tracker = new QWidget();
        tab_tracker->setObjectName(QStringLiteral("tab_tracker"));
        verticalLayout_5 = new QVBoxLayout(tab_tracker);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        plot_scifi_cluster_position_az = new QCustomPlot(tab_tracker);
        plot_scifi_cluster_position_az->setObjectName(QStringLiteral("plot_scifi_cluster_position_az"));
        sizePolicy.setHeightForWidth(plot_scifi_cluster_position_az->sizePolicy().hasHeightForWidth());
        plot_scifi_cluster_position_az->setSizePolicy(sizePolicy);

        verticalLayout_5->addWidget(plot_scifi_cluster_position_az);

        tabs_changePlot->addTab(tab_tracker, QString());

        verticalLayout_2->addWidget(tabs_changePlot);

        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy1);
        tabWidget->setMinimumSize(QSize(600, 220));
        tab_Navigation = new QWidget();
        tab_Navigation->setObjectName(QStringLiteral("tab_Navigation"));
        gridLayout_3 = new QGridLayout(tab_Navigation);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gb_Navigation = new QGroupBox(tab_Navigation);
        gb_Navigation->setObjectName(QStringLiteral("gb_Navigation"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(gb_Navigation->sizePolicy().hasHeightForWidth());
        gb_Navigation->setSizePolicy(sizePolicy2);
        gridLayout = new QGridLayout(gb_Navigation);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_spillNumber = new QLabel(gb_Navigation);
        label_spillNumber->setObjectName(QStringLiteral("label_spillNumber"));

        gridLayout->addWidget(label_spillNumber, 0, 2, 1, 1);

        label_spill = new QLabel(gb_Navigation);
        label_spill->setObjectName(QStringLiteral("label_spill"));

        gridLayout->addWidget(label_spill, 0, 1, 1, 1);

        label_GoToSpill = new QLabel(gb_Navigation);
        label_GoToSpill->setObjectName(QStringLiteral("label_GoToSpill"));

        gridLayout->addWidget(label_GoToSpill, 0, 5, 1, 1);

        int_goToSpill = new QSpinBox(gb_Navigation);
        int_goToSpill->setObjectName(QStringLiteral("int_goToSpill"));
        int_goToSpill->setMaximum(100000);

        gridLayout->addWidget(int_goToSpill, 0, 6, 1, 1);

        btn_previousEvent = new QPushButton(gb_Navigation);
        btn_previousEvent->setObjectName(QStringLiteral("btn_previousEvent"));

        gridLayout->addWidget(btn_previousEvent, 1, 0, 1, 1);

        int_goToEvent = new QSpinBox(gb_Navigation);
        int_goToEvent->setObjectName(QStringLiteral("int_goToEvent"));

        gridLayout->addWidget(int_goToEvent, 1, 6, 1, 1);

        btn_previousSpill = new QPushButton(gb_Navigation);
        btn_previousSpill->setObjectName(QStringLiteral("btn_previousSpill"));

        gridLayout->addWidget(btn_previousSpill, 0, 0, 1, 1);

        label_GoToEvent = new QLabel(gb_Navigation);
        label_GoToEvent->setObjectName(QStringLiteral("label_GoToEvent"));

        gridLayout->addWidget(label_GoToEvent, 1, 5, 1, 1);

        btn_nextSpill = new QPushButton(gb_Navigation);
        btn_nextSpill->setObjectName(QStringLiteral("btn_nextSpill"));

        gridLayout->addWidget(btn_nextSpill, 0, 3, 1, 1);

        btn_nextEvent = new QPushButton(gb_Navigation);
        btn_nextEvent->setObjectName(QStringLiteral("btn_nextEvent"));

        gridLayout->addWidget(btn_nextEvent, 1, 3, 1, 1);

        label_eventNumber = new QLabel(gb_Navigation);
        label_eventNumber->setObjectName(QStringLiteral("label_eventNumber"));

        gridLayout->addWidget(label_eventNumber, 1, 2, 1, 1);

        label_event = new QLabel(gb_Navigation);
        label_event->setObjectName(QStringLiteral("label_event"));

        gridLayout->addWidget(label_event, 1, 1, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_6, 1, 4, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_5, 0, 4, 1, 1);

        btn_previousSpill->raise();
        label_spill->raise();
        label_spillNumber->raise();
        btn_nextSpill->raise();
        label_GoToSpill->raise();
        int_goToSpill->raise();
        btn_previousEvent->raise();
        label_event->raise();
        label_eventNumber->raise();
        btn_nextEvent->raise();
        label_GoToEvent->raise();
        int_goToEvent->raise();

        gridLayout_3->addWidget(gb_Navigation, 0, 0, 1, 1);

        gb_EventSelection = new QGroupBox(tab_Navigation);
        gb_EventSelection->setObjectName(QStringLiteral("gb_EventSelection"));
        sizePolicy2.setHeightForWidth(gb_EventSelection->sizePolicy().hasHeightForWidth());
        gb_EventSelection->setSizePolicy(sizePolicy2);
        horizontalLayout_9 = new QHBoxLayout(gb_EventSelection);
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        chk_NoHits = new QCheckBox(gb_EventSelection);
        chk_NoHits->setObjectName(QStringLiteral("chk_NoHits"));

        horizontalLayout_9->addWidget(chk_NoHits);

        chk_TOF0 = new QCheckBox(gb_EventSelection);
        chk_TOF0->setObjectName(QStringLiteral("chk_TOF0"));

        horizontalLayout_9->addWidget(chk_TOF0);

        chk_TOF1 = new QCheckBox(gb_EventSelection);
        chk_TOF1->setObjectName(QStringLiteral("chk_TOF1"));

        horizontalLayout_9->addWidget(chk_TOF1);

        chk_UST = new QCheckBox(gb_EventSelection);
        chk_UST->setObjectName(QStringLiteral("chk_UST"));

        horizontalLayout_9->addWidget(chk_UST);

        chk_DST = new QCheckBox(gb_EventSelection);
        chk_DST->setObjectName(QStringLiteral("chk_DST"));

        horizontalLayout_9->addWidget(chk_DST);

        chk_TOF2 = new QCheckBox(gb_EventSelection);
        chk_TOF2->setObjectName(QStringLiteral("chk_TOF2"));

        horizontalLayout_9->addWidget(chk_TOF2);


        gridLayout_3->addWidget(gb_EventSelection, 1, 0, 1, 1);

        tabWidget->addTab(tab_Navigation, QString());
        tab_HeprepOptions = new QWidget();
        tab_HeprepOptions->setObjectName(QStringLiteral("tab_HeprepOptions"));
        gridLayout_4 = new QGridLayout(tab_HeprepOptions);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gb_HepRepSection = new QGroupBox(tab_HeprepOptions);
        gb_HepRepSection->setObjectName(QStringLiteral("gb_HepRepSection"));
        sizePolicy2.setHeightForWidth(gb_HepRepSection->sizePolicy().hasHeightForWidth());
        gb_HepRepSection->setSizePolicy(sizePolicy2);
        gb_HepRepSection->setMinimumSize(QSize(0, 0));
        gridLayout_2 = new QGridLayout(gb_HepRepSection);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        line_OutputDir = new QLineEdit(gb_HepRepSection);
        line_OutputDir->setObjectName(QStringLiteral("line_OutputDir"));
        line_OutputDir->setMinimumSize(QSize(0, 0));

        gridLayout_2->addWidget(line_OutputDir, 1, 5, 1, 2);

        lbl_HeprappDir = new QLabel(gb_HepRepSection);
        lbl_HeprappDir->setObjectName(QStringLiteral("lbl_HeprappDir"));
        lbl_HeprappDir->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(lbl_HeprappDir, 3, 4, 1, 1, Qt::AlignRight);

        chk_DisplayHepRApp = new QCheckBox(gb_HepRepSection);
        chk_DisplayHepRApp->setObjectName(QStringLiteral("chk_DisplayHepRApp"));

        gridLayout_2->addWidget(chk_DisplayHepRApp, 2, 0, 1, 1);

        lbl_GeometryFile = new QLabel(gb_HepRepSection);
        lbl_GeometryFile->setObjectName(QStringLiteral("lbl_GeometryFile"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(lbl_GeometryFile->sizePolicy().hasHeightForWidth());
        lbl_GeometryFile->setSizePolicy(sizePolicy3);
        lbl_GeometryFile->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(lbl_GeometryFile, 1, 0, 1, 1);

        chk_HepRepExport = new QCheckBox(gb_HepRepSection);
        chk_HepRepExport->setObjectName(QStringLiteral("chk_HepRepExport"));

        gridLayout_2->addWidget(chk_HepRepExport, 0, 0, 1, 1);

        btn_HeprappDir = new QToolButton(gb_HepRepSection);
        btn_HeprappDir->setObjectName(QStringLiteral("btn_HeprappDir"));

        gridLayout_2->addWidget(btn_HeprappDir, 3, 10, 1, 1);

        lbl_JavaDir = new QLabel(gb_HepRepSection);
        lbl_JavaDir->setObjectName(QStringLiteral("lbl_JavaDir"));
        sizePolicy3.setHeightForWidth(lbl_JavaDir->sizePolicy().hasHeightForWidth());
        lbl_JavaDir->setSizePolicy(sizePolicy3);
        lbl_JavaDir->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(lbl_JavaDir, 3, 0, 1, 1, Qt::AlignRight);

        lbl_OutputDir = new QLabel(gb_HepRepSection);
        lbl_OutputDir->setObjectName(QStringLiteral("lbl_OutputDir"));
        sizePolicy3.setHeightForWidth(lbl_OutputDir->sizePolicy().hasHeightForWidth());
        lbl_OutputDir->setSizePolicy(sizePolicy3);
        lbl_OutputDir->setLayoutDirection(Qt::LeftToRight);
        lbl_OutputDir->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(lbl_OutputDir, 1, 4, 1, 1, Qt::AlignRight);

        btn_GeometryFile = new QToolButton(gb_HepRepSection);
        btn_GeometryFile->setObjectName(QStringLiteral("btn_GeometryFile"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(btn_GeometryFile->sizePolicy().hasHeightForWidth());
        btn_GeometryFile->setSizePolicy(sizePolicy4);

        gridLayout_2->addWidget(btn_GeometryFile, 1, 3, 1, 1);

        line_GeometryFile = new QLineEdit(gb_HepRepSection);
        line_GeometryFile->setObjectName(QStringLiteral("line_GeometryFile"));
        line_GeometryFile->setMinimumSize(QSize(0, 0));

        gridLayout_2->addWidget(line_GeometryFile, 1, 1, 1, 2);

        btn_OutputDir = new QToolButton(gb_HepRepSection);
        btn_OutputDir->setObjectName(QStringLiteral("btn_OutputDir"));
        sizePolicy4.setHeightForWidth(btn_OutputDir->sizePolicy().hasHeightForWidth());
        btn_OutputDir->setSizePolicy(sizePolicy4);

        gridLayout_2->addWidget(btn_OutputDir, 1, 10, 1, 1);

        btn_JavaDir = new QToolButton(gb_HepRepSection);
        btn_JavaDir->setObjectName(QStringLiteral("btn_JavaDir"));

        gridLayout_2->addWidget(btn_JavaDir, 3, 3, 1, 1);

        line_JavaDir = new QLineEdit(gb_HepRepSection);
        line_JavaDir->setObjectName(QStringLiteral("line_JavaDir"));
        line_JavaDir->setMinimumSize(QSize(0, 0));

        gridLayout_2->addWidget(line_JavaDir, 3, 1, 1, 2);

        line_HeprappDir = new QLineEdit(gb_HepRepSection);
        line_HeprappDir->setObjectName(QStringLiteral("line_HeprappDir"));
        line_HeprappDir->setMinimumSize(QSize(0, 0));

        gridLayout_2->addWidget(line_HeprappDir, 3, 5, 1, 2);


        gridLayout_4->addWidget(gb_HepRepSection, 0, 0, 1, 1);

        tabWidget->addTab(tab_HeprepOptions, QString());

        verticalLayout_2->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1023, 25));
        menuSettings = new QMenu(menuBar);
        menuSettings->setObjectName(QStringLiteral("menuSettings"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);

        menuBar->addAction(menuSettings->menuAction());

        retranslateUi(MainWindow);

        tabs_changePlot->setCurrentIndex(0);
        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        label->setText(QApplication::translate("MainWindow", "Input MAUS file:", 0));
        line_inputFile->setText(QString());
        btn_inputFile->setText(QApplication::translate("MainWindow", "...", 0));
        btn_settings->setText(QApplication::translate("MainWindow", "Settings", 0));
        tabs_changePlot->setTabText(tabs_changePlot->indexOf(tab_position), QApplication::translate("MainWindow", "Position", 0));
        tabs_changePlot->setTabText(tabs_changePlot->indexOf(tab_momentum), QApplication::translate("MainWindow", "Momentum", 0));
        tabs_changePlot->setTabText(tabs_changePlot->indexOf(tab_time), QApplication::translate("MainWindow", "Time", 0));
        tabs_changePlot->setTabText(tabs_changePlot->indexOf(tab_tracker), QApplication::translate("MainWindow", "Tracker Clusters", 0));
        gb_Navigation->setTitle(QApplication::translate("MainWindow", "Navigation", 0));
        label_spillNumber->setText(QApplication::translate("MainWindow", "0", 0));
        label_spill->setText(QApplication::translate("MainWindow", "Spill ", 0));
        label_GoToSpill->setText(QApplication::translate("MainWindow", "Go to spill:", 0));
        btn_previousEvent->setText(QApplication::translate("MainWindow", "Previous Event", 0));
        btn_previousSpill->setText(QApplication::translate("MainWindow", "Previous Spill", 0));
        label_GoToEvent->setText(QApplication::translate("MainWindow", "Go to event:", 0));
        btn_nextSpill->setText(QApplication::translate("MainWindow", "Next Spill", 0));
        btn_nextEvent->setText(QApplication::translate("MainWindow", "Next Event", 0));
        label_eventNumber->setText(QApplication::translate("MainWindow", "0", 0));
        label_event->setText(QApplication::translate("MainWindow", "Event ", 0));
        gb_EventSelection->setTitle(QApplication::translate("MainWindow", "Event selection (if no boxes are checked all events are displayed)", 0));
        chk_NoHits->setText(QApplication::translate("MainWindow", "No Hits", 0));
        chk_TOF0->setText(QApplication::translate("MainWindow", "TOF 0", 0));
        chk_TOF1->setText(QApplication::translate("MainWindow", "TOF 1", 0));
        chk_UST->setText(QApplication::translate("MainWindow", "US Tracker", 0));
        chk_DST->setText(QApplication::translate("MainWindow", "DS Tracker", 0));
        chk_TOF2->setText(QApplication::translate("MainWindow", "TOF 2", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_Navigation), QApplication::translate("MainWindow", "Navigation and event selection", 0));
        gb_HepRepSection->setTitle(QApplication::translate("MainWindow", "HepRep export options", 0));
        lbl_HeprappDir->setText(QApplication::translate("MainWindow", "HepRApp directory:", 0));
        chk_DisplayHepRApp->setText(QApplication::translate("MainWindow", "Display in HepRApp", 0));
        lbl_GeometryFile->setText(QApplication::translate("MainWindow", "Detector geometry file:", 0));
        chk_HepRepExport->setText(QApplication::translate("MainWindow", "Export to HepRep", 0));
        btn_HeprappDir->setText(QApplication::translate("MainWindow", "...", 0));
        lbl_JavaDir->setText(QApplication::translate("MainWindow", "Java directory (JRE home):", 0));
        lbl_OutputDir->setText(QApplication::translate("MainWindow", "Output directory: ", 0));
        btn_GeometryFile->setText(QApplication::translate("MainWindow", "...", 0));
        btn_OutputDir->setText(QApplication::translate("MainWindow", "...", 0));
        btn_JavaDir->setText(QApplication::translate("MainWindow", "...", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_HeprepOptions), QApplication::translate("MainWindow", "HepRep export options", 0));
        menuSettings->setTitle(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
