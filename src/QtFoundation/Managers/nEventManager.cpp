#include <qtfoundation.h>

#define plan EventPlan

N::EventManager:: EventManager ( PurePlan * p )
                : EventPlan    (            p )
{
}

N::EventManager::~EventManager (void)
{
}

UUIDs N::EventManager::Uuids(SqlConnection & SC,QString Table)
{
  return SC . Uuids ( Table , "uuid" , SC . OrderByDesc ( "id" ) ) ;
}

bool N::EventManager::UpdateType (SqlConnection & Connection,QString Table,SUID uuid,int Type)
{
  QString Q                                                      ;
  Q = Connection.sql.Update(Table,"where uuid = :UUID",1,"type") ;
  Connection.Prepare (Q          )                               ;
  Connection.Bind    ("uuid",uuid)                               ;
  Connection.Bind    ("type",Type)                               ;
  return Connection . Exec (     )                               ;
}

SUID N::EventManager::AssureUuid(SqlConnection & Connection)
{
  SUID uuid = Connection.Unique(PlanTable(MajorUuid),"uuid"         ) ;
  Connection.assureUuid(PlanTable(MajorUuid   ),uuid,Types::Schedule) ;
  Connection.assureUuid(PlanTable(EventHistory),uuid,None           ) ;
  Connection.assureUuid(PlanTable(Events      ),uuid,None           ) ;
  return uuid                                                         ;
}

bool N::EventManager::AssureEvent(SqlConnection & Connection,SUID uuid,int Type)
{
  Connection.assureUuid(PlanTable(Events),uuid,Type) ;
  return true                                        ;
}

bool N::EventManager::UpdateName(SqlConnection & Connection,SUID uuid,QString task,QString oldName)
{
  QString Q                                                                      ;
  if (oldName.length()<=0)                                                       {
    Q = Connection.sql.InsertInto( PlanTable(Names),3,"uuid","language","name" ) ;
  } else                                                                         {
    Q = Connection.sql.Update    ( PlanTable(Names)                              ,
        "where uuid = :UUID and language = :LANGUAGE",1,"name"                 ) ;
  }                                                                              ;
  return Connection.insertName(Q,uuid,EventPlan->LanguageId,task)                ;
}

QString N::EventManager::Name(SqlConnection & Connection,SUID uuid)
{
  QString name = ""                                          ;
  name = Connection.getName                                  (
         PlanTable(Names),"uuid",EventPlan->LanguageId,uuid) ;
  return name                                                ;
}

int N::EventManager::Type(SqlConnection & Connection,SUID uuid)
{
  QString Q                                                   ;
  int     type = Error                                        ;
  Q = Connection.sql.SelectFrom                               (
        "type",PlanTable(EventHistory)                        ,
        Connection.WhereUuid(uuid)                          ) ;
  if (Connection.Fetch(Q)) type = Connection.Value(0).toInt() ;
  return type                                                 ;
}

QString N::EventManager::ActionQuery(SqlConnection & Connection)
{
  return Connection.sql.SelectFrom            (
           "type,uuid",PlanTable(ActionTypes) ,
           Connection.OrderByAsc("type")    ) ;
}

TUID N::EventManager::TimeSpot(SqlConnection & Connection,SUID uuid)
{
  QString Q                                          ;
  TUID    tuid = 0                                   ;
  Q = Connection.sql.SelectFrom                      (
        "stardate"                                   ,
        PlanTable(History)                           ,
        Connection.WhereUuid(uuid)                 ) ;
  if (Connection.Fetch(Q)) tuid = Connection.Tuid(0) ;
  return tuid                                        ;
}

bool N::EventManager::Delete(SqlConnection & Connection,SUID uuid)
{
  QString Q = Connection.sql.DeleteFrom      (
                PlanTable(Events)            ,
                Connection.WhereUuid(uuid) ) ;
  return Connection.Query(Q)                 ;
}

SUID N::EventManager::Task(SqlConnection & Connection,SUID uuid)
{
  SUID    task = 0                                   ;
  QString Q                                          ;
  Q = Connection.sql.SelectFrom                      (
        "first",PlanTable(OtherGroups)               ,
        QString("where second = %1 and t1 = %2 "
                "and t2 = %3 and relation = %4"      )
        .arg(uuid                                    )
        .arg(Types::Task                             )
        .arg(Types::Schedule                         )
        .arg(Groups::Subordination                   )
      )                                              ;
  if (Connection.Fetch(Q)) task = Connection.Uuid(0) ;
  return  task                                       ;
}

