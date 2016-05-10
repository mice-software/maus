#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "HepRepXMLWriter.h"

#include "JsonCppStreamer/IRStream.hh"

#include <sstream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_ui();

    filename = "";
    detGeomFileName = "";
    eventSelection = 0;
    exportOpt = "no_exp";
    displayOpt = 0;
    dummyValue = 5673390;

    initFile = QApplication::applicationDirPath() + "/ev.ini";
    std::cout << initFile.toStdString() << std::endl;
    LoadInitValues();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    SaveInitValues();
    event->accept();
}

void MainWindow::setup_ui(){
    spillNumber = 0;
    eventNumber = 0;

    connect(ui->btn_nextEvent, SIGNAL(clicked()), SLOT(next_event()));
    connect(ui->btn_nextSpill, SIGNAL(clicked()), SLOT(next_spill()));
    connect(ui->int_goToSpill, SIGNAL(valueChanged(int)), SLOT(choose_spill()));

    connect(ui->btn_previousEvent, SIGNAL(clicked()), SLOT(previous_event()));
    connect(ui->btn_previousSpill, SIGNAL(clicked()), SLOT(previous_spill_end())); //sub this later if previous_spill() is updated with alt next_event()
    connect(ui->int_goToEvent, SIGNAL(valueChanged(int)), SLOT(choose_event()));

    connect(ui->btn_inputFile, SIGNAL(clicked()), SLOT(choose_open_file()));

    connect(ui->btn_settings, SIGNAL(clicked()), SLOT(open_settings()));

    connect(ui->btn_GeometryFile, SIGNAL(clicked()), SLOT(choose_open_detgeom_file()));
    connect(ui->btn_OutputDir, SIGNAL(clicked()), SLOT(choose_output_dir()));

    connect(ui->chk_NoHits, SIGNAL(stateChanged(int)), SLOT(check_NoHits()));
    connect(ui->chk_TOF0, SIGNAL(stateChanged(int)), SLOT(check_TOF0()));
    connect(ui->chk_TOF1, SIGNAL(stateChanged(int)), SLOT(check_TOF1()));
    connect(ui->chk_UST, SIGNAL(stateChanged(int)), SLOT(check_UST()));
    connect(ui->chk_DST, SIGNAL(stateChanged(int)), SLOT(check_DST()));
    connect(ui->chk_TOF2, SIGNAL(stateChanged(int)), SLOT(check_TOF2()));

    connect(ui->chk_HepRepExport, SIGNAL(stateChanged(int)), SLOT(check_HRExport()));
    connect(ui->chk_DisplayHepRApp, SIGNAL(stateChanged(int)), SLOT(check_HRDisplay()));

    connect(ui->btn_JavaDir, SIGNAL(clicked()), SLOT(choose_java_dir()));
    connect(ui->btn_HeprappDir, SIGNAL(clicked()), SLOT(choose_heprapp_dir()));


    settings_window = new Settings();
    read_settings();
    plot_settings();
}

void MainWindow::LoadInitValues()
{
    QSettings settings(initFile, QSettings::NativeFormat);

    inputDirName = settings.value("inputDirName", "").toString();
    detGeomDirName = settings.value("detGeomDirName", "").toString();
    outputDirName = settings.value("outputDirName", "").toString();
    javaDirName = settings.value("javaDirName", "").toString();
    heprappDirName = settings.value("heprappDirName", "").toString();

}

void MainWindow::SaveInitValues()
{
    QSettings settings(initFile, QSettings::NativeFormat);

    QDir dir = QFileInfo(filename).absoluteDir();
    inputDirName = dir.absolutePath();

    dir = QFileInfo(detGeomFileName).absoluteDir();
    detGeomDirName = dir.absolutePath();

    settings.setValue("inputDirName", inputDirName);
    settings.setValue("detGeomDirName", detGeomDirName);
    settings.setValue("outputDirName", outputDirName);
    settings.setValue("javaDirName", javaDirName);
    settings.setValue("heprappDirName", heprappDirName);
}

void MainWindow::open_settings(){
    settings_window->exec();
    read_settings();
}

void MainWindow::check_NoHits()
{
    if (ui->chk_NoHits->isChecked()) {
        eventSelection += 1;
        ui->chk_TOF0->setChecked(false);
        ui->chk_TOF1->setChecked(false);
        ui->chk_TOF2->setChecked(false);
        ui->chk_UST->setChecked(false);
        ui->chk_DST->setChecked(false);
    }
    else
        eventSelection -= 1;
}

void MainWindow::check_TOF0()
{
    if (ui->chk_TOF0->isChecked()) {
        eventSelection += 2;
        ui->chk_NoHits->setChecked(false);
    }
    else
        eventSelection -= 2;
}

void MainWindow::check_TOF1()
{
    if (ui->chk_TOF1->isChecked()) {
        eventSelection += 4;
        ui->chk_NoHits->setChecked(false);
    }
    else
        eventSelection -= 4;
}

void MainWindow::check_UST()
{
    if (ui->chk_UST->isChecked()) {
        eventSelection += 8;
        ui->chk_NoHits->setChecked(false);
    }
    else
        eventSelection -= 8;
}

void MainWindow::check_DST()
{
    if (ui->chk_DST->isChecked()) {
        eventSelection += 16;
        ui->chk_NoHits->setChecked(false);
    }
    else
        eventSelection -= 16;
}

void MainWindow::check_TOF2()
{
    if (ui->chk_TOF2->isChecked()) {
        eventSelection += 32;
        ui->chk_NoHits->setChecked(false);
    }
    else
        eventSelection -= 32;
}

void MainWindow::check_HRExport()
{
    if (ui->chk_HepRepExport->isChecked())
        exportOpt = "exp";
    else
        exportOpt = "no_exp";
}

void MainWindow::check_HRDisplay()
{
    if (!ui->chk_HepRepExport->isChecked()) {
        QMessageBox msgBox;
        msgBox.setText("Export to HepRep option needs to be checked first!");
        msgBox.exec();
        ui->chk_DisplayHepRApp->setChecked(false);
        return;
    }


    if (ui->chk_DisplayHepRApp->isChecked())
        displayOpt = 1;
    else
        displayOpt = 0;
}

void MainWindow::choose_java_dir()
{
    QStringList filenames;
    QFileDialog dialog(this);
    dialog.setDirectory(javaDirName);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);
    if(dialog.exec()){
        filenames = dialog.selectedFiles();
        if(!filenames.empty()) {
            javaDirName = filenames.first();
            ui->line_JavaDir->setText(javaDirName);
        }
    }

    setenv("EV_JAVA_DIR", javaDirName.toStdString().c_str(), 1);
}

