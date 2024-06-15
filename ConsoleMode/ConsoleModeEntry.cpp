#include "ConsoleModeEntry.h"
#include "CommandLineArguments.h"
#include "../Logger/Log.h"
#include "../Utility/FileUtils.h"
#include "../Utility/version.h"
#include "../Utility/LogFilesClean.h"
#include "Config.h"
#include "CommandSettingValidator.h"
#include "ConsoleModeRunner.h"
#include "../Err/Exception.h"
#include "../Network/ConsoleModeNetworkIntf.h"
#include "../Cmd/ConsoleModeCallBack.h"
#include <cassert>

namespace ConsoleMode {

#ifdef _WIN32
#include <Windows.h>
class ConsoleSentry
{
public:
    ConsoleSentry(bool do_redirect)
    {
        attached_ = AttachConsole(ATTACH_PARENT_PROCESS);

        if (attached_ && (!do_redirect))
        {
            freopen("conin$", "r+t", stdin);
            freopen("conout$", "w+t", stdout);
            freopen("conout$", "w+t", stderr);
            /*
            BOOL WINAPI SetConsoleCtrlHandler(
              _In_opt_  PHANDLER_ROUTINE HandlerRoutine,
              _In_      BOOL Add
            );
            */
        }

    }

    ~ConsoleSentry()
    {
        if (attached_)
        {
            FreeConsole();
        }
    }

private:
    bool attached_;
};
#endif

ConsoleModeEntry::ConsoleModeEntry()
{
    ConsoleModeCallBack::setConsoleModeEntry(this);
}

ConsoleModeEntry::~ConsoleModeEntry()
{

}

int ConsoleModeEntry::run(const QStringList &arguments)
{
    try {
        std::shared_ptr<CommandLineArguments> cmdArg(new CommandLineArguments());
        CmdLineParseResult parser_result = cmdArg->parseCmdLine(arguments);

#ifdef _WIN32
        ConsoleSentry consoleSentry(cmdArg->needRedirect());
#endif

        openAndCleanLogs(cmdArg);

        switch (parser_result) {
        case CmdLineParseResult::CommandLineHelpRequested:
            LOGI("%s", qPrintable(cmdArg->getHelpInfo()));
            return 0;
        case CmdLineParseResult::CommandLineVersionRequested:
            LOGI("%s", qPrintable(cmdArg->getVersionInfo()));
            return 0;
        case CmdLineParseResult::CommandLineError:
            LOGE("Invalid parameter:\n%s", qPrintable(cmdArg->getParseErrorMsg()));
            LOGI("\\\n%s\n", qPrintable(cmdArg->getHelpInfo()));
            return -1;
        case CmdLineParseResult::CommandLineOk:
            LOGI("Command line arguments parse pass!\n");
            break;
        default:
            LOGE("Error: Unknow command line parse results!\n");
            return -1;
        }

        if(!cmdArg->validArgumentValues())
        {
            return -1;
        }

        LOGI("Begin");
        LOGI("%s", ToolInfo::ToolName().c_str());
        LOGI("Build Time: " __DATE__ " " __TIME__);

#ifdef _INTERNAL_PHONE_TRACKING_MODE
        ConsoleModeNetworkIntf consoleModeNetworkIntf;
        if (cmdArg->getCurrentMode() == ConsoleModeCurrentMode::CmdArgumentMode) {
            consoleModeNetworkIntf.setFlashXMLFile(cmdArg->getFlashXMLFilename());
            consoleModeNetworkIntf.startNetworkThread();
        }
#endif

#if defined(_LINUX64)
        FileUtils::copy_99ttyacms_file();
        FileUtils::runModemManagerCmd();
#endif

        Config config(cmdArg);

        //check validation of command setting
        CommandSettingValidator cmdSettingValidator(config, cmdArg);
        if (!cmdSettingValidator.validate())
        {
            return -1;
        }

#ifdef _INTERNAL_PHONE_TRACKING_MODE
        if (cmdArg->getCurrentMode() == ConsoleModeCurrentMode::ConfigXMLMode) {
            consoleModeNetworkIntf.setFlashXMLFile(config.getGeneralSetting()->getFlashXmlFile());
            consoleModeNetworkIntf.startNetworkThread();
        }
#endif

        if (!cmdSettingValidator.hasRSCXMLFile())
        {
            config.getCommandSetting()->removeRSCSetting();
        }

        ConsoleModeRunner runner;
        if (config.getCommandSetting()->hasDownloadCmd()) {
            LOGI("Begin verify load images checksum");
            runner.imageChksumVerify(config.getGeneralSetting()->getFlashXmlFile());
            LOGI("verify load images checksum done!");
        }

#ifdef _INTERNAL_PHONE_TRACKING_MODE
        runner.execute(config, &consoleModeNetworkIntf);
#else
        runner.execute(config, nullptr);
#endif
        LOGI("All command exec done!");
        return 0;
    } catch (const BaseException &e) {
        QString error_msg = e.err_msg() + QStringLiteral("\n") + e.context();
        LOGE("BaseException: err_msg[%s]", qPrintable(error_msg));
    } catch (const std::exception &e) {
        LOGE("std::exception: err_msg[%s]", e.what());
    } catch (...) {
        LOGE("Unknown error ocurred.");
    }
    return 1;
}

void ConsoleModeEntry::openAndCleanLogs(std::shared_ptr<CommandLineArguments> &cmd_args) const
{
    try {
        if (cmd_args->getCurrentMode() == ConsoleModeCurrentMode::ConfigXMLMode && cmd_args->validConfigXMLFile()) {
            std::shared_ptr<LogInfoSetting> sp_logInfoSetting = Config::getLogInfoSetting(cmd_args->getConfigXMLFileName());
            if (sp_logInfoSetting) {
                //using the log path configured in config.xml
                LogOperationMgr::ResetLogPath(sp_logInfoSetting->getLogPath().toStdString(), false);
                LogCleaner::cleanLogFiles(sp_logInfoSetting->getLogPath(), -(sp_logInfoSetting->getCleanHours()));
            }
        }
    } catch (...) {
        // pass
    }
    LogOperationMgr::LogSwitch(true);
}

}
