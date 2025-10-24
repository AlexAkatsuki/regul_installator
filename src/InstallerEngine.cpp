#include <QFile>
#include <QTextStream>
#include <QResource>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>

#include "InstallerEngine.h"

InstallerEngine::InstallerEngine(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_tempDir(new QTemporaryDir()) {

    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &InstallerEngine::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred,
            this, &InstallerEngine::onProcessErrorOccurred);
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &InstallerEngine::readProcessOutput);
    connect(m_process, &QProcess::readyReadStandardError,
            this, &InstallerEngine::readProcessOutput);
}

InstallerEngine::~InstallerEngine() {
    delete m_tempDir;
}

bool InstallerEngine::loadPackages() {

    m_packages.clear();

    QStringList listFiles;
    QDirIterator it(":/packages", QStringList() << "*.list",
                                QDir::Files, QDirIterator::Subdirectories);

    int foundCount = 0;
    while (it.hasNext()) {
        QString filePath = it.next();
        listFiles << filePath;
        foundCount++;
    }

    if (listFiles.isEmpty()) {

        QDir resourceDir(":/");
        QStringList allFiles = resourceDir.entryList(QDir::Files |
                                                     QDir::NoDotAndDotDot);

        foreach (const QString &file, allFiles) {
            if (file.endsWith(".list"))
                listFiles << ":/" + file;
        }
    }

    if (listFiles.isEmpty())
        return false;

    foreach (const QString &listFilePath, listFiles) {

        PackageInfo packageInfo = readPackageInfo(listFilePath);

        if (!packageInfo.displayName.isEmpty() && !packageInfo.debFiles.isEmpty())
            m_packages[packageInfo.displayName] = packageInfo.debFiles;
    }

    return !m_packages.isEmpty();
}

PackageInfo InstallerEngine::readPackageInfo(const QString &resourcePath) {

    PackageInfo info;

    QFile file(resourcePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QStringList allLines;

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty() && !line.startsWith('#')) {
                allLines.append(line);
            }
        }
        file.close();

        if (allLines.isEmpty())
            return info;

        info.displayName = allLines[0];

        for (int i = 1; i < allLines.size(); ++i) {
            QString debFile = allLines[i];

            QString packageDir = QFileInfo(resourcePath).dir().dirName();
            QString fullDebPath = packageDir + "/" + debFile;

            info.debFiles.append(fullDebPath);
        }

    }

    return info;
}

QStringList InstallerEngine::getAvailablePackages() const {
    return m_packages.keys();
}

void InstallerEngine::installPackage(const QString &packageName) {

    if (!m_packages.contains(packageName)) {
        emit installationError(tr("Пакет не найден: %1").arg(packageName));
        return;
    }

    if (m_process->state() == QProcess::Running) {
        m_process->kill();
        m_process->waitForFinished(3000);
    }

    m_packagesToInstall = m_packages[packageName];
    m_currentPackageName = packageName;

    emit installationStarted();
    emit installationProgress(tr("Начало установки %1").arg(packageName));

    executeRealInstallation(packageName);
}

QString InstallerEngine::getInstallStatus() const {
    return m_currentStatus;
}

void InstallerEngine::executeRealInstallation(const QString &packageName) {

    m_currentInstallationStage = 1;

    emit installationProgress(tr("Извлечение пакетов..."));

    if (!extractPackagesToTemp()) {
        emit installationError(tr("Ошибка извлечения пакетов"));
        return;
    }

    emit installationProgress(tr("Пакеты извлечены"));

    startLocalInstallation();
}

bool InstallerEngine::extractPackagesToTemp() {

    foreach (const QString &debFile, m_packagesToInstall) {
        QString resourcePath = ":/packages/" + debFile;

        QString filename = QFileInfo(debFile).fileName();

        if (!extractPackage(resourcePath, filename))
            return false;
    }
    return true;
}

bool InstallerEngine::extractPackage(const QString &resourcePath,
                                     const QString &filename) {

    QFile resourceFile(resourcePath);
    if (!resourceFile.exists())
        return false;

    QString tempFilePath = m_tempDir->path() + "/" + filename;

    if (QFile::exists(tempFilePath))
        QFile::remove(tempFilePath);

    if (resourceFile.copy(tempFilePath)) {

        QFileInfo tempFileInfo(tempFilePath);

        QFile tempFile(tempFilePath);
        tempFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner |
                                QFile::ReadUser  | QFile::ReadOther);
        return true;
    } else
        return false;
}

void InstallerEngine::startLocalInstallation() {

    m_currentInstallationStage = 2;
    emit installationProgress(tr("🔧 Установка пакетов..."));

    QStringList debPaths;
    foreach (const QString &debFile, m_packagesToInstall) {
        QString filename = QFileInfo(debFile).fileName();
        QString tempFilePath = m_tempDir->path() + "/" + filename;
        debPaths.append(tempFilePath);
    }
    foreach (const QString &debPath, debPaths) {
        if (!QFile::exists(debPath)) {
            emit installationProgress(tr("❌ Ошибка: файл пакета не найден"));
            emit installationFinished(false);
            return;
        }
    }

    QStringList installCommand = {"pkexec", "dpkg", "-i"};
    installCommand.append(debPaths);

    executeCommand(installCommand);
}

void InstallerEngine::executeCommand(const QStringList &command) {

    if (m_process->state() == QProcess::Running) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }

    m_process->start(command[0], command.mid(1));

    if (!m_process->waitForStarted(5000)) {
        emit installationError(tr("Не удалось запустить процесс: %1").
                                            arg(m_process->errorString()));
        return;
    }
}

void InstallerEngine::onProcessFinished(int exitCode) {

    QProcess::ExitStatus exitStatus = m_process->exitStatus();

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit installationProgress(tr("Пакет %1 установлен успешно!").
                                                arg(m_currentPackageName));
        emit installationFinished(true);
    } else {
        emit installationProgress(tr("Ошибка установки пакета %1").
                                                arg(m_currentPackageName));
        emit installationFinished(false);
    }
}

void InstallerEngine::onProcessErrorOccurred(QProcess::ProcessError error) {

    QString errorMsg = tr("❌ Ошибка во время установки");

    emit installationProgress(errorMsg);
    emit installationError(errorMsg);
    emit installationFinished(false);
}

void InstallerEngine::readProcessOutput() {

    QString output = m_process->readAllStandardOutput();
    if (!output.trimmed().isEmpty())
        emit installationProgress(output.trimmed());

    QString errorOutput = m_process->readAllStandardError();
    if (!errorOutput.trimmed().isEmpty())
        emit installationProgress(tr("%1").arg(errorOutput.trimmed()));
}

QString InstallerEngine::getPackageDisplayName(const QString &filename) {

    QString name = filename;
    name.replace("_", " ");
    name.replace("-", " ");

    if (!name.isEmpty())
        name[0] = name[0].toUpper();

    return name;
}
