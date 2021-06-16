#include <qtfoundation.h>

N::PurePlan * N::qPlan = NULL ;

typedef struct     {
  N::PurePlan * pp ;
} PurePlanPacket   ;

N::PurePlan:: PurePlan        ( void )
            : VirtualProgress (      )
{
  PurePlanPacket * pp = new PurePlanPacket ( )                    ;
  pp -> pp        = this                                          ;
  Application     = ""                                            ;
  Machine         = 0                                             ;
  Hostname        = QHostInfo::localHostName()                    ;
  LanguageId      = 1819                                          ;
  UserUuid        = 0                                             ;
  Verbose         = 20                                            ;
  MaxLogs         = -1                                            ;
  ThreadStack     = 64 * 1024 * 1024                              ;
  Visible         = true                                          ;
  scene           = false                                         ;
  canContinue     = true                                          ;
  Native          = NULL                                          ;
  Remote          = NULL                                          ;
  Ftp             = NULL                                          ;
  Neighbors       = NULL                                          ;
  Knock           = NULL                                          ;
  House           = NULL                                          ;
  uid             = 500                                           ;
  gid             = 500                                           ;
  geolocation     = NULL                                          ;
  KeyThread       = NULL                                          ;
  locator         = LocateFile                                    ;
  destroyerMutex  = new QMutex ( )                                ;
  RPC     . Data . Controller = &canContinue                      ;
  Request . Data . Controller = &canContinue                      ;
  languages.Supports.clear()                                      ;
  languages.Supports << 1819                                      ;
  languages.Supports << 4871                                      ;
  languages.Supports << 7523                                      ;
  languages.Supports << 2726                                      ;
  languages.Supports << 5568                                      ;
  languages.Supports << 5903                                      ;
  languages.Supports << 1932                                      ;
  languages.Supports << 3127                                      ;
  languages.Supports << 3439                                      ;
  languages.Supports << 2560                                      ;
  Encoders . clear ( )                                            ;
  AvEncoder :: setEncoders ( Encoders , 0 )                       ;
  AvEncoder :: setEncoders ( Encoders , 1 )                       ;
  AvEncoder :: setEncoders ( Encoders , 2 )                       ;
  /////////////////////////////////////////////////////////////////
  Variables [ "CPU"        ] = QVariant(cpu.Cpu          ())      ;
  Variables [ "Processors" ] = QVariant(cpu.Processors   ())      ;
  Variables [ "Cache"      ] = QVariant(cpu.CacheLineSize())      ;
  Variables [ "Memory"     ] = QVariant(Memory           ())      ;
  Variables [ "PurePlan"   ] = VoidVariant ( pp )                 ;
  /////////////////////////////////////////////////////////////////
  Booleans  [ "Knock"      ] = true                               ;
  Booleans  [ "House"      ] = true                               ;
  Booleans  [ "XmlRpc"     ] = true                               ;
  Booleans  [ "Clone"      ] = false                              ;
  Booleans  [ "Awake"      ] = true                               ;
  /////////////////////////////////////////////////////////////////
  Scripts . Creators [ "MtJS"        ]   = JsToolCreate           ;
  Scripts . Creators [ "FTP"           ] = FtpAttachement         ;
  Scripts . Creators [ "SQL"           ] = SqlAttachment          ;
  Scripts . Creators [ "File"          ] = FileAttachment         ;
  Scripts . Creators [ "Directory"     ] = DirectoryAttachment    ;
  Scripts . Creators [ "Socket"        ] = SocketAttachment       ;
  Scripts . Creators [ "Settings"      ] = SettingsAttachment     ;
  Scripts . Creators [ "ByteArray"     ] = ByteArrayAttachment    ;
  Scripts . Creators [ "ScriptStrings" ] = ScriptCreateString     ;
  Scripts . Creators [ "Http"          ] = AttacheHttp            ;
  Scripts . Creators [ "Xml"           ] = AttacheXml             ;
  Scripts . Creators [ "Mutex"         ] = MutexAttachment        ;
  Scripts . Creators [ "Transformer"   ] = TransformerAttachment  ;
  Scripts . Creators [ "MtAPI"         ] = JsFeederAttachment     ;
  Scripts . Creators [ "Dukascopy"     ] = DukascopyCreate        ;
  Scripts . Creators [ "Foundation"    ] = FoundationAttachment   ;
  Scripts . Creators [ "RpcServer"     ] = RPC::ServerAttachement ;
  Scripts . Creators [ "RpcClient"     ] = RPC::ClientAttachement ;
  /////////////////////////////////////////////////////////////////
  ProgressReporter::setVirtual  ( this        )                   ;
  personality = new Personality ( true        )                   ;
  parallel    = new Parallel    ( this        )                   ;
  /////////////////////////////////////////////////////////////////
  parallel   -> Join            ( personality )                   ;
}

