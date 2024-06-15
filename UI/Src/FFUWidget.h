#ifndef FFUWIDGET_H
#define FFUWIDGET_H

#include "TabWidgetBase.h"
#include "../../Cmd/CmdWrapper.h"
#include "../../Inc/CFlashToolTypes.h"

namespace Ui {
class FFUWidget;
}

class MainWindow;

class FFUWidget : public TabWidgetBase
{
    Q_OBJECT

public:
    FFUWidget(MainWindow *mainwindow, QTabWidget *parent);
    ~FFUWidget();

    DECLARE_TABWIDGET_VFUNCS()

    CONN_END_STAGE getConnStage() const;

private slots:
    void on_toolButton_Firmwate_Update_clicked();
    void on_toolButton_UFS_Config_clicked();
    void on_toolButton_Stop_clicked();
    void on_toolButton_openFwFile_clicked();
    void on_checkBox_tw_size_stateChanged(int state);
    void on_checkBox_hpb_size_stateChanged(int state);
    void on_checkBox_hpb_pinned_region_stateChanged(int state);
    void on_checkBox_lu3_setting_stateChanged(int state);
    void on_checkBox_ctrl_mode_stateChanged(int state);

private:
    std::shared_ptr<CmdWrapper> createFirmwareUpdateCmdWrapper();
    std::shared_ptr<CmdWrapper> createUFSConfigCmdWrapper();

private:
    Ui::FFUWidget *ui;
    MainWindow *m_mainwindow;
};

#endif // FFUWIDGET_H
