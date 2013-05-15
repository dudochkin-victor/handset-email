/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "emailutils.h"

QString Utils::bodyPlainText(const QMailMessagePartContainer *container)
{
    QMailMessageContentType contentType = container->contentType();
    if (container->hasBody() && contentType.type().toLower() == "text" && 
               contentType.subType().toLower() == "plain")
    {
        return container->body().data();
        
    }

    QString text("");
    for ( uint i = 0; i < container->partCount(); i++ ) 
    {
        QMailMessagePart messagePart = container->partAt(i);

        contentType = messagePart.contentType();
        if (messagePart.hasBody() && contentType.type().toLower() == "text" && 
                                     contentType.subType().toLower() == "plain")
        {
            text += messagePart.body().data() + "\n";
        } 
        else 
        {
            QMailMessagePart subPart;
            for (uint j = 0; j < messagePart.partCount(); j++) 
            {
                subPart = messagePart.partAt(j);
                contentType = subPart.contentType();
                if (subPart.hasBody() && contentType.type().toLower() == "text" && 
                                         contentType.subType().toLower() == "plain")
                    text += subPart.body().data() + "\n";
            }
        }
    }

    return text;
}

QString Utils::humanDateTime(const QDateTime &dateTime)
{
    // Default to system locale short date (17.09.09, 16.09.09, etc.)
    QString readable = dateTime.date().toString(Qt::SystemLocaleShortDate);

    QDateTime now = QDateTime::currentDateTime();

    int seconds = dateTime.secsTo(now);
    int hours = seconds / (60 * 60);
    int days = dateTime.daysTo(now);

    // See if it's in the last hour
    if ((seconds / 60) < 60) 
    {
        //% "%1 minutes ago"
        readable = QString(qtTrId("xx_minutes_ago").arg(seconds / 60));
    } 
    else if (hours < 6) // See if it's in the past 5 hours
    {
        if (hours == 1)
            //% "1 hour ago"
            readable = QString(qtTrId("xx_one_hour_ago"));
        else
            //% "%1 hours ago"
            readable = QString(qtTrId("xx_hours_ago").arg(hours));
    } 
    else if (days < 8) // See if it's in the past week
    {
        switch (days) {
        case 0:
            //% "Today"
            readable = QString(qtTrId("xx_today"));
            break;
        case 1:
            //% "Yesterday"
            readable = QString(qtTrId("xx_yesterday"));
            break;
        default:
            readable = dateTime.date().toString("dddd");
            break;
        }
    }

    return readable;
}
