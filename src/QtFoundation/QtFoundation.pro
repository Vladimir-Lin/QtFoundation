NAME         = QtFoundation
TARGET       = $${NAME}

QT           = core
QT          -= gui
QT          += network
QT          += sql
QT          += script
QT          += QtMtAPI
QT          += QtCURL
QT          += QtCUDA
QT          += QtOpenCV
QT          += QtFFmpeg
QT          += QtAudio
QT          += QtPCAP
QT          += Essentials
QT          += QtCalendar
QT          += QtCryptography
QT          += QtMIME
QT          += SqlAnalyzer
QT          += QtGMP
QT          += QtGSL
QT          += QtAlgebra
QT          += QtDiscrete
QT          += QtFFT
QT          += Mathematics
QT          += QtFuzzy
QT          += QtFLP
QT          += QtGeography
QT          += NetProtocol
QT          += QtXmlRPC
QT          += QtUDT
QT          += QtFTP
QT          += QtRPC
QT          += QtJsonRPC
QT          += AudioIO
QT          += QtPhonemes
QT          += QtVocal
QT          += QtLinguistics
QT          += AudioIO
QT          += QtFinance
QT          += QtComprehend

load(qt_build_config)
load(qt_module)

INCLUDEPATH += $${PWD}/../../include/$${NAME}

HEADERS     += $${PWD}/../../include/$${NAME}/qtfoundation.h

include ($${PWD}/Defaults/Defaults.pri)
include ($${PWD}/Managers/Managers.pri)
include ($${PWD}/Monetary/Monetary.pri)
include ($${PWD}/Forex/Forex.pri)

OTHER_FILES += $${PWD}/../../include/$${NAME}/headers.pri

include ($${PWD}/../../doc/Qt/Qt.pri)

TRNAME       = $${NAME}
include ($${PWD}/../../Translations.pri)
