#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QVector>
#include <TMath.h>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();
    QVector<double> GetTOF0Settings();
    QVector<double> GetTOF1Settings();
    QVector<double> GetTKUSettings();
    QVector<double> GetTKDSettings();
    QVector<double> GetTOF2Settings();

    int GetSpillRange();



public slots:
    void select_tof0_settings();
    void select_tof1_settings();
    void select_tof2_settings();
    void select_tku_settings();
    void select_tkd_settings();

private:
    Ui::Settings *ui;
    void setup_ui();
};

#endif // SETTINGS_H
