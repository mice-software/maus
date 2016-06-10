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

MAUS_ROOT_DIR = $$(MAUS_ROOT_DIR)
MAUS_THIRD_PARTY = $$(MAUS_THIRD_PARTY)

SOURCES += main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    settings.cpp \
    $${MAUS_ROOT_DIR}/third_party/install/heprep/HepRepXMLWriter.cc \
    EVExporter.cc \
    EVEvent.cc \
    EVHepRepExporter.cc \

HEADERS  += mainwindow.h \
    qcustomplot.h \
    settings.h \
    $${MAUS_ROOT_DIR}/third_party/install/heprep/HepRepXMLWriter.hh \
    EVExporter.hh \
    EVEvent.hh \
    EVHepRepExporter.hh \

FORMS    += mainwindow.ui \
    settings.ui

LIBS += -L$${MAUS_THIRD_PARTY}/third_party/build/root/lib -lCint -lCore -lMathCore
LIBS += -lMathMore -lHist -lTree -lMatrix -lRIO -lThread
LIBS += -lGui -lRIO -lNet -lGraf -lGraf3d -lGpad -lRint -lPostscript -lPhysics -lThread -pthread -lm -ldl -rdynamic
LIBS += -L$${MAUS_ROOT_DIR}/src/common_cpp -lMausCpp
LIBS += -L$${MAUS_THIRD_PARTY}/third_party/install/lib
LIBS += -L$${MAUS_THIRD_PARTY}/third_party/build/geant4.9.6.p02/outputs/library/Linux-g++
LIBS += -ljson -lPhysics
LIBS += -lCLHEP
LIBS += -lG4geometry -lG4graphics_reps -lG4materials -lG4particles
LIBS += -lG4processes -lG4run -lG4event -lG4global -lG4intercoms
LIBS += -lG4modeling -lG4tracking -lG4visHepRep -lG4VRML -lG4digits_hits
LIBS += -lG4FR -lG4physicslists -lG4vis_management -lG4clhep -lG4track -lG4zlib

INCLUDEPATH += $${MAUS_THIRD_PARTY}/third_party/build/root/include/
INCLUDEPATH += $${MAUS_ROOT_DIR}/src/common_cpp
INCLUDEPATH += $${MAUS_ROOT_DIR}
INCLUDEPATH += $${MAUS_ROOT_DIR}/src/legacy
INCLUDEPATH += $${MAUS_THIRD_PARTY}/third_party/install/include
INCLUDEPATH += $${MAUS_ROOT_DIR}/third_party/install/heprep

DEPENDPATH +=$${MAUS_THIRD_PARTY}/third_party/build/root/include

QMAKE_CXXFLAGS += -std=c++11
