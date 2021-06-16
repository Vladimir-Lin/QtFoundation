#include <qtfoundation.h>

#define plan HistoryPlan

N::HistoryManager:: HistoryManager ( PurePlan * p )
                  : HistoryPlan    (            p )
{
}

N::HistoryManager::~HistoryManager (void)
{
}

QString N::HistoryManager::Name      (
          SqlConnection & Connection ,
          SUID            uuid       ,
          int             language   )
{
  return Connection . getName (
           PlanTable(Names)   ,
           "uuid"             ,
           language           ,
           uuid             ) ;
}

bool N::HistoryManager::assureName (
       SqlConnection & Connection  ,
       SUID            uuid        ,
       int             language    ,
       QString         name        )
{
  return Connection . assureName   (
           PlanTable(Names)        ,
           uuid                    ,
           language                ,
           name                  ) ;
}

TUID N::HistoryManager::Time(SqlConnection & SC,SUID uuid)
{
  TUID    tuid = 0           ;
  QString Q                  ;
  Q = SC.sql.SelectFrom      (
        "stardate"           ,
        PlanTable(History)      ,
        SC.WhereUuid(uuid) ) ;
  if (SC.Fetch(Q))           {
    tuid = SC . Tuid ( 0 )   ;
  }                          ;
  return tuid                ;
}

TUID N::HistoryManager::GetTime(SqlConnection & SC,SUID uuid)
{
  return Time ( SC , uuid ) ;
}

SUID N::HistoryManager::assureTime (
       SqlConnection & SC          ,
       SUID            uuid        ,
       int             type        ,
       TUID            tuid        )
{
  QString Q                          ;
  if (uuid<=0)                       {
    uuid = SC . Unique               (
      PlanTable(MajorUuid)           ,
      "uuid"                         ,
      IdSections::HistoryHead      ) ;
    SC . assureUuid                  (
      PlanTable(MajorUuid)           ,
      uuid                           ,
      Types::History               ) ;
    Q = SC.sql.InsertInto            (
          PlanTable(History)         ,
          2                          ,
          "uuid"                     ,
          "type"                   ) ;
    SC . Prepare ( Q               ) ;
    SC . Bind    ( "uuid" , uuid   ) ;
    SC . Bind    ( "type" , type   ) ;
    SC . Exec    (                 ) ;
  }                                  ;
  Q = SC . sql . Update              (
        PlanTable(History)           ,
        SC.sql.Where(1,"uuid")       ,
        1,"stardate"               ) ;
  SC . Prepare ( Q                 ) ;
  SC . Bind    ( "uuid"     , uuid ) ;
  SC . Bind    ( "stardate" , tuid ) ;
  SC . Exec    (                   ) ;
  return uuid                        ;
}

SUID N::HistoryManager::GetHistoryType (
       SqlConnection & SC              ,
       UUIDs         & uuids           ,
       int             type            )
{
  QString Q                            ;
  SUID uuid                            ;
  foreach (uuid,uuids)                 {
    int T = 0                          ;
    Q = SC.sql.SelectFrom              (
          "type"                       ,
          PlanTable(History)           ,
          SC.WhereUuid(uuid) )         ;
    if (SC.Fetch(Q))                   {
      T = SC . Int ( 0 )               ;
      if ( T == type ) return uuid     ;
    }                                  ;
  }                                    ;
  return 0                             ;
}

SUID N::HistoryManager::assurePeriod (
       SqlConnection & SC          ,
       SUID            uuid        ,
       SUID            first       ,
       SUID            second      ,
       int             type        )
{
  QString Q                          ;
  if (uuid<=0)                       {
    uuid = SC . Unique               (
      PlanTable(MajorUuid)           ,
      "uuid"                         ,
      IdSections::PeriodHead       ) ;
    SC . assureUuid                  (
      PlanTable(MajorUuid)           ,
      uuid                           ,
      Types::Period                ) ;
    Q = SC.sql.InsertInto            (
          PlanTable(Periods)         ,
          4                          ,
          "uuid"                     ,
          "first"                    ,
          "second"                   ,
          "type"                   ) ;
  } else                             {
    Q = SC . sql . Update            (
          PlanTable(Periods)         ,
          SC.sql.Where(1,"uuid")     ,
          3                          ,
          "first"                    ,
          "second"                   ,
          "type"                   ) ;
  }                                  ;
  SC . Prepare ( Q                 ) ;
  SC . Bind    ( "uuid"   , uuid   ) ;
  SC . Bind    ( "first"  , first  ) ;
  SC . Bind    ( "second" , second ) ;
  SC . Bind    ( "type"   , type   ) ;
  SC . Exec    (                   ) ;
  return uuid                        ;
}

