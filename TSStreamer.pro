QT += quick network widgets printsupport

CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    tsstreamer.cpp \
    measinfo.cpp \
    qcustomplot.cpp \
    qmlplot.cpp \
    fftreal_vector.cpp \
    globaldata.cpp \
    systeminfo.cpp

RESOURCES += \
    qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    tsstreamer.h \
    measinfo.h \
    qcustomplot.h \
    qmlplot.h \
    fftreal_vector.h \
    iterator_templates.h \
    globaldata.h \
    systeminfo.h

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

#DEFINES += QCUSTOMPLOT_USE_OPENGL

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/libcrypto.so \
        $$PWD/libssl.so
}

RC_ICONS = ./pics/TSSTreamer.ico

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