N::PurePlan::~PurePlan(void)
{
  if ( NULL != destroyerMutex ) {
    delete destroyerMutex       ;
  }                             ;
  destroyerMutex = NULL         ;
}

int N::PurePlan::type (void) const
{
  return Plain ;
}

void N::PurePlan::arguments(int argc,char ** argv)
{
  QFileInfo F                                       ;
  QDir      D                                       ;
  QString   App                                     ;
  #if defined(Q_OS_WIN)
  App = N::ApplicationPath()                        ;
  #else
  App = argv[0]                                     ;
  #endif
  User = QDir::homePath()                           ;
  F.setFile(App)                                    ;
  if (F.isSymLink()) F.setFile(F.symLinkTarget())   ;
  Bin       = F.dir         ()                      ;
  D         = F.dir         ()                      ;
  D         . cdUp          ()                      ;
  Root      = D                                     ;
  Home      = D.absolutePath()                      ;
  Arguments . clear         ()                      ;
  QTextCodec * codec = QTextCodec::codecForLocale() ;
  for (int i=0;i<argc;i++)                          {
    Arguments << QString(codec->toUnicode(argv[i])) ;
  }                                                 ;
}

void N::PurePlan::addPlugin(QString path)
{
  if (!PluginPaths.contains(path))             {
    PluginPaths << Root.absoluteFilePath(path) ;
  }                                            ;
}

void N::PurePlan::setLibraryPaths(void)
{
  for (int n=0;n<PluginPaths.count();n++)  {
    if (NotNull(qApp))                     {
      qApp->addLibraryPath(PluginPaths[n]) ;
    }                                      ;
  }                                        ;
}

QString N::PurePlan::Path(QString name)
{
  return Root.absoluteFilePath(name);
}

QString N::PurePlan::Temporary(QString filename)
{
  QDir temp(Path("Temp"))                     ;
  if (Dirs.contains(N::Directory::Temporary)) {
    temp = Dirs[N::Directory::Temporary]      ;
  }                                           ;
  return temp.absoluteFilePath(filename)      ;
}

int N::PurePlan::addPath(int Id,QDir dir)
{
  Dirs[Id] = dir     ;
  return Dirs.size() ;
}

QDir N::PurePlan::Path(int Id)
{
  QDir d                           ;
  if (!Dirs.contains(Id)) return d ;
  return Dirs[Id]                  ;
}

int N::PurePlan::addMap(int id,QString name)
{
  NameMap[id  ] = name   ;
  IdMap  [name] = id     ;
  return NameMap.count() ;
}

int N::PurePlan::Map(QString name)
{
  return IdMap[name];
}

int N::PurePlan::addTable(int id,QString name)
{
  Tables[id] = sql.Name(name);
  return Tables.count();
}

QString N::PurePlan::Table(QString name)
{
  return Tables[IdMap[name]];
}

QString N::PurePlan::Table(int id)
{
  if (!containsTable(id)) return "" ;
  return Tables [ id ]              ;
}

bool N::PurePlan::containsTable(int id)
{
  return Tables . contains ( id ) ;
}

void N::PurePlan::AudioDepot(QString scope)
{
  AudioDepots = Native->getStringList("AudioDepot") ;
}

void N::PurePlan::VideoDepot(QString scope)
{
  VideoDepots = Native->getStringList("VideoDepot") ;
}

QString N::PurePlan::AudioFile(QString filename)
{
  return locator ( AudioDepots , filename ) ;
}

QString N::PurePlan::VideoFile(QString filename)
{
  return locator ( VideoDepots , filename ) ;
}

void N::PurePlan::Play(QString name)
{
  Audio . Play ( name                                  ,
                 Equipments   [ DefaultAudioOutput ]   ,
                 EquipmentIDs [ DefaultAudioOutput ] ) ;
}

void N::PurePlan::Play(SUID uuid)
{
  Audio . Play ( uuid                                  ,
                 Equipments   [ DefaultAudioOutput ]   ,
                 EquipmentIDs [ DefaultAudioOutput ] ) ;
}

int N::PurePlan::addSound(SUID uuid,QString name)
{
  AudioData data                 ;
  ////////////////////////////////
  #ifdef Q_OS_IOS
  #ifdef CIOSDEBUG
  qDebug ( "addSound(SUID uuid,QString name)" ) ;
  #endif
  #endif
  if (Acoustics::Load            (
        ME                       ,
        uuid                     ,
        data                 ) ) {
    Audio.Append ( name , data ) ;
    data.reset   (             ) ;
  } else                         {
    #ifdef Q_OS_IOS
    #ifdef CIOSDEBUG
    Debug ( 90 , QString("Append sound %1 %2 failure").arg(uuid).arg(name) ) ;
    #endif
    #endif
  }                              ;
  ////////////////////////////////
  return Audio.AudioBank.size()  ;
}

