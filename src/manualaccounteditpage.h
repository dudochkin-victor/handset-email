/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MANUALACCOUNTEDITPAGE_H
#define MANUALACCOUNTEDITPAGE_H

#include <MApplicationPage>
#include <MLabel>
#include <MTextEdit>
#include <MWidget>
#include <MComboBox>
#include <MLayout>
#include <MLinearLayoutPolicy>

#include "accountinfo.h"

class ManualAccountEditPage : public MApplicationPage
{
    Q_OBJECT

public:
    ManualAccountEditPage (AccountInfo *);
    virtual ~ManualAccountEditPage();
    virtual void createContent();

signals:
    void verifyAccount(AccountInfo *);

private slots:
    void displayReceiveSettings();
    void displaySendSettings();
    void reviewAccountSettings ();
    void onInServerTypeChanged (int);
    void onInServerSecurityChanged (int);
    void onOutServerTypeChanged (int);
    void onOutServerSecurityChanged (int);
    void onOutAuthenticationChanged (int);

private:
    AccountInfo *m_account;
    MLinearLayoutPolicy *m_policy;
    MWidget *m_sendSettingsWidget;
    MWidget *m_receiveSettingsWidget;

    MComboBox *m_inServerType;
    MTextEdit *m_inServerAddress;
    MTextEdit *m_inServerPort;
    MComboBox *m_inServerSecurity;
    MTextEdit *m_inServerUsername;
    MTextEdit *m_inServerPassword;

    MComboBox *m_outServerType;
    MTextEdit *m_outServerAddress;
    MTextEdit *m_outServerPort;
    MComboBox *m_outServerSecurity;
    MComboBox *m_outAuthentication;
    MTextEdit *m_outServerUsername;
    MTextEdit *m_outServerPassword;

    MTextEdit *m_senderName;
    MTextEdit *m_emailAddress;
};

#endif // MANUALACCOUNTEDITPAGE_H
