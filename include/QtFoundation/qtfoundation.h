/****************************************************************************
 *                                                                          *
 * Copyright (C) 2001~2016 Neutrino International Inc.                      *
 *                                                                          *
 * Author : Brian Lin <lin.foxman@gmail.com>, Skype: wolfram_lin            *
 *                                                                          *
 ****************************************************************************/

#ifndef QT_FOUNDATION_H
#define QT_FOUNDATION_H

#include <QtCURL>
#include <QtCUDA>
#include <QtOpenCV>
#include <QtFFmpeg>
#include <QtAudio>
#include <QtPCAP>
#include <Essentials>
#include <QtCalendar>
#include <QtCryptography>
#include <QtMIME>
#include <SqlAnalyzer>
#include <QtGMP>
#include <QtGSL>
#include <QtAlgebra>
#include <QtDiscrete>
#include <QtFFT>
#include <Mathematics>
#include <QtFuzzy>
#include <QtFLP>
#include <QtGeography>
#include <NetProtocol>
#include <QtXmlRPC>
#include <QtUDT>
#include <QtFTP>
#include <QtRPC>
#include <QtJsonRPC>
#include <AudioIO>
#include <QtPhonemes>
#include <QtVocal>
#include <QtLinguistics>
#include <AudioIO>
#include <QtMtAPI>
#include <QtFinance>
#include <QtComprehend>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#    if defined(QT_BUILD_QTFOUNDATION_LIB)
#      define Q_FOUNDATION_EXPORT Q_DECL_EXPORT
#    else
#      define Q_FOUNDATION_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define Q_FOUNDATION_EXPORT
#endif


namespace N
{

class Q_FOUNDATION_EXPORT PurePlan          ;
class Q_FOUNDATION_EXPORT Parallel          ;
class Q_FOUNDATION_EXPORT TimeFactory       ;
class Q_FOUNDATION_EXPORT ParamentQuery     ;
class Q_FOUNDATION_EXPORT MimeManager       ;
class Q_FOUNDATION_EXPORT HistoryManager    ;
class Q_FOUNDATION_EXPORT TaskManager       ;
class Q_FOUNDATION_EXPORT EventManager      ;
class Q_FOUNDATION_EXPORT CodeManager       ;
class Q_FOUNDATION_EXPORT NetworkManager    ;
class Q_FOUNDATION_EXPORT DocumentManager   ;
class Q_FOUNDATION_EXPORT KnowledgeManager  ;
class Q_FOUNDATION_EXPORT FinanceManager    ;
class Q_FOUNDATION_EXPORT VideoManager      ;
class Q_FOUNDATION_EXPORT SqlOrganizer      ;
class Q_FOUNDATION_EXPORT WatchDog          ;
class Q_FOUNDATION_EXPORT Synchronicity     ;
class Q_FOUNDATION_EXPORT JsTools           ;
class Q_FOUNDATION_EXPORT FoundationManager ;

Q_FOUNDATION_EXPORT extern PurePlan * qPlan ;

Q_FOUNDATION_EXPORT void InstallTables  (PurePlan & plan) ;
Q_FOUNDATION_EXPORT void BasicMetaTypes (void) ;

namespace Manager {
Q_FOUNDATION_EXPORT
bool Uuids     ( PurePlan         & plan                         ,
                 SqlConnection    & connection                   ,
                 UUIDs            & U                            ,
                 Types::ObjectTypes type                         ,
                 Qt::SortOrder      order = Qt::AscendingOrder ) ;
Q_FOUNDATION_EXPORT
bool NameUuids ( PurePlan         & plan                         ,
                 SqlConnection    & connection                   ,
                 SUID               uuid                         ,
                 UUIDs            & U                            ,
                 Qt::SortOrder      order = Qt::AscendingOrder ) ;
Q_FOUNDATION_EXPORT
bool UuidNames ( PurePlan         & plan                         ,
                 SqlConnection    & connection                   ,
                 UUIDs            & U                            ,
                 NAMEs            & N                          ) ;
Q_FOUNDATION_EXPORT
bool UuidNames ( PurePlan         & plan                         ,
                 SqlConnection    & connection                   ,
                 SUID               uuid                         ,
                 NAMEs            & N                          ) ;
Q_FOUNDATION_EXPORT
bool UuidNames ( PurePlan         & plan                         ,
                 SqlConnection    & connection                   ,
                 SUID               uuid                         ,
                 QStringList      & S                          ) ;
Q_FOUNDATION_EXPORT
bool FromNames ( PurePlan         & plan                         ,
                 SqlConnection    & connection                   ,
                 UUIDs            & U                            ,
                 QStringList      & S                          ) ;

}

namespace SQL {

Q_FOUNDATION_EXPORT
bool FetchSql              (
       QString    Filename ,
       QString    Username ,
       QString    Scope    ,
       PurePlan & Plan   ) ;

}

namespace Acoustics
{

Q_FOUNDATION_EXPORT bool Load (Sql & sql,QString table,QDir & dir,SUID uuid,AudioData & data) ;
Q_FOUNDATION_EXPORT bool Load (Sql & sql,QString table,QDir & dir,QString identifier,AudioData & data) ;
Q_FOUNDATION_EXPORT bool Load (PurePlan & plan,SUID uuid,AudioData & data) ;
Q_FOUNDATION_EXPORT bool Load (PurePlan & plan,QString identifier,AudioData & data) ;

}

namespace Cpp
{

Q_FOUNDATION_EXPORT QByteArray ExportToCpp              (
                               QByteArray & Data        ,
                               QString      DataName    ,
                               bool         ExportSize  ,
                               QString      DataSize  ) ;
Q_FOUNDATION_EXPORT bool       ExportToCpp              (
                               QString      source      ,
                               QString      filename    ,
                               QString      DataName    ,
                               bool         ExportSize  ,
                               QString      DataSize  ) ;
Q_FOUNDATION_EXPORT QByteArray FixedToCpp               (
                               QByteArray & Data        ,
                               int          Type        ,
                               QString      DataName    ,
                               bool         ExportSize  ,
                               QString      DataSize  ) ;
Q_FOUNDATION_EXPORT QByteArray FloatingToCpp            (
                               QByteArray & Data        ,
                               int          Type        ,
                               QString      DataName    ,
                               bool         isPrecision ,
                               int          Precision   ,
                               bool         ExportSize  ,
                               QString      DataSize  ) ;

}

namespace Forex
{

class Q_FOUNDATION_EXPORT ImporterPlugin  ;

}

class Q_FOUNDATION_EXPORT PurePlan : public VirtualProgress
{
  public:

    typedef enum  {
      None   = 0  ,
      Plain  = 1  ,
      GUI    = 2  ,
      Script = 3  }
      PlanTypeIDs ;

    typedef enum             {
      DefaultAudioInput  = 1 ,
      DefaultAudioOutput = 2 ,
      DefaultSpeech      = 3 ,
      SpeechVoice        = 4 ,
      DefaultEar         = 5 ,
      DefaultCamera      = 6 }
      PlanIDs                ;

    typedef enum             {
      DepotSQLID      = 1001 ,
      NetworkSQLID    = 1002 ,
      NewsSQLID       = 1003 ,
      ForexSQLID      = 1004 }
      SqlConnectionIDs       ;

