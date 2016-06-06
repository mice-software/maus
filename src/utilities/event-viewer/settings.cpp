#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
}

Settings::~Settings()
{
    delete ui;
}


QVector<double> Settings::GetTOF0Settings(){
    QVector<double> values;
    if(ui->radio_tof0_customOffsets->isChecked()){
        values << ui->tof0_xOffset->value() << ui->tof0_yOffset->value() << ui->tof0_zPosition->value();
    }
    else{
        values << 0.0 << 0.0 << 0.0;
    }
    return values;
}

QVector<double> Settings::GetTOF1Settings(){
    QVector<double> values;
    if(ui->radio_tof1_customOffsets->isChecked()){
        values << ui->tof1_xOffset->value() << ui->tof1_yOffset->value() << ui->tof1_zPosition->value();
    }
    else{
        values << 0.0 << 0.0 << 0.0;
    }
    return values;
}

QVector<double> Settings::GetTOF2Settings(){
    QVector<double> values;
    if(ui->radio_tof2_customOffsets->isChecked()){
        values << ui->tof2_xOffset->value() << ui->tof2_yOffset->value() << ui->tof2_zPosition->value();
    }
    else{
        values << 0.0 << 0.0 << 0.0;
    }
    return values;
}

QVector<double> Settings::GetTKUSettings(){
    QVector<double> values;
    if(ui->radio_tku_customOffsets->isChecked()){
        values << ui->tku_xOffset->value() << ui->tku_yOffset->value() << ui->tku_zOffset->value();
    }
    else{
        values << 0.0 << 0.0 << 0.0;
    }
    return values;
}

QVector<double> Settings::GetTKDSettings(){
    QVector<double> values;
    if(ui->radio_tkd_customOffsets->isChecked()){
        values << ui->tkd_xOffset->value() << ui->tkd_yOffset->value() << ui->tkd_zOffset->value();
    }
    else{
        values << 0.0 << 0.0 << 0.0;
    }
    return values;
}

int Settings::GetSpillRange(){
    return ui->int_spillChunkSize->value();
}

void Settings::setup_ui(){
    connect(ui->radio_tkd_customOffsets, SIGNAL(clicked()), SLOT(select_tkd_settings()));
    connect(ui->radio_tkd_offsetsFromMAUS, SIGNAL(clicked()), SLOT(select_tkd_settings()));

    connect(ui->radio_tku_customOffsets, SIGNAL(clicked()), SLOT(select_tku_settings()));
    connect(ui->radio_tku_offsetsFromMAUS, SIGNAL(clicked()), SLOT(select_tku_settings()));

    connect(ui->radio_tof0_customOffsets, SIGNAL(clicked()), SLOT(select_tof0_settings()));
    connect(ui->radio_tof0_offsetsFromMAUS, SIGNAL(clicked()), SLOT(select_tof0_settings()));

    connect(ui->radio_tof1_customOffsets, SIGNAL(clicked()), SLOT(select_tof1_settings()));
    connect(ui->radio_tof1_offsetsFromMAUS, SIGNAL(clicked()), SLOT(select_tof1_settings()));

    connect(ui->radio_tof2_customOffsets, SIGNAL(clicked()), SLOT(select_tof2_settings()));
    connect(ui->radio_tof2_offsetsFromMAUS, SIGNAL(clicked()), SLOT(select_tof2_settings()));
}

void Settings::select_tkd_settings(){
    if(ui->radio_tkd_customOffsets->isChecked()){
        ui->tkd_xOffset->setEnabled(true);
        ui->tkd_yOffset->setEnabled(true);
        ui->tkd_zOffset->setEnabled(true);
    }
    else{
        ui->tkd_xOffset->setEnabled(false);
        ui->tkd_yOffset->setEnabled(false);
        ui->tkd_zOffset->setEnabled(false);
    }
}

void Settings::select_tku_settings(){
    if(ui->radio_tku_customOffsets->isChecked()){
        ui->tku_xOffset->setEnabled(true);
        ui->tku_yOffset->setEnabled(true);
        ui->tku_zOffset->setEnabled(true);
    }
    else{
        ui->tku_xOffset->setEnabled(false);
        ui->tku_yOffset->setEnabled(false);
        ui->tku_zOffset->setEnabled(false);
    }
}

void Settings::select_tof0_settings(){
    if(ui->radio_tof0_customOffsets->isChecked()){
        ui->tof0_xOffset->setEnabled(true);
        ui->tof0_yOffset->setEnabled(true);
        ui->tof0_zPosition->setEnabled(true);
    }
    else{
        ui->tof0_xOffset->setEnabled(false);
        ui->tof0_yOffset->setEnabled(false);
        ui->tof0_zPosition->setEnabled(false);
    }
}

void Settings::select_tof1_settings(){
    if(ui->radio_tof1_customOffsets->isChecked()){
        ui->tof1_xOffset->setEnabled(true);
        ui->tof1_yOffset->setEnabled(true);
        ui->tof1_zPosition->setEnabled(true);
    }
    else{
        ui->tof1_xOffset->setEnabled(false);
        ui->tof1_yOffset->setEnabled(false);
        ui->tof1_zPosition->setEnabled(false);
    }
}

void Settings::select_tof2_settings(){
    if(ui->radio_tof2_customOffsets->isChecked()){
        ui->tof2_xOffset->setEnabled(true);
        ui->tof2_yOffset->setEnabled(true);
        ui->tof2_zPosition->setEnabled(true);
    }
    else{
        ui->tof2_xOffset->setEnabled(false);
        ui->tof2_yOffset->setEnabled(false);
        ui->tof2_zPosition->setEnabled(false);
    }
}
