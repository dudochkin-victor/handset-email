/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef EMAILSETTINGSPAGE_H
#define EMAILSETTINGSPAGE_H

#include <MApplicationPage>
#include <MLinearLayoutPolicy>
#include <MAction>
#include <MButton>
#include <QStringList>

#include "accountinfo.h"
#include "qmailaccount.h"

class EmailSettingsPage : public MApplicationPage
{
    Q_OBJECT
    Q_DISABLE_COPY(EmailSettingsPage)

public:

    ~EmailSettingsPage();
    void appear(MSceneWindow::DeletionPolicy policy=KeepWhenDone);

    static EmailSettingsPage *instance();

    int frequency()          { return m_frequency; }
    bool emaialAlert()       { return m_emailAlert; }
    bool askBeforeDeleting() { return m_askBeforeDeleting; }
    QString signature ()     { return m_signature; }

    QStringList incomingServerTypes() { return m_inServerTypes; }
    QStringList sendServerTypes()     { return m_outServerTypes; }
    QStringList securityTypes()       { return m_securityTypes; }
    QStringList authTypes()           { return m_authTypes; }

signals:
    void addNewAccount();
    void editExistingAccount(QMailAccountId);

private slots:
    void frequencyChanged(int);
    void editSignature();
    void emailAlertChanged(bool);
    void askBeforeDeletingChanged(bool);
    void handleAccountSelection();
    void handleAccountsChanged(const QMailAccountIdList &);

private:  // private member functions
    explicit EmailSettingsPage();
    void updateAccountDisplayList();
    void updateAccounts();

private:  // private member data
    MLinearLayoutPolicy *m_policy;
    MWidgetController *m_accountsContainer;
    QHash < MButton *, QMailAccountId > m_accountButtons;

    // global email settings.
    int m_frequency;
    bool m_frequencyModified;
    bool m_emailAlert;
    bool m_askBeforeDeleting;
    QString m_signature;
    bool m_signatureModified;

    static QStringList m_frequencyList;
    static QStringList m_inServerTypes;
    static QStringList m_outServerTypes;
    static QStringList m_securityTypes;
    static QStringList m_authTypes;

    static EmailSettingsPage *m_instance;
};

#endif  // EMAILSETTINGSPAGE_H
