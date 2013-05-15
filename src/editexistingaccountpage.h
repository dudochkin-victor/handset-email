/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef EDITEXISTINGACCOUNTPAGE_H
#define EDITEXISTINGACCOUNTPAGE_H

#include <MApplicationPage>
#include <MLinearLayoutPolicy>
#include <MComboBox>
#include <MTextEdit>

#include <qmailstore.h>
#include <qmailaccount.h>
#include <qmailserviceaction.h>

class EditExistingAccountPage : public MApplicationPage
{
    Q_OBJECT

public:
    EditExistingAccountPage(const QMailAccountId &);
    ~EditExistingAccountPage();

signals:
    void accountReady();

private slots:
    void onUpdateAccount();
    void onDeleteAccount();
    void activityChanged(QMailServiceAction::Activity activity);
    void testAccountConfiguration();
    void displayRetrieveProgress(uint, uint);
    void displayTransmitProgress(uint, uint);
    void onInServerTypeChanged (int);
    void onInServerSecurityChanged (int);
    void onOutServerTypeChanged (int);
    void onOutServerSecurityChanged (int);

private:
    MLinearLayoutPolicy *m_policy;
    QMailAccountId       m_accountId;

    QStringList m_services;
    QMap<QString, QString> m_inSvcCfgValues;
    QMap<QString, QString> m_outSvcCfgValues;
    QString  m_inServiceKey;
    QString  m_outServiceKey;

    MTextEdit *m_displayName;
    MTextEdit *m_emailAddress;

    MComboBox *m_inServerType;
    MTextEdit *m_inServerAddress;
    MTextEdit *m_inServerPort;
    MComboBox *m_inServerSecurity;
    MTextEdit *m_inUsername;
    MTextEdit *m_inPassword;

    MComboBox *m_outServerType;
    MTextEdit *m_outServerAddress;
    MTextEdit *m_outServerPort;
    MComboBox *m_outAuthentication;
    MComboBox *m_outServerSecurity;
    MTextEdit *m_outUsername;
    MTextEdit *m_outPassword;

    QMailRetrievalAction *m_retrievalAction;
    QMailTransmitAction *m_transmitAction;
};

#endif // EDITEXISTINGACCOUNTPAGE_H
