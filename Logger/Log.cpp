#include "Log.h"
#include "../Inc/CFlashToolAPI.h"

void LogOperationMgr::LogSwitch(bool enabled_log)
{
    if(enabled_log) {
        DebugLogsOn();
    } else {
        DebugLogsOff();
    }
}

void LogOperationMgr::ResetLogPath(const std::string &new_log_path, bool need_restart_log)
{
    if (need_restart_log) {
        LogOperationMgr::DebugLogsOff();
    }
    Logger::GetLogger().ResetLogPath(new_log_path);
    if (need_restart_log) {
        LogOperationMgr::DebugLogsOn();
    }
}

void LogOperationMgr::DebugLogsOn()
{
    Logger::GetLogger().DebugOn();

    QString log_folder = QString::fromStdString(Logger::GetLogger().GetSPFlashToolDumpFileFolder());
    CFlashToolAPI::cflashtool_env_set_log(logging_level_e::kdebug, log_folder);
}

void LogOperationMgr::DebugLogsOff()
{
    QString log_folder = QString::fromStdString(Logger::GetLogger().GetSPFlashToolDumpFileFolder());
    CFlashToolAPI::cflashtool_env_set_log(logging_level_e::koff, log_folder);

    Logger::GetLogger().DebugOff();
}