    QString                         Application     ; // Application Name
    Settings                      * Native          ;
    Settings                      * Remote          ;
    SUID                            Machine         ; // Equipment UUID
    QString                         Hostname        ; // Machine hostname
    QDir                            Root            ; // Application Root Directory
    QDir                            Home            ;
    QDir                            User            ;
    QDir                            Bin             ;
    QStringList                     Arguments       ;
    QStringList                     PluginPaths     ;
    Sql                             sql             ;
    Sql                             LocalSql        ;
    int                             LanguageId      ;
    Languages                       languages       ;
    SUID                            UserUuid        ;
    int                             uid             ;
    int                             gid             ;
    int                             Verbose         ;
    int                             MaxLogs         ;
    int                             ThreadStack     ;
    bool                            Visible         ;
    bool                            scene           ;
    bool                            canContinue     ;
    NAMEs                           Equipments      ;
    RMAPs                           EquipmentIDs    ;
    EMAPs                           Tables          ;
    EMAPs                           NameMap         ;
    UMAPs                           IdMap           ;
    UMAPs                           Values          ;
    LMAPs                           Booleans        ;
    ZMAPs                           Uuids           ;
    bool                            VoiceOutput     ;
    NAMEs                           AudioInput      ;
    NAMEs                           AudioOutput     ;
    NAMEs                           CameraInput     ;
    QStringList                     AudioDepots     ;
    QStringList                     VideoDepots     ;
    IMAPs                           Encoders        ;
    DecisionTables                  decisions       ;
    Junctions                       junctions       ;
    QMap<int,Sql                  > SQLs            ;
    QMap<int,QDir                 > Dirs            ;
    QMap<QString,QVariant         > Variables       ;
    QMap<int,SharedMemory        *> SharedMemories  ;
    QMap<QString,DataManipulator *> Manipulators    ;
    QMap<int,AudioChannel        *> AudioChannels   ;
    QList<Destroyer              *> Destroyers      ;
    FileLocator                     locator         ;
    CPU::Usage                      cpu             ;
    Rpc                             RPC             ;
    RpcCommands                     Request         ;
    NotifyPlayer                    Audio           ;
    AudioRecorder                   Recorder        ;
    SpeakProcessor                  Mouth           ;
    Hearing                      *  Ears            ;
    FtpSettings                  *  Ftp             ;
    CiosNeighbors                *  Neighbors       ;
    UdpBroadcast                 *  Knock           ;
    UdpBroadcast                 *  House           ;
    ScriptManager                   Scripts         ;
    IpGeolocation                *  geolocation     ;
    Parallel                     *  parallel        ;
    Personality                  *  personality     ;
    Thread                       *  KeyThread       ;

    explicit             PurePlan        (void) ;
    virtual             ~PurePlan        (void) ;

    virtual int          type            (void) const ;

    virtual bool         Initialize      (void) ;

    void                 arguments       (int argc,char ** argv) ;

    // Plugins
    void                 addPlugin       (QString path) ;
    void                 setLibraryPaths (void) ;

    // File path
    QString              Path            (QString name) ;
    QString              Temporary       (QString filename) ;

    // Paths
    int                  addPath         (int Id,QDir dir) ;
    QDir                 Path            (int Id) ;

    // Name maps
    int                  addMap          (int id,QString name) ;
    int                  Map             (QString name) ;

    void                 LoadSupports    (Languages & languages) ;

    int                  addTable        (int id,QString name) ;
    QString              Table           (QString name) ;
    QString              Table           (int id) ;
    bool                 containsTable   (int id) ;

    // Depots
    void                 AudioDepot      (QString scope) ;
    void                 VideoDepot      (QString scope) ;
    QString              AudioFile       (QString filename) ;
    QString              VideoFile       (QString filename) ;

    void                 Play            (QString name) ;
    void                 Play            (SUID uuid) ;

    int                  addSound        (SUID uuid,QString name) ;
    int                  addSound        (QString identifier,QString name) ;
    int                  addSound        (int command,QString name,QString identifier);
    int                  addSound        (int command,QString name,SUID uuid);

    // Settings
    bool                 setSettings     (QString name) ;
    bool                 existSettings   (void) ;

    int                  setLanguage     (int languageId) ;

    void                 loadUuids       (QString scope,QStringList items) ;
    void                 loadValues      (QString scope,QStringList items) ;
    void                 loadBooleans    (QString scope,QStringList items) ;

    // Paths
    int                  setPath         (int Id,QString scope,QString key) ;

    // Talk
    void                 StartTalk       (QString Path) ;
    void                 StopTalk        (void) ;
    void                 Talk            (QString message) ;
    void                 Talk            (int verbose,QString message) ;
    void                 TalkNumber      (long long digits,bool verbose = true) ;
    void                 TalkNumber      (double num,int digits,bool verbose = true) ;

    // Junctions and Conduits Management
    int                  Join            (Junction * junction) ;
    Junction           * GrabJunction    (QObject * parent) ;
    Junctions            Connectivity    (Conduit * conduit) ;
    bool                 Rehabilitate    (void) ;
    bool                 Dredge          (bool start) ;

    virtual void         processEvents   (void) ;

    // Debug
    virtual bool         Talkative       (int verbose) ;
    virtual void         Debug           (QString message);
    virtual void         Debug           (int verbose,QString message);

    virtual void         Notify          (QString sound,QString message) ;

    virtual void       * Percentage      (QString format = "") ;
    virtual void         setRange        (void * bar,int Min,int Max) ;
    virtual void         setValue        (void * bar,int value) ;
    virtual void         Finish          (void * bar) ;

    // Progress Manager
    virtual int          Progress        (QString name,QString format) ;
    virtual void         ProgressName    (int Id,QString name) ;
    virtual void         ProgressText    (int Id,QString message) ;
    virtual void         setProgress     (int Id,QString format) ;
    virtual void         setRange        (int Id,qint64 Min,qint64 Max) ;
    virtual void         setFrequency    (int Id,QString cFmt,QString rFmt) ;
    virtual void         Start           (int Id,qint64 * Value,bool * Running) ;
    virtual void         Finish          (int Id) ;
    virtual bool         ProgressReady   (int Id,int msecs = 1000) ;

    // Network Protocols
    virtual QStringList  LocalAddresses  (void) ;
    virtual QHostAddress LocalAddress    (QAbstractSocket::NetworkLayerProtocol protocol) ;
    virtual bool         Existing        (QObject * parent,int Port) ;
    virtual bool         UdpBind         (int Port) ;
    virtual bool         UdpBeacon       (int Port) ;

    virtual bool         LoadGeolocation (QString filename = "") ;

    virtual bool         Beacon          (int port) ;

    // Destroyer management

    virtual int          addDestroyer    (Destroyer * destroyer) ;
    virtual int          removeDestroyer (Destroyer * destroyer) ;
    virtual void         Recycling       (void) ;
    virtual void         Destructor      (void) ;

  protected:

    QMutex * destroyerMutex ;

  private:

} ;

class Q_FOUNDATION_EXPORT Parallel : public Computing
                                   , public Destroyer
{
  public:

    explicit     Parallel    (PurePlan * plan) ;
    virtual     ~Parallel    (void) ;

    virtual bool Recycling   (void) ;
    virtual bool Destructor  (void) ;

  protected:

    PurePlan * plan ;

    virtual void ReportError (int level,int code) ;

  private:

} ;

class Q_FOUNDATION_EXPORT TimeFactory
{
  public:

    PurePlan * plan    ;
    CMAPs      toUuids ;
    RMAPs      toTypes ;
    NAMEs      Names   ;
    NAMEs      Formats ;

    explicit TimeFactory (PurePlan * plan = NULL) ;
    virtual ~TimeFactory (void) ;