void MainWindow::choose_heprapp_dir()
{
    QStringList filenames;
    QFileDialog dialog(this);
    dialog.setDirectory(heprappDirName);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);
    if(dialog.exec()){
        filenames = dialog.selectedFiles();
        if(!filenames.empty()) {
            heprappDirName = filenames.first();
            ui->line_HeprappDir->setText(heprappDirName);
        }
    }

    setenv("EV_HEPRAPP_DIR", heprappDirName.toStdString().c_str(), 1);
}

void MainWindow::read_settings(){
    QVector<double> tof0_location = settings_window->GetTOF0Settings();
    QVector<double> tof1_location = settings_window->GetTOF1Settings();
    QVector<double> tku_location = settings_window->GetTKUSettings();
    QVector<double> tkd_location = settings_window->GetTKDSettings();
    QVector<double> tof2_location = settings_window->GetTOF2Settings();
    int spillRange = settings_window->GetSpillRange();

    //read_data->SetDetectorPositions(tof0_location, tof1_location, tku_location, tkd_location, tof2_location); //what is this for?
    //read_data->SetSpillRange(spillRange);
}

void MainWindow::choose_open_file(){
    QStringList filenames;
    QFileDialog dialog(this);
    dialog.setDirectory(inputDirName);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setNameFilter(tr("ROOT Files (*.root)"));
    if(dialog.exec()){
        filenames = dialog.selectedFiles();
    }

    if(!filenames.empty()){
        ui->line_inputFile->setText(filenames.first());
        filename = filenames.first();

        totalEvents = GetNumberOfEvents(filename);


        // - calculate event/spill ratio
        MAUS::Data data;
        irstream infile(filename.toStdString().c_str(), "Spill");
        int spillShift = 0;

        std::cout << "\nInitializing...\n" << std::endl; //add verbosity option

        for (int ev = 150; ev < totalEvents; ev += 300) {

            infile.set_current_event(ev);
            int counter = 0;
            while(infile >> readEvent != NULL) {
                ++counter;
                infile >> branchName("data") >> data;
                MAUS::Spill *spill = data.GetSpill();

                if(spill != NULL && spill->GetDaqEventType() == "physics_event"){
                    if (ev == 150)
                        spillShift = spill->GetSpillNumber();

                    double ratio = (double)(ev+counter)/spill->GetSpillNumber();
                    eventSpillRatio.insert(spill->GetSpillNumber()-spillShift, ratio);

                    break;

                }
            }
        }

        // - find max spill number to define spill range
        SetSpillRange(totalEvents);

        // - some initial values
        spillNumber = 0;
        eventNumber = 0;
        eventWindow = 15; //search window

        // - load first event that satisfies selection criteria
        if (!ReadEvent())
            next_event();
    }
}

void MainWindow::choose_open_detgeom_file(){
    //ToDo: add name field and write chosen filename in it
    QStringList filenames;
    QFileDialog dialog(this);
    dialog.setDirectory(detGeomDirName);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setNameFilter(tr("ROOT Files (*.heprep)"));
    if(dialog.exec()){
        filenames = dialog.selectedFiles();
        if(!filenames.empty()) {
            detGeomFileName = filenames.first();
            ui->line_GeometryFile->setText(detGeomFileName);
        }
    }

    setenv("EV_DETGEOM_FILE", detGeomFileName.toStdString().c_str(), 1);

}

void MainWindow::choose_output_dir(){
    QStringList filenames;
    QFileDialog dialog(this);
    dialog.setDirectory(outputDirName);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);
    if(dialog.exec()){
        filenames = dialog.selectedFiles();
        if(!filenames.empty()) {
            outputDirName = filenames.first();
            ui->line_OutputDir->setText(outputDirName);
      }
    }

    setenv("EV_DEST_DIR", outputDirName.toStdString().c_str(), 1);

}


void MainWindow::plot_settings(){
    position_plots();
    momentum_plots();
    time_plots();
    tracker_plots();
}