bool N::EventManager::Relegation(SqlConnection & Connection,SUID uuid,SUID task)
{
  QString Q                                                ;
  Q = Connection . sql . DeleteFrom                        (
        PlanTable(OtherGroups)                             ,
        QString ( "where first = %1 and second = %2 "
                  "and t1 = %3 and t2 = %4 and "
                  "relation = %5"                          )
        . arg ( task                                       )
        . arg ( uuid                                       )
        . arg ( Types::Task                                )
        . arg ( Types::Schedule                            )
        . arg ( Groups::Subordination                      )
      )                                                    ;
  Connection . Query ( Q )                                 ;
  if (task<=0) return true                                 ;
  Q = Connection.sql.InsertInto                            (
        PlanTable(OtherGroups)                             ,
        5,"first","second","t1","t2","relation"          ) ;
  Connection . Prepare ( Q                               ) ;
  Connection . Bind    ( "first"   ,task                 ) ;
  Connection . Bind    ( "second"  ,uuid                 ) ;
  Connection . Bind    ( "t1"      ,Types::Task          ) ;
  Connection . Bind    ( "t2"      ,Types::Schedule      ) ;
  Connection . Bind    ( "relation",Groups::Subordination) ;
  return Connection . Exec ( )                             ;
}

int N::EventManager::UuidType(SqlConnection & Connection,SUID uuid)
{
  int     type = Types::None                        ;
  QString Q    = Connection.sql.SelectFrom          (
        "type"                                      ,
        PlanTable(MajorUuid)                        ,
        Connection.WhereUuid(uuid)                ) ;
  if (Connection.Fetch(Q)) type = Connection.Int(0) ;
  return type                                       ;
}

QString N::EventManager::TimePeriod(SqlConnection & Connection,SUID uuid)
{
  return Connection.sql.SelectFrom      (
           "first,second"               ,
           PlanTable(Periods)           ,
           Connection.WhereUuid(uuid) ) ;
}

SUID N::EventManager::Chronology(SqlConnection & Connection,SUID uuid)
{
  SUID    tspot = 0                                   ;
  QString Q     = Connection.sql.SelectFrom           (
      "timespot" , PlanTable(Chronology)              ,
      QString                                         (
        "where uuid = %1 order by position desc"
        " limit 0,1").arg(uuid)                     ) ;
  if (Connection.Fetch(Q)) tspot = Connection.Uuid(0) ;
  return tspot                                        ;
}

UUIDs N::EventManager::Events(SqlConnection & Connection,QDateTime Start,QDateTime Final)
{
  QString  Q                                      ;
  StarDate STUID                                  ;
  StarDate FTUID                                  ;
  STUID    = Start                                ;
  FTUID    = Final                                ;
  TUID  SS = STUID.Stardate                       ;
  TUID  FF = FTUID.Stardate                       ;
  UUIDs   Uuids                                   ;
  Q = Connection.sql.SelectFrom("uuid"            ,
     PlanTable(EventHistory)                      ,
     QString("where tmode = %1 and "
             "( (start <= %2 and %3 <= final) or"
              " (start <= %4 and %5 <= final) or"
              " (%6 <= start and start <= %7) or"
              " (%8 <= final and final <= %9) )"
             " order by start asc"                )
     .arg(Calendars::Duration                     )
     .arg(SS).arg(SS).arg(FF).arg(FF)
     .arg(SS).arg(FF).arg(SS).arg(FF)
  )                                               ;
  SqlLoopNow(Connection,Q)                        ;
    Uuids << Connection.Uuid(0)                   ;
  SqlLoopErr(Connection,Q)                        ;
  SqlLoopEnd(Connection,Q)                        ;
  return Uuids                                    ;
}