int N::PurePlan::addSound(QString identifier,QString name)
{
  #ifdef DISABLE_IOS_SOUND
  // Temporary measure
  return Audio.AudioBank.size()  ;
  #endif
  AudioData data                 ;
  ////////////////////////////////
  #ifdef Q_OS_IOS
  #ifdef CIOSDEBUG
  qDebug ( "addSound(QString identifier,QString name)" ) ;
  #endif
  #endif
  if (Acoustics::Load            (
        ME                       ,
        identifier               ,
        data                 ) ) {
    Audio.Append ( name , data ) ;
    #ifdef Q_OS_IOS
    #ifdef CIOSDEBUG
    QAudioFormat FMT = data.AudioFormat ( )                              ;
    QString      emsg                                                    ;
    emsg = QString("Bytes Per Frame : %1").arg(FMT.bytesPerFrame())      ;
    Debug  ( emsg )                                                      ;
    emsg = QString("Channels        : %1").arg(FMT.channelCount())       ;
    Debug  ( emsg )                                                      ;
    emsg = QString("Sample rate     : %1").arg(FMT.sampleRate())         ;
    Debug  ( emsg )                                                      ;
    emsg = QString("Sample size     : %1").arg(FMT.sampleSize())         ;
    Debug  ( emsg )                                                      ;
    emsg = QString("Sample type     : %1").arg((int)FMT.sampleType())    ;
    Debug  ( emsg )                                                      ;
    #endif
    #endif
    data.reset   (             ) ;
  } else                         {
    #ifdef Q_OS_IOS
    #ifdef CIOSDEBUG
    Debug ( 90 , QString("Append sound %1 %2 failure").arg(identifier).arg(name) ) ;
    #endif
    #endif
  }                              ;
  ////////////////////////////////
  return Audio.AudioBank.size()  ;
}

int N::PurePlan::addSound(int command,QString name,QString identifier)
{
  Audio.SoundMaps [ command ] = name ;
  addSound(identifier,name)          ;
  return Audio.SoundMaps.size ( )    ;
}

int N::PurePlan::addSound(int command,QString name,SUID uuid)
{
  Audio.SoundMaps [ command ] = name ;
  addSound(uuid,name)                ;
  return Audio.SoundMaps.size ( )    ;
}

bool N::PurePlan::Initialize(void)
{
  Native -> Initialize                (       )                               ;
  sql      = Native -> getSqlSettings ( "SQL" )                               ;
  LocalSql = sql                                                              ;
  /////////////////////////////////////////////////////////////////////////////
  ConnectSQL    ( SC , *(Native->SQL) )                                       ;
    Native->beginGroup("System")                                              ;
    if (Native -> contains(SC,"Language"))                                    {
      LanguageId = Native -> value(SC,"Language").toInt()                     ;
    } else                                                                    {
      LanguageId = SystemLanguageId()                                         ;
    }                                                                         ;
    if (Native -> contains(SC,"User"))                                        {
      UserUuid   = Native -> value(SC,"User").toULongLong()                   ;
    } else UserUuid = 0                                                       ;
    if (Native -> contains(SC,"Machine"))                                     {
      Machine   = Native -> value(SC,"Machine").toULongLong()                 ;
    } else Machine  = 0                                                       ;
    if (Native -> contains(SC,"ThreadStack"))                                 {
      ThreadStack = Native -> value(SC,"ThreadStack").toInt()                 ;
    }                                                                         ;
    Native -> endGroup  (        )                                            ;
    ///////////////////////////////////////////////////////////////////////////
    AudioInput  = Acoustics :: Devices ( Acoustics :: Input  )                ;
    AudioOutput = Acoustics :: Devices ( Acoustics :: Output )                ;
    int AI      = 0                                                           ;
    int AO      = 0                                                           ;
    if (AI>=0 && AudioInput .contains(AI))                                    {
      Equipments[DefaultAudioInput ] = AudioInput [AI]                        ;
      Equipments[DefaultEar        ] = AudioInput [AI]                        ;
    }                                                                         ;
    if (AO>=0 && AudioOutput.contains(AO))                                    {
      Equipments[DefaultAudioOutput] = AudioOutput[AO]                        ;
      Equipments[DefaultSpeech     ] = AudioOutput[AO]                        ;
    }                                                                         ;
    ///////////////////////////////////////////////////////////////////////////
    Native -> beginGroup ("Interface"         )                               ;
    if (Native -> contains(SC,"AudioInput"   ))                               {
      Equipments[DefaultAudioInput ]=Native -> valueString(SC,"AudioInput" )  ;
    }                                                                         ;
    if (Native -> contains(SC,"AudioInputID" ))                               {
      EquipmentIDs[DefaultAudioInput ]=Native -> value(SC,"AudioInputID" ).toInt() ;
    }                                                                         ;
    if (Native -> contains(SC,"AudioOutput"  ))                               {
      Equipments[DefaultAudioOutput]=Native -> valueString(SC,"AudioOutput")  ;
    }                                                                         ;
    if (Native -> contains(SC,"AudioOutputID"))                               {
      EquipmentIDs[DefaultAudioOutput]=Native -> value(SC,"AudioOutputID").toInt() ;
    }                                                                         ;
    if (Native -> contains(SC,"Speech"     ))                                 {
      Equipments[DefaultSpeech     ]=Native -> valueString(SC,"Speech"     )  ;
    }                                                                         ;
    if (Native -> contains(SC,"Ear"        ))                                 {
      Equipments[DefaultEar        ]=Native -> valueString(SC,"Ear"        )  ;
    }                                                                         ;
    Native -> endGroup     (                    )                             ;
    Audio    . setDevice   ( Equipments  [DefaultAudioOutput] )               ;
    Audio    . setDeviceId ( EquipmentIDs[DefaultAudioOutput] )               ;
    Recorder . setDevice   ( Equipments  [DefaultAudioInput ] )               ;
    Recorder . setDeviceId ( EquipmentIDs[DefaultAudioInput ] )               ;
    ///////////////////////////////////////////////////////////////////////////
    Native -> beginGroup ("Logs")                                             ;
    if (Native -> contains(SC,"Verbose"))                                     {
      Verbose = Native -> value(SC,"Verbose").toInt()                         ;
    }                                                                         ;
    if (Native -> contains(SC,"Lines"  ))                                     {
      MaxLogs = Native -> value(SC,"Lines"  ).toInt()                         ;
    }                                                                         ;
    Native -> endGroup   (         )                                          ;
  DisconnectSQL ( SC                                                )         ;
  return true                                                                 ;
}