void MainWindow::position_plots(){
    QPen pen;

    /*
     * (z, x) and (z, y) plot settings:
     * -> colours, symbols, plot interactivity
     */

    ui->plot_position_xz->addGraph(); // graph 0
    ui->plot_position_xz->addGraph(); // tof0, graph 1
    ui->plot_position_xz->addGraph(); // tof1, graph 2
    ui->plot_position_xz->addGraph(); // tku, graph 3
    ui->plot_position_xz->addGraph(); // tkd, graph 4
    ui->plot_position_xz->addGraph(); // tof2, graph 5

    int yBoundLower = -180.0;
    int yBoundUpper = 180.0;

    ui->plot_position_xz->xAxis->setLabel("z (mm)");
    ui->plot_position_xz->yAxis->setLabel("x (mm)");
    ui->plot_position_xz->xAxis->setRange(5000.0, 25000.0);
    ui->plot_position_xz->yAxis->setRange(yBoundLower, yBoundUpper);

    pen.setColor(Qt::gray);
    ui->plot_position_xz->graph(0)->setPen(pen);

    pen.setColor(Qt::darkBlue);
    ui->plot_position_xz->graph(1)->setPen(pen);
    ui->plot_position_xz->graph(1)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_position_xz->graph(1)->setLineStyle(QCPGraph::lsNone);

    pen.setColor(Qt::red);
    ui->plot_position_xz->graph(2)->setPen(pen);
    ui->plot_position_xz->graph(2)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_position_xz->graph(2)->setLineStyle(QCPGraph::lsNone);

    pen.setColor(Qt::darkGreen);
    ui->plot_position_xz->graph(3)->setPen(pen);
    ui->plot_position_xz->graph(3)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_position_xz->graph(3)->setLineStyle(QCPGraph::lsNone);

    pen.setColor(Qt::green);
    ui->plot_position_xz->graph(4)->setPen(pen);
    ui->plot_position_xz->graph(4)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_position_xz->graph(4)->setLineStyle(QCPGraph::lsNone);

    pen.setColor(Qt::cyan);
    ui->plot_position_xz->graph(5)->setPen(pen);
    ui->plot_position_xz->graph(5)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_position_xz->graph(5)->setLineStyle(QCPGraph::lsNone);


    // interactive elements set here
    ui->plot_position_xz->setInteraction(QCP::iRangeDrag, true);
    ui->plot_position_xz->setInteraction(QCP::iRangeZoom, true);

    // legend set here
    ui->plot_position_xz->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    ui->plot_position_xz->legend->setVisible(true);
    ui->plot_position_xz->graph(0)->removeFromLegend();
    ui->plot_position_xz->graph(1)->setName("TOF0");
    ui->plot_position_xz->graph(2)->setName("TOF1");
    ui->plot_position_xz->graph(3)->setName("TkUS");
    ui->plot_position_xz->graph(4)->setName("TkDS");
    ui->plot_position_xz->graph(5)->setName("TOF2");

    ui->plot_position_yz->addGraph(); // graph 0
    ui->plot_position_yz->addGraph(); // tof0, graph 1
    ui->plot_position_yz->addGraph(); // tof1, graph 2
    ui->plot_position_yz->addGraph(); // tku, graph 3
    ui->plot_position_yz->addGraph(); // tkd, graph 4
    ui->plot_position_yz->addGraph(); // tof2, graph 5
    QCPItemLine *upstream_track_xz = new QCPItemLine(ui->plot_position_xz);
    QCPItemLine *downstream_track_xz = new QCPItemLine(ui->plot_position_xz);
    QCPItemLine *upstream_track_yz = new QCPItemLine(ui->plot_position_yz);
    QCPItemLine *downstream_track_yz = new QCPItemLine(ui->plot_position_yz);
    ui->plot_position_xz->addItem(upstream_track_xz);
    ui->plot_position_xz->addItem(downstream_track_xz);
    ui->plot_position_yz->addItem(upstream_track_yz);
    ui->plot_position_yz->addItem(downstream_track_yz);

    ui->plot_position_yz->xAxis->setLabel("z (mm)");
    ui->plot_position_yz->yAxis->setLabel("y (mm)");
    ui->plot_position_yz->xAxis->setRange(5000.0, 25000.0);
    ui->plot_position_yz->yAxis->setRange(yBoundLower, yBoundUpper);

    pen.setColor(Qt::gray);
    ui->plot_position_yz->graph(0)->setPen(pen);

    pen.setColor(Qt::darkBlue);
    ui->plot_position_yz->graph(1)->setPen(pen);
    ui->plot_position_yz->graph(1)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_position_yz->graph(1)->setLineStyle(QCPGraph::lsNone);

    pen.setColor(Qt::red);
    ui->plot_position_yz->graph(2)->setPen(pen);
    ui->plot_position_yz->graph(2)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_position_yz->graph(2)->setLineStyle(QCPGraph::lsNone);

    pen.setColor(Qt::darkGreen);
    ui->plot_position_yz->graph(3)->setPen(pen);
    ui->plot_position_yz->graph(3)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_position_yz->graph(3)->setLineStyle(QCPGraph::lsNone);

    pen.setColor(Qt::green);
    ui->plot_position_yz->graph(4)->setPen(pen);
    ui->plot_position_yz->graph(4)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_position_yz->graph(4)->setLineStyle(QCPGraph::lsNone);

    pen.setColor(Qt::cyan);
    ui->plot_position_yz->graph(5)->setPen(pen);
    ui->plot_position_yz->graph(5)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_position_yz->graph(5)->setLineStyle(QCPGraph::lsNone);

    // interactive elements set here
    ui->plot_position_yz->setInteraction(QCP::iRangeDrag, true);
    ui->plot_position_yz->setInteraction(QCP::iRangeZoom, true);

    // legend set here
    ui->plot_position_yz->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    ui->plot_position_yz->legend->setVisible(true);
    ui->plot_position_yz->graph(0)->removeFromLegend();
    ui->plot_position_yz->graph(1)->setName("TOF0");
    ui->plot_position_yz->graph(2)->setName("TOF1");
    ui->plot_position_yz->graph(3)->setName("TkUS");
    ui->plot_position_yz->graph(4)->setName("TkDS");
    ui->plot_position_yz->graph(5)->setName("TOF2");
}

void MainWindow::time_plots(){

    QPen pen;
    pen.setColor(Qt::gray);

    ui->plot_tof0_to_tof1->addGraph(); // time of flight histogram
    ui->plot_tof0_to_tof1->addGraph(); // time of flight of this particle
    ui->plot_tof0_to_tof1->xAxis->setLabel("Time of flight: TOF0 to TOF1 (ns)");
    ui->plot_tof0_to_tof1->yAxis->setLabel("Number of Particles");
    ui->plot_tof0_to_tof1->setInteraction(QCP::iRangeDrag, true);
    ui->plot_tof0_to_tof1->setInteraction(QCP::iRangeZoom, true);
    ui->plot_tof0_to_tof1->graph(0)->setPen(pen);


    ui->plot_tof0_to_tof2->addGraph(); // time of flight histogram
    ui->plot_tof0_to_tof2->addGraph(); // time of flight of this particle
    ui->plot_tof0_to_tof2->xAxis->setLabel("Time of flight: TOF0 to TOF2 (ns)");
    ui->plot_tof0_to_tof2->yAxis->setLabel("Number of Particles");
    ui->plot_tof0_to_tof2->setInteraction(QCP::iRangeDrag, true);
    ui->plot_tof0_to_tof2->setInteraction(QCP::iRangeZoom, true);
    ui->plot_tof0_to_tof2->graph(0)->setPen(pen);


    ui->plot_tof1_to_tof2->addGraph(); // time of flight histogram
    ui->plot_tof1_to_tof2->addGraph(); // time of flight of this particle
    ui->plot_tof1_to_tof2->xAxis->setLabel("Time of flight: TOF1 to TOF2 (ns)");
    ui->plot_tof1_to_tof2->yAxis->setLabel("Number of Particles");
    ui->plot_tof1_to_tof2->setInteraction(QCP::iRangeDrag, true);
    ui->plot_tof1_to_tof2->setInteraction(QCP::iRangeZoom, true);
    ui->plot_tof1_to_tof2->graph(0)->setPen(pen);

    pen.setColor(Qt::red);
    ui->plot_tof0_to_tof1->graph(1)->setPen(pen);
    ui->plot_tof0_to_tof1->graph(1)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_tof0_to_tof1->graph(1)->setLineStyle(QCPGraph::lsNone);

    ui->plot_tof0_to_tof2->graph(1)->setPen(pen);
    ui->plot_tof0_to_tof2->graph(1)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_tof0_to_tof2->graph(1)->setLineStyle(QCPGraph::lsNone);

    ui->plot_tof1_to_tof2->graph(1)->setPen(pen);
    ui->plot_tof1_to_tof2->graph(1)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_tof1_to_tof2->graph(1)->setLineStyle(QCPGraph::lsNone);

    ui->plot_tof0_to_tof1->legend->visible();
    ui->plot_tof0_to_tof1->graph(0)->setName("All events");
    ui->plot_tof0_to_tof1->graph(1)->setName("This event");

    ui->plot_tof0_to_tof2->legend->visible();
    ui->plot_tof0_to_tof2->graph(0)->setName("All events");
    ui->plot_tof0_to_tof2->graph(1)->setName("This event");

    ui->plot_tof1_to_tof2->legend->visible();
    ui->plot_tof1_to_tof2->graph(0)->setName("All events");
    ui->plot_tof1_to_tof2->graph(1)->setName("This event");
}

