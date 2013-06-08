/*
Copyright (c) 2013 Ronie Martinez (ronmarti18@gmail.com)
All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301  USA
*/

#include "qpspplugin.h"
#include "qpsphandler.h"


QPspPlugin::QPspPlugin(QObject *parent) :
    QImageIOPlugin(parent)
{
}

QPspPlugin::~QPspPlugin()
{
}

QStringList QPspPlugin::keys() const
{
    return QStringList() << "psp";
}

QImageIOPlugin::Capabilities QPspPlugin::capabilities(
    QIODevice *device, const QByteArray &format) const
{
    if (format == "psp")
        return Capabilities(CanRead);//TODO: add CanWrite support

    if (!(format.isEmpty() && device->isOpen()))
        return 0;

    Capabilities cap;
    if (device->isReadable() && QPspHandler::canRead(device))
        cap |= CanRead;
    return cap;
}

QImageIOHandler *QPspPlugin::create(
    QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new QPspHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(QPspPlugin)
Q_EXPORT_PLUGIN2(QPsp, QPspPlugin)
#endif // QT_VERSION < 0x050000