bool N::PurePlan::setSettings(QString name)
{
  if (IsNull(Native)) return false                     ;
  Native->Mode = "SQL"                                 ;
  Native->SQL  = new Sql()                             ;
  Native->SQL -> SqlMode = "SQLITE"                    ;
  Native->SQL -> dbName  = Root.absoluteFilePath(name) ;
  QFile F(Native->SQL->dbName)                         ;
  bool exists = F.exists()                             ;
  if (exists) Native->Initialize()                     ;
  return exists                                        ;
}

bool N::PurePlan::existSettings(void)
{
  if (IsNull(Native)) return false ;
  QFile F(Native->SQL->dbName)     ;
  return F.exists()                ;
}

void N::PurePlan::loadUuids(QString scope,QStringList items)
{
  if (IsNull(Native)) return                       ;
  if (items.count()<=0) return                     ;
  QString k                                        ;
  Uuids . clear()                                  ;
  ConnectSQL          ( SC , *(Native->SQL)      ) ;
  Native-> beginGroup (scope)                      ;
  foreach (k,items)                                {
    if (Native->contains(SC,k))                    {
      Uuids[k] = Native->value(SC,k).toULongLong() ;
    } else Uuids[k] = 0                            ;
  }                                                ;
  Native-> endGroup   (     )                      ;
  DisconnectSQL       ( SC                       ) ;
}

void N::PurePlan::loadValues(QString scope,QStringList items)
{
  if (IsNull(Native)) return                  ;
  if (items.count()<=0) return                ;
  QString k                                   ;
  Values . clear()                            ;
  ConnectSQL          ( SC , *(Native->SQL) ) ;
  Native-> beginGroup (scope)                 ;
  foreach (k,items)                           {
    if (Native->contains(SC,k))               {
      Values[k] = Native->value(SC,k).toInt() ;
    } else Values[k] = 0                      ;
  }                                           ;
  Native-> endGroup   (     )                 ;
  DisconnectSQL       ( SC                  ) ;
}

void N::PurePlan::loadBooleans(QString scope,QStringList items)
{
  if (IsNull(Native)) return                     ;
  if (items.count()<=0) return                   ;
  QString k                                      ;
  Booleans . clear()                             ;
  ConnectSQL          ( SC , *(Native->SQL)    ) ;
  Native-> beginGroup (scope)                    ;
  foreach (k,items)                              {
    if (Native->contains(SC,k))                  {
      Booleans[k] = Native->value(SC,k).toBool() ;
    } else Booleans[k] = false                   ;
  }                                              ;
  Native-> endGroup   (     )                    ;
  DisconnectSQL       ( SC                     ) ;
}

int N::PurePlan::setLanguage(int languageId)
{
  if (IsNull(Native)) return LanguageId          ;
  Native->beginGroup ( "System"                ) ;
  Native->setValue   ( "Language" , languageId ) ;
  Native->endGroup   (                         ) ;
  LanguageId = languageId                        ;
  return LanguageId                              ;
}