void MainWindow::momentum_plots(){

    ui->plot_momentum_t->addGraph(); // graph 0, all Px
    ui->plot_momentum_t->addGraph(); // graph 1, upstream tracker Px
    ui->plot_momentum_t->addGraph(); // graph 2, downstream tracker Px

    ui->plot_momentum_t->addGraph(); // graph 3, all Py
    ui->plot_momentum_t->addGraph(); // graph 4, upstream tracker Py
    ui->plot_momentum_t->addGraph(); // graph 5, downstream tracker Py

    ui->plot_momentum_t->xAxis->setLabel("z (mm)");
    ui->plot_momentum_t->xAxis->setRange(5000.0, 25000.0);
    ui->plot_momentum_t->yAxis->setLabel("Px or Py (MeV)");
    ui->plot_momentum_t->yAxis->setRange(-50.0, 50.0);

    ui->plot_momentum_t->setInteraction(QCP::iRangeDrag, true);
    ui->plot_momentum_t->setInteraction(QCP::iRangeZoom, true);

    ui->plot_momentum_z->addGraph(); // graph 0, all Pz
    ui->plot_momentum_z->addGraph(); // graph 1, upstream tracker Pz
    ui->plot_momentum_z->addGraph(); // graoh 2, downstream tracker Pz

    ui->plot_momentum_z->xAxis->setLabel("z (mm)");
    ui->plot_momentum_z->xAxis->setRange(5000.0, 25000.0);
    ui->plot_momentum_z->yAxis->setLabel("Pz (MeV)");
    ui->plot_momentum_z->yAxis->setRange(100.0, 400.0);

    ui->plot_momentum_z->setInteraction(QCP::iRangeDrag, true);
    ui->plot_momentum_z->setInteraction(QCP::iRangeZoom, true);

    QPen pen;
    pen.setColor(Qt::darkBlue);
    ui->plot_momentum_t->graph(0)->setPen(pen);
    ui->plot_momentum_t->graph(0)->setName("Px");
    pen.setColor(Qt::darkGreen);
    ui->plot_momentum_t->graph(1)->setPen(pen);
    ui->plot_momentum_t->graph(1)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_momentum_t->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->plot_momentum_t->graph(1)->setName("Px, Upstream Tracker");
    pen.setColor(Qt::green);
    ui->plot_momentum_t->graph(2)->setPen(pen);
    ui->plot_momentum_t->graph(2)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_momentum_t->graph(2)->setLineStyle(QCPGraph::lsNone);
    ui->plot_momentum_t->graph(2)->setName("Px, Downstream Tracker");


    pen.setColor(Qt::darkRed);
    ui->plot_momentum_t->graph(3)->setPen(pen);
    ui->plot_momentum_t->graph(3)->setName("Py");
    pen.setColor(Qt::darkGreen);
    ui->plot_momentum_t->graph(4)->setPen(pen);
    ui->plot_momentum_t->graph(4)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_momentum_t->graph(4)->setLineStyle(QCPGraph::lsNone);
    ui->plot_momentum_t->graph(4)->setName("Py, Upstream Tracker");
    pen.setColor(Qt::green);
    ui->plot_momentum_t->graph(5)->setPen(pen);
    ui->plot_momentum_t->graph(5)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_momentum_t->graph(5)->setLineStyle(QCPGraph::lsNone);
    ui->plot_momentum_t->graph(5)->setName("Py, Downstream Tracker");
    ui->plot_momentum_t->legend->setVisible(true);

    pen.setColor(Qt::darkBlue);
    ui->plot_momentum_z->graph(0)->setPen(pen);
    pen.setColor(Qt::darkGreen);
    ui->plot_momentum_z->graph(1)->setPen(pen);
    ui->plot_momentum_z->graph(1)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_momentum_z->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->plot_momentum_z->graph(1)->setName("Pz, Upstream Tracker");
    pen.setColor(Qt::green);
    ui->plot_momentum_z->graph(2)->setPen(pen);
    ui->plot_momentum_z->graph(1)->setName("Pz, Downstream Tracker");
    ui->plot_momentum_z->graph(2)->setScatterStyle(QCPScatterStyle::ssSquare);
    ui->plot_momentum_z->graph(2)->setLineStyle(QCPGraph::lsNone);
    ui->plot_momentum_z->legend->setVisible(true);
}

