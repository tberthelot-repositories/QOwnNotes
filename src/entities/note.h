#ifndef NOTE_H
#define NOTE_H

#include <utils/misc.h>

#include <QDateTime>
#include <QFile>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QUrl>

class Bookmark;
class NoteSubFolder;

#define NOTE_TEXT_ENCRYPTION_PRE_STRING "<!-- BEGIN ENCRYPTED TEXT --"
#define NOTE_TEXT_ENCRYPTION_POST_STRING "-- END ENCRYPTED TEXT -->"
#define BOTAN_SALT "Gj3%36/SmPoe12$snNAs-A-_.),?faQ1@!f32"

typedef enum mediaType {
	image,
	attachment};
	
class Note {
   public:
    explicit Note();

    int getId() const;

    QString getName() const;

    QString getFileName() const;

    QString getNoteText() const;

    bool getHasDirtyData() const;

    void setHasDirtyData(const bool hasDirtyData);

    void setName(QString text);

    void setNoteText(QString text);

    qint64 getCryptoKey() const;

    QString getCryptoPassword() const;

    void setCryptoKey(const qint64 cryptoKey);

    static bool addNote(const QString &name, const QString &fileName,
                        const QString &text);

    static Note fetch(int id);

    static Note fetchByName(const QRegularExpression &regExp,
                            int noteSubFolderId = -1);

    static Note fetchByFileName(const QString &fileName,
                                int noteSubFolderId = -1);

    static Note fetchByFileName(const QString &fileName,
                                const QString &noteSubFolderPathData);

    static Note fetchByName(const QString &name, int noteSubFolderId = -1);

    static Note fetchByName(const QString &name,
        const QString &noteSubFolderPathData);

    static QVector<Note> fetchAll(int limit = -1);

    static QVector<Note> fetchAllNotTagged(int activeNoteSubFolderId);

    static QVector<int> fetchAllNotTaggedIds();

    static int countAllNotTagged(int activeNoteSubFolderId = -1);

    static QVector<Note> search(const QString &text);

    static QVector<QString> searchAsNameListInCurrentNoteSubFolder(
        const QString &text, bool searchInNameOnly = false);

    static QVector<QString> searchAsNameList(const QString &text,
                                             bool searchInNameOnly = false);

    static QStringList fetchNoteNamesInCurrentNoteSubFolder();

    static QStringList fetchNoteNames();

    static QStringList fetchNoteFileNames();

    static Note noteFromQuery(const QSqlQuery &query);

    bool store();

    bool storeNewText(QString text);

    bool storeNoteTextFileToDisk();

    static QString defaultNoteFileExtension();

    static QStringList customNoteFileExtensionList(
        const QString &prefix = QString());

    static QString getFullFilePathForFile(const QString &fileName);

    QString getFilePathRelativeToNote(const Note &note) const;

    static int storeDirtyNotesToDisk(Note &currentNote,
                                     bool *currentNoteChanged = Q_NULLPTR,
                                     bool *noteWasRenamed = Q_NULLPTR);

    bool updateNoteTextFromDisk();

    friend QDebug operator<<(QDebug dbg, const Note &note);

    void createFromFile(QFile &file, int noteSubFolderId = 0,
                        bool withNoteNameHook = false);

    static bool deleteAll();

    bool fileExists() const;

    bool fileWriteable() const;

    bool exists() const;

    bool refetch();

    void fillFromQuery(const QSqlQuery &query);

    bool fillByFileName(const QString &fileName, int noteSubFolderId = -1);

    bool removeNoteFile();

    bool remove(bool withFile = false);

    QString toMarkdownHtml(const QString &notesPath, int maxImageWidth = 980,
                           bool forExport = false, bool decrypt = true,
                           bool base64Images = false);

    bool isFetched() const;

    bool copyToPath(const QString &destinationPath,
                    QString noteFolderPath = QString());

    bool moveToPath(const QString &destinationPath,
                    const QString &noteFolderPath = QString());

    static QString generateTextForLink(QString text);

    static qint64 qint64Hash(const QString &str);

    QString encryptNoteText();

    QString getDecryptedNoteText() const;

    bool hasEncryptedNoteText() const;

    void setCryptoPassword(const QString &password);

    bool canDecryptNoteText() const;

    static bool expireCryptoKeys();

    QUrl fullNoteFileUrl() const;

    QString fullNoteFilePath() const;

    QString fullNoteFileDirPath() const;

    static QString encodeCssFont(const QFont &refFont);

    void setDecryptedNoteText(QString text);

    bool storeNewDecryptedText(QString text);

    void setDecryptedText(QString text);

    QDateTime getFileLastModified() const;

    QDateTime getFileCreated() const;

    QDateTime getModified() const;

    static int countAll();

    static bool allowDifferentFileName();

    bool renameNoteFile(QString newName);

    QString fileNameSuffix() const;

    bool modifyNoteTextFileNameFromQMLHook();

