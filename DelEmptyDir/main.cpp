#include <QCoreApplication>
#include <QDir>
#include <QFileInfoList>
#include <QDebug>



void DelEmptyDir(const QString& path) {
    const QDir cur_dir(path);

    auto files = cur_dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (auto file : files) {
        auto tpath = file.absoluteFilePath();
        DelEmptyDir(tpath);
    }

    //判断目录是否为空，为空则删除
    if (cur_dir.isEmpty()) {
        cur_dir.rmdir(path);
        qDebug() << "del" << path;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() != 2) {
        qDebug() << "DelEmptyDir path";
        return 0;
    }

    QString path = arguments.at(1);

    DelEmptyDir(path);

    qDebug() << "Finish...";

    return 0;
}