bool N::EventManager::assureName(SqlConnection & Connection,SUID uuid,int language,QString name)
{
  QString Q                                             ;
  Q = Connection.sql.SelectFrom("name",PlanTable(Names) ,
        QString ( "where uuid = %1 and language = %2"   )
        .arg(uuid).arg(language)                      ) ;
  if (Connection.Fetch(Q))                              {
    Q = Connection.NameUpdate(PlanTable(Names))         ;
  } else                                                {
    Q = Connection.NameSyntax(PlanTable(Names))         ;
  }                                                     ;
  return Connection.insertName(Q,uuid,language,name)    ;
}

SUID N::EventManager::attachSpot(SqlConnection & Connection,SUID uuid,int Type,TUID start,TUID final)
{
  QString Q        = ""                                                        ;
  int     Position = 0                                                         ;
  SUID    TimeSpot = 0                                                         ;
  SUID    First    = 0                                                         ;
  SUID    Second   = 0                                                         ;
  TimeSpot = Connection . Unique ( PlanTable(MajorUuid) , "uuid"             ) ;
  Connection.assureUuid ( PlanTable(MajorUuid),TimeSpot,Types::Period )        ;
  Q = Connection.sql.SelectFrom ( "position" , PlanTable ( Chronology )        ,
      QString("where uuid = %1 order by position desc limit 0,1").arg(uuid)  ) ;
  if (Connection.Fetch(Q)) Position = Connection . Int ( 0 )                   ;
  Position ++                                                                  ;
  Q = Connection.sql.InsertInto( PlanTable ( Chronology )                      ,
      3,"uuid","timespot","position"                                         ) ;
  Connection . Prepare ( Q )                                                   ;
  Connection.Bind("uuid"    ,uuid     )                                        ;
  Connection.Bind("timespot",TimeSpot )                                        ;
  Connection.Bind("position",Position )                                        ;
  Connection . Exec    (   )                                                   ;
  First  = Connection.Unique(PlanTable(MajorUuid),"uuid"                     ) ;
  Connection.assureUuid(PlanTable(MajorUuid),First ,Types::History           ) ;
  Q = Connection.sql.InsertInto(PlanTable(History),3,"uuid","type","stardate") ;
  Connection . Prepare ( Q )                                                   ;
  Connection.Bind("uuid"    ,First                                           ) ;
  Connection.Bind("type"    ,History::TimeSpot                               ) ;
  Connection.Bind("stardate",start                                           ) ;
  Connection . Exec    (   )                                                   ;
  Second = Connection.Unique(PlanTable(MajorUuid),"uuid"                     ) ;
  Connection.assureUuid(PlanTable(MajorUuid),Second,Types::History           ) ;
  Q = Connection.sql.InsertInto(PlanTable(History),3,"uuid","type","stardate") ;
  Connection . Prepare ( Q                                                   ) ;
  Connection . Bind    ( "uuid"     , Second                                 ) ;
  Connection . Bind    ( "type"     , History::TimeSpot                      ) ;
  Connection . Bind    ( "stardate" , final                                  ) ;
  Connection . Exec    (                                                     ) ;
  Q = Connection.sql.InsertInto( PlanTable(Periods)                            ,
      4,"uuid","first","second","type"                                       ) ;
  Connection . Prepare ( Q                                                   ) ;
  Connection . Bind    ( "uuid"   , TimeSpot                                 ) ;
  Connection . Bind    ( "first"  , First                                    ) ;
  Connection . Bind    ( "second" , Second                                   ) ;
  Connection . Bind    ( "type"   , Type                                     ) ;
  Connection . Exec    (                                                     ) ;
  return TimeSpot                                                              ;
}

bool N::EventManager::UpdateEvent(SqlConnection & Connection,SUID uuid,int Type,TUID start,TUID final)
{
  QString Q                                ;
  Q  = Connection . sql . Update           (
         PlanTable(EventHistory)           ,
         "where uuid = :UUID"              ,
        3                                  ,
        "tmode"                            ,
        "start"                            ,
        "final"                          ) ;
  Connection . Prepare ( Q               ) ;
  Connection . Bind    ( "uuid"  , uuid  ) ;
  Connection . Bind    ( "tmode" , Type  ) ;
  Connection . Bind    ( "start" , start ) ;
  Connection . Bind    ( "final" , final ) ;
  Connection . Exec    (                 ) ;
  return true                              ;
}