    StarDate * create    (int type) ;
    bool       load      (void) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT ParamentQuery
{
  public:

    int     Type    ;
    int     Variety ;
    QString Scope   ;

    explicit ParamentQuery     (int type,int variety,QString scope) ;
    explicit ParamentQuery     (void) ;
             ParamentQuery     (const ParamentQuery & query) ;
    virtual ~ParamentQuery     (void) ;

    ParamentQuery & operator = (const ParamentQuery & query) ;

    TUID     Value    (DeclSQL ,
                       QString         table      ,
                       QString         name     ) ;
    TUID     Value    (DeclSQL ,
                       QString         table      ,
                       SUID            uuid     ) ;
    double   Floating (DeclSQL ,
                       QString         table      ,
                       QString         name     ) ;
    double   Floating (DeclSQL ,
                       QString         table      ,
                       SUID            uuid     ) ;
    QVariant Data     (DeclSQL ,
                       QString         table      ,
                       QString         name     ) ;
    QVariant Data     (DeclSQL ,
                       QString         table      ,
                       SUID            uuid     ) ;
    QString  String   (DeclSQL ,
                       QString         table      ,
                       QString         name     ) ;
    QString  String   (DeclSQL ,
                       QString         table      ,
                       SUID            uuid     ) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT MimeManager
{
  public:

    PurePlan     *  MimePlan   ;
    QList<MimeType> MimeTypes  ;
    UUIDs           Extensions ;
    NAMEs           Suffixes   ;
    MimeType        Default    ;

    explicit MimeManager    (PurePlan * plan = NULL) ;
    virtual ~MimeManager    (void) ;

    int         count       (void) ;
    bool        contains    (QString mime) ;
    bool        contains    (int MimeId) ;

    QStringList supports    (void) ;
    QStringList supports    (QString catalog) ;

    MimeType & operator []  (QString mime) ;
    MimeType & operator []  (int MimeId) ;

    bool        at          (QString mime  ,MimeType & mt) ;
    bool        at          (int     MimeId,MimeType & mt) ;

    bool        load        (SqlConnection    & Connection) ;
    MimeType    append      (SqlConnection    & Connection  ,
                             QString            mime      ) ;

    QStringList Launcher    (SqlConnection    & Connection  ,
                             int                extension ) ;

    QString documentMime    (SqlConnection    & Connection  ,
                             SUID               uuid        ,
                             int                type        ,
                             TUID               publish   ) ;
    bool    documentMime    (SqlConnection    & Connection  ,
                             SUID               uuid        ,
                             int                type        ,
                             TUID               publish     ,
                             MimeType         & mime      ) ;
    bool assureDocumentMime (SqlConnection    & Connection  ,
                             SUID               uuid        ,
                             int                type        ,
                             TUID               publish     ,
                             QString            mime      ) ;

    bool MimeUuids          (SqlConnection    & Connection                     ,
                             QString            mimetype                       ,
                             UUIDs            & Uuids                          ,
                             enum Qt::SortOrder Sorting = Qt::AscendingOrder ) ;
    bool MimeLikeUuids      (SqlConnection    & Connection                     ,
                             QString            mimetype                       ,
                             UUIDs            & Uuids                          ,
                             enum Qt::SortOrder Sorting = Qt::AscendingOrder ) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT HistoryManager
{
  public:

    PurePlan * HistoryPlan ;

    explicit HistoryManager     (PurePlan * plan = NULL);
    virtual ~HistoryManager     (void);

    QString  Name               (DeclSQL,SUID    uuid ,int language) ;
    bool     assureName         (DeclSQL,SUID    uuid ,int language,QString name) ;

    TUID     Time               (DeclSQL,SUID    uuid) ;
    SUID     assureTime         (DeclSQL,SUID    uuid ,int type,TUID tuid) ;
    SUID     GetHistoryType     (DeclSQL,UUIDs & Uuids,int type) ;
    TUID     GetTime            (DeclSQL,SUID    uuid) ;

    SUID     assurePeriod       (DeclSQL     ,
                                 SUID uuid   ,
                                 SUID first  ,
                                 SUID second ,
                                 int  type ) ;
    SUID     assurePeriod       (DeclSQL         ,
                                 SUID uuid       ,
                                 TUID first      ,
                                 TUID second     ,
                                 int  type       ,
                                 int  spotType ) ;

    SUID     assureChronology   (DeclSQL         ,
                                 SUID uuid       ,
                                 SUID timespot ) ;

    SUID     appendHistoryType  (DeclSQL) ;

    static QDateTime toDateTime (TUID tuid) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT TaskManager
{
  public:

    PurePlan * TaskPlan     ;
    UUIDs      Tasks        ;
    NAMEs      Names        ;
    RMAPs      Behaviors    ;
    RMAPs      Status       ;
    XMAPs      Roots        ;
    SMAPs      Flags        ;
    SMAPs      Parents      ;
    SMAPs      Predecessors ;
    SMAPs      Successors   ;
    NMAPs      Entries      ;
    NMAPs      Heirs        ;

    explicit TaskManager    (PurePlan * plan = NULL) ;
    virtual ~TaskManager    (void) ;

    void     Clear          (void) ;

    UUIDs    TopLevels      (void) ;
    UUIDs    Frameworks     (void) ;
    UUIDs    Predecessor    (void) ;
    UUIDs    Successor      (void) ;

    UUIDs    Frame          (SUID uuid) ;

    bool     isFramework    (SUID uuid) ;
    bool     isPredecessor  (SUID uuid) ;
    bool     isSuccessor    (SUID uuid) ;

    bool     Load           (DeclSQL,SUID project = 0) ;

    UUIDs    ListProjects   (DeclSQL) ;

    UUIDs    ListTasks      (DeclSQL) ;
    UUIDs    ProjectTasks   (DeclSQL,SUID project) ;

    int      TaskBehaviors  (DeclSQL,UUIDs & Uuids) ;
    int      TaskRelations  (DeclSQL,UUIDs & Uuids) ;

    NAMEs    LoadNames      (DeclSQL,UUIDs & Uuids,int language) ;
    bool     assureName     (DeclSQL,SUID uuid,int language,QString name);

    bool     UpdateStatus   (DeclSQL,SUID uuid,int status) ;
    bool     UpdateBehavior (DeclSQL,SUID uuid,int behavior) ;

    SUID     NewTask        (DeclSQL,int behavior,int status,SUID flags) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT EventManager
{
  public:

    PurePlan * EventPlan ;

    explicit EventManager       (PurePlan * plan = NULL) ;
    virtual ~EventManager       (void) ;

    static QDateTime toDateTime (TUID tuid) ;
    static SUID      Append     (PurePlan * plan,int type,QString name,QDateTime Start,QDateTime End) ;
    static bool      Complete   (PurePlan * plan,SUID uu,QDateTime Start,QDateTime End) ;

    UUIDs   Uuids               (DeclSQL,QString Table) ;
    bool    UpdateType          (DeclSQL,QString Table,SUID uuid,int Type) ;
    SUID    AssureUuid          (DeclSQL) ;
    bool    AssureEvent         (DeclSQL,SUID uuid,int Type) ;
    bool    UpdateName          (DeclSQL,SUID uuid,QString task,QString oldName) ;
    QString Name                (DeclSQL,SUID uuid) ;
    int     Type                (DeclSQL,SUID uuid) ;
    QString ActionQuery         (DeclSQL) ;
    TUID    TimeSpot            (DeclSQL,SUID uuid) ;
    bool    Delete              (DeclSQL,SUID uuid) ;
    SUID    Task                (DeclSQL,SUID uuid) ;
    bool    Relegation          (DeclSQL,SUID uuid,SUID task) ;
    int     UuidType            (DeclSQL,SUID uuid) ;
    QString TimePeriod          (DeclSQL,SUID uuid);
    SUID    Chronology          (DeclSQL,SUID uuid);
    UUIDs   Events              (DeclSQL,QDateTime Start,QDateTime Final);
    bool    assureName          (DeclSQL,SUID uuid,int language,QString name) ;
    SUID    attachSpot          (DeclSQL,SUID uuid,int Type,TUID start,TUID final) ;
    bool    UpdateEvent         (DeclSQL,SUID uuid,int Type,TUID start,TUID final) ;
    bool    DeleteScheduling    (DeclSQL,SUID uuid) ;
    bool    AddScheduling       (DeclSQL,SUID uuid) ;
    UUIDs   Scheduling          (DeclSQL) ;
    TMAPs   Monitoring          (DeclSQL,UUIDs & Uuids) ;
    QString EventNote           (DeclSQL,SUID uuid,int language) ;
    bool    SaveNote            (DeclSQL,SUID uuid,int language,QString note) ;
    bool    hasOwners           (DeclSQL,SUID uuid) ;
    bool    hasTriggers         (DeclSQL,SUID uuid) ;
    bool    hasDependency       (DeclSQL,SUID uuid) ;
    bool    isRepeated          (DeclSQL,SUID uuid) ;
    bool    hasTasks            (DeclSQL,SUID uuid) ;
    bool    SaveVariable        (DeclSQL,SUID uuid,QString name,QVariant & value) ;
    bool    LoadVariable        (DeclSQL,SUID uuid,QString name,QVariant & value) ;
    bool    DeleteVariable      (DeclSQL,SUID uuid,QString name) ;
    SUID    GetTrigger          (DeclSQL,int type,SUID uuid) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT CodeManager
{
  public:

    typedef enum                {
      FaFile       = 0x00000001 ,
      FaDir        = 0x00000002 ,
      FaExecutable = 0x00000004 ,
      FaHidden     = 0x00000008 ,
      FaReadable   = 0x00000010 ,
      FaWritable   = 0x00000020 ,
      FaSymLink    = 0x00000100 ,
      FaRoot       = 0x00000200 ,
      FaRelative   = 0x00000400 }
      FileAttributes            ;

    PurePlan * CodePlan ;

    explicit    CodeManager        (PurePlan * plan = NULL) ;
    virtual    ~CodeManager        (void) ;

    QString     Name               (SqlConnection  & Connection ,
                                    SUID             uuid       ,
                                    int              language ) ;
    NAMEs       Names              (SqlConnection  & Connection ,
                                    UUIDs          & Uuids      ,
                                    int              language ) ;
    bool        assureName         (SqlConnection  & Connection ,
                                    SUID             uuid       ,
                                    int              language   ,
                                    QString          name     ) ;

    QString     QueryMembers       (SqlConnection  & Connection) ;
    SUID        assureMember       (SqlConnection  & Connection  ,
                                    SUID             uuid        ,
                                    int              language    ,
                                    QString          name        ,
                                    QString          identifier) ;
    UUIDs       Members            (SqlConnection  & Connection) ;

    QString     QuerySets          (SqlConnection  & Connection) ;
    SUID        assureSet          (SqlConnection  & Connection  ,
                                    SUID             uuid        ,
                                    int              language    ,
                                    QString          name        ,
                                    int              Type        ,
                                    int              Uniqueness) ;
    UUIDs       Sets               (SqlConnection  & Connection) ;

    bool        isDirectory        (int attribute) ;
    bool        isAttribute        (int            attribute ,
                                    FileAttributes fa      ) ;
    int         SetAttribute       (int            attribute ,
                                    FileAttributes fa      ) ;
    int         FileAttribute      (QFileInfo      & info) ;
    bool        GetAttributes      (SqlConnection  & Connection ,
                                    SUID             uuid       ,
                                    int            & attributes ,
                                    int            & size     ) ;
    QByteArray  LoadFile           (SqlConnection  & Connection ,
                                    SUID             uuid     ) ;
    bool        LoadFile           (SqlConnection  & Connection ,
                                    SUID             uuid       ,
                                    QByteArray     & Body     ) ;
    SUID        FindFile           (SqlConnection  & SC         ,
                                    int              attribute  ,
                                    QByteArray     & data     ) ;
    SUID        addEmpty           (SqlConnection  & SC         ,
                                    QFileInfo      & info     ) ;
    SUID        assureFile         (SqlConnection  & SC         ,
                                    QString          filename ) ;
    SUID        assureFile         (SqlConnection  & SC             ,
                                    int              attribute      ,
                                    QDateTime        Created        ,
                                    QDateTime        LastModified   ,
                                    QByteArray     & Body           ,
                                    bool             Find = false ) ;

    SUID        addVersion         (SqlConnection  & SC         ,
                                    SUID             sourceUuid ,
                                    QString          version  ) ;
    bool        attachVersion      (SqlConnection  & SC              ,
                                    SUID             versionUuid     ,
                                    SUID             fileUuid        ,
                                    QString          path            ,
                                    int              subversion = 1) ;
    UUIDs       SourceVersions     (SqlConnection  & SC           ,
                                    SUID             sourceUuid ) ;

    UUIDs       SourceUrls         (SqlConnection  & SC          ,
                                    SUID             sourceUuid) ;
    bool        DeleteSourceUrl    (SqlConnection  & SC          ,
                                    SUID             sourceUuid  ,
                                    SUID             url       ) ;
    bool        UpdateUrlPositions (SqlConnection  & SC          ,
                                    SUID             sourceUuid  ,
                                    UUIDs          & Uuids     ) ;

    UUIDs       DataTypes          (SqlConnection  & SC) ;
    NAMEs       TypeNames          (SqlConnection  & SC      ,
                                    UUIDs          & Uuids ) ;

    SUID        findDefine         (SqlConnection  & SC      ,
                                    QString          Define) ;
    SUID        assureDefine       (SqlConnection  & SC      ,
                                    QString          Define) ;
    SUID        assureDefine       (SqlConnection  & SC      ,
                                    SUID             file    ,
                                    QString          Define) ;

    SUID        findInclude        (SqlConnection  & SC        ,
                                    QString          Include ) ;
    SUID        assureInclude      (SqlConnection  & SC        ,
                                    QString          Include ) ;
    SUID        assureInclude      (SqlConnection  & SC        ,
                                    SUID             file      ,
                                    QString          Include ) ;

    SUID        findNamespace      (SqlConnection  & SC          ,
                                    QString          Namespace ) ;
    SUID        assureNamespace    (SqlConnection  & SC          ,
                                    QString          Namespace ) ;
    SUID        assureNamespace    (SqlConnection  & SC          ,
                                    SUID             file        ,
                                    QString          Namespace ) ;

    SUID        findEnum           (SqlConnection  & SC     ,
                                    QString          Enum ) ;
    SUID        assureEnum         (SqlConnection  & SC     ,
                                    QString          Enum ) ;
    SUID        assureEnum         (SqlConnection  & SC     ,
                                    SUID             file   ,
                                    QString          Enum ) ;

    bool        insertPreprocessor (SqlConnection  & SC     ,
                                    SUID             uuid   ,
                                    int              Type ) ;
    bool        insertComponent    (SqlConnection  & SC     ,
                                    SUID             uuid   ,
                                    int              Type   ,
                                    int              PDL  ) ;

    bool        insertGroup        (SqlConnection  & SC         ,
                                    SUID             first      ,
                                    SUID             second     ,
                                    int              t1         ,
                                    int              t2         ,
                                    int              relation ) ;

    UUIDs       SelectFiles        (SqlConnection  & SC       ,
                                    QStringList    & Filters) ;

    UUIDs       ListPDLs           (SqlConnection  & SC) ;
    RMAPs       PdlIdentifications (SqlConnection  & SC    ,
                                    UUIDs          & PDLs) ;
    NAMEs       ListExtensions     (SqlConnection  & SC) ;
    bool        AddPdlExtension    (SqlConnection  & SC          ,
                                    int              pdl         ,
                                    int              extension ) ;
    bool        DeletePdlExtension (SqlConnection  & SC          ,
                                    int              pdl         ,
                                    int              extension ) ;
    QStringList Extensions         (SqlConnection  & SC,
                                    int              pdl) ;

    UUIDs       Platforms          (SqlConnection  & SC) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT NetworkManager
{
  public:

    PurePlan  * NetPlan    ;
    QStringList TLDs       ;
    QStringList SLDs       ;
    SMAPs       SldTLDs    ;
    RMAPs       TldIDs     ;
    RMAPs       SldIDs     ;
    ZMAPs       TldUuids   ;
    ZMAPs       SldUuids   ;
    RMAPs       TldNations ;
    CMAPs       Countries  ;
    NAMEs       TldNames   ;
    NAMEs       SldNames   ;
    NAMEs       Nations    ;
    NAMEs       NICs       ;
    NAMEs       Comments   ;
    QStringList RLDs       ;
    ZMAPs       UuidRlds   ;
    QStringList RXDs       ;
    ZMAPs       UuidRxds   ;
    ZMAPs       SitesCache ;

    explicit NetworkManager  (PurePlan * Plan = NULL) ;
    virtual ~NetworkManager  (void) ;

    bool    toURLs           (QStringList & URIs  ,
                              QList<QUrl> & URLs) ;
    QString Reverse          (QString name) ;
    QString URI              (QUrl & url) ;

    void    ClearIndex       (void) ;

    QString Name             (SqlConnection  & Connection ,
                              SUID             uuid     ) ;
    QString Name             (SqlConnection  & Connection ,
                              SUID             uuid       ,
                              int              language ) ;
    bool    LoadNations      (SqlConnection  & Connection) ;
    bool    LoadTLDs         (SqlConnection  & Connection) ;
    bool    LoadSLDs         (SqlConnection  & Connection) ;
    int     SiteTotal        (SqlConnection  & Connection) ;
    int     TldCounts        (SqlConnection  & Connection  ,
                              SUID             tld       ) ;
    int     TldTotal         (SqlConnection  & Connection  ,
                              SUID             tld       ) ;
    int     SldCounts        (SqlConnection  & Connection  ,
                              SUID             sld       ) ;
    TUID    SiteId           (SqlConnection  & Connection  ,
                              SUID             site      ) ;
    SUID    LastestTld       (SqlConnection  & Connection) ;
    SUID    LastestSld       (SqlConnection  & Connection) ;
    bool    InsertTldMap     (SqlConnection  & Connection  ,
                              SUID             site        ,
                              SUID             tld       ) ;
    bool    InsertSldMap     (SqlConnection  & Connection  ,
                              SUID             site        ,
                              SUID             sld       ) ;

    SUID    DomainUuid       (SqlConnection  & Connection  ,
                              QUrl           & url       ) ;
    SUID    InsertDomain     (SqlConnection  & Connection  ,
                              QUrl           & url       ) ;
    SUID    UrlUuid          (SqlConnection  & Connection  ,
                              QUrl           & url       ) ;
    SUID    UrlUuidCache     (SqlConnection  & Connection  ,
                              QUrl           & url       ) ;
    SUID    InsertUrl        (SqlConnection  & Connection  ,
                              QUrl           & url       ) ;

    bool    LoadDomainIndex  (SqlConnection  & Connection) ;

    void    PrepareDomainMap (void) ;
    SUID    UrlTLD           (QUrl & url) ;
    SUID    UrlSLD           (QUrl & url) ;

    SUID    NewBookmark      (SqlConnection  & Connection ,
                              QString          name     ) ;
    bool    assureName       (SqlConnection  & Connection ,
                              SUID             uuid       ,
                              int              language   ,
                              QString          name     ) ;

    NAMEs   Bookmarks        (SqlConnection  & Connection  ) ;

    SUID    insertPage       (SqlConnection  & Connection ,
                              QUrl           & url      ) ;
    QUrl    PageUrl          (SqlConnection  & Connection    ,
                              SUID             uuid        ) ;
    QString PageURI          (SqlConnection  & Connection    ,
                              SUID             uuid        ) ;
    SUID    PageUuid         (SqlConnection  & Connection    ,
                              QUrl           & url         ) ;
    SUID    PageUuid         (SqlConnection  & Connection    ,
                              QString          page        ) ;
    bool    PageUuids        (SqlConnection  & Connection             ,
                              QList<QUrl>    & URLs                   ,
                              UUIDs          & Uuids                  ,
                              bool             processEvent = false ) ;
    bool    PageLike         (SqlConnection  & Connection ,
                              QString          Where      ,
                              QString          Page       ,
                              QString          Item       ,
                              UUIDs          & Uuids      ,
                              bool           & KeepGo   ) ;
    SUID    assurePage       (SqlConnection  & Connection    ,
                              QUrl           & url         ) ;

    QString PageTitle        (SqlConnection  & Connection    ,
                              SUID             uuid          ,
                              TUID             publish = 0 ) ;
    bool    assureTitle      (SqlConnection  & Connection    ,
                              SUID             uuid          ,
                              TUID             publish       ,
                              QString          title       ) ;

    int     HttpStatus       (SqlConnection  & Connection    ,
                              SUID             uuid        ) ;
    bool    assureHttpStatus (SqlConnection  & Connection    ,
                              SUID             uuid          ,
                              int              retcode     ) ;

    bool    FromPage         (SqlConnection  & Connection    ,
                              SUID             page          ,
                              UUIDs          & Uuids       ) ;
    bool    FromPage         (SqlConnection  & Connection    ,
                              SUID             page          ,
                              UUIDs          & Uuids         ,
                              bool           & KeepGo      ) ;
    bool    FromSite         (SqlConnection  & Connection    ,
                              SUID             site          ,
                              UUIDs          & Uuids         ,
                              bool           & KeepGo      ) ;
    SUID    PageParent       (SqlConnection  & Connection    ,
                              SUID             uuid        ) ;
    bool    insertParent     (SqlConnection  & Connection    ,
                              SUID             uuid          ,
                              SUID             fromPage    ) ;
    bool    updateParent     (SqlConnection  & Connection    ,
                              SUID             uuid          ,
                              SUID             fromPage    ) ;
    bool    assureParent     (SqlConnection  & Connection    ,
                              SUID             uuid          ,
                              SUID             fromPage    ) ;

    SUID    Username         (SqlConnection  & Connection ,
                              QString          name     ) ;
    SUID    assureUsername   (SqlConnection  & Connection ,
                              QString          name     ) ;

    SUID    EMail            (SqlConnection  & Connection ,
                              QString          mail     ) ;
    SUID    EMail            (SqlConnection  & Connection ,
                              QString          account    ,
                              QString          domain   ) ;
    SUID    EMail            (SqlConnection  & Connection ,
                              QString          account    ,
                              QUrl             url      ) ;
    SUID    assureEMail      (SqlConnection  & Connection ,
                              QString          account    ,
                              QString          domain   ) ;
    SUID    assureEMail      (SqlConnection  & Connection ,
                              QString          account    ,
                              QUrl             url      ) ;

    int     PatternId        (SqlConnection  & Connection ,
                              SUID             type       ,
                              QString          name     ) ;
    bool    InsertPattern    (SqlConnection  & Connection ,
                              SUID             type       ,
                              int              position   ,
                              QString          name     ) ;
    bool    UpdatePattern    (SqlConnection  & Connection ,
                              SUID             type       ,
                              int              position   ,
                              QString          name     ) ;
    bool    UpdatePattern    (SqlConnection  & Connection ,
                              SUID             type       ,
                              int              position   ,
                              QString          name       ,
                              QString          pattern  ) ;
    bool    UrlPatterns      (SqlConnection  & Connection ,
                              SUID             type       ,
                              QString          name       ,
                              QStringList    & patterns ) ;
    bool    DeletePatterns   (SqlConnection  & Connection ,
                              SUID             type       ,
                              int              position   ,
                              QString          name     ) ;
    QString SelectPatterns   (SqlConnection  & Connection ,
                              Qt::SortOrder    Sorting) ;

    bool    assignBody       (SqlConnection  & Connection ,
                              SUID             uuid       ,
                              QByteArray     & Body     ) ;
    bool    fetchBody        (SqlConnection  & Connection ,
                              SUID             uuid       ,
                              QByteArray     & Body     ) ;
    bool    deleteBody       (SqlConnection  & Connection ,
                              SUID             uuid     ) ;
    bool    assignHeader     (SqlConnection  & Connection ,
                              SUID             uuid       ,
                              QByteArray     & Header   ) ;

    ///////////////////////////////////////////////////////
    // DNS Handlers

    bool    DnsUuids         (SqlConnection       & Connection                    ,
                              UUIDs               & Uuids                         ,
                              Qt::SortOrder         Sorting = Qt::AscendingOrder) ;
    bool    DnsAddress       (SqlConnection       & Connection ,
                              SUID                  uuid       ,
                              QHostAddress        & host     ) ;
    bool    DnsAddresses     (SqlConnection       & Connection  ,
                              UUIDs               & Uuids       ,
                              QList<QHostAddress> & Addresses ) ;

    ///////////////////////////////////////////////////////
    // Search tools

    bool FromTable           (SqlConnection  & Connection ,
                              QString          Table      ,
                              Binary         & binary     ,
                              UUIDs          & Uuids    ) ;
    bool FromProtocol        (SqlConnection  & Connection ,
                              Binary         & binary     ,
                              UUIDs          & Uuids    ) ;
    bool FromPage            (SqlConnection  & Connection ,
                              Binary         & binary     ,
                              UUIDs          & Uuids    ) ;
    bool FromTitle           (SqlConnection  & Connection ,
                              Binary         & binary     ,
                              UUIDs          & Uuids    ) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT DocumentManager
{
  public:

    PurePlan * DocumentPlan ;

    explicit   DocumentManager   (PurePlan * plan = NULL) ;
    virtual ~  DocumentManager   (void) ;

    SUID       appendDocument    (SqlConnection  & Connection) ;
    QString    Name              (SqlConnection  & Connection  ,
                                  SUID             uuid        ,
                                  int              language  ) ;
    bool       assureName        (SqlConnection  & Connection  ,
                                  SUID             uuid        ,
                                  int              language    ,
                                  QString          name      ) ;

    UUIDs      Documents         (SqlConnection  & Connection                     ,
                                  Qt::SortOrder    Sorting = Qt::DescendingOrder) ;
    UUIDs      Documents         (SqlConnection  & Connection ,
                                  SUID             uuid     ) ;
    UUIDs      Documents         (SqlConnection  & Connection ,
                                  SUID             uuid       ,
                                  int              Type       ,
                                  Qt::SortOrder    Sorting  ) ;

    int        Rating            (SqlConnection  & Connection ,
                                  SUID             uuid       ,
                                  QString          name     ) ;
    bool       setRating         (SqlConnection  & Connection ,
                                  SUID             uuid       ,
                                  QString          name       ,
                                  int              rating   ) ;

    SUID       acquirePlainText  (SqlConnection  & Connection) ;
    QByteArray PlainText         (SqlConnection  & Connection  ,
                                  SUID             uuid      ) ;
    bool       PlainText         (SqlConnection  & Connection  ,
                                  SUID             uuid        ,
                                  QByteArray     & Body      ) ;
    int        PlainTextSize     (SqlConnection  & Connection  ,
                                  SUID             uuid      ) ;
    QString    PlainEncoding     (SqlConnection  & Connection  ,
                                  SUID             uuid      ) ;
    bool       emptyPlainText    (SqlConnection  & Connection  ,
                                  SUID             uuid      ) ;
    bool       savePlainText     (SqlConnection  & Connection  ,
                                  SUID             uuid        ,
                                  QString        & text      ) ;
    bool       savePlainText     (SqlConnection  & Connection  ,
                                  SUID             uuid        ,
                                  QString          encoding    ,
                                  QByteArray     & Body      ) ;
    bool       savePlainText     (SqlConnection  & Connection  ,
                                  SUID             uuid        ,
                                  int              encoding    ,
                                  QByteArray     & Body      ) ;
    SUID       FindPlainText     (SqlConnection  & Connection  ,
                                  QByteArray     & Body      ) ;
    int        JoinDocument      (SqlConnection  & Connection  ,
                                  SUID             document    ,
                                  SUID             file        ,
                                  int              dtype       ,
                                  QString          name      ) ;
    bool       UpdateVersion     (SqlConnection  & Connection  ,
                                  SUID             document    ,
                                  SUID             file        ,
                                  QString          name      ) ;

    SUID       FindKeyword       (SqlConnection  & Connection  ,
                                  int              language    ,
                                  QString          text      ) ;
    SUID       assureKeyword     (SqlConnection  & Connection  ,
                                  int              language    ,
                                  QString          text      ) ;
    SUID       assureTerminology (SqlConnection  & Connection  ,
                                  int              language    ,
                                  QString          text      ) ;
    SUID       assureKnowledge   (SqlConnection  & Connection  ,
                                  int              language    ,
                                  QString          text      ) ;
    SUID       assureSentence    (SqlConnection  & Connection  ,
                                  int              language    ,
                                  QString          text      ) ;
    SUID       assureParagraph   (SqlConnection  & Connection  ,
                                  int              language    ,
                                  QString          text      ) ;

    // XML Handlers
    bool       GetXml            (SqlConnection  & Connection  ,
                                  SUID             uuid        ,
                                  QByteArray     & Body      ) ;
    bool       UpdateXml         (SqlConnection  & Connection  ,
                                  SUID             uuid        ,
                                  int              type        ,
                                  QByteArray     & Body      ) ;

    // Encodings
    int        EncodingId        (QString          codec       ,
                                  UMAPs          & Encodings ) ;
    bool       getEncodings      (SqlConnection  & Connection  ,
                                  UMAPs          & Encodings ) ;
    bool       getEncodings      (SqlConnection  & Connection  ,
                                  NAMEs          & Encodings ) ;
    bool       assureEncoding    (SqlConnection  & Connection  ,
                                  SUID             uuid        ,
                                  TUID             publish     ,
                                  QString          encoding  ) ;
    bool       assureEncoding    (SqlConnection  & Connection  ,
                                  SUID             uuid        ,
                                  TUID             publish     ,
                                  int              codec       ,
                                  QString          encoding  ) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT KnowledgeManager
{
  public:

    PurePlan * KnowledgePlan ;

    explicit KnowledgeManager (PurePlan * plan = NULL);
    virtual ~KnowledgeManager (void);

    QString  Name             (DeclSQL,SUID uuid,int language) ;
    bool     assureName       (DeclSQL,SUID uuid,int language,QString name) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT FinanceManager
{
  public:

    PurePlan * FinancePlan ;

    explicit FinanceManager  (PurePlan * plan = NULL) ;
    virtual ~FinanceManager  (void) ;

    // Product Management
    bool    assureIdentifier (DeclSQL,SUID    uuid,QString name) ;
    SUID    FindIdentifier   (DeclSQL,QString name) ;
    QString FetchIdentifier  (DeclSQL,SUID    uuid) ;

    SUID    appendCommodity  (DeclSQL) ;
    SUID    appendNamemap    (DeclSQL,SUID uuid,int language,QString name) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT VideoManager
{
  public:

    PurePlan * VideoPlan ;

    explicit            VideoManager    (PurePlan * plan = NULL) ;
    virtual            ~VideoManager    (void);

    virtual QStringList M3U             (QString filename) ;
    virtual TUID        Duration        (QString filename) ;
    virtual QString     Find            (QStringList Depots,QString filename,qint64 filesize) ;
    virtual UUIDs       Clips           (DeclSQL) ;
    virtual UUIDs       Clips           (DeclSQL,SUID album) ;
    virtual NAMEs       ClipLists       (DeclSQL) ;
    virtual QStringList Names           (DeclSQL,SUID clip ) ;
    virtual bool        AlbumClip       (DeclSQL,SUID album,SUID clip) ;
    virtual bool        RemoveClip      (DeclSQL,SUID album,SUID clip) ;
    virtual bool        AlbumOrders     (DeclSQL,SUID album,UUIDs & clips) ;
    virtual QString     Filename        (DeclSQL,SUID clip) ;
    virtual QString     Filename        (        SUID clip) ;
    virtual QSize       Dimension       (DeclSQL,SUID clip) ;
    virtual QString     Location        (Sql & sql,QString table,QDir & dir,SUID uuid) ;
    virtual QString     Location        (Sql & sql,QString table,QDir & dir,QString identifier) ;
    virtual QString     Location        (PurePlan & plan,SUID uuid) ;
    virtual QString     Location        (PurePlan & plan,QString identifier) ;
    virtual QString     Location        (DeclSQL,SUID uuid) ;
    virtual SUID        AppendAlbum     (DeclSQL,int language,QString name) ;
    virtual bool        FindClipsBySize (DeclSQL,qint64 size,UUIDs & uuids) ;
    virtual bool        ClipsFilename   (DeclSQL,QString filename,UUIDs & uuids) ;
    virtual bool        ClipSuffix      (DeclSQL,SUID uuid,QString & suffix) ;
    virtual bool        ClipToAlbums    (DeclSQL,SUID clip,UUIDs & albums) ;
    virtual bool        ClipsToAlbums   (DeclSQL,UUIDs & clips,UUIDs & albums) ;
    virtual bool        ImageToAlbums   (DeclSQL,SUID image,UUIDs & albums) ;
    virtual bool        ImagesToAlbums  (DeclSQL,UUIDs & images,UUIDs & albums) ;

  protected:

  private:

} ;

class Q_FOUNDATION_EXPORT SqlOrganizer
{
  public:

    explicit SqlOrganizer    (PurePlan * plan = NULL) ;
    virtual ~SqlOrganizer    (void) ;

    virtual bool Sequential  (SqlConnection & Connection     ,
                              int             section        ,
                              QString         table          ,
                              QString         column  = "id" ,
                              int             StartId = 1  ) ;
    virtual void ExtractHtml (SqlConnection & Connection     ,
                              int             section      ) ;

  protected:

    virtual void Start       (int section,int & value,bool & go  ) ;
    virtual void setRange    (int section,int   MinV ,int    MaxV) ;
    virtual void Complete    (int section                        ) ;
    virtual void Action      (int section,int   step             ) ;

  private:

    PurePlan * plan ;

} ;

class Q_FOUNDATION_EXPORT WatchDog : public QObject
                                   , public Thread
{
  Q_OBJECT
  public:

    PurePlan * plan     ;
    bool       working  ;
    bool       clock    ;
    bool       exact    ;
    bool       half     ;
    QDateTime  lastTime ;
    TMAPs      events   ;
    UUIDs      current  ;

    explicit WatchDog  (QObject * parent = NULL);
    virtual ~WatchDog  (void);

    bool WatchClock    (bool reported);

  protected:

    QMutex Mutex ;

    virtual void run   (void);

  private:

    void reportTime    (QDateTime current);
    void IsEvents      (void) ;

  public slots:

    void load          (void);
    void Reload        (void);
    void AcceptClock   (bool Exact,bool Half);

  private slots:

    void Gotcha        (void) ;
    void PrivateNotify (SUID uuid,int eType) ;

  signals:

    void Kick          (void) ;
    void LocalNotify   (SUID uuid,int eType) ;
    void Notify        (SUID uuid,int eType) ;

} ;

class Q_FOUNDATION_EXPORT RpcLogHandler : public XmlRpcLogHandler
{
  public:

    explicit RpcLogHandler (PurePlan * plan) ;
    virtual ~RpcLogHandler (void) ;

    virtual void log       (int level,const char * msg) ;

  protected:

    PurePlan * plan ;

} ;

class Q_FOUNDATION_EXPORT RpcErrorHandler : public XmlRpcErrorHandler
{
  public:

    explicit RpcErrorHandler (PurePlan * plan) ;
    virtual ~RpcErrorHandler (void) ;

    virtual void error       (const char * msg) ;

  protected:

    PurePlan * plan ;

} ;

class Q_FOUNDATION_EXPORT Synchronicity : public QObject
                                        , public QScriptable
                                        , public Thread
{
  Q_OBJECT
  public:

    explicit Synchronicity    (QObject * parent = NULL) ;
    virtual ~Synchronicity    (void) ;

  protected:

    QFileSystemWatcher * watcher ;
    bool                 started ;
    bool                 waiting ;
    QDateTime            lastest ;
    QStringList          pathes  ;
    UMAPs                values  ;

    virtual void Configure    (void) ;

    virtual void run          (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual void startup      (bool enabled) ;

    virtual void addWatchers  (void) ;
    virtual void Waiting      (void) ;

    int          addDir       (const QString       dir  ) ;
    bool         addPath      (const QString     & path ) ;
    QStringList  addPaths     (const QStringList & paths) ;
    bool         removePath   (const QString     & path ) ;
    QStringList  removePaths  (const QStringList & paths) ;

    int          Value        (QString key) ;
    int          setValue     (QString key,int value) ;

  protected slots:

    virtual void filesChanged (const QString & path) ;

  private slots:

    void Redirect (void) ;

  signals:

    void Updated (void) ;
    void DropOut (void) ;

} ;

class Q_FOUNDATION_EXPORT JsTools : public QObject
                                  , public QScriptable
                                  , public Thread
                                  , public Enabler
{
  Q_OBJECT
  public:

    ScriptManager   Scripts     ;
    IpGeolocation * geolocation ;
    QtPCAP        * pcap        ;

    explicit           JsTools         (QObject * parent) ;
    virtual           ~JsTools         (void) ;

  protected:

    QTextCodec                      * codec         ;
    QMutex                            ThreadLocker  ;
    QDir                              Bin           ;
    QString                           MtAPI         ;
    QMap < QString , int            > FunctionMaps  ;
    QMap < int     , QScriptEngine *> Engines       ;
    // Values
    QMap < int     , int            > IntInt        ;
    QMap < QString , int            > IntStr        ;
    QMap < int     , double         > DoubleInt     ;
    QMap < QString , double         > DoubleStr     ;
    QMap < int     , QString        > StringInt     ;
    QMap < QString , QString        > StringStr     ;
    QMap < int     , QByteArray     > DataInt       ;
    QMap < QString , QByteArray     > DataStr       ;
    QMap < int     , QDateTime      > DateTimeInt   ;
    QMap < QString , QDateTime      > DateTimeStr   ;
    QMap < QString , QStringList    > StringStrList ;

    virtual void       Configure       (void) ;

    virtual void       run             (int type,N::ThreadData * data) ;

    virtual void       download        (N::VarArgs & args,int Id) ;
    virtual void       RunScript       (N::VarArgs & args,int Id) ;

    virtual QScriptEngine * GetEngine  (int id) ;

  private:

  public slots:

    virtual bool       SetEnabled      (int Id,bool enable) ;
    virtual bool       IsEnabled       (int Id) ;
    virtual bool       SetEnabled      (QString Id,bool enable) ;
    virtual bool       IsEnabled       (QString Id) ;

    virtual void       setInt          (int     id,int value) ;
    virtual void       setInt          (QString id,int value) ;
    virtual int        Int             (int     id) ;
    virtual int        Int             (QString id) ;

    virtual void       setDouble       (int     id,double value) ;
    virtual void       setDouble       (QString id,double value) ;
    virtual double     Double          (int     id) ;
    virtual double     Double          (QString id) ;

    virtual void       setString       (int     id,QString value) ;
    virtual void       setString       (QString id,QString value) ;
    virtual QString    String          (int     id) ;
    virtual QString    String          (QString id) ;

    virtual void       toStrings       (QString id,QString ids,QString splitter) ;
    virtual void       setStrings      (QString id,QStringList ids) ;
    virtual int        Strings         (QString id) ;
    virtual QString    Strings         (QString id,int position) ;
    virtual void       PrepareStrings  (QString id) ;
    virtual void       JoinStrings     (QString id,QString str) ;

    virtual void       setByteArray    (int     id,QByteArray value) ;
    virtual void       setByteArray    (QString id,QByteArray value) ;
    virtual QByteArray ByteArray       (int     id) ;
    virtual QByteArray ByteArray       (QString id) ;

    virtual QDateTime  currentDateTime (void) ;
    virtual void       setDateTime     (int     id,QDateTime dt) ;
    virtual void       setDateTime     (QString id,QDateTime dt) ;
    virtual QDateTime  getDateTime     (int     id) ;
    virtual QDateTime  getDateTime     (QString id) ;
    virtual long long  Elapsed         (int     t1,int     t2) ;
    virtual long long  Elapsed         (QString t1,QString t2) ;

    virtual void       toConsole       (QString message ) ;
    virtual void       toConsoleLn     (QString message ) ;
    virtual void       setRoot         (QString rootpath) ;
    virtual void       setMtAPI        (QString mtapi   ) ;

    virtual bool       Start           (QString command) ;
    virtual int        Execute         (QString command) ;
    virtual bool       Go              (QString command) ;

    virtual void       msleep          (int msecs) ;
    virtual void       sleep           (int secs) ;

    virtual void       setGlobal       (QString key,QScriptValue value) ;
    virtual void       setGlobal       (int threadId,QString key,QScriptValue value) ;

  protected slots:

  private slots:

  signals:

} ;

class Q_FOUNDATION_EXPORT FoundationManager : public QObject
                                            , public QScriptable
                                            , public Thread
{
  Q_OBJECT
  public:

    explicit          FoundationManager (QObject * parent , PurePlan * plan) ;
    virtual          ~FoundationManager (void) ;

    QScriptEngine *   GetEngine         (void) ;

    virtual SUID      Append            (SqlConnection & SC    ,
                                         int             type  ,
                                         QString         name  ,
                                         QDateTime       Start ,
                                         QDateTime       End ) ;
    virtual bool      CloneVariables    (SqlConnection & SC       ,
                                         SUID            original ,
                                         SUID            target ) ;
    virtual bool      CloneVariable     (SqlConnection & SC       ,
                                         SUID            original ,
                                         SUID            target   ,
                                         QString         key    ) ;
    virtual bool      CloneFirst        (SqlConnection & SC       ,
                                         SUID            original ,
                                         SUID            target   ,
                                         int             type   ) ;
    virtual bool      CloneFirst        (SqlConnection & SC       ,
                                         SUID            original ,
                                         SUID            target   ,
                                         int             type     ,
                                         int             group  ) ;
    virtual bool      CloneSecond       (SqlConnection & SC       ,
                                         SUID            original ,
                                         SUID            target   ,
                                         int             type   ) ;
    virtual bool      CloneDependency   (SqlConnection & SC       ,
                                         SUID            original ,
                                         SUID            target ) ;
    virtual void      CompleteEvent     (SqlConnection & SC,SUID uuid) ;
    virtual void      PostponeEvent     (SqlConnection & SC,SUID uuid,qint64 delayMs) ;
    virtual bool      isOwner           (SqlConnection & SC,SUID uuid) ;
    virtual bool      SetScheduleMode   (SqlConnection & SC,SUID uuid,int mode) ;

  protected:

    PurePlan               *  plan        ;
    WMAPs                     Variables   ;
    QMap<int,QScriptEngine *> Engines     ;
    QMap<int,SqlConnection *> Connections ;
    QMap<QString,Sql        > SQLs        ;

    virtual void      run               (int type,ThreadData * data) ;

  private:

  public slots:

    virtual void      print             (QString message) ;
    virtual void      print             (int verbose,QString message) ;

    virtual bool      toBool            (QString key) ;
    virtual QString   toString          (QString key) ;
    virtual QDateTime toDateTime        (QString key) ;
    virtual QVariant  Value             (QString key) ;
    virtual void      setValue          (QString key,QVariant value) ;

    virtual QDateTime addYears          (QDateTime dt,int years) ;
    virtual QDateTime addMonths         (QDateTime dt,int months) ;
    virtual QDateTime addDays           (QDateTime dt,int days) ;
    virtual QDateTime addSeconds        (QDateTime dt,int seconds) ;

    virtual void      execute           (SUID uuid) ;
    virtual void      EventHandler      (SUID uuid) ;
    virtual void      Dispatch          (SUID uuid,int eType,int position) ;
    virtual bool      Verify            (SUID uuid,int eType) ;

    virtual bool      TryDownload       (SUID uuid) ;
    virtual bool      TryUpload         (SUID uuid) ;
    virtual bool      JavaDownload      (SUID uuid,QString JS) ;
    virtual bool      JavaUpload        (SUID uuid,QString JS) ;
    virtual bool      JavaFetch         (SUID uuid,QString JS,QString func) ;
    virtual bool      Download          (SUID uuid,QString url,QString file,qint64 mtimeout) ;
    virtual bool      Upload            (SUID    uuid       ,
                                         QString url        ,
                                         QString file       ,
                                         QString username   ,
                                         QString password   ,
                                         qint64  mtimeout ) ;

    virtual void      CompleteEvent     (SUID uuid) ;
    virtual void      PostponeEvent     (SUID uuid,qint64 delayMs) ;
    virtual bool      isOwner           (SUID uuid) ;

    virtual SUID      GetExecution      (SUID uuid) ;
    virtual bool      Run               (SUID uuid) ;
    virtual void      HandleExecution   (SUID uuid) ;

    virtual void      HandleRepeated    (SUID uuid,int position) ;
    virtual QString   CloneEvent        (QString   uuid    ,
                                         QString   name    ,
                                         QDateTime start   ,
                                         QDateTime final ) ;

    virtual QString   Append            (int type,QString name,QDateTime Start,QDateTime End) ;
    virtual bool      Complete          (QString uuid,QDateTime Start,QDateTime End) ;

    virtual void      HandleDependency  (SUID uuid,int position) ;
    virtual void      HandleDependency  (SUID uuid,UUIDs & After,UUIDs & Current) ;

    virtual void      CreateDirs        (QDir          & source  ,
                                         QDir          & target  ,
                                         QFileInfoList & infos ) ;
    virtual void      Copy              (QDir          & source  ,
                                         QDir          & target  ,
                                         QFileInfoList & infos ) ;

    virtual bool      HandleBackup      (SUID uuid,int position) ;
    virtual bool      Backup            (QString profile) ;
    virtual bool      Copy              (QString source,QString target) ;

    virtual void      HandleMusic       (SUID uuid) ;
    virtual void      HandleVideo       (SUID uuid) ;

  protected slots:

    virtual void      RelayMusic        (void) ;

  private slots:

  signals:

    void PrivateMusic                   (void) ;
    void Music                          (SUID uuid) ;
    void ShowAudio                      (void) ;

} ;

Q_FOUNDATION_EXPORT QScriptValue JsToolCreate         (QScriptContext * context,QScriptEngine * engine) ;
Q_FOUNDATION_EXPORT QScriptValue JsFeederAttachment   (QScriptContext * context,QScriptEngine * engine) ;
Q_FOUNDATION_EXPORT QScriptValue SyncAttachment       (QScriptContext * context,QScriptEngine * engine) ;
Q_FOUNDATION_EXPORT QScriptValue FoundationAttachment (QScriptContext * context,QScriptEngine * engine) ;

namespace Forex
{

class Q_FOUNDATION_EXPORT ImporterPlugin
{
  public:

    explicit ImporterPlugin(void) ;
    virtual ~ImporterPlugin(void) ;

    virtual QString name         (void) const = 0 ;
    virtual bool    initialize   (PurePlan * plan     ,
                                  Sql      & sql      ) = 0 ;
    virtual bool    guess        (QString    filename ) = 0 ;
    virtual bool    acceptable   (QString    supplier ,
                                  QString    symbol   ,
                                  QString    TimeName ) = 0 ;
    virtual bool    go           (bool     & running  ,
                                  PurePlan * plan     ,
                                  QString    Filename ,
                                  QString    supplier ,
                                  QString    symbol   ,
                                  QString    TimeName ) = 0 ;

} ;

}

}

Q_DECLARE_METATYPE(N::PurePlan)
Q_DECLARE_METATYPE(N::TimeFactory)
Q_DECLARE_METATYPE(N::ParamentQuery)
Q_DECLARE_METATYPE(N::MimeManager)
Q_DECLARE_METATYPE(N::HistoryManager)
Q_DECLARE_METATYPE(N::TaskManager)
Q_DECLARE_METATYPE(N::EventManager)
Q_DECLARE_METATYPE(N::CodeManager)
Q_DECLARE_METATYPE(N::NetworkManager)
Q_DECLARE_METATYPE(N::DocumentManager)
Q_DECLARE_METATYPE(N::KnowledgeManager)
Q_DECLARE_METATYPE(N::FinanceManager)
Q_DECLARE_METATYPE(N::SqlOrganizer)

Q_DECLARE_INTERFACE(N::Forex::ImporterPlugin , "com.neutrino.forex.importer" )

QT_END_NAMESPACE

#endif