void MainWindow::tracker_plots(){
    ui->plot_scifi_cluster_position_az->addGraph();  // TkUS Plane 0
    ui->plot_scifi_cluster_position_az->addGraph();  // TkUS Plane 1
    ui->plot_scifi_cluster_position_az->addGraph();  // TkUS Plane 2
    ui->plot_scifi_cluster_position_az->addGraph();  // TkDS Plane 0
    ui->plot_scifi_cluster_position_az->addGraph();  // TkDS Plane 1
    ui->plot_scifi_cluster_position_az->addGraph();  // TkDS Plane 2

    ui->plot_scifi_cluster_position_az->xAxis->setLabel("z (mm)");
    ui->plot_scifi_cluster_position_az->xAxis->setRange(-1300.0, 1300.0);
    ui->plot_scifi_cluster_position_az->yAxis->setLabel("alpha (channel #)");
    ui->plot_scifi_cluster_position_az->yAxis->setRange(-160.0, 160.0);

    ui->plot_scifi_cluster_position_az->setInteraction(QCP::iRangeDrag, true);
    ui->plot_scifi_cluster_position_az->setInteraction(QCP::iRangeZoom, true);

    QPen pen;
    pen.setColor(Qt::blue);
    ui->plot_scifi_cluster_position_az->graph(0)->setPen(pen);
    ui->plot_scifi_cluster_position_az->graph(0)->setScatterStyle(QCPScatterStyle::ssTriangle);
    ui->plot_scifi_cluster_position_az->graph(3)->setPen(pen);
    ui->plot_scifi_cluster_position_az->graph(3)->setScatterStyle(QCPScatterStyle::ssTriangleInverted);

    pen.setColor(Qt::green);
    ui->plot_scifi_cluster_position_az->graph(1)->setPen(pen);
    ui->plot_scifi_cluster_position_az->graph(1)->setScatterStyle(QCPScatterStyle::ssTriangle);
    ui->plot_scifi_cluster_position_az->graph(4)->setPen(pen);
    ui->plot_scifi_cluster_position_az->graph(4)->setScatterStyle(QCPScatterStyle::ssTriangleInverted);

    pen.setColor(Qt::red);
    ui->plot_scifi_cluster_position_az->graph(2)->setPen(pen);
    ui->plot_scifi_cluster_position_az->graph(2)->setScatterStyle(QCPScatterStyle::ssTriangle);
    ui->plot_scifi_cluster_position_az->graph(5)->setPen(pen);
    ui->plot_scifi_cluster_position_az->graph(5)->setScatterStyle(QCPScatterStyle::ssTriangleInverted);

    // legend set here
    ui->plot_scifi_cluster_position_az->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    ui->plot_scifi_cluster_position_az->legend->setVisible(true);
    ui->plot_scifi_cluster_position_az->graph(0)->setName("TkUS P0");
    ui->plot_scifi_cluster_position_az->graph(1)->setName("TkUS P1");
    ui->plot_scifi_cluster_position_az->graph(2)->setName("TkUS P2");
    ui->plot_scifi_cluster_position_az->graph(3)->setName("TkDS P0");
    ui->plot_scifi_cluster_position_az->graph(4)->setName("TkDS P1");
    ui->plot_scifi_cluster_position_az->graph(5)->setName("TkDS P2");
}


void MainWindow::next_event() {

    QMessageBox msgBox;

    if (!InputFileIsOpen()) {
        msgBox.setText("You must choose input file first");
        msgBox.exec();
        //std::cerr << "WARNING: You must choose input file first!" << std::endl; //substitute with message box later?
        return;
    }

    int maxEventNum = GetMaxEventNumber(spillNumber);
    int read = 0;

    while (!read){
        eventNumber++;
        if (eventNumber < maxEventNum) {
            read = ReadEvent();
        }
        else {
            if (spillNumber >= 0 && spillNumber < maxSpill) { //lower boundary is 0 now so it can find first non-empty spill on start (can be changed to minSpill again if no spill is to be loaded initialy)
                next_spill(); //this should navigate to first event that satisfies selection criteria in one of the following spills
                read = 1;
            }
            else {
                eventNumber = maxEventNum - 1;
                msgBox.setText("No next event for this run");
                msgBox.exec();
                //std::cout << "WARNING! No next event for this run!" << std::endl;
                break;
            }
        }
    }
}

void MainWindow::next_spill() {

    QMessageBox msgBox;

    if (!InputFileIsOpen()) {
        msgBox.setText("You must choose input file first");
        msgBox.exec();
        //std::cerr << "WARNING: You must choose input file first!" << std::endl; //substitute with message box later?
        return;
    }

    int read = 0;

    while (!read) {
        if (spillNumber < 0 || spillNumber >= maxSpill) {
            std::stringstream msg;
            msg << "Spill value out of range! Choose spill from interval [" << minSpill << " ," << maxSpill << "]";
            msgBox.setText(msg.str().c_str());
            msgBox.exec();
            //std::cout << "Spill value out of range! Choose spill from range [" << minSpill << " ," << maxSpill << "]" << std::endl; //check whether these messages are informative enough
            break;
        }
        else if (spillNumber >= 0 && spillNumber < maxSpill) { //lower boundary is 0 now so it can find first non-empty spill on start
            ++spillNumber;
            eventNumber = 0;

            read = ReadEvent();
            if (!read) {
                next_event();
                read = 1;
            }
        }
    }
}

void MainWindow::previous_event() {

    QMessageBox msgBox;

    if (!InputFileIsOpen()) {
        msgBox.setText("You must choose input file first");
        msgBox.exec();
        //std::cerr << "WARNING: You must choose input file first!" << std::endl; //substitute with message box later?
        return;
    }

    int read = 0;

    while (!read) {
        eventNumber--;
        if (eventNumber >= 0) {
            read = ReadEvent();
        }
        else {
            if (spillNumber > minSpill && spillNumber <= maxSpill) {
                previous_spill_end();
                read = 1;
            }
            else {
                eventNumber = 0;
                msgBox.setText("No previous event for this run");
                msgBox.exec();
                //std::cout << "No previous event for this run!" << std::endl;
                break;
            }
        }
    }
}

void MainWindow::previous_spill() { //maybe this should be substituted everywhere with previous_spill_end() to prevent pointless looping with spills that have no event satisfying selection criteria

    QMessageBox msgBox;

    if (!InputFileIsOpen()) {
        msgBox.setText("You must choose input file first");
        msgBox.exec();
        //std::cerr << "WARNING: You must choose input file first!" << std::endl; //substitute with message box later?
        return;
    }

    eventNumber = 0;
    int read = 0;

    while (!read) {
        if (spillNumber <= minSpill || spillNumber > maxSpill+1) {
            std::stringstream msg;
            msg << "Spill value out of range! Choose spill from interval [" << minSpill << " ," << maxSpill << "]";
            msgBox.setText(msg.str().c_str());
            msgBox.exec();
            //std::cout << "Spill value out of range! Choose spill from range [" << minSpill << " ," << maxSpill << "]" << std::endl;
            break;
        }
        else if (spillNumber > minSpill && spillNumber <= maxSpill+1) {
            --spillNumber;
            //int maxEventNum = GetMaxEventNumber(spillNumber);

            eventNumber = 0;

            read = ReadEvent();
            if (!read) {
                next_event(); //this can cause unwanted looping - maybe previous spill should start with last event...see previous_event_end()...another option is to introduce another next_event() which would go to previous spill if !read
                read = 1;
            }
        }
    }
}