bool N::EventManager::DeleteScheduling(SqlConnection & Connection,SUID uuid)
{
  QString Q                           ;
  Q = Connection . sql . DeleteFrom   (
    PlanTable(Scheduling)             ,
    Connection . WhereUuid ( uuid ) ) ;
  return Connection . Query( Q )      ;
}

bool N::EventManager::AddScheduling(SqlConnection & Connection,SUID uuid)
{
  QString   Q                                         ;
  QDateTime CC       = QDateTime::currentDateTime()   ;
  SUID      TimeSpot = Chronology(Connection,uuid)    ;
  int       eType    = Types::Schedule                ;
  int       sType    = History::WorkingPeriod         ;
  TUID      CcTime   = 0                              ;
  TUID      StTime   = 0                              ;
  TUID      FiTime   = 0                              ;
  SUID      First    = 0                              ;
  SUID      Second   = 0                              ;
  StarDate  NSD                                       ;
  NSD    = CC                                         ;
  CcTime = NSD.Stardate                               ;
  Q = Connection.sql.SelectFrom                       (
        "start,final"                                 ,
        PlanTable(EventHistory)                       ,
        Connection.WhereUuid(uuid)                  ) ;
  if (Connection.Fetch(Q))                            {
    StTime = Connection . Tuid ( 0 )                  ;
    FiTime = Connection . Tuid ( 1 )                  ;
  }                                                   ;
  Q = TimePeriod(Connection,TimeSpot)                 ;
  if (Connection.Fetch(Q))                            {
    First  = Connection . Uuid ( 0 )                  ;
    Second = Connection . Uuid ( 1 )                  ;
  }                                                   ;
  if (CcTime==0) return false                         ;
  if (StTime==0) return false                         ;
  if (FiTime==0) return false                         ;
  if (First ==0) return false                         ;
  if (Second==0) return false                         ;
  if (CcTime>FiTime) return false                     ;
  if (CcTime>StTime)                                  {
    sType = History::ActionEnd                        ;
  }                                                   ;
  Q = Connection.sql.SelectFrom                       (
        "id",PlanTable(Scheduling)                    ,
        Connection.WhereUuid(uuid)                  ) ;
  if (Connection.Fetch(Q))                            {
    Q = Connection.sql.Update                         (
          PlanTable(Scheduling)                       ,
          "where `uuid` = :UUID"                      ,
          6                                           ,
          "type"                                      ,
          "mode"                                      ,
          "start"                                     ,
          "action"                                    ,
          "finish"                                    ,
          "final"                                   ) ;
  } else                                              {
    Q = Connection.sql.InsertInto                     (
          PlanTable(Scheduling)                       ,
          7                                           ,
          "uuid"                                      ,
          "type"                                      ,
          "mode"                                      ,
          "start"                                     ,
          "action"                                    ,
          "finish"                                    ,
          "final"                                   ) ;
  }                                                   ;
  Connection . Prepare ( Q                          ) ;
  Connection . Bind    ( "uuid"   , uuid            ) ;
  Connection . Bind    ( "type"   , eType           ) ;
  Connection . Bind    ( "mode"   , sType           ) ;
  Connection . Bind    ( "start"  , StTime          ) ;
  Connection . Bind    ( "action" , First           ) ;
  Connection . Bind    ( "finish" , FiTime          ) ;
  Connection . Bind    ( "final"  , Second          ) ;
  Connection . Exec    (                            ) ;
  return true                                         ;
}

UUIDs N::EventManager::Scheduling(SqlConnection & Connection)
{
  QDateTime CC = nTimeNow                                    ;
  QDateTime ST = CC                                          ;
  QDateTime FI = CC                                          ;
  StarDate  SV                                               ;
  StarDate  FV                                               ;
  UUIDs     Uuids                                            ;
  QString   Q                                                ;
  ST = ST . addSecs ( -43200 )                               ;
  FI = FI . addSecs (  43200 )                               ;
  SV = ST                                                    ;
  FV = FI                                                    ;
  Q  = Connection . sql . SelectFrom                         (
    "uuid",PlanTable(Scheduling)                             ,
    QString                                                  (
      "where ( ( %1 <= `start`  ) and ( `start`  <= %2 ) ) or"
           " ( ( %3 <= `finish` ) and ( `finish` <= %4 ) ) " )
    . arg ( SV . Stardate ) . arg ( FV . Stardate            )
    . arg ( SV . Stardate ) . arg ( FV . Stardate            )
  )                                                          ;
  SqlLoopNow ( Connection , Q )                              ;
    Uuids << Connection . Uuid (0)                           ;
  SqlLoopErr ( Connection , Q )                              ;
  SqlLoopEnd ( Connection , Q )                              ;
  return Uuids                                               ;
}

