//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#ifndef VANDALNW_H
#define VANDALNW_H

#include <huggle_core/definitions.hpp>

#include <QDockWidget>
#include <QHash>
#include <QTimer>
#include <QUrl>

namespace Ui
{
    class VandalNw;
}

namespace libircclient
{
    class Channel;
    class Server;
    class Parser;
    class User;
    class Network;
}

namespace Huggle
{
    class WikiPage;
    class WikiEdit;
    class WikiUser;
    class WikiSite;

    //! This namespace contains HAN classes

    //! Huggle Antivandalism Network is a system that allows users of huggle and other tools
    //! cooperate with each other so that they are more effective
    namespace HAN
    {
        enum MessageType
        {
            MessageType_User,
            MessageType_Bot,
            MessageType_UserTalk,
            MessageType_Info
        };

        //! This is base class that can be used to store information about HAN items

        //! These "HAN items" are for example information about rollbacks, because they
        //! share common properties, they are all inherited from this GenericItem :o
        class HUGGLE_EX_UI GenericItem
        {
            public:
                GenericItem(WikiSite *site);
                GenericItem(WikiSite *site, revid_ht _revID, QString _user, QString _ident, QString _hostname);
                GenericItem(const GenericItem &i);
                GenericItem(GenericItem *i);
                //! User who changed the edit
                QString User;
                QString Ident;
                QString Host;
                WikiSite *Site;
                //! ID of edit
                revid_ht RevID;
        };

        //! This class is used to store information regarding page rescoring

        //! Rescoring is an event when other user define own score for some edit,
        //! this score is then added to our own score, problem is that these scores
        //! can't be applied to edits that weren't parsed by huggle yet. So in case
        //! that other user has faster internet connection we cache the information
        //! using this class and use it later when edit is parsed
        class HUGGLE_EX_UI RescoreItem : public GenericItem
        {
            public:
                RescoreItem(WikiSite *site, revid_ht _revID, long _score, QString _user, QString _ident, QString _hostname);
                RescoreItem(const RescoreItem &item);
                RescoreItem(RescoreItem *item);
                long Score;
        };

    }

    //! Vandalism network

    //! Huggle 3 comes with a system that allows all clients to operate together in order
    //! to be effective in reverting of vandalism
    class HUGGLE_EX_UI VandalNw : public QDockWidget
    {
            Q_OBJECT

            /// \todo We should make a user list of other huggle users here
            /// \todo Share a version of your huggle with others in sane way
            /// \todo Hook to VERSION
        public:
            static QString SafeHtml(QString text);
            static QString GenerateWikiDiffLink(QString text, QString revid, WikiSite *site);

            explicit VandalNw(QWidget *parent = nullptr);
            ~VandalNw();
            /*!
             * \brief Insert text to window
             * \param text is a string that will be inserted to window, must not be terminated with newline
             */
            void Insert(QString text, HAN::MessageType type);
            void Connect();
            void Disconnect();
            bool IsConnected();
            //! This will deliver an edit to others as a good edit
            void Good(WikiEdit *edit);
            //! Notify others about a rollback of edit
            void Rollback(WikiEdit *edit);
            void SuspiciousWikiEdit(WikiEdit *edit);
            void WarningSent(WikiUser *user, byte_ht level);
            void GetChannel();
            bool IsParsed(WikiEdit *edit);
            void Rescore(WikiEdit *edit);
            void SendMessage();
            void SendMessage(QString text);
            //! For debugging only
            void WriteTest(WikiEdit *edit);
            bool IsBot(QString nick, QString host);
            QHash<QString,WikiSite*> Ch2Site;
            QHash<WikiSite*,QString> Site2Channel;
            //! Prefix to special commands that are being sent to network to other users
            QString Prefix;
            QList<HAN::RescoreItem> UnparsedScores;
            QList<HAN::GenericItem> UnparsedGood;
            QList<HAN::GenericItem> UnparsedRoll;
            QList<HAN::GenericItem> UnparsedSusp;
        private:
            void ProcessGood(WikiEdit *edit, QString nick, QString ident, QString host);
            void ProcessRollback(WikiEdit *edit, QString nick, QString ident, QString host);
            void ProcessSusp(WikiEdit *edit, QString nick, QString ident, QString hn);
            //! Reload user list
            void refreshUL();
            void ProcessCommand(WikiSite *site, QString nick, QString ident, QString host, QString message);
            Ui::VandalNw *ui;
            //! Pointer to irc server
            libircclient::Network *irc;
            //! Using this we track if channel was joined or not, because we need to send
            //! the request some time after connection or irc server would skip it
            bool JoinedMain;
        private slots:
            void OnGood(WikiEdit *edit);
            void OnSuspicious(WikiEdit *edit);
            void OnWarning(WikiUser *user, byte_ht level);
            void OnRevert(WikiEdit *edit);
            void on_pushButton_clicked();
            void on_lineEdit_returnPressed();
			//The following function should be replaced with an equivalent from the QPlainTextEdit class.
            void TextEdit_anchorClicked(QString link);
            // IRC related
            void OnIRCUserJoin(libircclient::Parser *px, libircclient::User *user, libircclient::Channel *channel);
            void OnIRCSelfJoin(libircclient::Channel *channel);
            void OnIRCChannelNames(libircclient::Parser *px);
            void OnIRCNetworkFailure(QString reason, int code);
            void OnIRCUserPart(libircclient::Parser *px, libircclient::Channel *channel);
            void OnIRCSelfPart(libircclient::Parser *px, libircclient::Channel *channel);
            void OnIRCChannelMessage(libircclient::Parser *px);
            void OnIRCChannelCTCP(libircclient::Parser *px, QString command, QString parameters);
            void OnIRCChannelQuit(libircclient::Parser *px, libircclient::Channel *channel);
            void OnIRCLoggedIn(libircclient::Parser *px);
            void OnConnected();
            void OnDisconnected();
    };
}

#endif // VANDALNW_H
