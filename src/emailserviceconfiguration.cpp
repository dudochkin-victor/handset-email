/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "emailserviceconfiguration.h"

EmailServiceConfiguration::EmailServiceConfiguration (QMailAccountConfiguration *config, 
                                                       const QString& service)
    : QMailServiceConfiguration (config, service)
{
}

void EmailServiceConfiguration::setIntValue (const QString& name, int  value)
{
    setValue (name, QString::number(value));
}

QString EmailServiceConfiguration::encode (const QString& value)
{
    return (encodeValue(value));
}

QString EmailServiceConfiguration::decode (const QString& value)
{
    return (decodeValue(value));
}