void MainWindow::previous_spill_end() {

    QMessageBox msgBox;

    if (!InputFileIsOpen()) {
        msgBox.setText("You must choose input file first");
        msgBox.exec();
        //std::cerr << "WARNING: You must choose input file first!" << std::endl; //substitute with message box later?
        return;
    }

    int read = 0;

    while (!read) {
        if (spillNumber <= minSpill || spillNumber > maxSpill+1) {
            std::stringstream msg;
            msg << "Spill value out of range! Choose spill from interval [" << minSpill << " ," << maxSpill << "]";
            msgBox.setText(msg.str().c_str());
            msgBox.exec();
            //std::cout << "Spill value out of range! Choose spill from range [" << minSpill << " ," << maxSpill << "]" << std::endl;
            break;
        }
        else if (spillNumber > minSpill && spillNumber <= maxSpill+1) {
            --spillNumber;
            int maxEventNum = GetMaxEventNumber(spillNumber);
            eventNumber = maxEventNum - 1;

            read = ReadEvent();
            if (!read) {
                previous_event();
                read = 1;
            }
        }
    }
}

void MainWindow::choose_spill(){

    QMessageBox msgBox;

    if (!InputFileIsOpen()) {
        msgBox.setText("You must choose input file first");
        msgBox.exec();
        //std::cerr << "WARNING: You must choose input file first!" << std::endl; //substitute with message box later?
        return;
    }

    spillNumber = ui->int_goToSpill->value();
    eventNumber = 0;

    if (spillNumber >= minSpill && spillNumber <= maxSpill) {
        ReadEvent();
    }
    else {
        std::stringstream msg;
        msg << "Spill value out of range! Choose spill from interval [" << minSpill << " ," << maxSpill << "]";
        msgBox.setText(msg.str().c_str());
        msgBox.exec();
        //std::cout << "Spill value out of range! Choose spill from range [" << minSpill << " ," << maxSpill << "]" << std::endl;
    }
}

void MainWindow::choose_event() {

    QMessageBox msgBox;

    if (!InputFileIsOpen()) {
        msgBox.setText("You must choose input file first");
        msgBox.exec();
        //std::cerr << "WARNING: You must choose input file first!" << std::endl; //substitute with message box later?
        return;
    }

    eventNumber = ui->int_goToEvent->value();
    int maxEventNum = GetMaxEventNumber(spillNumber);

    if (eventNumber >= 0 && eventNumber < maxEventNum)
        ReadEvent();
    else {
        std::stringstream msg;
        msg << "Event value out of range! Choose event from range [" << 0 << " ," << maxEventNum - 1 << "]";
        msgBox.setText(msg.str().c_str());
        msgBox.exec();
        //std::cout << "Event value out of range! Choose event from range [" << 0 << " ," << maxEventNum - 1 << "]" << std::endl;
    }
}

