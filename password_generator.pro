QT += core gui widgets

TARGET = PasswordGenerator
TEMPLATE = app

SOURCES += main.cpp mainwindow.cpp
HEADERS += mainwindow.h

# Configuración para OpenSSL
INCLUDEPATH += /usr/include/openssl
LIBS += -L/usr/lib -lssl -lcrypto

# Usar C++17 para compatibilidad
CONFIG += c++17