TMAPs N::EventManager::Monitoring(SqlConnection & Connection,UUIDs & Uuids)
{
  TMAPs   E                                   ;
  TMAP    M                                   ;
  SUID    uuid                                ;
  QString Q                                   ;
  if (Uuids.count()<=0) return E              ;
  foreach (uuid,Uuids)                        {
    Q = Connection.sql.SelectFrom             (
          "start,finish,mode"                 ,
          PlanTable(Scheduling)               ,
          Connection.WhereUuid(uuid))         ;
    if (Connection.Fetch(Q))                  {
      int  mode = Connection.Int (2)          ;
      switch ( mode )                         {
        case History::WorkingPeriod           :
          M.first   = Connection.Tuid(0)      ;
          M.second  = uuid                    ;
          E << M                              ;
          M.first   = Connection.Tuid(1)      ;
          M.second  = uuid                    ;
          E << M                              ;
        break                                 ;
        case History::ActionStart             :
          M.first   = Connection.Tuid(0)      ;
          M.second  = uuid                    ;
          E << M                              ;
        break                                 ;
        case History::ActionEnd               :
          M.first   = Connection.Tuid(1)      ;
          M.second  = uuid                    ;
          E << M                              ;
        break                                 ;
      }                                       ;
    }                                         ;
  }                                           ;
  return E                                    ;
}

QString N::EventManager::EventNote(SqlConnection & SC,SUID uuid,int language)
{
  return SC . getName ( PlanTable(EventNotes) , "uuid" , language , uuid ) ;
}

bool N::EventManager::SaveNote(SqlConnection & SC,SUID uuid,int language,QString note)
{
  QString Q                                             ;
  Q = SC . sql . SelectFrom                             (
        "name"                                          ,
        PlanTable(EventNotes)                           ,
        QString("where uuid = %1 and language = %2"     )
        .arg(uuid).arg(language)                      ) ;
  if (SC.Fetch(Q))                                      {
    Q = SC.NameUpdate(PlanTable(EventNotes))            ;
  } else                                                {
    Q = SC.NameSyntax(PlanTable(EventNotes))            ;
  }                                                     ;
  return SC . insertName ( Q , uuid , language , note ) ;
}

bool N::EventManager::hasOwners(SqlConnection & SC,SUID u)
{
  QString Q                             ;
  int     total = 0                     ;
  Q = SC . sql . SelectFrom             (
        "count(*)"                      ,
        PlanTable(Groups)               ,
        QString("where `t2` = %1"
                " and ( ( `t1` = %2 ) or"
                      " ( `t1` = %3 ) or"
                      " ( `t1` = %4 ) )"
                " and `relation` = %5"
                " and `second` = %6"    )
        . arg ( Types::Schedule         )
        . arg ( Types::Equipment        )
        . arg ( Types::People           )
        . arg ( Types::Organization     )
        . arg ( Groups::Subordination   )
        . arg ( u                   ) ) ;
  if ( SC . Fetch ( Q ) )               {
    total = SC . Int ( 0 )              ;
  }                                     ;
  return ( total > 0 )                  ;
}

bool N::EventManager::hasTriggers(SqlConnection & SC,SUID u)
{
  QString Q                               ;
  SUID    S                               ;
  Q = SC . sql . SelectFrom               (
        "second"                          ,
        PlanTable(OtherGroups)            ,
        QString("where `first` = %1"
                 " and `t1` = %2"
                 " and `t2` = %3"
                 " and `relation` = %4"   )
        . arg  ( u                        )
        . arg  ( Types::Schedule          )
        . arg  ( Types::Trigger           )
        . arg  ( Groups::StartTrigger ) ) ;
  if ( SC . Fetch ( Q ) )                 {
    S = SC . Uuid ( 0 )                   ;
    if ( S > 0 ) return true              ;
  }                                       ;
  /////////////////////////////////////////
  Q = SC . sql . SelectFrom               (
        "second"                          ,
        PlanTable(OtherGroups)            ,
        QString("where `first` = %1"
                 " and `t1` = %2"
                 " and `t2` = %3"
                 " and `relation` = %4"   )
        . arg ( u                         )
        . arg ( Types::Schedule           )
        . arg ( Types::Trigger            )
        . arg ( Groups::FinalTrigger      )
      )                                   ;
  if ( SC . Fetch ( Q ) )                 {
    S = SC . Uuid ( 0 )                   ;
    if ( S > 0 ) return true              ;
  }                                       ;
  return false                            ;
}

