#include <qtfoundation.h>

N::SqlOrganizer:: SqlOrganizer ( PurePlan * p )
                : plan         (            p )
{
}

N::SqlOrganizer::~SqlOrganizer(void)
{
}

void N::SqlOrganizer::Start(int section,int & value,bool & go)
{
}

void N::SqlOrganizer::setRange (int section,int MinV,int MaxV)
{
}

void N::SqlOrganizer::Complete(int section)
{
}

void N::SqlOrganizer::Action(int section,int step)
{
}

bool N::SqlOrganizer::Sequential (
       SqlConnection & SC        ,
       int             section   ,
       QString         table     ,
       QString         column    ,
       int             StartId   )
{
  QString Q                               ;
  int     total                           ;
  int     offshift                        ;
  int     lastest  = 0                    ;
  /////////////////////////////////////////
  Action ( section , 0 )                  ;
  Q = SC.sql.SelectFrom("count(*)",table) ;
  if (SC.Fetch(Q)) total = SC.Int(0)      ;
  if (total<=0)                           {
    int  index = 0                        ;
    bool go    = true                     ;
    Start      ( section , index , go   ) ;
    setRange   ( section , 0     , 1    ) ;
    Complete   ( section                ) ;
    return false                          ;
  }                                       ;
  /////////////////////////////////////////
  Action ( section , 1 )                  ;
  Q = SC.sql.SelectFrom                   (
        column                            ,
        table                             ,
        SC . OrderByDesc ( column )       ,
        SC . sql . Limit ( 0 , 1  )     ) ;
  if (SC.Fetch(Q)) lastest = SC.Int(0)    ;
  offshift = lastest - StartId + 1        ;
  if ( offshift == total )                {
    int  index = 0                        ;
    bool go    = true                     ;
    Start      ( section , index , go   ) ;
    setRange   ( section , 0     , 1    ) ;
    Complete   ( section                ) ;
    return true                           ;
  }                                       ;
  /////////////////////////////////////////
  QList<TUID> TUIDs                       ;
  Action ( section , 2 )                  ;
  Q = SC.sql.SelectFrom                   (
        column                            ,
        table                             ,
        SC . OrderByAsc ( column )      ) ;
  SqlLoopNow ( SC , Q )                   ;
    TUIDs << SC . Tuid ( 0 )              ;
  SqlLoopErr ( SC , Q )                   ;
  SqlLoopEnd ( SC , Q )                   ;
  if ( ( TUIDs.count() <= 0     )        ||
       ( TUIDs.count() != total )       ) {
    Action ( section , 3 )                ;
    int  index = 0                        ;
    bool go    = true                     ;
    Start      ( section , index , go   ) ;
    setRange   ( section , 0     , 1    ) ;
    Complete   ( section                ) ;
    return false                          ;
  }                                       ;
  /////////////////////////////////////////
  Action ( section , 4 )                  ;
  int  index = 0                          ;
  bool go    = true                       ;
  TUID s     = StartId                    ;
  TUID v                                  ;
  Start    ( section , index , go       ) ;
  setRange ( section , 0     , total    ) ;
  for (index=0;go && index<total;index++) {
    v = TUIDs [ index ]                   ;
    if ( s != v )                         {
      Q = QString ( "update `%1` set `%2` = %3 where `%4` = %5 ;")
          . arg   ( table                 )
          . arg   ( column                )
          . arg   ( s                     )
          . arg   ( column                )
          . arg   ( v                   ) ;
      SC  . Query ( Q                   ) ;
    }                                     ;
    s++                                   ;
  }                                       ;
  /////////////////////////////////////////
  if ( go )                               {
    Action ( section , 5 )                ;
    Q = QString("alter table `%1` auto_increment = %2 ;").arg(table).arg(s) ;
    SC . Query ( Q )                      ;
  }                                       ;
  Complete ( section                    ) ;
  /////////////////////////////////////////
  return true                             ;
}

void N::SqlOrganizer::ExtractHtml(SqlConnection & SC,int section)
{
  DocumentManager DM ( plan )                                   ;
  MimeManager     MM ( plan )                                   ;
  NetworkManager  NM ( plan )                                   ;
  ///////////////////////////////////////////////////////////////
  QString    Q                                                  ;
  QByteArray H                                                  ;
  int        T                                                  ;
  UUIDs      Uuids                                              ;
  SUID       uuid                                               ;
  int        Value = 0                                          ;
  bool       Go    = true                                       ;
  Start  ( section , Value , Go )                               ;
  Action ( section , 0          )                               ;
  ///////////////////////////////////////////////////////////////
  MM . MimeUuids                                                (
    SC                                                          ,
    "text/html"                                                 ,
    Uuids                                                       ,
    Qt::DescendingOrder                                       ) ;
  ///////////////////////////////////////////////////////////////
  if (Uuids.count()>0)                                          {
    UUIDs       Iuids                                           ;
    UUIDs       Xuids                                           ;
    SC.Uuids                                                    (
      Iuids                                                     ,
      PlanTable(ImportBodies)                                   ,
      "uuid"                                                    ,
      SC.OrderByDesc("id")                                    ) ;
    Xuids = Uuids                                               ;
    Uuids = Xuids & Iuids                                       ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  T = Uuids . count ( )                                         ;
  if ( T <= 0 ) return                                          ;
  Action   ( section , 1     )                                  ;
  setRange ( section , 0 , T )                                  ;
  for (int i=0;Go && i<T;i++)                                   {
    Value = i + 1                                               ;
    uuid  = Uuids [ i ]                                         ;
    Q = SC.sql.SelectFrom                                       (
          "length"                                              ,
          PlanTable(WebContents)                                ,
          SC.WhereUuid(uuid)                                  ) ;
    if (SC.Fetch(Q))                                            {
      int  length  = SC.Int(1)                                  ;
      int  retcode = NM . HttpStatus ( SC , uuid )              ;
      int  content = 0                                          ;
      int  otype   = Types::URL                                 ;
      bool correct = true                                       ;
      Q = SC.sql.SelectFrom                                     (
            "length"                                            ,
            PlanTable(ImportBodies)                             ,
            SC.WhereUuid(uuid)                                ) ;
      if (SC.Fetch(Q)) content = SC.Int(0)                      ;
      Q = SC.sql.SelectFrom                                     (
            "type"                                              ,
            PlanTable(MajorUuid)                                ,
            SC.WhereUuid(uuid)                                ) ;
      if (SC.Fetch(Q)) otype = SC.Int(0)                        ;
      if (retcode!=200   ) correct = false                      ;
      if (length <=  0   ) correct = false                      ;
      if (length!=content) correct = false                      ;
      if (correct)                                              {
        H . clear ( )                                           ;
        NM.fetchBody(SC,uuid,H)                                 ;
        if (H.size()>0)                                         {
          DM.UpdateXml  ( SC , uuid , otype , H )               ;
          NM.deleteBody ( SC , uuid             )               ;
        }                                                       ;
      } else
      if (retcode==200 && content>0)                            {
        H  . clear     (               )                        ;
        NM . fetchBody ( SC , uuid , H )                        ;
        if (H.size()>0)                                         {
          DM . UpdateXml  ( SC , uuid , otype , H )             ;
          NM . deleteBody ( SC , uuid             )             ;
        }                                                       ;
      }                                                         ;
    }                                                           ;
  }                                                             ;
  Complete ( section )                                          ;
}
