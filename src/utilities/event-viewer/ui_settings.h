/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Settings
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QToolBox *page_settings;
    QWidget *page_TOF0;
    QVBoxLayout *verticalLayout_6;
    QRadioButton *radio_tof0_offsetsFromMAUS;
    QRadioButton *radio_tof0_customOffsets;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer;
    QDoubleSpinBox *tof0_zPosition;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer_2;
    QDoubleSpinBox *tof0_xOffset;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QSpacerItem *horizontalSpacer_3;
    QDoubleSpinBox *tof0_yOffset;
    QWidget *page_TOF1;
    QVBoxLayout *verticalLayout_7;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *radio_tof1_offsetsFromMAUS;
    QRadioButton *radio_tof1_customOffsets;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QSpacerItem *horizontalSpacer_4;
    QDoubleSpinBox *tof1_zPosition;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_5;
    QDoubleSpinBox *tof1_xOffset;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QSpacerItem *horizontalSpacer_6;
    QDoubleSpinBox *tof1_yOffset;
    QWidget *page_TKU;
    QVBoxLayout *verticalLayout_8;
    QRadioButton *radio_tku_offsetsFromMAUS;
    QRadioButton *radio_tku_customOffsets;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_8;
    QSpacerItem *horizontalSpacer_7;
    QDoubleSpinBox *tku_zOffset;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_9;
    QSpacerItem *horizontalSpacer_8;
    QDoubleSpinBox *tku_xOffset;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_10;
    QSpacerItem *horizontalSpacer_9;
    QDoubleSpinBox *tku_yOffset;
    QWidget *page_TKD;
    QVBoxLayout *verticalLayout_9;
    QRadioButton *radio_tkd_offsetsFromMAUS;
    QVBoxLayout *verticalLayout_4;
    QRadioButton *radio_tkd_customOffsets;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_11;
    QSpacerItem *horizontalSpacer_10;
    QDoubleSpinBox *tkd_zOffset;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_12;
    QSpacerItem *horizontalSpacer_11;
    QDoubleSpinBox *tkd_xOffset;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_13;
    QSpacerItem *horizontalSpacer_12;
    QDoubleSpinBox *tkd_yOffset;
    QWidget *page_TOF2;
    QVBoxLayout *verticalLayout_10;
    QRadioButton *radio_tof2_offsetsFromMAUS;
    QVBoxLayout *verticalLayout_5;
    QRadioButton *radio_tof2_customOffsets;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_14;
    QSpacerItem *horizontalSpacer_13;
    QDoubleSpinBox *tof2_zPosition;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_15;
    QSpacerItem *horizontalSpacer_14;
    QDoubleSpinBox *tof2_xOffset;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_16;
    QSpacerItem *horizontalSpacer_15;
    QDoubleSpinBox *tof2_yOffset;
    QWidget *page_general;
    QVBoxLayout *verticalLayout_11;
    QLabel *label_17;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_18;
    QSpinBox *int_spillChunkSize;

    void setupUi(QDialog *Settings)
    {
        if (Settings->objectName().isEmpty())
            Settings->setObjectName(QStringLiteral("Settings"));
        Settings->resize(425, 460);
        buttonBox = new QDialogButtonBox(Settings);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(330, 20, 81, 381));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(Settings);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 101, 31));
        QFont font;
        font.setPointSize(18);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        page_settings = new QToolBox(Settings);
        page_settings->setObjectName(QStringLiteral("page_settings"));
        page_settings->setGeometry(QRect(10, 60, 281, 391));
        page_TOF0 = new QWidget();
        page_TOF0->setObjectName(QStringLiteral("page_TOF0"));
        page_TOF0->setGeometry(QRect(0, 0, 281, 205));
        verticalLayout_6 = new QVBoxLayout(page_TOF0);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        radio_tof0_offsetsFromMAUS = new QRadioButton(page_TOF0);
        radio_tof0_offsetsFromMAUS->setObjectName(QStringLiteral("radio_tof0_offsetsFromMAUS"));
        radio_tof0_offsetsFromMAUS->setEnabled(false);

        verticalLayout_6->addWidget(radio_tof0_offsetsFromMAUS);

        radio_tof0_customOffsets = new QRadioButton(page_TOF0);
        radio_tof0_customOffsets->setObjectName(QStringLiteral("radio_tof0_customOffsets"));
        radio_tof0_customOffsets->setChecked(true);

        verticalLayout_6->addWidget(radio_tof0_customOffsets);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_2 = new QLabel(page_TOF0);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        tof0_zPosition = new QDoubleSpinBox(page_TOF0);
        tof0_zPosition->setObjectName(QStringLiteral("tof0_zPosition"));
        tof0_zPosition->setMaximum(25000);
        tof0_zPosition->setSingleStep(0.5);
        tof0_zPosition->setValue(0);

        horizontalLayout->addWidget(tof0_zPosition);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new QLabel(page_TOF0);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_2->addWidget(label_3);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        tof0_xOffset = new QDoubleSpinBox(page_TOF0);
        tof0_xOffset->setObjectName(QStringLiteral("tof0_xOffset"));
        tof0_xOffset->setMinimum(-500);
        tof0_xOffset->setMaximum(500);

        horizontalLayout_2->addWidget(tof0_xOffset);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_4 = new QLabel(page_TOF0);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_3->addWidget(label_4);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        tof0_yOffset = new QDoubleSpinBox(page_TOF0);
        tof0_yOffset->setObjectName(QStringLiteral("tof0_yOffset"));
        tof0_yOffset->setMinimum(-500);
        tof0_yOffset->setMaximum(500);

        horizontalLayout_3->addWidget(tof0_yOffset);


        verticalLayout->addLayout(horizontalLayout_3);


        verticalLayout_6->addLayout(verticalLayout);

        page_settings->addItem(page_TOF0, QStringLiteral("TOF0"));
        page_TOF1 = new QWidget();
        page_TOF1->setObjectName(QStringLiteral("page_TOF1"));
        page_TOF1->setGeometry(QRect(0, 0, 281, 205));
        verticalLayout_7 = new QVBoxLayout(page_TOF1);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        radio_tof1_offsetsFromMAUS = new QRadioButton(page_TOF1);
        radio_tof1_offsetsFromMAUS->setObjectName(QStringLiteral("radio_tof1_offsetsFromMAUS"));
        radio_tof1_offsetsFromMAUS->setEnabled(false);

        verticalLayout_2->addWidget(radio_tof1_offsetsFromMAUS);

        radio_tof1_customOffsets = new QRadioButton(page_TOF1);
        radio_tof1_customOffsets->setObjectName(QStringLiteral("radio_tof1_customOffsets"));
        radio_tof1_customOffsets->setChecked(true);

        verticalLayout_2->addWidget(radio_tof1_customOffsets);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_5 = new QLabel(page_TOF1);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_4->addWidget(label_5);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        tof1_zPosition = new QDoubleSpinBox(page_TOF1);
        tof1_zPosition->setObjectName(QStringLiteral("tof1_zPosition"));
        tof1_zPosition->setMaximum(25000);
        tof1_zPosition->setSingleStep(0.5);
        tof1_zPosition->setValue(0);

        horizontalLayout_4->addWidget(tof1_zPosition);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_6 = new QLabel(page_TOF1);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_5->addWidget(label_6);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);

        tof1_xOffset = new QDoubleSpinBox(page_TOF1);
        tof1_xOffset->setObjectName(QStringLiteral("tof1_xOffset"));
        tof1_xOffset->setMinimum(-500);
        tof1_xOffset->setMaximum(500);

        horizontalLayout_5->addWidget(tof1_xOffset);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_7 = new QLabel(page_TOF1);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_6->addWidget(label_7);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);

        tof1_yOffset = new QDoubleSpinBox(page_TOF1);
        tof1_yOffset->setObjectName(QStringLiteral("tof1_yOffset"));
        tof1_yOffset->setMinimum(-500);
        tof1_yOffset->setMaximum(500);

        horizontalLayout_6->addWidget(tof1_yOffset);


        verticalLayout_2->addLayout(horizontalLayout_6);


        verticalLayout_7->addLayout(verticalLayout_2);

        page_settings->addItem(page_TOF1, QStringLiteral("TOF1"));
        page_TKU = new QWidget();
        page_TKU->setObjectName(QStringLiteral("page_TKU"));
        page_TKU->setGeometry(QRect(0, 0, 205, 175));
        verticalLayout_8 = new QVBoxLayout(page_TKU);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        radio_tku_offsetsFromMAUS = new QRadioButton(page_TKU);
        radio_tku_offsetsFromMAUS->setObjectName(QStringLiteral("radio_tku_offsetsFromMAUS"));
        radio_tku_offsetsFromMAUS->setEnabled(false);

        verticalLayout_8->addWidget(radio_tku_offsetsFromMAUS);

        radio_tku_customOffsets = new QRadioButton(page_TKU);
        radio_tku_customOffsets->setObjectName(QStringLiteral("radio_tku_customOffsets"));
        radio_tku_customOffsets->setChecked(true);

        verticalLayout_8->addWidget(radio_tku_customOffsets);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        label_8 = new QLabel(page_TKU);
        label_8->setObjectName(QStringLiteral("label_8"));

        horizontalLayout_7->addWidget(label_8);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_7);

        tku_zOffset = new QDoubleSpinBox(page_TKU);
        tku_zOffset->setObjectName(QStringLiteral("tku_zOffset"));
        tku_zOffset->setMaximum(0);
        tku_zOffset->setSingleStep(0.5);
        tku_zOffset->setValue(0);

        horizontalLayout_7->addWidget(tku_zOffset);


        verticalLayout_3->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        label_9 = new QLabel(page_TKU);
        label_9->setObjectName(QStringLiteral("label_9"));

        horizontalLayout_8->addWidget(label_9);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_8);

        tku_xOffset = new QDoubleSpinBox(page_TKU);
        tku_xOffset->setObjectName(QStringLiteral("tku_xOffset"));
        tku_xOffset->setMinimum(-500);
        tku_xOffset->setMaximum(500);

        horizontalLayout_8->addWidget(tku_xOffset);


        verticalLayout_3->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        label_10 = new QLabel(page_TKU);
        label_10->setObjectName(QStringLiteral("label_10"));

        horizontalLayout_9->addWidget(label_10);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_9);

        tku_yOffset = new QDoubleSpinBox(page_TKU);
        tku_yOffset->setObjectName(QStringLiteral("tku_yOffset"));
        tku_yOffset->setMinimum(-500);
        tku_yOffset->setMaximum(500);

        horizontalLayout_9->addWidget(tku_yOffset);


        verticalLayout_3->addLayout(horizontalLayout_9);


        verticalLayout_8->addLayout(verticalLayout_3);

        page_settings->addItem(page_TKU, QStringLiteral("Upstream Tracker"));
        page_TKD = new QWidget();
        page_TKD->setObjectName(QStringLiteral("page_TKD"));
        page_TKD->setGeometry(QRect(0, 0, 217, 175));
        verticalLayout_9 = new QVBoxLayout(page_TKD);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        radio_tkd_offsetsFromMAUS = new QRadioButton(page_TKD);
        radio_tkd_offsetsFromMAUS->setObjectName(QStringLiteral("radio_tkd_offsetsFromMAUS"));
        radio_tkd_offsetsFromMAUS->setEnabled(false);

        verticalLayout_9->addWidget(radio_tkd_offsetsFromMAUS);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        radio_tkd_customOffsets = new QRadioButton(page_TKD);
        radio_tkd_customOffsets->setObjectName(QStringLiteral("radio_tkd_customOffsets"));
        radio_tkd_customOffsets->setChecked(true);

        verticalLayout_4->addWidget(radio_tkd_customOffsets);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        label_11 = new QLabel(page_TKD);
        label_11->setObjectName(QStringLiteral("label_11"));

        horizontalLayout_10->addWidget(label_11);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_10);

        tkd_zOffset = new QDoubleSpinBox(page_TKD);
        tkd_zOffset->setObjectName(QStringLiteral("tkd_zOffset"));
        tkd_zOffset->setMaximum(25000);
        tkd_zOffset->setSingleStep(0.5);
        tkd_zOffset->setValue(0);

        horizontalLayout_10->addWidget(tkd_zOffset);


        verticalLayout_4->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        label_12 = new QLabel(page_TKD);
        label_12->setObjectName(QStringLiteral("label_12"));

        horizontalLayout_11->addWidget(label_12);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_11);

        tkd_xOffset = new QDoubleSpinBox(page_TKD);
        tkd_xOffset->setObjectName(QStringLiteral("tkd_xOffset"));
        tkd_xOffset->setMinimum(-500);
        tkd_xOffset->setMaximum(500);

        horizontalLayout_11->addWidget(tkd_xOffset);


        verticalLayout_4->addLayout(horizontalLayout_11);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        label_13 = new QLabel(page_TKD);
        label_13->setObjectName(QStringLiteral("label_13"));

        horizontalLayout_12->addWidget(label_13);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_12);

        tkd_yOffset = new QDoubleSpinBox(page_TKD);
        tkd_yOffset->setObjectName(QStringLiteral("tkd_yOffset"));
        tkd_yOffset->setMinimum(-500);
        tkd_yOffset->setMaximum(500);

        horizontalLayout_12->addWidget(tkd_yOffset);


        verticalLayout_4->addLayout(horizontalLayout_12);


        verticalLayout_9->addLayout(verticalLayout_4);

        page_settings->addItem(page_TKD, QStringLiteral("Downstream Tracker"));
        page_TOF2 = new QWidget();
        page_TOF2->setObjectName(QStringLiteral("page_TOF2"));
        page_TOF2->setGeometry(QRect(0, 0, 281, 205));
        verticalLayout_10 = new QVBoxLayout(page_TOF2);
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        radio_tof2_offsetsFromMAUS = new QRadioButton(page_TOF2);
        radio_tof2_offsetsFromMAUS->setObjectName(QStringLiteral("radio_tof2_offsetsFromMAUS"));
        radio_tof2_offsetsFromMAUS->setEnabled(false);

        verticalLayout_10->addWidget(radio_tof2_offsetsFromMAUS);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        radio_tof2_customOffsets = new QRadioButton(page_TOF2);
        radio_tof2_customOffsets->setObjectName(QStringLiteral("radio_tof2_customOffsets"));
        radio_tof2_customOffsets->setChecked(true);

        verticalLayout_5->addWidget(radio_tof2_customOffsets);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        label_14 = new QLabel(page_TOF2);
        label_14->setObjectName(QStringLiteral("label_14"));

        horizontalLayout_13->addWidget(label_14);

        horizontalSpacer_13 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_13);

        tof2_zPosition = new QDoubleSpinBox(page_TOF2);
        tof2_zPosition->setObjectName(QStringLiteral("tof2_zPosition"));
        tof2_zPosition->setMaximum(25000);
        tof2_zPosition->setSingleStep(0.5);
        tof2_zPosition->setValue(0);

        horizontalLayout_13->addWidget(tof2_zPosition);


        verticalLayout_5->addLayout(horizontalLayout_13);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
        label_15 = new QLabel(page_TOF2);
        label_15->setObjectName(QStringLiteral("label_15"));

        horizontalLayout_14->addWidget(label_15);

        horizontalSpacer_14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer_14);

        tof2_xOffset = new QDoubleSpinBox(page_TOF2);
        tof2_xOffset->setObjectName(QStringLiteral("tof2_xOffset"));
        tof2_xOffset->setMinimum(-500);
        tof2_xOffset->setMaximum(500);

        horizontalLayout_14->addWidget(tof2_xOffset);


        verticalLayout_5->addLayout(horizontalLayout_14);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        label_16 = new QLabel(page_TOF2);
        label_16->setObjectName(QStringLiteral("label_16"));

        horizontalLayout_15->addWidget(label_16);

        horizontalSpacer_15 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_15->addItem(horizontalSpacer_15);

        tof2_yOffset = new QDoubleSpinBox(page_TOF2);
        tof2_yOffset->setObjectName(QStringLiteral("tof2_yOffset"));
        tof2_yOffset->setMinimum(-500);
        tof2_yOffset->setMaximum(500);

        horizontalLayout_15->addWidget(tof2_yOffset);


        verticalLayout_5->addLayout(horizontalLayout_15);


        verticalLayout_10->addLayout(verticalLayout_5);

        page_settings->addItem(page_TOF2, QStringLiteral("TOF2"));
        page_general = new QWidget();
        page_general->setObjectName(QStringLiteral("page_general"));
        page_general->setGeometry(QRect(0, 0, 281, 205));
        verticalLayout_11 = new QVBoxLayout(page_general);
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        label_17 = new QLabel(page_general);
        label_17->setObjectName(QStringLiteral("label_17"));

        verticalLayout_11->addWidget(label_17);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
        label_18 = new QLabel(page_general);
        label_18->setObjectName(QStringLiteral("label_18"));

        horizontalLayout_16->addWidget(label_18);

        int_spillChunkSize = new QSpinBox(page_general);
        int_spillChunkSize->setObjectName(QStringLiteral("int_spillChunkSize"));
        int_spillChunkSize->setMaximum(10000);
        int_spillChunkSize->setValue(100);

        horizontalLayout_16->addWidget(int_spillChunkSize);


        verticalLayout_11->addLayout(horizontalLayout_16);

        page_settings->addItem(page_general, QStringLiteral("Spills"));

        retranslateUi(Settings);
        QObject::connect(buttonBox, SIGNAL(accepted()), Settings, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Settings, SLOT(reject()));

        page_settings->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(Settings);
    } // setupUi

    void retranslateUi(QDialog *Settings)
    {
        Settings->setWindowTitle(QApplication::translate("Settings", "Dialog", 0));
        label->setText(QApplication::translate("Settings", "Settings", 0));
        radio_tof0_offsetsFromMAUS->setText(QApplication::translate("Settings", "Position from MAUS", 0));
        radio_tof0_customOffsets->setText(QApplication::translate("Settings", "Custom Position", 0));
        label_2->setText(QApplication::translate("Settings", "z Position:", 0));
        tof0_zPosition->setSuffix(QApplication::translate("Settings", " mm", 0));
        label_3->setText(QApplication::translate("Settings", "x Offset:", 0));
        tof0_xOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        label_4->setText(QApplication::translate("Settings", "y Offset:", 0));
        tof0_yOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        page_settings->setItemText(page_settings->indexOf(page_TOF0), QApplication::translate("Settings", "TOF0", 0));
        radio_tof1_offsetsFromMAUS->setText(QApplication::translate("Settings", "Position from MAUS", 0));
        radio_tof1_customOffsets->setText(QApplication::translate("Settings", "Custom Position", 0));
        label_5->setText(QApplication::translate("Settings", "z Position:", 0));
        tof1_zPosition->setSuffix(QApplication::translate("Settings", " mm", 0));
        label_6->setText(QApplication::translate("Settings", "x Offset:", 0));
        tof1_xOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        label_7->setText(QApplication::translate("Settings", "y Offset:", 0));
        tof1_yOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        page_settings->setItemText(page_settings->indexOf(page_TOF1), QApplication::translate("Settings", "TOF1", 0));
        radio_tku_offsetsFromMAUS->setText(QApplication::translate("Settings", "Position from MAUS", 0));
        radio_tku_customOffsets->setText(QApplication::translate("Settings", "Custom Position", 0));
        label_8->setText(QApplication::translate("Settings", "z Offset:", 0));
        tku_zOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        label_9->setText(QApplication::translate("Settings", "x Offset:", 0));
        tku_xOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        label_10->setText(QApplication::translate("Settings", "y Offset:", 0));
        tku_yOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        page_settings->setItemText(page_settings->indexOf(page_TKU), QApplication::translate("Settings", "Upstream Tracker", 0));
        radio_tkd_offsetsFromMAUS->setText(QApplication::translate("Settings", "Position from MAUS", 0));
        radio_tkd_customOffsets->setText(QApplication::translate("Settings", "Custom Position", 0));
        label_11->setText(QApplication::translate("Settings", "z Offset:", 0));
        tkd_zOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        label_12->setText(QApplication::translate("Settings", "x Offset:", 0));
        tkd_xOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        label_13->setText(QApplication::translate("Settings", "y Offset:", 0));
        tkd_yOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        page_settings->setItemText(page_settings->indexOf(page_TKD), QApplication::translate("Settings", "Downstream Tracker", 0));
        radio_tof2_offsetsFromMAUS->setText(QApplication::translate("Settings", "Position from MAUS", 0));
        radio_tof2_customOffsets->setText(QApplication::translate("Settings", "Custom Position", 0));
        label_14->setText(QApplication::translate("Settings", "z Position:", 0));
        tof2_zPosition->setSuffix(QApplication::translate("Settings", " mm", 0));
        label_15->setText(QApplication::translate("Settings", "x Offset:", 0));
        tof2_xOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        label_16->setText(QApplication::translate("Settings", "y Offset:", 0));
        tof2_yOffset->setSuffix(QApplication::translate("Settings", " mm", 0));
        page_settings->setItemText(page_settings->indexOf(page_TOF2), QApplication::translate("Settings", "TOF2", 0));
        label_17->setText(QApplication::translate("Settings", "<html><head/><body><p align=\"justify\">'Spills to read' is the number of spills <br/>read in one go from the MAUS output<br/>file. Decreasing this number may <br/>increase performance.</p><p align=\"justify\">When a spill is selected outside of the<br/>current spill range, the next section <br/>of spills is loaded into memory for<br/>display.</p></body></html>", 0));
        label_18->setText(QApplication::translate("Settings", "Spills to read:", 0));
        page_settings->setItemText(page_settings->indexOf(page_general), QApplication::translate("Settings", "Spills", 0));
    } // retranslateUi

};

namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