bool N::EventManager::hasDependency(SqlConnection & SC,SUID u)
{
  QString Q                                         ;
  int     total = 0                                 ;
  Q = SC . sql . SelectFrom                         (
        "count(*)"                                  ,
        PlanTable(Triples)                          ,
        QString("where `first` = %1 and `type` = %2")
        . arg  ( u                                  )
        . arg  ( Types::Schedule                ) ) ;
  if ( SC . Fetch ( Q ) ) total = SC . Int ( 0 )    ;
  return ( total > 0 )                              ;
}

bool N::EventManager::isRepeated(SqlConnection & SC,SUID u)
{
  QVariant V                                                     ;
  if ( ! LoadVariable ( SC , u , "Repeated" , V ) ) return false ;
  return V . toBool ( )                                          ;
}

bool N::EventManager::hasTasks(SqlConnection & SC,SUID u)
{
  QString Q                           ;
  int     total = 0                   ;
  Q = SC . sql . SelectFrom           (
        "count(*)"                    ,
        PlanTable(OtherGroups)        ,
        QString("where `t2` = %1"
                 " and `t1` = %2"
                 " and `relation` = %3"
                 " and `second` = %4" )
        . arg ( Types::Schedule       )
        . arg ( Types::Task           )
        . arg ( Groups::Subordination )
        . arg ( u                 ) ) ;
  if ( SC . Fetch ( Q ) )             {
    total = SC . Int ( 0 )            ;
  }                                   ;
  return ( total > 0 )                ;
}

bool N::EventManager::SaveVariable(SqlConnection & SC,SUID u,QString n,QVariant & v)
{
  if ( u              <= 0 ) return false        ;
  if ( n . length ( ) <= 0 ) return false        ;
  ////////////////////////////////////////////////
  QString Q                                      ;
  Q = SC . sql . DeleteFrom                      (
        PlanTable(Variables)                     ,
        QString("where `uuid` = :UUID"
                 " and `type` = :TYPE"
                 " and `name` = :NAME"       ) ) ;
  SC . Prepare     ( Q                         ) ;
  SC . Bind        ( "uuid"  , u               ) ;
  SC . Bind        ( "type"  , Types::Schedule ) ;
  SC . Bind        ( "name"  , n               ) ;
  SC . Exec        (                           ) ;
  ////////////////////////////////////////////////
  Q = SC . sql . InsertInto                      (
        PlanTable(Variables)                     ,
        4                                        ,
        "uuid"                                   ,
        "type"                                   ,
        "name"                                   ,
        "value"                                ) ;
  SC . Prepare     ( Q                         ) ;
  SC . Bind        ( "uuid"  , u               ) ;
  SC . Bind        ( "type"  , Types::Schedule ) ;
  SC . Bind        ( "name"  , n               ) ;
  SC . Bind        ( "value" , v               ) ;
  return SC . Exec (                           ) ;
}

bool N::EventManager::LoadVariable(SqlConnection & SC,SUID u,QString n,QVariant & v)
{
  if ( u              <= 0 ) return false ;
  if ( n . length ( ) <= 0 ) return false ;
  QString Q                               ;
  bool    correct = false                 ;
  Q = QString("select `value` from `%1`"
              " where `uuid` = %2"
                " and `type` = %3"
                " and `name` = '%4' ;"    )
      . arg ( PlanTable(Variables)        )
      . arg ( u                           )
      . arg ( Types::Schedule             )
      . arg ( n                         ) ;
  if ( SC . Fetch ( Q ) )                 {
    correct = true                        ;
    v       = SC . Value ( 0 )            ;
  }                                       ;
  return correct                          ;
}

