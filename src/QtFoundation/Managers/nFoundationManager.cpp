#include <qtfoundation.h>

QScriptValue N::FoundationAttachment(QScriptContext * context,QScriptEngine * engine)
{
  Q_UNUSED ( context )                                                     ;
  N::FoundationManager * nfm = new N::FoundationManager ( engine , qPlan ) ;
  return engine -> newQObject                           ( nfm            ) ;
}

N::FoundationManager:: FoundationManager ( QObject * parent , PurePlan * p )
                     : QObject           (           parent                )
                     , QScriptable       (                                 )
                     , Thread            ( 0                , false        )
                     , plan              (                               p )
{
  WidgetClass                                    ;
  setValue ( "Machine"       , plan -> Machine ) ;
  setValue ( "Error"         , true            ) ;
  setValue ( "None"          , true            ) ;
  setValue ( "Notify"        , true            ) ;
  setValue ( "Period"        , true            ) ;
  setValue ( "Record"        , true            ) ;
  setValue ( "Meeting"       , true            ) ;
  setValue ( "Automation"    , true            ) ;
  setValue ( "Operation"     , true            ) ;
  setValue ( "Sync"          , true            ) ;
  setValue ( "Download"      , true            ) ;
  setValue ( "Upload"        , true            ) ;
  setValue ( "Audio"         , true            ) ;
  setValue ( "Video"         , true            ) ;
  setValue ( "TV"            , true            ) ;
  setValue ( "Programming"   , true            ) ;
  setValue ( "Backup"        , true            ) ;
  setValue ( "FileUpdate"    , true            ) ;
  setValue ( "Cooking"       , true            ) ;
  setValue ( "Sleep"         , true            ) ;
  setValue ( "Housework"     , true            ) ;
  setValue ( "Shopping"      , true            ) ;
  setValue ( "Communication" , true            ) ;
  setValue ( "Finish"        , true            ) ;
  setValue ( "Obsolete"      , true            ) ;
  setValue ( "Others"        , true            ) ;
  setValue ( "FOREX"         , true            ) ;
  ////////////////////////////////////////////////
  nConnect ( this , SIGNAL ( PrivateMusic () )   ,
             this , SLOT   ( RelayMusic   () ) ) ;
}

N::FoundationManager::~FoundationManager(void)
{
}

void N::FoundationManager::RelayMusic(void)
{
  SUID mv = Value ( "RelayMusic" ) . toULongLong ( ) ;
  emit Music      ( mv )                             ;
  emit ShowAudio  (    )                             ;
}

void N::FoundationManager::print(QString message)
{
  plan -> Debug ( 30 , message ) ;
}

void N::FoundationManager::print(int verbose,QString message)
{
  plan -> Debug ( verbose , message ) ;
}

void N::FoundationManager::run(int type,ThreadData * data)
{
  switch ( type )                                                  {
    case 10001                                                     :
      if ( 1 == data -> Arguments . count ( ) )                    {
        EventHandler ( data -> Arguments [ 0 ] . toULongLong ( ) ) ;
      }                                                            ;
    break                                                          ;
  }                                                                ;
}

bool N::FoundationManager::toBool(QString key)
{
  if ( ! Variables . contains ( key ) ) return false ;
  return Variables [ key ] . toBool ( )              ;
}

QString N::FoundationManager::toString(QString key)
{
  if ( ! Variables . contains ( key ) ) return "" ;
  return Variables [ key ] . toString ( )         ;
}

QDateTime N::FoundationManager::toDateTime(QString key)
{
  if ( ! Variables . contains ( key ) ) return nTimeNow ;
  return Variables [ key ] . toDateTime ( )             ;
}

QVariant N::FoundationManager::Value(QString key)
{
  QVariant v                                     ;
  if ( ! Variables . contains ( key ) ) return v ;
  return Variables [ key ]                       ;
}

void N::FoundationManager::setValue(QString key,QVariant value)
{
  Variables [ key ] = value ;
}

QDateTime N::FoundationManager::addYears(QDateTime dt,int years)
{
  QDateTime S = dt           ;
  S = S . addYears ( years ) ;
  return S                   ;
}

QDateTime N::FoundationManager::addMonths(QDateTime dt,int months)
{
  QDateTime S = dt             ;
  S = S . addMonths ( months ) ;
  return S                     ;
}

QDateTime N::FoundationManager::addDays(QDateTime dt,int days)
{
  QDateTime S = dt         ;
  S = S . addDays ( days ) ;
  return S                 ;
}

QDateTime N::FoundationManager::addSeconds(QDateTime dt,int seconds)
{
  QDateTime S = dt            ;
  S = S . addSecs ( seconds ) ;
  return S                    ;
}

