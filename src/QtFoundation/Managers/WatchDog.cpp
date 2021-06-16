#include <qtfoundation.h>

N::WatchDog:: WatchDog ( QObject * parent )
            : QObject  (           parent )
            , Thread   ( 0 , false        )
            , plan     ( NULL             )
            , working  ( false            )
            , clock    ( false            )
            , exact    ( true             )
            , half     ( true             )
{
  WidgetClass                                             ;
  lastTime = nTimeNow                                     ;
  nConnect ( this , SIGNAL ( Kick          (        ) )   ,
             this , SLOT   ( Gotcha        (        ) ) ) ;
  nConnect ( this , SIGNAL ( LocalNotify   (SUID,int) )   ,
             this , SLOT   ( PrivateNotify (SUID,int) ) ) ;
}

N::WatchDog::~WatchDog (void)
{
}

void N::WatchDog::load(void)
{
  if (IsNull(plan)) return ;
}

void N::WatchDog::Reload(void)
{
  if ( ! Mutex . tryLock ( 100 ) ) return   ;
  events . clear ( )                        ;
  EventManager  EM ( plan        )          ;
  SqlConnection SC ( plan -> sql )          ;
  if ( SC . open ( FunctionString ) )       {
    UUIDs Uuids                             ;
    Uuids  = EM . Scheduling ( SC         ) ;
    events = EM . Monitoring ( SC , Uuids ) ;
    SC  . close  ( )                        ;
  }                                         ;
  SC    . remove ( )                        ;
  Mutex . unlock ( )                        ;
  lastTime = QDateTime::currentDateTime ( ) ;
}

void N::WatchDog::AcceptClock(bool Exact,bool Half)
{
  exact = Exact ;
  half  = Half  ;
}

void N::WatchDog::reportTime(QDateTime d)
{
  QStringList H;
  QString R;
  H << tr("0");
  H << tr("1");
  H << tr("2");
  H << tr("3");
  H << tr("4");
  H << tr("5");
  H << tr("6");
  H << tr("7");
  H << tr("8");
  H << tr("9");
  H << tr("10");
  H << tr("11");
  H << tr("12");
  H << tr("13");
  H << tr("14");
  H << tr("15");
  H << tr("16");
  H << tr("17");
  H << tr("18");
  H << tr("19");
  H << tr("20");
  H << tr("21");
  H << tr("22");
  H << tr("23");
  R = H[d.time().hour()];
  if (d.time().minute()==0) {
    R.append(" ");
    R.append(tr("hour exact"));
  } else
  if (d.time().minute()==30) {
    R.append(" ");
    R.append(tr("hour 30 minutes"));
  } else {
    R.append(" ");
    R.append(tr("hour"));
    R.append(" ");
    R.append(QString::number(d.time().minute()));
    R.append(" ");
    R.append(tr("minute"));
  };
  if (R.length()>0) plan->Talk(R);
}

bool N::WatchDog::WatchClock(bool reported)
{
  if (!clock      ) return reported          ;
  if (IsNull(plan)) return reported          ;
  QDateTime d = QDateTime::currentDateTime() ;
  if (d.time().second()==0)                  {
    if ((exact && d.time().minute()== 0)    ||
        (half  && d.time().minute()==30))    {
      if (!reported)                         {
        reportTime(d)                        ;
        reported = true                      ;
      }                                      ;
    }                                        ;
  } else reported = false                    ;
  return reported                            ;
}

void N::WatchDog::PrivateNotify(SUID uuid,int eType)
{
  emit Notify (uuid,eType) ;
}

void N::WatchDog::Gotcha(void)
{
  if ( ! Mutex . tryLock ( 100 ) ) return                  ;
  SqlConnection     SC ( plan -> sql )                     ;
  FoundationManager FM ( this , plan )                     ;
  if ( SC . open ( FunctionString ) )                      {
    while ( current . count ( ) > 0 )                      {
      QString Q                                            ;
      SUID uuid = current . first ( )                      ;
      int  type = 0                                        ;
      int  mode = N::History::TimeSpot                     ;
      current . takeFirst ( )                              ;
      if ( FM . isOwner ( SC , uuid ) )                    {
        Q = SC.sql.SelectFrom                              (
          "type,mode"                                      ,
          PlanTable(Scheduling)                            ,
          SC.WhereUuid(uuid)                       )       ;
        if ( SC . Fetch ( Q ) )                            {
          type = SC . Int ( 0 )                            ;
          mode = SC . Int ( 1 )                            ;
          switch ( mode )                                  {
            case History::WorkingPeriod                    :
            case History::ActionStart                      : // SetScheduleMode
              Q = SC . sql . Update                        (
                    PlanTable(Scheduling)                  ,
                    SC . sql . Where ( 1 , "uuid" )        ,
                    1                                      ,
                    "mode"                               ) ;
              SC . Prepare ( Q                           ) ;
              SC . Bind    ( "uuid" , uuid               ) ;
              SC . Bind    ( "mode" , History::ActionEnd ) ;
              SC . Exec    (                             ) ;
              emit LocalNotify ( uuid , type )             ;
            break                                          ;
            case History::ActionEnd                        : // Delete Scheduling
              Q = SC . sql . DeleteFrom                    (
                    PlanTable(Scheduling)                  ,
                    SC . WhereUuid ( uuid )              ) ;
              SC . Query ( Q )                             ;
              emit LocalNotify ( uuid , type )             ;
            break                                          ;
          }                                                ;
        }                                                  ;
      }                                                    ;
    }                                                      ;
    SC  . close  ( )                                       ;
  }                                                        ;
  SC    . remove ( )                                       ;
  Mutex . unlock ( )                                       ;
}

void N::WatchDog::IsEvents(void)
{
  if (events.count()<=0) return              ;
  QDateTime C = QDateTime::currentDateTime() ;
  StarDate  S                                ;
  int       i = 0                            ;
  S = C                                      ;
  while (i<events.count())                   {
    TMAP T = events[i]                       ;
    if (T.first<S.Stardate)                  {
      SUID uuid = T.second                   ;
      events . takeAt ( i )                  ;
      current << uuid                        ;
    } else                                   {
      i++                                    ;
    }                                        ;
  }                                          ;
  if (current.count()>0) emit Kick ( )       ;
}

void N::WatchDog::run(void)
{
  bool reported = false                     ;
  QDateTime d                               ;
  working = true                            ;
  Reload ( )                                ;
  while ( working && plan -> canContinue )  {
    reported = WatchClock(reported)         ;
    d        = QDateTime::currentDateTime() ;
    IsEvents()                              ;
    if (lastTime.secsTo(d)>=(30))           {
      Reload ( )                            ;
    }                                       ;
    Time::msleep ( 500 )                    ;
  }                                         ;
}
