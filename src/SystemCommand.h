#ifndef SYSTEMCOMMAND_H
#define SYSTEMCOMMAND_H

#include <array>
#include <QStringList>

class SystemCommands {

    static constexpr std::array<const char*, 3> INSTALL_CMD = {"pkexec", "dpkg", "-i"};
    static constexpr std::array<const char*, 3> REMOVE_CMD = {"pkexec", "dpkg", "-r"};
    static constexpr std::array<const char*, 3> UPDATE_CMD = {"pkexec", "apt", "update"};

public:

    static auto install() {
        return QStringList(INSTALL_CMD.begin(), INSTALL_CMD.end());
    }

    // Пример команды удаления (не используется в задании)
    static auto remove() {
        return QStringList(REMOVE_CMD.begin(), REMOVE_CMD.end());
    }

    // Пример команды обновления пакетов apt (не используется в задании)
    static auto update() {
        return QStringList(UPDATE_CMD.begin(), UPDATE_CMD.end());
    }
};

#endif // SYSTEMCOMMAND_H
