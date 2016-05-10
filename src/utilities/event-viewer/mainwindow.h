#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "readmaus.h"
#include <QString>
#include <QPen>
#include <QFont>
#include "settings.h"
#include <QTextStream>

#include <DataStructure/Data.hh>

#include "EVExporter.h"
#include "EVEvent.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void next_spill();
    void previous_spill();
    void previous_spill_end();
    void next_event();
    void previous_event();
    void choose_event();
    void choose_spill();
    void choose_open_file();
    void choose_open_detgeom_file();
    void choose_output_dir();
    void open_settings();
    void check_NoHits();
    void check_TOF0();
    void check_TOF1();
    void check_UST();
    void check_DST();
    void check_TOF2();
    void check_HRExport();
    void check_HRDisplay();
    void choose_java_dir();
    void choose_heprapp_dir();

private:
    Ui::MainWindow *ui;
    Settings* settings_window;

    void closeEvent(QCloseEvent *event);

    void setup_ui();

    QString filename;
    int spillNumber, eventNumber;
    QString spillLabel, eventLabel;

    QString inputDirName;
    QString detGeomFileName, detGeomDirName;
    QString outputDirName;
    QString javaDirName;
    QString heprappDirName;

    QString initFile;
    void LoadInitValues();
    void SaveInitValues();

    double dummyValue;
    int eventSelection;
    int eventWindow;
    long minSpill, maxSpill;
    long long totalEvents;
    int displayOpt;
    std::string exportOpt;
    char* homeDir;

    EVEvent evEvent;
    EVExporter *exp;
    QMap<int, double> eventSpillRatio;

    void replot();

    int ReadEvent();
    double GetRatio(int spillNum);
    long long GetNumberOfEvents(QString filename);
    void SetSpillRange(long long totalEvents);
    size_t GetMaxEventNumber(int spillNum);
    void PushValue(QVector<double> &out, QVector<double> in, double controlVal);
    int InputFileIsOpen();

    void read_settings();
    void plot_settings();
    void position_plots();
    void momentum_plots();
    void time_plots();
    void tracker_plots();
};

#endif // MAINWINDOW_H