void N::PurePlan::processEvents (void)
{
}

int N::PurePlan::setPath(int Id,QString scope,QString key)
{
  Native->beginGroup(scope)       ;
  if (Native->contains(key))      {
    QString ph                    ;
    ph = Native->valueString(key) ;
    QDir P                        ;
    P . setPath ( ph )            ;
    Dirs [ Id ] = P               ;
  }                               ;
  Native->endGroup  (     )       ;
  return Dirs.size()              ;
}

void N::PurePlan::StartTalk(QString Path)
{ Q_UNUSED ( Path                                ) ;
  nDropOut ( !Equipments.contains(DefaultSpeech) ) ;
  QString devname = Equipments  [DefaultSpeech]    ;
  int     devId   = EquipmentIDs[DefaultSpeech]    ;
  Mouth . setLanguage ( LanguageId )               ;
  Mouth . setDevice   ( devname    )               ;
//  Mouth . setDeviceId ( devId      )               ;
  Mouth . Start       (            )               ;
}

void N::PurePlan::StopTalk(void)
{
  Mouth . Stop ( ) ;
}

void N::PurePlan::Talk(QString message)
{
  if (!VoiceOutput) return   ;
  Mouth . Append ( message ) ;
}

void N::PurePlan::Talk(int verbose,QString message)
{
  if (verbose>Verbose) return ;
  if (!VoiceOutput) return    ;
  Mouth . Append ( message )  ;
}

void N::PurePlan::TalkNumber(long long digits,bool verbose)
{
  if (!VoiceOutput) return ;
  switch (LanguageId)      {
    case 4871              :
    break                  ;
    case 7523              :
    break                  ;
    case 1819              :
    break                  ;
    default                :
    break                  ;
  }                        ;
}

void N::PurePlan::TalkNumber(double num,int digits,bool verbose)
{
  if (!VoiceOutput) return ;
  switch (LanguageId)      {
    case 4871              :
    break                  ;
    case 7523              :
    break                  ;
    case 1819              :
    break                  ;
    default                :
    break                  ;
  }                        ;
}

bool N::PurePlan::Talkative(int verbose)
{
  return ( Verbose > verbose ) ;
}

void N::PurePlan::Debug(QString message)
{
  N::printf(message,true,false) ;
}

void N::PurePlan::Debug(int verbose,QString message)
{
  if (verbose>Verbose) return   ;
  N::printf(message,true,false) ;
}

void N::PurePlan::Notify(QString sound,QString message)
{
  if ( sound . length ( ) > 0 ) Play ( sound ) ;
  if ( message . length ( ) > 0 )              {
    Talk  ( message )                          ;
    Debug ( message )                          ;
  }                                            ;
}

int N::PurePlan::Join(Junction * junction)
{
  if (!junctions.contains(junction)) {
    junctions << junction            ;
  }                                  ;
  return junctions.count()           ;
}

N::Junction * N::PurePlan::GrabJunction(QObject * parent)
{
  Junction * junction = new Junction(parent) ;
  junctions << junction                      ;
  return junction                            ;
}

N::Junctions N::PurePlan::Connectivity(Conduit * conduit)
{
  Junctions j                                     ;
  for (int i=0;i<junctions.count();i++)           {
    if (junctions[i]->conduits.contains(conduit)) {
      j << junctions[i]                           ;
    }                                             ;
  }                                               ;
  return j                                        ;
}

bool N::PurePlan::Rehabilitate(void)
{
  nKickOut ( junctions . count ( ) <= 0 , false ) ;
  Junctions j                                     ;
  for (int i=0;i<junctions.count();i++)           {
    if (junctions[i]->dead())                     {
      j << junctions[i]                           ;
    }                                             ;
  }                                               ;
  /////////////////////////////////////////////////
  for (int i=0;i<j.count();i++)                   {
    int index = junctions.indexOf(j[i])           ;
    if (index>=0) junctions.takeAt(index)         ;
    delete j[i]                                   ;
  }                                               ;
  return true                                     ;
}

bool N::PurePlan::Dredge(bool start)
{
  nKickOut ( junctions . count ( ) <= 0 , false ) ;
  if (start)                                      {
    for (int i=0;i<junctions.count();i++)         {
      if (!junctions[i]->draining)                {
        junctions[i]->start()                     ;
      }                                           ;
    }                                             ;
  } else                                          {
    for (int i=0;i<junctions.count();i++)         {
      if (junctions[i]->draining)                 {
        junctions[i]->draining = false            ;
      }                                           ;
    }                                             ;
  }                                               ;
  return true                                     ;
}

void * N::PurePlan::Percentage(QString format)
{
  return NULL ;
}

void N::PurePlan::setRange(void * bar,int Min,int Max)
{
}