bool N::EventManager::DeleteVariable(SqlConnection & SC,SUID u,QString n)
{
  if ( u              <= 0 ) return false ;
  if ( n . length ( ) <= 0 ) return false ;
  QString Q                               ;
  Q = QString("delete from `%1`"
              " where `uuid` = %2"
                " and `type` = %3"
                " and `name` = '%4' ;"    )
      . arg ( PlanTable(Variables)        )
      . arg ( u                           )
      . arg ( Types::Schedule             )
      . arg ( n                         ) ;
  return SC . Query ( Q )                 ;
}

SUID N::EventManager::GetTrigger(SqlConnection & SC,int type,SUID u)
{
  if ( u <= 0 ) return 0                ;
  QString Q                             ;
  Q = SC . sql . SelectFrom             (
        "second"                        ,
        PlanTable(OtherGroups)          ,
        QString("where `first` = %1"
                 " and `t1` = %2"
                 " and `t2` = %3"
                 " and `relation` = %4" )
        . arg  ( u                      )
        . arg  ( Types::Schedule        )
        . arg  ( Types::Trigger         )
        . arg  ( type               ) ) ;
  if ( SC . Fetch ( Q ) )               {
    return SC . Uuid ( 0 )              ;
  }                                     ;
  return 0                              ;
}

QDateTime N::EventManager::toDateTime(TUID tuid)
{
  StarDate S              ;
  S.Stardate = tuid       ;
  S.locate()              ;
  return S.toLocalTime () ;
}

SUID N::EventManager::Append(PurePlan * p,int type,QString name,QDateTime Start,QDateTime End)
{
  EventManager  EM ( p        )                                    ;
  SqlConnection SC ( p -> sql )                                    ;
  SUID          uu = 0                                             ;
  StarDate      ST                                                 ;
  StarDate      FT                                                 ;
  ST = Start                                                       ;
  FT = End                                                         ;
  if ( SC . open ( FunctionString ) )                              {
    uu = EM . AssureUuid ( SC                                    ) ;
    EM . UpdateType      ( SC                                      ,
                           p -> Tables [ N::Tables::EventHistory ] ,
                           uu                                      ,
                           type                                  ) ;
    EM . AssureEvent     ( SC , uu ,type                         ) ;
    EM . UpdateType      ( SC                                      ,
                           p -> Tables [ N::Tables::Events ]       ,
                           uu                                      ,
                           type                                  ) ;
    SC . assureName      ( p -> Tables [ N::Tables::Names ]        ,
                           uu                                      ,
                           p -> LanguageId                         ,
                           name                                  ) ;
    EM . UpdateEvent     ( SC                                      ,
                           uu                                      ,
                           N::Calendars::Duration                  ,
                           ST . Stardate                           ,
                           FT . Stardate                         ) ;
    EM . attachSpot      ( SC                                      ,
                           uu                                      ,
                           N::History::WorkingPeriod               ,
                           ST.Stardate                             ,
                           FT.Stardate                           ) ;
    SC . close ( )                                                 ;
  }                                                                ;
  SC . remove ( )                                                  ;
  return uu                                                        ;
}

bool N::EventManager::Complete(PurePlan * plan,SUID uu,QDateTime Start,QDateTime End)
{
  if ( 0 == uu ) return false                         ;
  EventManager  EM ( plan        )                    ;
  SqlConnection SC ( plan -> sql )                    ;
  StarDate      ST                                    ;
  StarDate      FT                                    ;
  ST = Start                                          ;
  FT = End                                            ;
  if ( SC . open ( FunctionString )                 ) {
    EM . Delete           ( SC , uu                 ) ;
    EM . UpdateEvent      ( SC                        ,
                            uu                        ,
                            N::Calendars::Duration    ,
                            ST . Stardate             ,
                            FT . Stardate           ) ;
    EM . attachSpot       ( SC                        ,
                            uu                        ,
                            N::History::WorkingPeriod ,
                            ST.Stardate               ,
                            FT.Stardate             ) ;
    EM . DeleteScheduling ( SC , uu                 ) ;
    SC . close ( )                                    ;
  }                                                   ;
  SC . remove ( )                                     ;
  return true                                         ;
}
