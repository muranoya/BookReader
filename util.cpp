#include "util.h"

QString BookReader::Util::connectFilePath(const QString &parent, const QString &child)
{
    if (parent.isEmpty())
    {
        return QString();
    }

    if (child.isEmpty())
    {
        return parent;
    }

    if (parent.at(parent.length()-1) == QDir::separator())
    {
        return parent + child;
    }
    else
    {
        return parent + QDir::separator() + child;
    }
}
