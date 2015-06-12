#ifndef STACKTRACE_WIN_DLG_H
#define STACKTRACE_WIN_DLG_H

#include <QTextStream>
#include <QClipboard>
#include "boost/version.hpp"
#include "libtorrent/version.hpp"
#include "ui_stacktrace_win_dlg.h"

class StraceDlg: public QDialog, private Ui::errorDialog
{
    Q_OBJECT

public:
    StraceDlg(QWidget* parent = 0): QDialog(parent)
    {
        setupUi(this);
    }

    ~StraceDlg()
    {
    }

    void setStacktraceString(const QString& trace)
    {
        QString htmlStr;
        QTextStream outStream(&htmlStr);
        outStream << "<p align=center><b><font size=7 color=red>" <<
            "nodecast has crashed" <<
            "</font></b></p>" <<
            "<font size=4>" <<
            "<p>" <<
            "Please report a bug at <a href=\"http://bugs.nodecast.org\">" <<
            "http://bugs.nodecast.org</a>" <<
            " and provide the following backtrace." <<
            "</p>" <<
            "</font>" <<
            "<br/><hr><br/>" <<
            "<p align=center><font size=4>nodecast version: " << VERSION <<
            "<br/>Libtorrent version: " << LIBTORRENT_VERSION <<
            "<br/>Qt version: " << QT_VERSION_STR <<
            "<br/>Boost version: " << QString::number(BOOST_VERSION / 100000) << '.' <<
            QString::number((BOOST_VERSION / 100) % 1000) << '.' <<
            QString::number(BOOST_VERSION % 100) << "</font></p><br/>"
            "<pre><code>" <<
            trace <<
            "</code></pre>" <<
            "<br/><hr><br/><br/>";

        errorText->setHtml(htmlStr);
    }
};

#endif