void MainWindow::replot(){
    spillLabel = QString::number(spillNumber);
    eventLabel = QString::number(eventNumber);
    ui->label_eventNumber->setText(eventLabel);
    ui->label_spillNumber->setText(spillLabel);

    // plot TOF0:
    QVector<double> tof0_x, tof0_y, tof0_z;
    MAUS::ThreeVector tof0 = evEvent.tofPoints[0];
    tof0_x << tof0.x();
    tof0_y << tof0.y();
    tof0_z << tof0.z();

    ui->plot_position_xz->graph(1)->setData(tof0_z, tof0_x);
    ui->plot_position_yz->graph(1)->setData(tof0_z, tof0_y);


    // plot TOF1:
    QVector<double> tof1_x, tof1_y, tof1_z;
    MAUS::ThreeVector tof1 = evEvent.tofPoints[1];
    tof1_x << tof1.x();
    tof1_y << tof1.y();
    tof1_z << tof1.z();

    ui->plot_position_xz->graph(2)->setData(tof1_z, tof1_x);
    ui->plot_position_yz->graph(2)->setData(tof1_z, tof1_y);


    // plot upstream tracker:
    QVector<double> tku_x, tku_y, tku_z, tku_px, tku_py, tku_pt, tku_pz, tku_p;
    for (int i=0; i<5; ++i) {
        tku_x << evEvent.scifiUSTrackerPoints[i].x();
        tku_y << evEvent.scifiUSTrackerPoints[i].y();
        tku_z << evEvent.scifiUSTrackerPoints[i].z();
        tku_px << evEvent.scifiUSTrackerPointsMomenta[i].x();
        tku_py << evEvent.scifiUSTrackerPointsMomenta[i].y();
        tku_pz << evEvent.scifiUSTrackerPointsMomenta[i].z();
    }

    ui->plot_position_xz->graph(3)->setData(tku_z, tku_x);
    ui->plot_position_yz->graph(3)->setData(tku_z, tku_y);
    ui->plot_momentum_t->graph(1)->setData(tku_z, tku_px);
    ui->plot_momentum_t->graph(4)->setData(tku_z, tku_py);
    ui->plot_momentum_z->graph(1)->setData(tku_z, tku_pz);


    // plot downstream tracker:
    QVector<double> tkd_x, tkd_y, tkd_z, tkd_px, tkd_py, tkd_pt, tkd_pz, tkd_p;
    for (int i=0; i<5; ++i) {
        tkd_x << evEvent.scifiDSTrackerPoints[i].x();
        tkd_y << evEvent.scifiDSTrackerPoints[i].y();
        tkd_z << evEvent.scifiDSTrackerPoints[i].z();
        tkd_px << evEvent.scifiDSTrackerPointsMomenta[i].x();
        tkd_py << evEvent.scifiDSTrackerPointsMomenta[i].y();
        tkd_pz << evEvent.scifiDSTrackerPointsMomenta[i].z();
    }

    ui->plot_position_xz->graph(4)->setData(tkd_z, tkd_x);
    ui->plot_position_yz->graph(4)->setData(tkd_z, tkd_y);
    ui->plot_momentum_t->graph(2)->setData(tkd_z, tkd_px);
    ui->plot_momentum_t->graph(5)->setData(tkd_z, tkd_py);
    ui->plot_momentum_z->graph(2)->setData(tkd_z, tkd_pz);


    // plot straight tracks
    QCPItemLine* upstream_track_xz = static_cast<QCPItemLine*>(ui->plot_position_xz->item(0));
    upstream_track_xz->setPen(QPen(Qt::red));
    upstream_track_xz->start->setCoords(dummyValue, dummyValue);
    upstream_track_xz->end->setCoords(dummyValue, dummyValue);
    QCPItemLine* downstream_track_xz = static_cast<QCPItemLine*>(ui->plot_position_xz->item(1));
    downstream_track_xz->setPen(QPen(Qt::blue));
    downstream_track_xz->start->setCoords(dummyValue, dummyValue);
    downstream_track_xz->end->setCoords(dummyValue, dummyValue);

    upstream_track_xz->start->setCoords(evEvent.scifiUSTrackerStraightTrackPoints.at(0).z(), evEvent.scifiUSTrackerStraightTrackPoints.at(0).x());
    upstream_track_xz->end->setCoords(evEvent.scifiUSTrackerStraightTrackPoints.at(1).z(), evEvent.scifiUSTrackerStraightTrackPoints.at(1).x());

    downstream_track_xz->start->setCoords(evEvent.scifiDSTrackerStraightTrackPoints.at(0).z(), evEvent.scifiDSTrackerStraightTrackPoints.at(0).x());
    downstream_track_xz->end->setCoords(evEvent.scifiDSTrackerStraightTrackPoints.at(1).z(), evEvent.scifiDSTrackerStraightTrackPoints.at(1).x());

    QCPItemLine* upstream_track_yz = static_cast<QCPItemLine*>(ui->plot_position_yz->item(0));
    upstream_track_yz->setPen(QPen(Qt::red));
    upstream_track_yz->start->setCoords(dummyValue, dummyValue);
    upstream_track_yz->end->setCoords(dummyValue, dummyValue);
    QCPItemLine* downstream_track_yz = static_cast<QCPItemLine*>(ui->plot_position_yz->item(1));
    downstream_track_yz->setPen(QPen(Qt::blue));
    downstream_track_yz->start->setCoords(dummyValue, dummyValue);
    downstream_track_yz->end->setCoords(dummyValue, dummyValue);

    upstream_track_yz->start->setCoords(evEvent.scifiUSTrackerStraightTrackPoints.at(0).z(), evEvent.scifiUSTrackerStraightTrackPoints.at(0).y());
    upstream_track_yz->end->setCoords(evEvent.scifiUSTrackerStraightTrackPoints.at(1).z(), evEvent.scifiUSTrackerStraightTrackPoints.at(1).y());

    downstream_track_yz->start->setCoords(evEvent.scifiDSTrackerStraightTrackPoints.at(0).z(), evEvent.scifiDSTrackerStraightTrackPoints.at(0).y());
    downstream_track_yz->end->setCoords(evEvent.scifiDSTrackerStraightTrackPoints.at(1).z(), evEvent.scifiDSTrackerStraightTrackPoints.at(1).y());

    ui->plot_position_yz->replot();


    // plot TOF2:
    QVector<double> tof2_x, tof2_y, tof2_z;
    MAUS::ThreeVector tof2 = evEvent.tofPoints[2];
    tof2_x << tof2.x();
    tof2_y << tof2.y();
    tof2_z << tof2.z();

    ui->plot_position_xz->graph(5)->setData(tof2_z, tof2_x);
    ui->plot_position_yz->graph(5)->setData(tof2_z, tof2_y);

    ui->plot_position_xz->replot();
    ui->plot_position_yz->replot();
    ui->plot_momentum_t->replot();
    ui->plot_momentum_z->replot();


    // Plot tracker specific data
    QVector<double> tkus_cluster_alpha_plane0 = QVector<double>::fromStdVector(evEvent.scifiUSTrackerClusters[0][0]);
    QVector<double> tkus_cluster_z_plane0 = QVector<double>::fromStdVector(evEvent.scifiUSTrackerClusters[0][1]);
    QVector<double> tkus_cluster_alpha_plane1 = QVector<double>::fromStdVector(evEvent.scifiUSTrackerClusters[1][0]);
    QVector<double> tkus_cluster_z_plane1 = QVector<double>::fromStdVector(evEvent.scifiUSTrackerClusters[1][1]);
    QVector<double> tkus_cluster_alpha_plane2 = QVector<double>::fromStdVector(evEvent.scifiUSTrackerClusters[2][0]);
    QVector<double> tkus_cluster_z_plane2 = QVector<double>::fromStdVector(evEvent.scifiUSTrackerClusters[2][1]);

    QVector<double> tkds_cluster_alpha_plane0 = QVector<double>::fromStdVector(evEvent.scifiDSTrackerClusters[0][0]);
    QVector<double> tkds_cluster_z_plane0 = QVector<double>::fromStdVector(evEvent.scifiDSTrackerClusters[0][1]);
    QVector<double> tkds_cluster_alpha_plane1 = QVector<double>::fromStdVector(evEvent.scifiDSTrackerClusters[1][0]);
    QVector<double> tkds_cluster_z_plane1 = QVector<double>::fromStdVector(evEvent.scifiDSTrackerClusters[1][1]);
    QVector<double> tkds_cluster_alpha_plane2 = QVector<double>::fromStdVector(evEvent.scifiDSTrackerClusters[2][0]);
    QVector<double> tkds_cluster_z_plane2 = QVector<double>::fromStdVector(evEvent.scifiDSTrackerClusters[2][1]);

    ui->plot_scifi_cluster_position_az->graph(0)->setData(tkus_cluster_z_plane0, tkus_cluster_alpha_plane0);
    ui->plot_scifi_cluster_position_az->graph(1)->setData(tkus_cluster_z_plane1, tkus_cluster_alpha_plane1);
    ui->plot_scifi_cluster_position_az->graph(2)->setData(tkus_cluster_z_plane1, tkus_cluster_alpha_plane2);
    ui->plot_scifi_cluster_position_az->graph(3)->setData(tkds_cluster_z_plane0, tkds_cluster_alpha_plane0);
    ui->plot_scifi_cluster_position_az->graph(4)->setData(tkds_cluster_z_plane1, tkds_cluster_alpha_plane1);
    ui->plot_scifi_cluster_position_az->graph(5)->setData(tkds_cluster_z_plane1, tkds_cluster_alpha_plane2);
    ui->plot_scifi_cluster_position_az->replot();


    // coordinate and momentum line plots
    QVector<double> x, y, z, px, py, pz;

    PushValue(x, tof0_x, dummyValue);
    PushValue(y, tof0_y, dummyValue);
    PushValue(z, tof0_z, dummyValue);

    PushValue(x, tof1_x, dummyValue);
    PushValue(y, tof1_y, dummyValue);
    PushValue(z, tof1_z, dummyValue);

    PushValue(x, tku_x, dummyValue);
    PushValue(y, tku_y, dummyValue);
    PushValue(z, tku_z, dummyValue);

    PushValue(x, tkd_x, dummyValue);
    PushValue(y, tkd_y, dummyValue);
    PushValue(z, tkd_z, dummyValue);

    PushValue(x, tof2_x, dummyValue);
    PushValue(y, tof2_y, dummyValue);
    PushValue(z, tof2_z, dummyValue);


    ui->plot_position_xz->graph(0)->setData(z, x);
    ui->plot_position_yz->graph(0)->setData(z, y);

    ui->plot_position_xz->replot();
    ui->plot_position_yz->replot();

    ui->plot_momentum_t->graph(0)->setData(z, px);
    ui->plot_momentum_t->graph(3)->setData(z, py);

    ui->plot_momentum_t->replot();
    ui->plot_momentum_t->replot();

    ui->plot_momentum_z->graph(0)->setData(z, pz);
    ui->plot_momentum_z->replot();

}