void N::FoundationManager::CreateDirs (
       QDir          & source         ,
       QDir          & target         ,
       QFileInfoList & infos          )
{
  QFileInfoList dirs                                                         ;
  for (int i=0;i<infos.count();i++)                                          {
    if ( infos [ i ] . isDir ( ) ) dirs << infos [ i ]                       ;
  }                                                                          ;
  if ( dirs . count ( ) <= 0 ) return                                        ;
  if ( ! target . exists ( ) ) return                                        ;
  ////////////////////////////////////////////////////////////////////////////
  qint64 value = 0                                                           ;
  bool   go    = true                                                        ;
  int    pid                                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  pid = plan -> Progress  ( tr("Create directories") , tr("%v/%m") )         ;
  plan -> Start           ( pid , &value , &go                     )         ;
  plan -> setRange        ( pid , 0      , dirs . count ( )        )         ;
  ////////////////////////////////////////////////////////////////////////////
  for (int i = 0 ; go && ( i < dirs . count ( ) ) ; i++ )                    {
    QFileInfo fi = dirs [ i ]                                                ;
    QString   rp = source . relativeFilePath ( fi . absoluteFilePath ( ) )   ;
    QString   tp = target . absoluteFilePath ( rp                        )   ;
    if ( ( rp . length ( ) > 0 ) && ( tp . length ( ) > 0 ) )                {
      plan -> ProgressText ( pid , tp )                                      ;
      target . mkpath ( tp )                                                 ;
    }                                                                        ;
    value++                                                                  ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  plan -> Finish ( pid  )                                                    ;
}

void N::FoundationManager::Copy (
       QDir          & source   ,
       QDir          & target   ,
       QFileInfoList & infos    )
{
  QFileInfoList files                                                        ;
  qint64        total = 0                                                    ;
  for (int i=0;i<infos.count();i++)                                          {
    if ( infos [ i ] . isFile ( ) )                                          {
      QFileInfo fi = infos [ i ]                                             ;
      QString   rp = source . relativeFilePath ( fi . absoluteFilePath ( ) ) ;
      QString   tp = target . absoluteFilePath ( rp                        ) ;
      QFileInfo ti ( tp )                                                    ;
      bool      same = true                                                  ;
      if ( ! ti . exists ( )                          ) same = false         ;
      if ( same && ( fi . size ( ) != ti . size ( ) ) ) same = false         ;
      if ( same && ( fi . lastModified ( ) != ti . lastModified ( ) ) )      {
        same = false                                                         ;
      }                                                                      ;
      if ( ! same )                                                          {
        files << infos [ i ]                                                 ;
        total += infos [ i ] . size ( )                                      ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  if ( files . count ( ) <= 0 ) return                                       ;
  if ( ! target . exists ( )  ) return                                       ;
  ////////////////////////////////////////////////////////////////////////////
  qint64 sum   = 0                                                           ;
  qint64 value = 0                                                           ;
  bool   go    = true                                                        ;
  int    pid                                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  pid = plan -> Progress  ( tr("Copy files") , tr("%v/%m") )                 ;
  plan -> Start           ( pid , &value , &go             )                 ;
  plan -> setRange        ( pid , 0      , total / 1024    )                 ;
  ////////////////////////////////////////////////////////////////////////////
  for (int i = 0 ; go && ( i < files . count ( ) ) ; i++ )                   {
    QFileInfo fi = files [ i ]                                               ;
    QString   sp = fi     . absoluteFilePath (    )                          ;
    QString   rp = source . relativeFilePath ( sp )                          ;
    QString   tp = target . absoluteFilePath ( rp )                          ;
    if ( ( rp . length ( ) > 0 ) && ( tp . length ( ) > 0 ) )                {
      QFileInfo xi ( tp )                                                    ;
      plan -> ProgressText ( pid , tp )                                      ;
      if ( xi . exists ( ) ) QFile::remove ( tp )                            ;
      QFile::copy ( sp , tp )                                                ;
      sum  += fi . size ( )                                                  ;
      value = ( sum + 1023 ) / 1024                                          ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  plan -> Finish ( pid  )                                                    ;
}

bool N::FoundationManager::Copy(QString s,QString t)
{
  qint64 step = 0                                                ;
  bool   go   = true                                             ;
  int    pid                                                     ;
  QDir   source ( s )                                            ;
  QDir   target ( t )                                            ;
  ////////////////////////////////////////////////////////////////
  if ( ! source . exists ( ) ) return false                      ;
  target . mkpath ( target . absolutePath ( ) )                  ;
  ////////////////////////////////////////////////////////////////
  QFileInfoList infos                                            ;
  pid = plan -> Progress  ( s   , tr("%v/%m")            )       ;
  plan -> Start           ( pid , &step  , &go           )       ;
  plan -> setRange        ( pid , 0      , 3             )       ;
  ////////////////////////////////////////////////////////////////
  plan -> ProgressText ( pid , tr("Scan source files"        ) ) ;
  step ++                                                        ;
  if ( go ) infos = File::FileInfos ( source )                   ;
  ////////////////////////////////////////////////////////////////
  plan -> ProgressText ( pid , tr("Create target directories") ) ;
  step ++                                                        ;
  if ( go ) CreateDirs ( source , target , infos )               ;
  ////////////////////////////////////////////////////////////////
  plan -> ProgressText ( pid , tr("Copy files"               ) ) ;
  step ++                                                        ;
  if ( go ) Copy       ( source , target , infos )               ;
  ////////////////////////////////////////////////////////////////
  plan -> Finish  ( pid                  )                       ;
  return true                                                    ;
}

QScriptEngine * N::FoundationManager::GetEngine(void)
{
  QScriptEngine *  engine = new QScriptEngine ( )              ;
  N::ScriptManager FSM                                         ;
  //////////////////////////////////////////////////////////////
  FSM . Creators [ "FTP"         ] = N::FtpAttachement         ;
  FSM . Creators [ "SQL"         ] = N::SqlAttachment          ;
  FSM . Creators [ "File"        ] = N::FileAttachment         ;
  FSM . Creators [ "Directory"   ] = N::DirectoryAttachment    ;
  FSM . Creators [ "Socket"      ] = N::SocketAttachment       ;
  FSM . Creators [ "Settings"    ] = N::SettingsAttachment     ;
  FSM . Creators [ "ByteArray"   ] = N::ByteArrayAttachment    ;
  FSM . Creators [ "Mutex"       ] = N::MutexAttachment        ;
  FSM . Creators [ "Transformer" ] = N::TransformerAttachment  ;
  FSM . Creators [ "RpcServer"   ] = N::RPC::ServerAttachement ;
  FSM . Creators [ "RpcClient"   ] = N::RPC::ClientAttachement ;
  FSM . AttachCreators ( *engine )                             ;
  //////////////////////////////////////////////////////////////
  return engine                                                ;
}

void N::FoundationManager::CompleteEvent(SqlConnection & SC,SUID uuid)
{
  int          eType      = N::None                    ;
  TUID         StartTime  = 0                          ;
  TUID         FinishTime = 0                          ;
  QString      Q                                       ;
  EventManager EM ( plan )                             ;
  //////////////////////////////////////////////////////
  eType = EM . Type ( SC , uuid )                      ;
  Q     = SC . sql . SelectFrom                        (
            "start,final,type"                         ,
            PlanTable(EventHistory)                    ,
            SC . WhereUuid ( uuid )                  ) ;
  if (SC.Fetch(Q))                                     {
    StartTime  = SC . Tuid ( 0 )                       ;
    FinishTime = SC . Tuid ( 1 )                       ;
    eType      = SC . Int  ( 2 )                       ;
    ////////////////////////////////////////////////////
    QDateTime   CC = QDateTime::currentDateTime ( )    ;
    N::StarDate SD                                     ;
    SD         = CC                                    ;
    FinishTime = SD . Stardate                         ;
    ////////////////////////////////////////////////////
    EM . Delete           ( SC , uuid                ) ;
    EM . UpdateEvent      ( SC                         ,
                            uuid                       ,
                            N::Calendars::Duration     ,
                            StartTime                  ,
                            FinishTime               ) ;
    EM . attachSpot       ( SC                         ,
                            uuid                       ,
                            N::History::WorkingPeriod  ,
                            StartTime                  ,
                            FinishTime               ) ;
    EM . DeleteScheduling ( SC , uuid                ) ;
  }                                                    ;
}

void N::FoundationManager::PostponeEvent(SqlConnection & SC,SUID uuid,qint64 delayMs)
{
  int          eType      = N::None                    ;
  TUID         StartTime  = 0                          ;
  TUID         FinishTime = 0                          ;
  QString      Q                                       ;
  EventManager EM ( plan )                             ;
  //////////////////////////////////////////////////////
  eType = EM . Type ( SC , uuid )                      ;
  Q     = SC . sql . SelectFrom                        (
            "start,final,type"                         ,
            PlanTable(EventHistory)                    ,
            SC . WhereUuid ( uuid )                  ) ;
  if (SC.Fetch(Q))                                     {
    qint64 delay = delayMs / 1000                      ;
    ////////////////////////////////////////////////////
    StartTime   = SC . Tuid ( 0 )                      ;
    FinishTime  = SC . Tuid ( 1 )                      ;
    eType       = SC . Int  ( 2 )                      ;
    ////////////////////////////////////////////////////
    StartTime  += delay                                ;
    FinishTime += delay                                ;
    ////////////////////////////////////////////////////
    EM . AssureEvent      ( SC , uuid , eType        ) ;
    EM . UpdateEvent      ( SC                         ,
                            uuid                       ,
                            N::Calendars::Duration     ,
                            StartTime                  ,
                            FinishTime               ) ;
    EM . attachSpot       ( SC                         ,
                            uuid                       ,
                            N::History::WorkingPeriod  ,
                            StartTime                  ,
                            FinishTime               ) ;
    EM . DeleteScheduling ( SC , uuid                ) ;
    EM . AddScheduling    ( SC , uuid                ) ;
  }                                                    ;
}

bool N::FoundationManager::isOwner(SqlConnection & SC,SUID uuid)
{
  EventManager EM ( plan )                                     ;
  QString      Q                                               ;
  SUID         u                                               ;
  bool         match = false                                   ;
  SUID         m     = plan -> Machine                         ;
  QVariant     V                                               ;
  Q = SC . sql . SelectFrom                                    (
        "first"                                                ,
        PlanTable(OtherGroups)                                 ,
        QString("where `t2` = %1"
                 " and `t1` = %2"
                 " and `relation` = %3"
                 " and `second` = %4"                          )
        . arg ( Types::Schedule                                )
        . arg ( Types::Equipment                               )
        . arg ( Groups::Subordination                          )
        . arg ( uuid                                         ) ,
        SC . OrderByAsc ( "id" )                             ) ;
  SqlLoopNow ( SC , Q )                                        ;
    u = SC . Uuid ( 0 )                                        ;
    if ( 0 != m )                                              {
      if ( m == u ) match = true                               ;
    }                                                          ;
  SqlLoopErr ( SC , Q )                                        ;
  SqlLoopEnd ( SC , Q )                                        ;
  //////////////////////////////////////////////////////////////
  if ( match )                                                 {
    if ( EM . LoadVariable ( SC , uuid , "Application" , V ) ) {
      QString app = V . toString ( )                           ;
      if ( app . length ( ) > 0 )                              {
        QString mapp = plan -> Application                     ;
        match = ( app . toLower ( ) == mapp . toLower ( ) )    ;
      }                                                        ;
    }                                                          ;
  }                                                            ;
  return match                                                 ;
}

bool N::FoundationManager::SetScheduleMode(SqlConnection & SC,SUID uuid,int mode)
{
  QString Q                             ;
  Q = SC . sql . Update                 (
        PlanTable(Scheduling)           ,
        SC . sql . Where ( 1 , "uuid" ) ,
        1                               ,
        "mode"                        ) ;
  SC        . Prepare ( Q             ) ;
  SC        . Bind    ( "uuid" , uuid ) ;
  SC        . Bind    ( "mode" , mode ) ;
  return SC . Exec    (               ) ;
}

void N::FoundationManager::CompleteEvent(SUID uuid)
{
  SqlConnection SC ( plan -> sql )    ;
  if ( SC . open ( FunctionString ) ) {
    CompleteEvent ( SC , uuid )       ;
    SC.close()                        ;
  }                                   ;
  SC.remove()                         ;
}

void N::FoundationManager::PostponeEvent(SUID uuid,qint64 delayMs)
{
  SqlConnection SC ( plan -> sql )        ;
  if ( SC . open ( FunctionString ) )     {
    PostponeEvent ( SC , uuid , delayMs ) ;
    SC.close()                            ;
  }                                       ;
  SC.remove()                             ;
}

bool N::FoundationManager::isOwner(SUID uuid)
{
  bool ior = false                    ;
  SqlConnection SC ( plan -> sql )    ;
  if ( SC . open ( FunctionString ) ) {
    ior = isOwner ( SC , uuid )       ;
    SC.close()                        ;
  }                                   ;
  SC.remove()                         ;
  return ior                          ;
}

void N::FoundationManager::execute(SUID uuid)
{
  VarArgs args           ;
  args << uuid           ;
  start ( 10001 , args ) ;
}

void N::FoundationManager::EventHandler(SUID uuid)
{
  int  eType      = N::None                              ;
  TUID StartTime  = 0                                    ;
  TUID FinishTime = 0                                    ;
  int  relation   = N::None                              ;
  bool correct    = false                                ;
  ////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                       ;
  EventManager  EM ( plan        )                       ;
  if ( SC . open ( FunctionString ) )                    {
    QString Q                                            ;
    QString name                                         ;
    //////////////////////////////////////////////////////
    eType = EM . Type ( SC , uuid )                      ;
    name  = EM . Name ( SC , uuid )                      ;
    //////////////////////////////////////////////////////
    Q = SC . sql . SelectFrom                            (
          "start,final,type"                             ,
          PlanTable(EventHistory)                        ,
          SC . WhereUuid ( uuid )                      ) ;
    if (SC.Fetch(Q))                                     {
      StartTime  = SC . Tuid ( 0 )                       ;
      FinishTime = SC . Tuid ( 1 )                       ;
      eType      = SC . Int  ( 2 )                       ;
      correct    = true                                  ;
    }                                                    ;
    //////////////////////////////////////////////////////
    QDateTime   CC = QDateTime::currentDateTime ( )      ;
    N::StarDate SD                                       ;
    SD = CC                                              ;
    //////////////////////////////////////////////////////
    if ( FinishTime < SD . Stardate )                    {
      relation = N::Groups::FinalTrigger                 ;
    } else
    if ( ( StartTime     < SD . Stardate )              &&
         ( SD . Stardate < FinishTime    )             ) {
      relation = N::Groups::StartTrigger                 ;
    } else                                               {
      correct  = false                                   ;
    }                                                    ;
    //////////////////////////////////////////////////////
    QDateTime ST = EM . toDateTime ( StartTime  )        ;
    QDateTime ET = EM . toDateTime ( FinishTime )        ;
    setValue ( "UUID"  , uuid  )                         ;
    setValue ( "Type"  , eType )                         ;
    setValue ( "Name"  , name  )                         ;
    setValue ( "Start" , ST    )                         ;
    setValue ( "Final" , ET    )                         ;
    //////////////////////////////////////////////////////
    SC.close()                                           ;
  }                                                      ;
  SC.remove()                                            ;
  ////////////////////////////////////////////////////////
  if ( ! correct ) return                                ;
  Dispatch ( uuid , eType , relation )                   ;
}

void N::FoundationManager::Dispatch(SUID uuid,int eType,int position)
{
  switch ( eType )                                                           {
    case N::None                                                             :
      // do completely nothing
    return                                                                   ;
    case N::Notify                                                           :
      // Communication does not launch addition functions, notify only
      if ( toBool ( "Notify" ) )                                             {
//        if ( isOwner ( uuid ) )                                              {
//          HandleRepeated   ( uuid , position )                               ;
//          HandleDependency ( uuid , position )                               ;
//        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Period                                                           :
      if ( toBool ( "Period" ) )                                             {
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Record                                                           :
      if ( toBool ( "Record" ) )                                             {
        // Record does not launch addition functions, notify only
         if ( isOwner ( uuid ) )                                             {
         HandleRepeated   ( uuid , position )                                ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Meeting                                                          :
      if ( toBool ( "Meeting" ) )                                            {
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
            if ( isOwner ( uuid ) )                                          {
              // Notification script
            }                                                                ;
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Automation                                                       :
      if ( toBool ( "Automation" ) )                                         {
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
            if ( isOwner ( uuid ) )                                          {
              HandleExecution ( uuid )                                       ;
            }                                                                ;
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Operation                                                        :
      if ( toBool ( "Operation" ) )                                          {
        // Operation does not launch addition functions, notify only for now
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Sync                                                             :
      if ( toBool ( "Sync" ) )                                               {
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
            if ( isOwner ( uuid ) )                                          {
              // File sync
            }                                                                ;
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Download                                                         :
      if ( toBool ( "Download" ) )                                           {
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
            if ( isOwner ( uuid ) )                                          {
              if ( TryDownload ( uuid ) )                                    {
                position = N::Groups::FinalTrigger                           ;
              }                                                              ;
            }                                                                ;
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Upload                                                           :
      if ( toBool ( "Upload" ) )                                             {
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
            if ( isOwner ( uuid ) )                                          {
              if ( TryUpload ( uuid ) )                                      {
                position = N::Groups::FinalTrigger                           ;
              }                                                              ;
            }                                                                ;
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Audio                                                            :
      if ( toBool ( "Audio" ) )                                              {
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
            if ( isOwner ( uuid ) )                                          {
              HandleMusic ( uuid )                                           ;
            }                                                                ;
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Video                                                            :
      if ( toBool ( "Video" ) )                                              {
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
            if ( isOwner ( uuid ) )                                          {
              HandleVideo ( uuid )                                           ;
            }                                                                ;
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::TV                                                               :
      if ( toBool ( "TV" ) )                                                 {
        // Watch TV does not launch addition functions, notify only
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Programming                                                      :
      if ( toBool ( "Programming" ) )                                        {
        // Programming does not launch addition functions, notify only for now
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Backup                                                           :
      if ( toBool ( "Backup" ) )                                             {
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
            if ( isOwner ( uuid ) )                                          {
              if ( HandleBackup ( uuid , position ) )                        {
                position = N::Groups::FinalTrigger                           ;
              }                                                              ;
            }                                                                ;
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::FileUpdate                                                       :
      if ( toBool ( "FileUpdate" ) )                                         {
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
            if ( isOwner ( uuid ) )                                          {
              // File update script
            }                                                                ;
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Cooking                                                          :
      if ( toBool ( "Cooking" ) )                                            {
        // Cooking does not launch addition functions, notify only for now
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Sleep                                                            :
      if ( toBool ( "Sleep" ) )                                              {
        // Sleep does not launch addition functions, notify only
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Housework                                                        :
      if ( toBool ( "Housework" ) )                                          {
        // Housework does not launch addition functions, notify only
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Shopping                                                         :
      if ( toBool ( "Shopping" ) )                                           {
        // Shopping does not launch addition functions, notify only
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Communication                                                    :
      if ( toBool ( "Communication" ) )                                      {
        // Communication does not launch addition functions, notify only
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::Finish                                                           :
      if ( toBool ( "Finish" ) )                                             {
      }                                                                      ;
    break                                                                    ;
    case N::Obsolete                                                         :
      if ( toBool ( "Obsolete" ) )                                           {
      }                                                                      ;
    return                                                                   ;
    case N::Others                                                           :
      if ( toBool ( "Others" ) )                                             {
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case N::ForexTrade                                                       :
      if ( toBool ( "FOREX" ) )                                              {
        // Programming does not launch addition functions, notify only for now
        switch ( position )                                                  {
          case N::Groups::StartTrigger                                       :
          break                                                              ;
          case N::Groups::FinalTrigger                                       :
          break                                                              ;
        }                                                                    ;
        if ( isOwner ( uuid ) )                                              {
          HandleRepeated   ( uuid , position )                               ;
          HandleDependency ( uuid , position )                               ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    default                                                                  :
    break                                                                    ;
  }                                                                          ;
}

bool N::FoundationManager::HandleBackup(SUID uuid,int position)
{
  if ( N::Groups::StartTrigger != position ) return false         ;
  /////////////////////////////////////////////////////////////////
  QString  BackupINI = ""                                         ;
  /////////////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                                ;
  EventManager  EM ( plan        )                                ;
  if ( SC . open ( FunctionString ) )                             {
    QVariant V                                                    ;
    ///////////////////////////////////////////////////////////////
    if ( EM . LoadVariable ( SC , uuid , "BackupINI" , V ) )      {
      BackupINI = V . toString ( )                                ;
    }                                                             ;
    SC.close()                                                    ;
  }                                                               ;
  SC.remove()                                                     ;
  /////////////////////////////////////////////////////////////////
  if ( BackupINI . length ( ) <= 0 ) return false                 ;
  /////////////////////////////////////////////////////////////////
  bool correct = Backup ( BackupINI )                             ;
  if ( correct )                                                  {
    CompleteEvent ( uuid               )                          ;
  } else                                                          {
    PostponeEvent ( uuid , 1800 * 1000 )                          ;
  }                                                               ;
  /////////////////////////////////////////////////////////////////
  return correct                                                  ;
}

bool N::FoundationManager::Backup(QString filename)
{
  QFileInfo FXI ( filename )                                      ;
  if ( ! FXI . exists ( ) ) return false                          ;
  /////////////////////////////////////////////////////////////////
  N::Settings sf  ( filename , QSettings::IniFormat )             ;
  int         total = 0                                           ;
  QStringList S                                                   ;
  QStringList T                                                   ;
  /////////////////////////////////////////////////////////////////
  sf . beginGroup ( "Settings" )                                  ;
  total = sf . value ( "Total" ) . toInt ( )                      ;
  sf . endGroup   (            )                                  ;
  if ( total <= 0 ) return false                                  ;
  /////////////////////////////////////////////////////////////////
  int count = 0                                                   ;
  for (int i=0;i<total;i++)                                       {
    QString n = QString::number(i+1)                              ;
    QString s                                                     ;
    QString t                                                     ;
    sf . beginGroup      ( n        )                             ;
    s = sf . valueString ( "Source" )                             ;
    t = sf . valueString ( "Target" )                             ;
    sf . endGroup        (          )                             ;
    if ( ( s . length ( ) > 0 ) && ( t . length ( ) > 0 ) )       {
      S << s                                                      ;
      T << t                                                      ;
      count++                                                     ;
    }                                                             ;
  }                                                               ;
  if ( count <= 0 ) return false                                  ;
  /////////////////////////////////////////////////////////////////
  int    i = 0                                                    ;
  int    pid                                                      ;
  qint64 value = 0                                                ;
  bool   go    = true                                             ;
  /////////////////////////////////////////////////////////////////
  pid = plan -> Progress ( tr("Copy directories") , tr("%v/%m") ) ;
  plan -> Start          ( pid , &value , &go                   ) ;
  plan -> setRange       ( pid , 0      , count                 ) ;
  while ( go && ( i < count ) )                                   {
    value ++                                                      ;
    QString sx = S [ i ]                                          ;
    QString tx = T [ i ]                                          ;
    if ( ( sx . length ( ) > 0 ) && ( tx . length ( ) > 0 ) )     {
      plan -> ProgressText ( pid , sx )                           ;
      Copy ( sx , tx )                                            ;
    }                                                             ;
    i ++                                                          ;
  }                                                               ;
  /////////////////////////////////////////////////////////////////
  plan -> Finish ( pid            )                               ;
  Time::msleep   ( 1000           )                               ;
  /////////////////////////////////////////////////////////////////
  return true                                                     ;
}

bool N::FoundationManager::Run(SUID u)
{
  QString Name       = ""                                            ;
  QString Register   = ""                                            ;
  SUID    Platform   = 0                                             ;
  int     Identifier = 0                                             ;
  bool    Parallel   = true                                          ;
  QString Executable = ""                                            ;
  QString Environ    = ""                                            ;
  QString Paraments  = ""                                            ;
  bool    correct    = false                                         ;
  ////////////////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                                   ;
  ////////////////////////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )                                {
    QString Q                                                        ;
    Name = SC . getName                                              (
             PlanTable(Names)                                        ,
             "uuid"                                                  ,
             plan->LanguageId                                        ,
             u                                                     ) ;
    Q = SC . sql . SelectFrom                                        (
          "identifier,platform,parallel,register,executable,environment,paraments",
          PlanTable(Executions)                                      ,
          SC . WhereUuid ( u )                                     ) ;
    if ( SC . Fetch ( Q ) )                                          {
      Identifier =   SC . Value  ( 0 ) . toInt ( )                   ;
      Platform   =   SC . Uuid   ( 1 )                               ;
      Parallel   = ( SC . Value  ( 2 ) . toInt ( ) == 1 )            ;
      Register   =   SC . String ( 3 )                               ;
      Executable =   SC . String ( 4 )                               ;
      Environ    =   SC . String ( 5 )                               ;
      Paraments  =   SC . String ( 6 )                               ;
      correct    = true                                              ;
    }                                                                ;
    SC . close( )                                                    ;
  }                                                                  ;
  SC . remove ( )                                                    ;
  ////////////////////////////////////////////////////////////////////
  if ( ! correct ) return false                                      ;
  plan -> Talk ( Name )                                              ;
  ////////////////////////////////////////////////////////////////////
  QStringList arguments = Paraments . split ( ' ' )                  ;
  if ( Parallel )                                                    {
    if ( Paraments . length ( ) > 0 )                                {
      if ( Environ . length ( ) <= 0 )                               {
        QProcess::startDetached ( Executable , arguments           ) ;
      } else                                                         {
        QProcess::startDetached ( Executable , arguments , Environ ) ;
      }                                                              ;
      return true                                                    ;
    } else                                                           {
      if ( Environ . length ( ) > 0 )                                {
        arguments . clear ( )                                        ;
        QProcess::startDetached ( Executable , arguments , Environ ) ;
      } else                                                         {
        QProcess::startDetached ( Executable                       ) ;
      }                                                              ;
      return true                                                    ;
    }                                                                ;
  } else                                                             {
    if ( Environ . length ( ) <= 0 && Paraments . length ( ) <= 0  ) {
      QProcess::execute         ( Executable                       ) ;
    } else
    if ( Environ . length ( ) <= 0 )                                 {
      QProcess::execute         ( Executable , arguments           ) ;
    } else                                                           {
      QProcess P                                                     ;
      P . setWorkingDirectory   ( Environ                          ) ;
      P . start                 ( Executable , arguments           ) ;
    }                                                                ;
    return true                                                      ;
  }                                                                  ;
  ////////////////////////////////////////////////////////////////////
  return false                                                       ;
}

SUID N::FoundationManager::GetExecution(SUID u)
{
  SUID          eu = 0                             ;
  SqlConnection SC ( plan -> sql )                 ;
  //////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )              {
    QString Q                                      ;
    Q = SC . sql . SelectFrom                      (
          "second"                                 ,
          PlanTable(OtherGroups)                   ,
          QString( "where `first` = %1"
                    " and `t1` = %2"
                    " and `t2` = %3"
                    " and `relation` = %4"         )
          . arg ( u                                )
          . arg ( N :: Types  :: Schedule          )
          . arg ( N :: Types  :: Execution         )
          . arg ( N :: Groups :: Subordination ) ) ;
    if ( SC . Fetch ( Q ) ) eu = SC . Uuid ( 0 )   ;
    SC . close( )                                  ;
  }                                                ;
  SC . remove ( )                                  ;
  return eu                                        ;
}

void N::FoundationManager::HandleExecution(SUID uuid)
{
  SUID eu = GetExecution ( uuid )          ;
  if ( eu > 0 )                            {
    if ( Run ( eu ) )                      {
//      CompleteEvent ( uuid               ) ;
    } else                                 {
//      PostponeEvent ( uuid , 1800 * 1000 ) ;
    }                                      ;
  }                                        ;
}

bool N::FoundationManager::Verify(SUID u,int e)
{
  bool          correct = true               ;
  EventManager  EM ( plan        )           ;
  SqlConnection SC ( plan -> sql )           ;
  ////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )        {
    QString     Q                            ;
    TUID        Final                        ;
    TUID        Current                      ;
    N::StarDate CST                          ;
    //////////////////////////////////////////
    Q = SC . sql . SelectFrom                (
          "final"                            ,
          PlanTable(EventHistory)            ,
          SC . WhereUuid ( u )             ) ;
    if (SC.Fetch(Q))                         {
      Final   = SC . Tuid ( 0 )              ;
      CST     = QDateTime::currentDateTime() ;
      Current = CST . Stardate               ;
      correct = true                         ;
      if ( isOwner ( SC , u ) )              {
        if ( Current > Final )               {
//          EM . DeleteScheduling ( SC , u )   ; Delete Scheduling is replaced by FoundationManager Handlers
          EM . Delete           ( SC , u )   ;
        }                                    ;
      } else correct = false                 ;
    }                                        ;
    SC . close( )                            ;
  }                                          ;
  SC . remove ( )                            ;
  ////////////////////////////////////////////
  return correct                             ;
}

bool N::FoundationManager::TryDownload(SUID uuid)
{
  bool     FtpUseJava     = false                                 ;
  QString  FileTransferJS = ""                                    ;
  QString  DownloadURL    = ""                                    ;
  QString  DownloadFile   = ""                                    ;
  /////////////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                                ;
  EventManager  EM ( plan        )                                ;
  if ( SC . open ( FunctionString ) )                             {
    QVariant V                                                    ;
    ///////////////////////////////////////////////////////////////
    if ( EM . LoadVariable ( SC , uuid , "FtpUseJava"     , V ) ) {
      FtpUseJava     = V . toBool   ( )                           ;
    }                                                             ;
    if ( EM . LoadVariable ( SC , uuid , "FileTransferJS" , V ) ) {
      FileTransferJS = V . toString ( )                           ;
    }                                                             ;
    if ( EM . LoadVariable ( SC , uuid , "DownloadURL"    , V ) ) {
      DownloadURL    = V . toString ( )                           ;
    }                                                             ;
    if ( EM . LoadVariable ( SC , uuid , "DownloadFile"   , V ) ) {
      DownloadFile   = V . toString ( )                           ;
    }                                                             ;
    SC.close()                                                    ;
  }                                                               ;
  SC.remove()                                                     ;
  /////////////////////////////////////////////////////////////////
  bool cr = false                                                 ;
  if ( FtpUseJava )                                               {
    if ( FileTransferJS . length ( ) <= 0 ) return false          ;
    QFileInfo FXI ( FileTransferJS )                              ;
    if ( ! FXI . exists ( )               ) return false          ;
    cr = JavaDownload ( uuid , FileTransferJS )                   ;
  } else                                                          {
    qint64 dst = 60 * 1000                                        ;
    if ( DownloadURL    . length ( ) <= 0 ) return false          ;
    if ( DownloadFile   . length ( ) <= 0 ) return false          ;
    cr = Download ( uuid , DownloadURL , DownloadFile , dst )     ;
  }                                                               ;
  return cr                                                       ;
}

bool N::FoundationManager::TryUpload(SUID uuid)
{
  bool     FtpUseJava     = false                                 ;
  QString  FileTransferJS = ""                                    ;
  QString  UploadURL      = ""                                    ;
  QString  UploadFile     = ""                                    ;
  QString  UploadUser     = ""                                    ;
  QString  UploadPass     = ""                                    ;
  QVariant V                                                      ;
  /////////////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                                ;
  EventManager  EM ( plan        )                                ;
  if ( SC . open ( FunctionString ) )                             {
    ///////////////////////////////////////////////////////////////
    if ( EM . LoadVariable ( SC , uuid , "FtpUseJava"     , V ) ) {
      FtpUseJava     = V . toBool   ( )                           ;
    }                                                             ;
    if ( EM . LoadVariable ( SC , uuid , "FileTransferJS" , V ) ) {
      FileTransferJS = V . toString ( )                           ;
    }                                                             ;
    if ( EM . LoadVariable ( SC , uuid , "UploadURL"      , V ) ) {
      UploadURL      = V . toString ( )                           ;
    }                                                             ;
    if ( EM . LoadVariable ( SC , uuid , "UploadFile"     , V ) ) {
      UploadFile     = V . toString ( )                           ;
    }                                                             ;
    if ( EM . LoadVariable ( SC , uuid , "UploadUser"     , V ) ) {
      UploadUser     = V . toString ( )                           ;
    }                                                             ;
    if ( EM . LoadVariable ( SC , uuid , "UploadPass"     , V ) ) {
      UploadPass     = V . toString ( )                           ;
    }                                                             ;
    SC.close()                                                    ;
  }                                                               ;
  SC.remove()                                                     ;
  /////////////////////////////////////////////////////////////////
  bool cr = false                                                 ;
  if ( FtpUseJava )                                               {
    if ( FileTransferJS . length ( ) <= 0 ) return false          ;
    QFileInfo FXI ( FileTransferJS )                              ;
    if ( ! FXI . exists ( )               ) return false          ;
    cr = JavaUpload ( uuid , FileTransferJS )                     ;
  } else                                                          {
    qint64 dst = 60 * 1000                                        ;
    if ( UploadURL      . length ( ) <= 0 ) return false          ;
    if ( UploadFile   . length ( ) <= 0 ) return false            ;
    cr = Upload ( uuid                                            ,
                  UploadURL                                       ,
                  UploadFile                                      ,
                  UploadUser                                      ,
                  UploadPass                                      ,
                  dst                                           ) ;
  }                                                               ;
  return cr                                                       ;
}

bool N::FoundationManager::JavaFetch(SUID uuid,QString JS,QString dname)
{
  QByteArray B                                               ;
  if ( ! File::toByteArray ( JS , B ) ) return false         ;
  if ( B . size ( ) <= 0              ) return false         ;
  ////////////////////////////////////////////////////////////
  QScriptEngine * engine = GetEngine              (        ) ;
  QString         script = QString::fromUtf8      ( B      ) ;
  QScriptValue    func   = engine -> evaluate     ( script ) ;
  QScriptValue    g      = engine -> globalObject (        ) ;
  QScriptValue    fnjs   = engine -> newQObject   ( this   ) ;
  QScriptValue    dnfunc = g       . property     ( dname  ) ;
  QScriptValue    result = QScriptValue           ( false  ) ;
  bool            r      = false                             ;
  ////////////////////////////////////////////////////////////
  if ( dnfunc . isFunction ( ) )                             {
    QScriptValueList args                                    ;
    args   << fnjs                                           ;
    args   << QString::number ( uuid        )                ;
    result  = dnfunc  . call  ( func , args )                ;
    engine -> collectGarbage  (             )                ;
    r       = result . toBool (             )                ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  if ( r )                                                   {
    CompleteEvent ( uuid               )                     ;
  } else                                                     {
    PostponeEvent ( uuid , 1800 * 1000 )                     ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  engine -> deleteLater ( )                                  ;
  ////////////////////////////////////////////////////////////
  return r                                                   ;
}

bool N::FoundationManager::JavaDownload(SUID uuid,QString JS)
{
  return JavaFetch ( uuid , JS , "Download" ) ;
}

bool N::FoundationManager::JavaUpload(SUID uuid,QString JS)
{
  return JavaFetch ( uuid , JS , "Upload" ) ;
}

bool N::FoundationManager::Download (
       SUID    uuid                 ,
       QString url                  ,
       QString file                 ,
       qint64  mtimeout             )
{
  ScriptableFtp * ftp = new ScriptableFtp ( NULL                  ) ;
  bool            dr  = ftp -> Download   ( url , file , mtimeout ) ;
  ///////////////////////////////////////////////////////////////////
  if ( dr )                                                         {
    CompleteEvent ( uuid               )                            ;
  } else                                                            {
    PostponeEvent ( uuid , 1800 * 1000 )                            ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  delete ftp                                                        ;
  return dr                                                         ;
}

#pragma message("Upload function requires some addition in QtCURL and QtFTP")

bool N::FoundationManager::Upload (
       SUID    uuid               ,
       QString url                ,
       QString file               ,
       QString username           ,
       QString password           ,
       qint64  mtimeout           )
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  ScriptableFtp * ftp = new ScriptableFtp ( NULL                  ) ;
  bool            dr  = ftp -> Download   ( url , file , mtimeout ) ;
  ///////////////////////////////////////////////////////////////////
  if ( dr )                                                         {
    CompleteEvent ( uuid               )                            ;
  } else                                                            {
    PostponeEvent ( uuid , 1800 * 1000 )                            ;
  }                                                                 ;
  ///////////////////////////////////////////////////////////////////
  delete ftp                                                        ;
  return dr                                                         ;
#endif
  return true ;
}

void N::FoundationManager::HandleRepeated(SUID u,int position)
{
  int     repeated =  0                                       ;
  int     entry    = -1                                       ;
  QString script   = ""                                       ;
  /////////////////////////////////////////////////////////////
  EventManager  EM ( plan        )                            ;
  SqlConnection SC ( plan -> sql )                            ;
  if ( SC . open ( FunctionString ) )                         {
    QVariant V                                                ;
    if ( EM . LoadVariable ( SC , u , "Repeated"      , V ) ) {
      repeated = V . toInt    ( )                             ;
    }                                                         ;
    if ( EM . LoadVariable ( SC , u , "RepeatedEntry" , V ) ) {
      entry    = V . toInt    ( )                             ;
    }                                                         ;
    if ( EM . LoadVariable ( SC , u , "RepeatedScript", V ) ) {
      script   = V . toString ( )                             ;
    }                                                         ;
    SC . close  ( )                                           ;
  }                                                           ;
  SC   . remove ( )                                           ;
  /////////////////////////////////////////////////////////////
  if ( entry               <  0 ) return                      ;
  if ( repeated            <= 0 ) return                      ;
  if ( script . length ( ) <= 0 ) return                      ;
  /////////////////////////////////////////////////////////////
  switch ( position )                                         {
    case N::Groups::StartTrigger                              :
      if ( 0 != entry ) return                                ;
    break                                                     ;
    case N::Groups::FinalTrigger                              :
      if ( 1 != entry ) return                                ;
    break                                                     ;
  }                                                           ;
  /////////////////////////////////////////////////////////////
  JavaFetch ( u , script , "Repeated" )                       ;
}

SUID N::FoundationManager::Append(SqlConnection & SC,int type,QString name,QDateTime Start,QDateTime End)
{
  N::EventManager  EM ( plan        )                                        ;
  SUID             uu = 0                                                    ;
  N::StarDate      ST                                                        ;
  N::StarDate      FT                                                        ;
  ST = Start                                                                 ;
  FT = End                                                                   ;
  ////////////////////////////////////////////////////////////////////////////
  uu = EM . AssureUuid ( SC                                                ) ;
  EM . UpdateType      ( SC                                                  ,
                         plan -> Tables [ N::Tables::EventHistory ]          ,
                         uu                                                  ,
                         type                                              ) ;
  EM . AssureEvent     ( SC , uu ,type                                     ) ;
  EM . UpdateType      ( SC                                                  ,
                         plan -> Tables [ N::Tables::Events ]                ,
                         uu                                                  ,
                         type                                              ) ;
  SC . assureName      ( plan -> Tables [ N::Tables::Names ]                 ,
                         uu                                                  ,
                         plan -> LanguageId                                  ,
                         name                                              ) ;
  EM . UpdateEvent     ( SC                                                  ,
                         uu                                                  ,
                         N::Calendars::Duration                              ,
                         ST . Stardate                                       ,
                         FT . Stardate                                     ) ;
  EM . attachSpot      ( SC                                                  ,
                         uu                                                  ,
                         N::History::WorkingPeriod                           ,
                         ST.Stardate                                         ,
                         FT.Stardate                                       ) ;
  ////////////////////////////////////////////////////////////////////////////
  return uu                                                                  ;
}

QString N::FoundationManager::Append(int type,QString name,QDateTime ST,QDateTime ET)
{
  SUID uu = EventManager::Append ( plan , type , name , ST , ET ) ;
  return QString::number         ( uu                           ) ;
}

bool N::FoundationManager::Complete(QString uuid,QDateTime ST,QDateTime ET)
{
  SUID uu = uuid . toULongLong  (                     ) ;
  return EventManager::Complete ( plan , uu , ST , ET ) ;
}

bool N::FoundationManager::CloneVariable (
       SqlConnection & SC                ,
       SUID            original          ,
       SUID            target            ,
       QString         key               )

{
  EventManager EM ( plan )                                            ;
  QVariant V                                                          ;
  if ( ! EM . LoadVariable ( SC , original , key , V ) ) return false ;
  return EM . SaveVariable ( SC , target   , key , V )                ;
}

bool N::FoundationManager::CloneVariables (
       SqlConnection & SC                 ,
       SUID            original           ,
       SUID            target             )
{
  CloneVariable ( SC , original , target , "FtpUseJava"     ) ;
  CloneVariable ( SC , original , target , "FileTransferJS" ) ;
  CloneVariable ( SC , original , target , "DownloadURL"    ) ;
  CloneVariable ( SC , original , target , "DownloadFile"   ) ;
  CloneVariable ( SC , original , target , "UploadURL"      ) ;
  CloneVariable ( SC , original , target , "UploadFile"     ) ;
  CloneVariable ( SC , original , target , "UploadUser"     ) ;
  CloneVariable ( SC , original , target , "UploadPass"     ) ;
  /////////////////////////////////////////////////////////////
  CloneVariable ( SC , original , target , "Repeated"       ) ;
  CloneVariable ( SC , original , target , "RepeatedEntry"  ) ;
  CloneVariable ( SC , original , target , "RepeatedScript" ) ;
  /////////////////////////////////////////////////////////////
  CloneVariable ( SC , original , target , "BackupINI"      ) ;
  /////////////////////////////////////////////////////////////
  CloneVariable ( SC , original , target , "ForexJs"        ) ;
  CloneVariable ( SC , original , target , "PrivateJs"      ) ;
  /////////////////////////////////////////////////////////////
  CloneVariable ( SC , original , target , "MusicUUID"      ) ;
  CloneVariable ( SC , original , target , "VideoUUID"      ) ;
  CloneVariable ( SC , original , target , "VideoFile"      ) ;
  /////////////////////////////////////////////////////////////
  CloneVariable ( SC , original , target , "Application"    ) ;
  /////////////////////////////////////////////////////////////
  return true                                                 ;
}

bool N::FoundationManager::CloneFirst (
       SqlConnection & SC             ,
       SUID            original       ,
       SUID            target         ,
       int             type           )
{
  return CloneFirst ( SC                      ,
                      original                ,
                      target                  ,
                      type                    ,
                      Groups::Subordination ) ;
}

bool N::FoundationManager::CloneFirst (
       SqlConnection & SC             ,
       SUID            original       ,
       SUID            target         ,
       int             type           ,
       int             relation       )
{
  UUIDs   U                                             ;
  QString Q                                             ;
  SUID    u                                             ;
  Q = SC . sql . SelectFrom                             (
        "second"                                        ,
        PlanTable(Groups)                               ,
        QString("where `first` = %1"
                 " and `t1` = %2"
                 " and `t2` = %3"
                 " and `relation` = %4"                 )
        . arg ( original                                )
        . arg ( Types::Schedule                         )
        . arg ( type                                    )
        . arg ( relation                            ) ) ;
  SqlLoopNow       ( SC , Q )                           ;
    U << SC . Uuid ( 0      )                           ;
  SqlLoopErr       ( SC , Q )                           ;
  SqlLoopEnd       ( SC , Q )                           ;
  ///////////////////////////////////////////////////////
  if ( U . count ( ) <= 0 ) return true                 ;
  ///////////////////////////////////////////////////////
  foreach ( u , U )                                     {
    Q = SC . sql . InsertInto                           (
          PlanTable(Groups)                             ,
          5                                             ,
          "first"                                       ,
          "second"                                      ,
          "t1"                                          ,
          "t2"                                          ,
          "relation"                                  ) ;
    SC . Prepare ( Q                                  ) ;
    SC . Bind    ( "first"    , target                ) ;
    SC . Bind    ( "second"   , u                     ) ;
    SC . Bind    ( "t1"       , Types::Schedule       ) ;
    SC . Bind    ( "t2"       , type                  ) ;
    SC . Bind    ( "relation" , relation              ) ;
    SC . Exec    (                                    ) ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  return true                                           ;
}

bool N::FoundationManager::CloneSecond (
       SqlConnection & SC              ,
       SUID            original        ,
       SUID            target          ,
       int             type            )
{
  UUIDs   U                                             ;
  QString Q                                             ;
  SUID    u                                             ;
  Q = SC . sql . SelectFrom                             (
        "first"                                         ,
        PlanTable(Groups)                               ,
        QString("where `second` = %1"
                 " and `t1` = %2"
                 " and `t2` = %3"
                 " and `relation` = %4"                 )
        . arg ( original                                )
        . arg ( type                                    )
        . arg ( Types::Schedule                         )
        . arg ( Groups::Subordination               ) ) ;
  SqlLoopNow       ( SC , Q )                           ;
    U << SC . Uuid ( 0      )                           ;
  SqlLoopErr       ( SC , Q )                           ;
  SqlLoopEnd       ( SC , Q )                           ;
  ///////////////////////////////////////////////////////
  if ( U . count ( ) <= 0 ) return true                 ;
  ///////////////////////////////////////////////////////
  foreach ( u , U )                                     {
    Q = SC . sql . InsertInto                           (
          PlanTable(Groups)                             ,
          5                                             ,
          "first"                                       ,
          "second"                                      ,
          "t1"                                          ,
          "t2"                                          ,
          "relation"                                  ) ;
    SC . Prepare ( Q                                  ) ;
    SC . Bind    ( "first"    , u                     ) ;
    SC . Bind    ( "second"   , target                ) ;
    SC . Bind    ( "t1"       , type                  ) ;
    SC . Bind    ( "t2"       , Types::Schedule       ) ;
    SC . Bind    ( "relation" , Groups::Subordination ) ;
    SC . Exec    (                                    ) ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  return true                                           ;
}

bool N::FoundationManager::CloneDependency (
       SqlConnection & SC                  ,
       SUID            original            ,
       SUID            target              )
{
  QString Q                                                   ;
  UUIDs   U                                                   ;
  SUID    u                                                   ;
  Q = SC . sql . SelectFrom                                   (
        "second"                                              ,
        PlanTable(Triples)                                    ,
        QString("where `first` = %1 and `type` = %2"          )
        . arg ( original                                      )
        . arg ( Types::Schedule                             ) ,
        SC . OrderByAsc ( "id" )                            ) ;
  SqlLoopNow ( SC , Q )                                       ;
    U << SC . Uuid ( 0 )                                      ;
  SqlLoopErr ( SC , Q )                                       ;
  SqlLoopEnd ( SC , Q )                                       ;
  foreach ( u , U )                                           {
    Q = SC . sql . SelectFrom                                 (
          "third,flags"                                       ,
          PlanTable(Triples)                                  ,
          QString("where `first` = %1"
                   " and `second` = %2"
                   " and `type` = %3"                         )
          . arg ( original                                    )
          . arg ( u                                           )
          . arg ( Types::Schedule                         ) ) ;
    if ( SC . Fetch ( Q ) )                                   {
      SUID t = SC . Uuid ( 0 )                                ;
      SUID f = SC . Uuid ( 1 )                                ;
      SUID r = 0                                              ;
      /////////////////////////////////////////////////////////
      switch ( ToGroupID ( t ) )                              {
        case Groups::After                                    :
          r = RelationUUID(Before)                            ;
        break                                                 ;
        case Groups::Before                                   :
          r = RelationUUID(After)                             ;
        break                                                 ;
        case Groups::Subordination                            :
          r = RelationUUID(Contains)                          ;
        break                                                 ;
        case Groups::Contains                                 :
          r = RelationUUID(Subordination)                     ;
        break                                                 ;
      }                                                       ;
      /////////////////////////////////////////////////////////
      Q = QString ( "insert into `%1`"
                    " (`first`,`second`,`third`,`type`,`flags`)"
                    " values (%2,%3,%4,%5,%6) ;"              )
          . arg   ( PlanTable(Triples)                        )
          . arg   ( target                                    )
          . arg   ( u                                         )
          . arg   ( t                                         )
          . arg   ( Types::Schedule                           )
          . arg   ( f                                       ) ;
      SC . Query  ( Q                                       ) ;
      /////////////////////////////////////////////////////////
      Q = QString ( "insert into `%1`"
                    " (`first`,`second`,`third`,`type`,`flags`)"
                    " values (%2,%3,%4,%5,%6) ;"              )
          . arg   ( PlanTable(Triples)                        )
          . arg   ( u                                         )
          . arg   ( target                                    )
          . arg   ( r                                         )
          . arg   ( Types::Schedule                           )
          . arg   ( f                                       ) ;
      SC . Query  ( Q                                       ) ;
      /////////////////////////////////////////////////////////
    }                                                         ;
  }                                                           ;
  return true                                                 ;
}

QString N::FoundationManager::CloneEvent (
          QString   uuid                 ,
          QString   name                 ,
          QDateTime start                ,
          QDateTime final                )
{
  SUID nu = 0                                                                ;
  SUID uu = uuid . toULongLong ( )                                           ;
  ////////////////////////////////////////////////////////////////////////////
  EventManager  EM ( plan        )                                           ;
  SqlConnection SC ( plan -> sql )                                           ;
  if ( SC . open ( FunctionString ) )                                        {
    QString Q                                                                ;
    //////////////////////////////////////////////////////////////////////////
    Q = SC . sql . SelectFrom                                                (
          "type,status,adjust"                                               ,
          PlanTable(EventHistory)                                            ,
          SC . WhereUuid ( uu )                                            ) ;
    if ( SC . Fetch ( Q ) )                                                  {
      int eType  = SC . Int ( 0 )                                            ;
      int status = SC . Int ( 1 )                                            ;
      int adjust = SC . Int ( 2 )                                            ;
      nu = Append ( SC , eType , name , start , final )                      ;
      if ( nu > 0 )                                                          {
        //////////////////////////////////////////////////////////////////////
        QString note                                                         ;
        note = SC . getName ( PlanTable(EventNotes)                          ,
                              "uuid"                                         ,
                              plan -> LanguageId                             ,
                              uu                             )               ;
        EM . SaveNote   ( SC , nu , plan->LanguageId , note  )               ;
        //////////////////////////////////////////////////////////////////////
        CloneVariables  ( SC , uu , nu                       )               ;
        //////////////////////////////////////////////////////////////////////
        CloneFirst      ( SC                                                 ,
                          uu                                                 ,
                          nu                                                 ,
                          Types::Trigger                                     ,
                          Groups::StartTrigger                             ) ;
        CloneFirst      ( SC                                                 ,
                          uu                                                 ,
                          nu                                                 ,
                          Types::Trigger                                     ,
                          Groups::FinalTrigger               )               ;
        CloneFirst      ( SC , uu , nu , Types::Execution    )               ;
        //////////////////////////////////////////////////////////////////////
        CloneSecond     ( SC , uu , nu , Types::Task         )               ;
        CloneSecond     ( SC , uu , nu , Types::Equipment    )               ;
        CloneSecond     ( SC , uu , nu , Types::People       )               ;
        CloneSecond     ( SC , uu , nu , Types::Organization )               ;
        //////////////////////////////////////////////////////////////////////
        CloneDependency ( SC , uu , nu                       )               ;
        //////////////////////////////////////////////////////////////////////
      }                                                                      ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    SC . close  ( )                                                          ;
  }                                                                          ;
  SC   . remove ( )                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  return QString::number ( nu )                                              ;
}

void N::FoundationManager::HandleMusic(SUID uuid)
{
  SUID          mv = 0                                       ;
  SqlConnection SC ( plan -> sql )                           ;
  EventManager  EM ( plan        )                           ;
  ////////////////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )                        {
    QVariant V                                               ;
    //////////////////////////////////////////////////////////
    if ( EM . LoadVariable ( SC , uuid , "MusicUUID" , V ) ) {
      mv = V . toULongLong ( )                               ;
    }                                                        ;
    SC.close()                                               ;
  }                                                          ;
  SC.remove()                                                ;
  ////////////////////////////////////////////////////////////
  if ( 0 == mv ) return                                      ;
  setValue ( "RelayMusic" , mv )                             ;
  emit PrivateMusic ( )                                      ;
}

void N::FoundationManager::HandleVideo(SUID uuid)
{
  QString player = plan -> Bin . absoluteFilePath ( "Player.exe" ) ;
  QString wdir   = plan -> Bin . absolutePath     (              ) ;
  //////////////////////////////////////////////////////////////////
  if ( Variables . contains ( "Player" ) )                         {
    player = Value ( "Player" ) . toString ( )                     ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  QFileInfo FXI ( player )                                         ;
  if ( ! FXI . exists ( ) ) return                                 ;
  //////////////////////////////////////////////////////////////////
  EventManager  EM ( plan        )                                 ;
  SqlConnection SC ( plan -> sql )                                 ;
  //////////////////////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )                              {
    EM . DeleteScheduling ( SC , uuid )                            ;
    SC.close()                                                     ;
  }                                                                ;
  SC.remove()                                                      ;
  //////////////////////////////////////////////////////////////////
  QStringList args                                                 ;
  args << "Command"                                                ;
  args << "52001"                                                  ;
  args << QString::number ( uuid )                                 ;
  //////////////////////////////////////////////////////////////////
  QProcess::startDetached ( player , args , wdir )                 ;
}

void N::FoundationManager::HandleDependency(SUID uuid,int position)
{
  if ( N::Groups::FinalTrigger != position ) return ;
  UUIDs After                                       ;
  UUIDs Current                                     ;
  UUIDs Next                                        ;
  Next << uuid                                      ;
  while ( Next . count ( ) > 0 )                    {
    uuid = Next . first ( )                         ;
    Next . takeFirst    ( )                         ;
    if ( ! Current . contains ( uuid ) )            {
      Current << uuid                               ;
    }                                               ;
    HandleDependency ( uuid , After , Current )     ;
    foreach ( uuid , After )                        {
      if ( ! Next . contains ( uuid ) )             {
        Next << uuid                                ;
      }                                             ;
    }                                               ;
    After . clear ( )                               ;
  }                                                 ;
}

void N::FoundationManager::HandleDependency(SUID uuid,UUIDs & After,UUIDs & Current)
{
  UUIDs         U                                                            ;
  SUID          u                                                            ;
  SUID          t                                                            ;
  SUID          f                                                            ;
  SMAPs         dt                                                           ;
  EventManager  EM ( plan        )                                           ;
  SqlConnection SC ( plan -> sql )                                           ;
  ////////////////////////////////////////////////////////////////////////////
  if ( SC . open ( FunctionString ) )                                        {
    QString Q                                                                ;
    //////////////////////////////////////////////////////////////////////////
    Q = SC . sql . SelectFrom                                                (
          "second,third,flags"                                               ,
          PlanTable(Triples)                                                 ,
          QString("where `first` = %1 and `type` = %2"                       )
          . arg ( uuid                                                       )
          . arg ( Types::Schedule                                          ) ,
          SC . OrderByAsc ( "id" )                                         ) ;
    SqlLoopNow ( SC , Q )                                                    ;
      u = SC . Uuid ( 0 )                                                    ;
      t = SC . Uuid ( 1 )                                                    ;
      f = SC . Uuid ( 2 )                                                    ;
      switch ( ToGroupID ( t ) )                                             {
        case Groups::After                                                   :
        break                                                                ;
        case Groups::Before                                                  :
          U << u                                                             ;
          dt [ u ] = f                                                       ;
        break                                                                ;
        case Groups::Subordination                                           :
        break                                                                ;
        case Groups::Contains                                                :
        break                                                                ;
      }                                                                      ;
    SqlLoopErr ( SC , Q )                                                    ;
    SqlLoopEnd ( SC , Q )                                                    ;
    //////////////////////////////////////////////////////////////////////////
    if ( U . count ( ) > 0 )                                                 {
      Q = SC . sql . SelectFrom                                              (
            "start,final,type"                                               ,
            PlanTable(EventHistory)                                          ,
             SC . WhereUuid ( uuid )                                       ) ;
      if ( SC . Fetch ( Q ) )                                                {
        SUID      FT = SC . Tuid       ( 1  )                                ;
        QDateTime LT = EM . toDateTime ( FT )                                ;
        foreach ( u , U ) if ( ! Current . contains ( u ) )                  {
          SUID st                                                            ;
          SUID ft                                                            ;
          f = dt [ u ]                                                       ;
          Q = SC . sql . SelectFrom                                          (
                "start,final"                                                ,
                PlanTable(EventHistory)                                      ,
                 SC . WhereUuid ( u )                                      ) ;
          if ( SC . Fetch ( Q ) )                                            {
            //////////////////////////////////////////////////////////////////
            st = SC . Tuid ( 0 )                                             ;
            ft = SC . Tuid ( 1 )                                             ;
            t  = ft - st                                                     ;
            st = FT + f                                                      ;
            ft = st + t                                                      ;
            //////////////////////////////////////////////////////////////////
            EM . UpdateEvent ( SC                                            ,
                               u                                             ,
                               N::Calendars::Duration                        ,
                               st                                            ,
                               ft                                          ) ;
            EM . attachSpot  ( SC                                            ,
                               u                                             ,
                               N::History::WorkingPeriod                     ,
                               st                                            ,
                               ft                                          ) ;
            //////////////////////////////////////////////////////////////////
            LT = EM . toDateTime ( ft )                                      ;
            if ( nTimeNow > LT )                                             {
              EM . DeleteScheduling ( SC , u      )                          ;
              EM . Delete           ( SC , u      )                          ;
            } else                                                           {
              int tt                                                         ;
              tt = EM . Type        ( SC , u      )                          ;
              EM . AssureEvent      ( SC , u , tt )                          ;
              EM . DeleteScheduling ( SC , u      )                          ;
              EM . AddScheduling    ( SC , u      )                          ;
            }                                                                ;
          }                                                                  ;
        }                                                                    ;
        if ( ! After . contains ( u ) ) After << u                           ;
      }                                                                      ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    SC.close()                                                               ;
  }                                                                          ;
  SC.remove()                                                                ;
}