void N::PurePlan::setValue(void * bar,int value)
{
}

void N::PurePlan::Finish(void * bar)
{
}

int N::PurePlan::Progress(QString name,QString format)
{
  return -1 ;
}

void N::PurePlan::setRange(int Id,qint64 Min,qint64 Max)
{
}

void N::PurePlan::setFrequency(int Id,QString cFmt,QString rFmt)
{
}

void N::PurePlan::ProgressName(int Id,QString name)
{
}

void N::PurePlan::ProgressText(int Id,QString message)
{
}

void N::PurePlan::setProgress(int Id,QString format)
{
}

void N::PurePlan::Start(int Id,qint64 * Value,bool * Running)
{
}

void N::PurePlan::Finish(int Id)
{
}

bool N::PurePlan::ProgressReady (int Id,int msecs)
{
  return true ;
}

void N::PurePlan::LoadSupports(Languages & L)
{
  L . Default   = LanguageId                                   ;
  L . languages . clear ( )                                    ;
  L . Uuids     . clear ( )                                    ;
  L . Names     . clear ( )                                    ;
  if (!containsTable(Tables::MajorUuid)) return                ;
  if (!containsTable(Tables::Names    )) return                ;
  if (!containsTable(Tables::Languages)) return                ;
  SqlConnection LC(sql)                                        ;
  if (LC.open("nLanguages","equal"))                           {
    QString Q                                                  ;
    Q = LC.sql.SelectFrom                                      (
          "id,uuid,native"                                     ,
          Tables[Tables::Languages]                            ,
          "order by rank asc ;"                              ) ;
    SqlLoopNow(LC,Q)                                           ;
      int     id     = LC.Value (0).toInt()                    ;
      SUID    uuid   = LC.Uuid  (1)                            ;
      QString Native = LC.String(2)                            ;
      if (uuid>0)                                              {
        L.languages       << id                                ;
        L.Uuids   [ id   ] = uuid                              ;
        L.UuidIDs [ uuid ] = id                                ;
        L.Names   [ uuid ] = Native                            ;
      }                                                        ;
      processEvents ( )                                        ;
    SqlLoopErr(LC,Q)                                           ;
    SqlLoopEnd(LC,Q)                                           ;
    ForLanguage(LID,L.languages)                               ;
      SUID u = L.Uuids[LID]                                    ;
      Q = LC . sql . SelectFrom                                (
            "name"                                             ,
            Tables[Tables::Names]                              ,
            QString ( "where uuid = %1 and language = %2"      )
            .arg    ( u                                        )
            .arg    ( L.Default                            ) ) ;
      IfSqlQuery(LC,Q) L.Names[u] = LC.String(0)               ;
      processEvents ( )                                        ;
    EndLanguage(LID,L.languages)                               ;
    LC.close()                                                 ;
  }                                                            ;
  LC.remove()                                                  ;
}

QStringList N::PurePlan::LocalAddresses(void)
{
  QHostInfo           info                ;
  QList<QHostAddress> address             ;
  QStringList         msg                 ;
  info    = QHostInfo::fromName(Hostname) ;
  address = info . addresses ( )          ;
  if ( address . count ( ) > 0 )          {
    for (int i=0;i<address.count();i++)   {
      msg << address [ i ] . toString ( ) ;
    }                                     ;
  }                                       ;
  return msg                              ;
}

QHostAddress N::PurePlan::LocalAddress(QAbstractSocket::NetworkLayerProtocol protocol)
{
  QHostInfo           info                     ;
  QList<QHostAddress> address                  ;
  info    = QHostInfo::fromName(Hostname)      ;
  address = info . addresses ( )               ;
  if ( address . count ( ) > 0 )               {
    for (int i=0;i<address.count();i++)        {
      if ( protocol == address[i].protocol() ) {
        return address[i]                      ;
      }                                        ;
    }                                          ;
  }                                            ;
  return QHostAddress ( )                      ;
}

