NAME         = Society
TARGET       = $${NAME}

QT           = core
QT          += gui
QT          += network
QT          += sql
QT          += script
QT          += positioning
QT          += widgets
QT          += opengl
QT          += printsupport
QT          += multimedia
QT          += multimediawidgets
QT          += opengl
QT          += QtCUDA
QT          += QtOpenCV
QT          += Essentials
QT          += QtCalendar
QT          += QtGMP
QT          += QtGSL
QT          += QtAlgebra
QT          += QtDiscrete
QT          += QtFFT
QT          += Mathematics
QT          += QtFuzzy
QT          += QtFLP
QT          += QtFoundation
QT          += QtGeometry
QT          += QtGadgets
QT          += QtComponents
QT          += QtManagers
QT          += QtVCF
QT          += DataWidgets
QT          += QtPhotography

load(qt_build_config)
load(qt_module)

INCLUDEPATH += $${PWD}/../../include/$${NAME}

HEADERS     += $${PWD}/../../include/$${NAME}/society.h

include ($${PWD}/History/History.pri)
include ($${PWD}/Organizations/Organizations.pri)
include ($${PWD}/Telecom/Telecom.pri)

OTHER_FILES += $${PWD}/../../include/$${NAME}/headers.pri
include ($${PWD}/../../doc/Qt/Qt.pri)

TRNAME       = $${NAME}

include ($${PWD}/../../Translations.pri)
