#-------------------------------------------------
#
# Project created by QtCreator 2014-11-12T11:25:13
#
#-------------------------------------------------

QT       += core gui
QT       += xml network testlib

QMAKE_CXXFLAGS += -fpermissive

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = hiddendragon
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    controller.cpp \
    logger.cpp \
    config.cpp \
    image.cpp \
    encodeddata.cpp \
    data.cpp \
    hiddendragonexception.cpp \
    optiondialog.cpp \
    decodedialog.cpp \
    encodedialog.cpp \
    jpeg/arc.c \
    jpeg/err.c \
    jpeg/golay.c \
    jpeg/iterator.c \
    jpeg/jpg.c \
    jpeg/md5.c \
    jpeg/outguess.c \
    jpeg/pnm.c \
    blackdragon/aes-tables.c \
    blackdragon/bd.c \
    blackdragon/blackdragon.cpp \
    bmp/bmpsteg.cpp \
    splitfile.cpp \
    dropbox/qdropbox.cpp \
    dropbox/qdropboxaccount.cpp \
    dropbox/qdropboxfile.cpp \
    dropbox/qdropboxfileinfo.cpp \
    dropbox/qdropboxjson.cpp \
    dropboxclient.cpp \
    dropboxdialog.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    controller.h \
    logger.h \
    config.h \
    image.h \
    encodeddata.h \
    data.h \
    hiddendragonexception.h \
    optiondialog.h \
    decodedialog.h \
    encodedialog.h \
    imagewidget.h \
    jpeg/arc.h \
    jpeg/config.h \
    jpeg/err.h \
    jpeg/golay.h \
    jpeg/iterator.h \
    jpeg/jconfig.h \
    jpeg/jmorecfg.h \
    jpeg/jpeglib.h \
    jpeg/jpg.h \
    jpeg/md5.h \
    jpeg/mman.h \
    jpeg/outguess.h \
    jpeg/pnm.h \
    blackdragon/bd.h \
    blackdragon/defs.h \
    blackdragon/blackdragon.h \
    bmp/bmpsteg.h \
    splitfile.h \
    dropbox/qdropbox.h \
    dropbox/qdropboxaccount.h \
    dropbox/qdropboxfile.h \
    dropbox/qdropboxfileinfo.h \
    dropbox/qdropboxjson.h \
    dropbox/qtdropbox.h \
    dropbox/qtdropbox_global.h \
    dropboxclient.h \
    dropboxdialog.h

FORMS    += \
    ui/mainwindow.ui \
    ui/aboutdialog.ui \
    ui/imagewidget.ui \
    ui/camera.ui \
    ui/optiondialog.ui \
    ui/dropboxdialog.ui

RESOURCES += \
    img/hiddendragon.qrc


win32: LIBS += -L$$PWD/jpeg/lib/ -ljpeg

INCLUDEPATH += $$PWD/jpeg
DEPENDPATH += $$PWD/jpeg

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/jpeg/lib/jpeg.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/jpeg/lib/libjpeg.a

win32: LIBS += -L$$PWD/jpeg/lib/ -lmman

INCLUDEPATH += $$PWD/jpeg
DEPENDPATH += $$PWD/jpeg

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/jpeg/lib/mman.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/jpeg/lib/libmman.a



INCLUDEPATH += D:\\opencv-2.4.5\\build\\install\\include

LIBS+= -LD:\\opencv-2.4.5\\build\\install\\lib \
    -lopencv_core245.dll\
    -lopencv_highgui245.dll\
    -lopencv_imgproc245.dll

INCLUDEPATH +=D:\\WorkSpace\\SourceCode\\c++lib\\QtDropbox-qt4.8\\src
LIBS += -LD:\\WorkSpace\\SourceCode\\c++lib\\QtDropbox-qt4.8\\src \
     -lQtDropbox

