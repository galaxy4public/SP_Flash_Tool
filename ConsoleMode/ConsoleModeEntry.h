#ifndef CONSOLEMODEENTRY_H
#define CONSOLEMODEENTRY_H

#include <memory>
#include <QString>

namespace ConsoleMode
{

class CommandLineArguments;

class ConsoleModeEntry
{
public:
    ConsoleModeEntry();
    virtual ~ConsoleModeEntry();

    int run(const QStringList &arguments);

private:
    void openAndCleanLogs(std::shared_ptr<CommandLineArguments> &cmd_args) const;

private:
    ConsoleModeEntry(const ConsoleModeEntry &) = delete;
    ConsoleModeEntry & operator=(const ConsoleModeEntry &) = delete;

    ConsoleModeEntry(const ConsoleModeEntry &&) = delete;
    ConsoleModeEntry & operator=(const ConsoleModeEntry &&) = delete;
};

}

#endif // CONSOLEMODEENTRY_H