SUID N::HistoryManager::assurePeriod (
       SqlConnection & SC            ,
       SUID            uuid          ,
       TUID            first         ,
       TUID            second        ,
       int             type          ,
       int             spotType      )
{
  QString Q                                               ;
  SUID    F = 0                                           ;
  SUID    S = 0                                           ;
  if ( uuid <= 0 )                                        {
    F    = assureTime   ( SC , F    , spotType , first  ) ;
    S    = assureTime   ( SC , S    , spotType , second ) ;
    uuid = assurePeriod ( SC , uuid , F , S    , type   ) ;
  } else                                                  {
    Q    = SC . sql . SelectFrom                          (
             "first,second"                               ,
             PlanTable(Periods)                           ,
             SC.WhereUuid(uuid)                         ) ;
    if (SC.Fetch(Q))                                      {
      F    = SC . Uuid ( 0 )                              ;
      S    = SC . Uuid ( 1 )                              ;
    }                                                     ;
    F    = assureTime   ( SC , F    , spotType , first  ) ;
    S    = assureTime   ( SC , S    , spotType , second ) ;
    uuid = assurePeriod ( SC , uuid , F , S    , type   ) ;
  }                                                       ;
  return uuid                                             ;
}

SUID N::HistoryManager::assureChronology (
       SqlConnection & SC                ,
       SUID            uuid              ,
       SUID            timespot          )
{
  QString Q                              ;
  int     position = -1                  ;
  SUID    ts                             ;
  if ( uuid <= 0 ) return uuid           ;
  Q = SC . sql . SelectFrom              (
        "timespot,position"              ,
        PlanTable(Chronology)            ,
        SC.WhereUuid(uuid)               ,
        SC.OrderByDesc("position")       ,
        SC.sql.Limit(0,1)              ) ;
  if ( SC.Fetch(Q) )                     {
    ts       = SC . Uuid ( 0 )           ;
    position = SC . Int  ( 1 )           ;
    if ( ts == timespot ) return uuid    ;
  }                                      ;
  position++                             ;
  ////////////////////////////////////////
  Q = SC . sql . InsertInto              (
        PlanTable(Chronology)            ,
        3                                ,
        "uuid"                           ,
        "timespot"                       ,
        "position"                     ) ;
  SC . Prepare ( Q                     ) ;
  SC . Bind    ( "uuid"     , uuid     ) ;
  SC . Bind    ( "timespot" , timespot ) ;
  SC . Bind    ( "position" , position ) ;
  SC . Exec    (                       ) ;
  return uuid                            ;
}

SUID N::HistoryManager::appendHistoryType ( SqlConnection & SC )
{
  SUID u = 0                                                                 ;
  u = SC . Unique                                                            (
        PlanTable(MajorUuid)                                                 ,
        "uuid"                                                               ,
        IdSections::HistoryTypeHead                                        ) ;
  if ( ! SC . assureUuid ( PlanTable(MajorUuid) , u , Types::HistoryType ) ) {
    u = 0                                                                    ;
  }                                                                          ;
  if ( u > 0 )                                                               {
    if ( ! SC . insertUuid ( PlanTable(HistoryTypes) , u , "uuid" ) )        {
      u = 0                                                                  ;
    }                                                                        ;
  }                                                                          ;
  return u                                                                   ;
}

QDateTime N::HistoryManager::toDateTime(TUID tuid)
{
  StarDate S               ;
  S . Stardate = tuid      ;
  S . locate()             ;
  return S.toLocalTime ( ) ;
}