    static QVector<int> searchInNotes(QString query,
                                      bool ignoreNoteSubFolder = false,
                                      int noteSubFolderId = -1);

    int countSearchTextInNote(const QString &search) const;

    static QStringList buildQueryStringList(
        QString searchString, bool escapeForRegularExpression = false);

    QString fileBaseName(bool withFullName = false);

    NoteSubFolder getNoteSubFolder() const;

    void setNoteSubFolder(const NoteSubFolder &noteSubFolder);

    void setNoteSubFolderId(int id);

    static QVector<Note> fetchAllByNoteSubFolderId(int noteSubFolderId);

    static QVector<int> noteIdListFromNoteList(const QVector<Note> &noteList);

    static int countByNoteSubFolderId(int noteSubFolderId = 0,
                                      bool recursive = false);

    int getNoteSubFolderId() const;

    bool isInCurrentNoteSubFolder() const;

    QString relativeNoteFilePath(QString separator = QString()) const;

    QString relativeNoteSubFolderPath() const;

    QString noteSubFolderPathData() const;

    bool isSameFile(const Note &note) const;

    bool isShareEditAllowed() const;

    QString relativeNoteFilePath(QString separator = "");

    static Note fetchByShareId(int shareId);

    qint64 getFileSize() const;

    static Note updateOrCreateFromFile(QFile &file,
                                       const NoteSubFolder &noteSubFolder,
                                       bool withNoteNameHook = false);

    static QVector<int> fetchAllIds(int limit = -1, int offset = -1);

    QVector<int> findLinkedNoteIds() const;

    void handleNoteMoving(const Note &oldNote) const;

    static QString createNoteHeader(const QString &name);

    QString getInsertEmbedmentMarkdown(QFile *file, mediaType type, bool addNewLine = true,
                                   bool returnUrlOnly = false,
                                   QString title = QString());
	
    QString getInsertPDFMarkdown(QFile *file,
                                 bool addNewLine = true);
	
    static bool scaleDownImageFileIfNeeded(QFile &file);

    QString downloadUrlToEmbedment(const QUrl &url, bool returnUrlOnly = false);

    QString importMediaFromBase64(
        QString &data, const QString &imageSuffix = QStringLiteral("dat"));

    bool canWriteToNoteFile();

    static QString generateNoteFileNameFromName(const QString &name);

    void generateFileNameFromName();

    QString textToMarkdownHtml(QString str, const QString &notesPath,
                               int maxImageWidth = 980, bool forExport = false,
                               bool base64Images = false);

    QStringList getEmbedmentFileList();

    static Note fetchByUrlString(const QString &urlString);

    static QVector<int> fetchAllIdsByNoteTextPart(const QString &textPart);

    QStringList getAttachmentsFileList() const;

    QString getNotePreviewText(bool asHtml = false, int lines = 3) const;

    static QString generateMultipleNotesPreviewText(const QVector<Note> &notes);

    bool handleNoteTextFileName();

    QString getNoteIdURL() const;

    static QString cleanupFileName(QString name);

    static QString extendedCleanupFileName(QString name);

    QVector<Bookmark> getParsedBookmarks() const;

    QString getParsedBookmarksWebServiceJsonText() const;

    void resetNoteTextHtmlConversionHash();

    QString getFileURLFromFileName(QString fileName,
                                   bool urlDecodeFileName = false) const;

    static bool fileUrlIsNoteInCurrentNoteFolder(const QUrl &url);

    static QString fileUrlInCurrentNoteFolderToRelativePath(const QUrl &url);

    QString relativeFilePath(const QString &path) const;

    static Note fetchByFileUrl(const QUrl &url);

    static Note fetchByRelativeFilePath(const QString &relativePath);

    QString getNoteUrlForLinkingTo(const Note &note,
                                   bool forceLegacy = false) const;

    QString embedmentUrlStringForFileName(const QString &fileName) const;

    Note fetchByRelativeFileName(const QString &fileName) const;

    static Utils::Misc::ExternalImageHash *externalImageHash();

    static QString urlEncodeNoteUrl(const QString &url);

    static QString urlDecodeNoteUrl(QString url);
	
   protected:
    QString name;
    QString fileName;
    QString _noteTextHtml;
    QString _noteTextHtmlConversionHash;
    QString noteText;
    QString decryptedNoteText;
    bool hasDirtyData;
    QDateTime fileCreated;
    QDateTime fileLastModified;
    QDateTime created;
    QDateTime modified;
    qint64 fileSize;
    qint64 cryptoKey;
    int id;
    int noteSubFolderId;
	QString cryptoPassword;
    QRegularExpression getEncryptedNoteTextRegularExpression();

    QRegularExpression getEncryptedNoteTextRegularExpression() const;
    QString getEncryptedNoteText() const;

    static const QString getNoteURL(const QString &baseName);

    static const QString getNoteURLFromFileName(const QString &fileName);
};

Q_DECLARE_TYPEINFO(Note, Q_MOVABLE_TYPE);

#endif    // NOTE_H