bool N::PurePlan::Existing(QObject * parent,int port)
{
  QHostAddress        address = LocalAddress(QAbstractSocket::IPv4Protocol)  ;
  QString             Local   = address . toString (     )                   ;
  QStringList         lists   = LocalAddresses     (     )                   ;
  QString             msg     = lists . join       ( " " )                   ;
  UdpBroadcast        udp                                                    ;
  CiosDiscovery     * discovery                                              ;
  CiosNeighbors     * neighbors                                              ;
  bool                existing = false                                       ;
  ////////////////////////////////////////////////////////////////////////////
  discovery          = new CiosDiscovery (                                 ) ;
  if ( IsNull(Neighbors) )                                                   {
    neighbors        = new CiosNeighbors ( parent                          ) ;
    neighbors       -> Used  = 1                                             ;
  } else                                                                     {
    neighbors        = Neighbors                                             ;
    neighbors       -> Used ++                                               ;
  }                                                                          ;
  discovery         -> neighbors = neighbors                                 ;
  udp . Interpreter  = discovery                                             ;
  udp . Interpreter -> setParameter      ( "Application" , Application     ) ;
  udp . Interpreter -> setParameter      ( "Hostname"    , Hostname        ) ;
  udp . Interpreter -> setParameter      ( "Addresses"   , msg             ) ;
  udp . Interpreter -> setParameter      ( "Address"     , Local           ) ;
  udp . Interpreter -> setParameter      ( "Port"        , port            ) ;
  udp . Interpreter -> setParameter      ( "State"       , 102             ) ;
  udp . Interpreter -> setParameter      ( "Interval"    , 50              ) ;
  udp . Probe                            (                                 ) ;
  ////////////////////////////////////////////////////////////////////////////
  if ( udp . Interpreter -> Parameters . contains ( "Myself" ) )             {
    if ( udp . Interpreter -> Parameters [ "Myself" ] . toBool ( ) )         {
      existing = true                                                        ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  udp . Interpreter -> setParameter       ( "Running"    , false           ) ;
  Neighbors = neighbors                                                      ;
  return existing                                                            ;
}

bool N::PurePlan::UdpBind(int port)
{
  if ( IsNull  ( Neighbors ) ) return false                                  ;
  if ( NotNull ( House     ) ) return false                                  ;
  ////////////////////////////////////////////////////////////////////////////
  QHostAddress    address = LocalAddress(QAbstractSocket::IPv4Protocol)      ;
  QString         Local   = address . toString (     )                       ;
  QStringList     lists   = LocalAddresses     (     )                       ;
  QString         msg     = lists . join       ( " " )                       ;
  CiosDiscovery * discovery                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  House                 = new UdpBroadcast  (                              ) ;
  discovery             = new CiosDiscovery (                              ) ;
  Neighbors            -> Used ++                                            ;
  discovery            -> neighbors = Neighbors                              ;
  ////////////////////////////////////////////////////////////////////////////
  House -> Data . Controller = &canContinue                                  ;
  House -> Interpreter  = discovery                                          ;
  House -> Interpreter -> setParameter      ( "Application" , Application  ) ;
  House -> Interpreter -> setParameter      ( "Hostname"    , Hostname     ) ;
  House -> Interpreter -> setParameter      ( "Addresses"   , msg          ) ;
  House -> Interpreter -> setParameter      ( "Address"     , Local        ) ;
  House -> Interpreter -> setParameter      ( "Port"        , port         ) ;
  House -> Interpreter -> setParameter      ( "Interval"    , 15           ) ;
  House -> start                            ( 10001                        ) ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

bool N::PurePlan::UdpBeacon(int port)
{
  if ( IsNull  ( Neighbors ) ) return false                                  ;
  if ( NotNull ( Knock     ) ) return false                                  ;
  ////////////////////////////////////////////////////////////////////////////
  QHostAddress    address = LocalAddress(QAbstractSocket::IPv4Protocol)      ;
  QString         Local   = address . toString (     )                       ;
  QStringList     lists   = LocalAddresses     (     )                       ;
  QString         msg     = lists . join       ( " " )                       ;
  CiosDiscovery * discovery                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  Knock                 = new UdpBroadcast  (                              ) ;
  discovery             = new CiosDiscovery (                              ) ;
  Neighbors            -> Used ++                                            ;
  discovery            -> neighbors = Neighbors                              ;
  ////////////////////////////////////////////////////////////////////////////
  Knock -> Data . Controller = &canContinue                                  ;
  Knock -> Interpreter  = discovery                                          ;
  Knock -> Interpreter -> setParameter      ( "Application" , Application  ) ;
  Knock -> Interpreter -> setParameter      ( "Hostname"    , Hostname     ) ;
  Knock -> Interpreter -> setParameter      ( "Addresses"   , msg          ) ;
  Knock -> Interpreter -> setParameter      ( "Address"     , Local        ) ;
  Knock -> Interpreter -> setParameter      ( "Port"        , port         ) ;
  Knock -> Interpreter -> setParameter      ( "Interval"    , 150          ) ;
  Knock -> start                            ( 10002                        ) ;
  ////////////////////////////////////////////////////////////////////////////
  return true                                                                ;
}

bool N::PurePlan::LoadGeolocation(QString filename)
{
  if ( NULL != geolocation ) return true               ;
  if ( filename . length ( ) <= 0 )                    {
    QDir d = Dirs [ Directory::Users ]                 ;
    filename = d . absoluteFilePath ( "GeoLite2.txt" ) ;
  }                                                    ;
  geolocation  = new IpGeolocation ( )                 ;
  return geolocation -> FromTXT ( filename )           ;
}

bool N::PurePlan::Beacon(int p)
{
  if ( p    <= 0        ) return false                ;
  if ( NULL == parallel ) return false                ;
  Variables ["MyPort"] = p                            ;
  if ( NULL != personality )                          {
    personality   -> setPort         ( p            ) ;
    personality   -> ObtainAddresses ( Hostname     ) ;
  }                                                   ;
  parallel        -> setHost         ( Hostname , p ) ;
  return parallel -> Prepare         ( true         ) ;
}

int N::PurePlan::addDestroyer(Destroyer * destroyer)
{
  if ( NULL != destroyerMutex ) destroyerMutex -> lock   ( ) ;
  Destroyers << destroyer                                    ;
  if ( NULL != destroyerMutex ) destroyerMutex -> unlock ( ) ;
  return Destroyers . count ( )                              ;
}

int N::PurePlan::removeDestroyer(Destroyer * destroyer)
{
  if ( NULL != destroyerMutex ) destroyerMutex -> lock   ( ) ;
  int idx = Destroyers . indexOf ( destroyer )               ;
  if ( idx >= 0 ) Destroyers . takeAt ( idx )                ;
  if ( NULL != destroyerMutex ) destroyerMutex -> unlock ( ) ;
  return Destroyers . count ( )                              ;
}

void N::PurePlan::Recycling(void)
{
  if ( NULL != destroyerMutex ) destroyerMutex -> lock   ( ) ;
  for (int i = 0 ; i < Destroyers . count ( ) ; i++ )        {
    Destroyers [ i ] -> Recycling ( )                        ;
  }                                                          ;
  if ( NULL != destroyerMutex ) destroyerMutex -> unlock ( ) ;
}

void N::PurePlan::Destructor(void)
{
  if ( NULL != destroyerMutex ) destroyerMutex -> lock   ( ) ;
  for (int i = 0 ; i < Destroyers . count ( ) ; i++ )        {
    Destroyers [ i ] -> Destructor ( )                       ;
  }                                                          ;
  Destroyers . clear ( )                                     ;
  if ( NULL != destroyerMutex ) destroyerMutex -> unlock ( ) ;
}

bool N::SQL::     FetchSql (
       QString    Filename ,
       QString    Username ,
       QString    Scope    ,
       PurePlan & Plan     )
{
  bool correct = false                          ;
  Sql sql                                       ;
  sql.SqlMode = "SQLITE"                        ;
  sql.dbName  = Filename                        ;
  SqlConnection SC ( sql )                      ;
  QString       CN = QtUUID::createUuidString() ;
  if (SC.open("Neutrino",CN))                   {
    QString Q                                   ;
    QString W                                   ;
    W = QString ( "where username like '%1' "
                  "and scope like '%2' ;"       )
        . arg   ( Username                      )
        . arg   ( Scope                       ) ;
    Q = SC.sql.SelectFrom                       (
          "keyword,value"                       ,
          "settings"                            ,
          W                                   ) ;
    SqlLoopNow ( SC , Q )                       ;
      QString k = SC.String(0)                  ;
      QString v = SC.String(1)                  ;
      if (k=="profile"   )                      {
        Plan.sql.Profile      = v               ;
      }                                         ;
      if (k=="sqlmode"   )                      {
        Plan.sql.SqlMode      = v               ;
      }                                         ;
      if (k=="hostname"  )                      {
        Plan.sql.Hostname     = v               ;
      }                                         ;
      if (k=="username"  )                      {
        Plan.sql.Username     = v               ;
      }                                         ;
      if (k=="password"  )                      {
        Plan.sql.Password     = v               ;
      }                                         ;
      if (k=="database"  )                      {
        Plan.sql.dbName       = v               ;
      }                                         ;
      if (k=="prefix"    )                      {
        Plan.sql.PrefixName   = v               ;
      }                                         ;
      if (k=="table"     )                      {
        Plan.sql.TableName    = v               ;
      }                                         ;
      if (k=="command"   )                      {
        Plan.sql.SqlCommand   = v               ;
      }                                         ;
      if (k=="connection")                      {
        Plan.sql.Connection   = v               ;
      }                                         ;
      if (k=="options"   )                      {
        Plan.sql.Options      = v               ;
      }                                         ;
      if (k=="foreignkey")                      {
        Plan.sql.isForeignKey = ( v == "true" ) ;
      }                                         ;
      if (k=="indexing"  )                      {
        Plan.sql.Indexing     = ( v == "true" ) ;
      }                                         ;
    SqlLoopErr ( SC , Q )                       ;
    SqlLoopEnd ( SC , Q )                       ;
    correct = Plan . sql . isValid ( )          ;
    SC . close  ( )                             ;
  }                                             ;
  SC   . remove ( )                             ;
  return correct                                ;
}
