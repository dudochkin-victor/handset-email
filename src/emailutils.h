/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef _EMAILUTILS_H
#define _EMAILUTILS_H

#include <QString>
#include <QDateTime>

#include <qmailmessage.h>

class Utils: public QObject
{
Q_OBJECT
public:

    /**
     * Get the text for the body of the message
     * @param mail the mail message to get the body of
     * @returns string containing content of body of the message
     */
    static QString bodyPlainText(const QMailMessagePartContainer *mail);

   /**
     * generate a human readable date/time for the view
     * @param dateTime the datetime of the message
     * @returns a string in the following format:
     *  1 minute ago - 59 minutes ago
     *  1 hour ago - 5 hours ago
     *  today yesterday Thursday Friday Saturday Sunday Monday
     *  17.09.09 16.09.09 etc...
     */
    static QString humanDateTime(const QDateTime &dateTime);
};

#endif