int MainWindow::ReadEvent()
{
    MAUS::Data data;
    irstream infile(filename.toStdString().c_str(), "Spill");

    int aproxEvent = 0;
    double ratio = GetRatio(spillNumber);

    aproxEvent = spillNumber*ratio;

    for (int ev=aproxEvent-eventWindow; ev < aproxEvent+eventWindow; ++ev) {

        if (ev < 0 || ev >= totalEvents) continue;

        infile.set_current_event(ev);
        if (infile >> readEvent != NULL) {
            infile >> branchName("data") >> data;
            MAUS::Spill *spill = data.GetSpill();
            if(spill != NULL && spill->GetDaqEventType() == "physics_event") {
                if (spill->GetSpillNumber() == spillNumber) {
                    //std::cout << std::endl << std::endl << "------->found spill: " << spill->GetSpillNumber() << " event: " << eventNumber << std::endl;//remove after checks
                    exp = new EVExporter(spill);
                    int detectorsHit = exp->ReadOneEvent(eventNumber, exportOpt.c_str(), eventSelection, displayOpt);
                    //std::cout << "--->detectorsHit: " << detectorsHit << "  eventSelection: " << eventSelection << std::endl; //remove after checks
                    if (eventSelection == 0) { //if eventSelectio is 0 every event should be displayed
                        std::cout << std::endl << "...Processing spill: " << spill->GetSpillNumber() << "  event: " << eventNumber << std::endl; //remove or add verbosity option
                        evEvent = exp->evEvent;
                        replot();
                        infile.close();
                        return 1;
                    }
                    else if (detectorsHit == eventSelection) { //add event selection to ReadOneEvent() and add control to GUI for export option
                        std::cout << std::endl << "...Processing spill: " << spill->GetSpillNumber() << "  event: " << eventNumber << std::endl; //remove or add verbosity option
                        evEvent = exp->evEvent;
                        replot();
                        infile.close();
                        return 1;
                    }
                    else {
                        infile.close();
                        return 0;
                    }
                }
            }
        }
    }

    //std::cout << "--->did not find spill: " << spillNumber << " event: " << eventNumber << std::endl;//remove after checks
    infile.close();
    return 0;
}

double MainWindow::GetRatio(int spillNum)
{
    double ratio = 0;

    // check vars
    int lastKey = 0;
    double lastVal = 0;

    QMap<int, double>::iterator itLast = eventSpillRatio.begin();

    for (QMap<int, double>::iterator it = eventSpillRatio.begin(); it != eventSpillRatio.end(); ++it) {
        if (spillNum >= it.key()) {
            ratio = itLast.value();
            lastKey = itLast.key();
            lastVal = itLast.value();
        }

        ++itLast;
    }

    return ratio;
}

long long MainWindow::GetNumberOfEvents(QString filename)
{
    TFile *inputFile = new TFile(filename.toStdString().c_str());
    TTree *tree = (TTree*)inputFile->Get("Spill");
    TBranch *branch = (TBranch*)tree->GetBranch("data");
    long long entries = branch->GetEntries();

    inputFile->Close();
    return entries;
}

void MainWindow::SetSpillRange(long long totalEvents)
{
    MAUS::Data data;
    irstream infile(filename.toStdString().c_str(), "Spill");

    for (long long evt=totalEvents-1; evt>0; --evt) {
        infile.set_current_event(evt);
        if (infile >> readEvent != NULL) {
            infile >> branchName("data") >> data;
            MAUS::Spill *spill = data.GetSpill();
            if(spill != NULL && spill->GetDaqEventType() == "physics_event") {
                maxSpill = spill->GetSpillNumber();
                break;
            }
        }
    }

    for (long long evt=0; evt<totalEvents; ++evt) {
        infile.set_current_event(evt);
        if (infile >> readEvent != NULL) {
            infile >> branchName("data") >> data;
            MAUS::Spill *spill = data.GetSpill();
            if(spill != NULL && spill->GetDaqEventType() == "physics_event") {
                minSpill = spill->GetSpillNumber();
                break;
            }
        }
    }
}

size_t MainWindow::GetMaxEventNumber(int spillNum)
{
    MAUS::Data data;
    irstream infile(filename.toStdString().c_str(), "Spill");

    int aproxEvent = 0;
    if (spillNumber > 0)
        aproxEvent = spillNumber*GetRatio(spillNumber);

    for (int ev=aproxEvent-eventWindow; ev < aproxEvent+eventWindow; ++ev) {

        if (ev < 0) continue;

        infile.set_current_event(ev);
        if (infile >> readEvent != NULL) {
            infile >> branchName("data") >> data;
            MAUS::Spill *spill = data.GetSpill();
            if(spill != NULL && spill->GetDaqEventType() == "physics_event") {
                if (spill->GetSpillNumber() == spillNumber) {
                    size_t maxEventNumber = spill->GetReconEventSize();
                    infile.close();
                    return maxEventNumber;
                }
            }
        }
    }

    infile.close();
    std::cerr << "--->Spill " << spillNumber << " may not hold any recon events" << std::endl; //this may be an unnecessary check
    return 0; // or some other value
}

void MainWindow::PushValue(QVector<double> &out, QVector<double> in, double controlVal)
{
    for (QVector<double>::iterator it=in.begin(); it != in.end(); ++it) {
        if (*it != controlVal)
            out.push_back(*it);
    }
}

int MainWindow::InputFileIsOpen()
{
    int open;

    if (filename == "")
        open = 0;
    else
        open = 1;

    return open;
}
