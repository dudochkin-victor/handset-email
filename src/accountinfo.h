/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H

#include <qmailaccount.h>
#include <qmailaccountconfiguration.h>
#include <qmailserviceaction.h>


class AccountInfo : public QObject
{
    Q_OBJECT

public:

    enum AuthenticationType {
        Authentication_None= 0,
        Authentication_Login = 1,
        Authentication_Plain = 2
    };

    AccountInfo();
//    explicit  AccountInfo(QMailAccount *account);
    ~AccountInfo();

    void setEmailAddress (const QString &emailAddress) {m_emailAddress = emailAddress; }
    void setDisplayName (const QString &name) { m_displayName = name; }

    void setInPassword (const QString &password) { m_inPassword = password; }
    void setInPort (int port) { m_inPort = port; }
    void setInSecurity (const QString &security) { m_inSecurity = security; }
    void setInServerAddress (const QString &address) { m_inServerAddress = address; }
    void setInServerType (const QString &serverType) { m_inServerType = serverType; }
    void setInUsername (const QString &username) { m_inUsername = username; }

    void setOutAuthentication (AuthenticationType authentication) { m_outAuthentication = authentication; }
    void setOutPassword (const QString &password) { m_outPassword = password; }
    void setOutPort (int port) { m_outPort = port; }
    void setOutSecurity (const QString &security) { m_outSecurity = security; }
    void setOutServerAddress (const QString &address) { m_outServerAddress = address; }
    void setOutServerType (const QString &serverType = "SMTP") { m_outServerType = serverType; }
    void setOutUsername (const QString &username) { m_outUsername = username; }

    QString displayName () { return m_displayName; }
    QString emailAddress () { return m_emailAddress; }

    QString inPassword () { return m_inPassword; }
    int inPort () { return m_inPort; }
    QString inSecurity () { return m_inSecurity; }
    QString inServerAddress () { return m_inServerAddress; }
    QString inServerType () { return m_inServerType; }
    QString inUsername () { return m_inUsername; }

    AuthenticationType outAuthentication () { return m_outAuthentication; }
    QString outPassword () { return m_outPassword; }
    int outPort () { return m_outPort; }
    QString outSecurity () { return m_outSecurity; }
    QString outServerAddress () { return m_outServerAddress; }
    QString outServerType () { return m_outServerType; }
    QString outUsername () { return m_outUsername; }

    bool accountIsValid ();

    QMailAccount *mailAccount() { return m_account; }

signals:
    void accountReady();

private slots:
    void activityChanged(QMailServiceAction::Activity activity);
    void testAccountConfiguration();
    void displayRetrieveProgress(uint, uint);
    void displayTransmitProgress(uint, uint);

private:
    void setStandardFolder(QMailAccount *, QMailFolder::StandardFolder, const QString&);

private:
    QMailAccount *m_account;
    QMailAccountConfiguration *m_config;

    QMailRetrievalAction *m_retrievalAction;
    QMailTransmitAction *m_transmitAction;

    QString m_emailAddress;
    QString m_displayName;

    QString m_inPassword;
    int m_inPort;
    QString m_inSecurity;
    QString m_inServerAddress;
    QString m_inServerType;
    QString m_inUsername;

    AuthenticationType m_outAuthentication;
    QString m_outPassword;
    int m_outPort;
    QString m_outSecurity;
    QString m_outServerAddress;
    QString m_outServerType;
    QString m_outUsername;
};

#endif // ACCOUNTINFO_H
