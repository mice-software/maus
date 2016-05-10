#-------------------------------------------------
#
# Project created by QtCreator 2015-09-01T14:18:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = EventViewer
TEMPLATE = app

CONFIG += qt debug

SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    settings.cpp \
    HepRepXMLWriter.cc \
    EVExporter.cc \
    EVEvent.cc \
    EVHepRepExporter.cc \

HEADERS  += mainwindow.h \
    qcustomplot.h \
    settings.h \
    HepRepXMLWriter.h \
    EVExporter.h \
    EVEvent.h \
    EVHepRepExporter.h \

FORMS    += mainwindow.ui \
    settings.ui

MAUS_DIR = $$(MAUS_ROOT_DIR)
#MAUS_DIR = /media/msavic/NTFSVolume/\!Mihailo/Ubuntu/Programs/MAUS/MAUS-v2.0.0
#MAUS_DIR = /home/msavic/Programs/MAUS/MAUS-v2.0.0/
#MAUS_DIR = /home/adobbs/MAUS/maus/trunk
#MAUS_DIR = /vols/fets2/adobbs/MAUS/maus/trunk

LIBS += -L$${MAUS_DIR}/third_party/build/root/lib -lCint -lCore -lMathCore
LIBS += -lMathMore -lHist -lTree -lMatrix -lRIO -lThread
LIBS += -lGui -lRIO -lNet -lGraf -lGraf3d -lGpad -lRint -lPostscript -lPhysics -lThread -pthread -lm -ldl -rdynamic
LIBS += -L$${MAUS_DIR}/src/common_cpp -lMausCpp
LIBS += -L$${MAUS_DIR}/third_party/install/lib
LIBS += -L$${MAUS_DIR}/third_party/build/geant4.9.6.p02/outputs/library/Linux-g++
LIBS += -ljson -lPhysics
LIBS += -lCLHEP
LIBS += -lG4geometry -lG4graphics_reps -lG4materials -lG4particles
LIBS += -lG4processes -lG4run -lG4event -lG4global -lG4intercoms
LIBS += -lG4modeling -lG4tracking -lG4visHepRep -lG4VRML -lG4digits_hits
LIBS += -lG4FR -lG4physicslists -lG4vis_management -lG4clhep -lG4track -lG4zlib


INCLUDEPATH += $${MAUS_DIR}/third_party/build/root/include/


INCLUDEPATH += $${MAUS_DIR}/src/common_cpp
INCLUDEPATH += $${MAUS_DIR}
INCLUDEPATH += $${MAUS_DIR}/src/legacy
INCLUDEPATH += $${MAUS_DIR}/third_party/install/include

DEPENDPATH +=$${MAUS_DIR}/third_party/build/root/include

QMAKE_CXXFLAGS += -std=c++11
