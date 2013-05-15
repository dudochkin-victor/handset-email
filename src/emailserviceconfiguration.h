/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef EMAILSERVICECONFIGURATION_H
#define EMAILSERVICECONFIGURATION_H

#include <qmailserviceconfiguration.h>

class EmailServiceConfiguration : public QMailServiceConfiguration
{

public:
    EmailServiceConfiguration(QMailAccountConfiguration *config, const QString& service);

    void setIntValue (const QString & name, int  value);
    QString encode (const QString& value);
    QString decode (const QString& value);
};

#endif // EMAILSERVICECONFIGURATION_H
