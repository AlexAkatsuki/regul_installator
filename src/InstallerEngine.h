// #ifndef INSTALLERENGINE_H
// #define INSTALLERENGINE_H

// #include <QObject>
// #include <QString>
// #include <QStringList>
// #include <QProcess>
// #include <QMap>
// #include <QDir>

// struct PackageInfo {
//     QString displayName;
//     QStringList debFiles;
// };

// class InstallerEngine : public QObject
// {
//     Q_OBJECT

// public:
//     explicit InstallerEngine(QObject *parent = nullptr);

//     bool loadPackages();
//     QStringList getAvailablePackages() const;
//     void installPackage(const QString &packageName);
//     QString getInstallStatus() const;

// signals:
//     void installationStarted();
//     void installationProgress(const QString &message);
//     void installationFinished(bool success);
//     void installationError(const QString &error);

// private slots:
//     void onProcessFinished(int exitCode);
//     void onProcessErrorOccurred(QProcess::ProcessError error);
//     void readProcessOutput();

// private:
//     QMap<QString, QStringList> m_packages;
//     QProcess *m_process;
//     QString m_currentStatus;

//     int m_currentInstallationStage;
//     QStringList m_packagesToInstall;
//     QString m_currentPackageName;

//     QStringList readPackageFile(const QString &resourcePath);
//     QString getPackageDisplayName(const QString &filename);
//     void executeRealInstallation(const QStringList &packagesToInstall, const QString &packageName);
//     void startUpdateProcess();
//     void startInstallProcess();
//     void executeCommand(const QStringList &command);

//     QStringList readPackageList(const QString &resourcePath);

//     PackageInfo readPackageInfo(const QString &resourcePath);
// };

// #endif // INSTALLERENGINE_H


#ifndef INSTALLERENGINE_H
#define INSTALLERENGINE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QMap>
#include <QDir>
#include <QTemporaryDir>

struct PackageInfo {
    QString displayName;
    QStringList debFiles;
};

class InstallerEngine : public QObject {
    Q_OBJECT

public:
    explicit InstallerEngine(QObject *parent = nullptr);
    ~InstallerEngine();

    void installPackage(const QString &packageName);

    bool loadPackages();

    QString getInstallStatus() const;
    QStringList getAvailablePackages() const;

signals:
    void installationStarted();
    void installationProgress(const QString &message);
    void installationFinished(bool success);
    void installationError(const QString &error);

private slots:
    void onProcessFinished(int exitCode);
    void onProcessErrorOccurred(QProcess::ProcessError error);
    void readProcessOutput();

private:
    int m_currentInstallationStage;

    QString m_currentPackageName;
    QString m_currentStatus;
    QStringList m_packagesToInstall;

    QMap<QString, QStringList> m_packages;

    QProcess *m_process;
    QTemporaryDir *m_tempDir;

    void executeRealInstallation(const QString &packageName);
    void startLocalInstallation();
    void executeCommand(const QStringList &command);

    bool extractPackagesToTemp();
    bool extractPackage(const QString &resourcePath, const QString &filename);

    QString getPackageDisplayName(const QString &filename);
    QString findResourceFile(const QString &filename);

    PackageInfo readPackageInfo(const QString &resourcePath);
};

#endif // INSTALLERENGINE_H
