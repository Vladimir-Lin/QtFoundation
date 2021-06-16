#include <qtfoundation.h>

#define TABLES(ID) plan.Tables[Tables::ID]
#define SQLX       SC.sql

bool N :: Manager :: Uuids         (
       PurePlan            & plan  ,
       SqlConnection       & SC    ,
       UUIDs               & U     ,
       N::Types::ObjectTypes type  ,
       Qt::SortOrder         order )
{
  QString T                        ;
  QString O                        ;
  QString C = "uuid"               ;
  //////////////////////////////////
  switch (order)                   {
    case Qt::AscendingOrder        :
    case Qt::DescendingOrder       :
      O = SC.OrderBy("id",order)   ;
    break                          ;
    default                        :
    return false                   ;
  }                                ;
  //////////////////////////////////
  switch (type)                    {
    case N::Types::Type            :
      T = TABLES ( Types         ) ;
      C = "type"                   ;
    break                          ;
    case N::Types::Language        :
      T = TABLES ( Languages     ) ;
    break                          ;
    case N::Types::Action          :
      T = TABLES ( ActionTypes   ) ;
    break                          ;
    case N::Types::HistoryType     :
      T = TABLES ( HistoryTypes  ) ;
    break                          ;
    case N::Types::History         :
      T = TABLES ( History       ) ;
    break                          ;
    case N::Types::People          :
      T = TABLES ( PeopleUuid    ) ;
    break                          ;
    case N::Types::Picture         :
      T = TABLES ( PictureOrders ) ;
    break                          ;
    case N::Types::Video           :
      T = TABLES ( Videos        ) ;
    break                          ;
    case N::Types::Audio           :
      T = TABLES ( Audios        ) ;
    break                          ;
    case N::Types::SetMember       :
      T = TABLES ( SetMembers    ) ;
    break                          ;
    case N::Types::Culture         :
      T = TABLES ( Cultures      ) ;
    break                          ;
    case N::Types::Phoneme         :
      T = TABLES ( Phonemes      ) ;
    break                          ;
    case N::Types::Emotion         :
      T = TABLES ( Emotions      ) ;
    break                          ;
    default                        :
    return false                   ;
  }                                ;
  SC . Uuids ( U , T , C , O )     ;
  return true                      ;
}

bool N :: Manager :: NameUuids (
       PurePlan      & plan    ,
       SqlConnection & SC      ,
       SUID            uuid    ,
       UUIDs         & U       ,
       Qt::SortOrder   order   )
{
  QString T = TABLES ( NameMaps )             ;
  QString C = "name"                          ;
  QString O                                   ;
  QString W                                   ;
  QString Q                                   ;
  /////////////////////////////////////////////
  switch (order)                              {
    case Qt::AscendingOrder                   :
    case Qt::DescendingOrder                  :
      O = SC.OrderBy("prefer",order)          ;
    break                                     ;
    default                                   :
    return false                              ;
  }                                           ;
  /////////////////////////////////////////////
  W  = QString("where `uuid` = %1").arg(uuid) ;
  W += " "                                    ;
  W += O                                      ;
  /////////////////////////////////////////////
  Q  = SQLX . SelectFrom ( C , T , W )        ;
  SqlLoopNow ( SC , Q )                       ;
    U << SC . Uuid ( 0 )                      ;
  SqlLoopErr ( SC , Q )                       ;
  SqlLoopEnd ( SC , Q )                       ;
  /////////////////////////////////////////////
  return true                                 ;
}

bool N :: Manager :: UuidNames (
       PurePlan      & plan    ,
       SqlConnection & SC      ,
       UUIDs         & U       ,
       NAMEs         & N       )
{
  QString T = TABLES ( Names )          ;
  QString C = "name"                    ;
  QString L = SQLX.Limit(0,1)           ;
  QString O = SC . OrderByDesc ( "id" ) ;
  QString W                             ;
  QString Q                             ;
  SUID    u                             ;
  ///////////////////////////////////////
  O += " "                              ;
  O += L                                ;
  ///////////////////////////////////////
  foreach (u,U)                         {
    W  = SC . WhereUuid ( u )           ;
    W += " "                            ;
    W += O                              ;
    Q  = SQLX.SelectFrom(C,T,W)         ;
    if (SC.Fetch(Q))                    {
      N[u] = SC . String ( 0 )          ;
    }                                   ;
  }                                     ;
  ///////////////////////////////////////
  return true                           ;
}

bool N :: Manager :: UuidNames (
       PurePlan      & plan    ,
       SqlConnection & SC      ,
       SUID            uuid    ,
       NAMEs         & N       )
{
  UUIDs U                        ;
  NameUuids                      (
    plan                         ,
    SC                           ,
    uuid                         ,
    U                            ,
    Qt::AscendingOrder         ) ;
  if (U.count()<=0) return false ;
  ////////////////////////////////
  UuidNames                      (
    plan                         ,
    SC                           ,
    U                            ,
    N                          ) ;
  if (N.count()<=0) return false ;
  ////////////////////////////////
  return ( N . count ( ) > 0 )   ;
}

bool N :: Manager :: UuidNames (
       PurePlan      & plan    ,
       SqlConnection & SC      ,
       SUID            uuid    ,
       QStringList   & S       )
{
  UUIDs U                        ;
  NameUuids                      (
    plan                         ,
    SC                           ,
    uuid                         ,
    U                            ,
    Qt::AscendingOrder         ) ;
  if (U.count()<=0) return false ;
  ////////////////////////////////
  NAMEs N                        ;
  UuidNames                      (
    plan                         ,
    SC                           ,
    U                            ,
    N                          ) ;
  if (N.count()<=0) return false ;
  ////////////////////////////////
  SUID  u                        ;
  foreach (u,U) S << N [ u ]     ;
  ////////////////////////////////
  return ( S . count ( ) > 0 )   ;
}

bool N :: Manager :: FromNames (
       PurePlan      & plan    ,
       SqlConnection & SC      ,
       UUIDs         & U       ,
       QStringList   & S       )
{
  QString T = TABLES ( Names )                   ;
  QString Q                                      ;
  QString s                                      ;
  foreach ( s , S )                              {
    Q = SQLX . SelectFrom                        (
          "uuid"                                 ,
          T                                      ,
          SQLX . Where ( 1 , "name" )          ) ;
    SC . Prepare ( Q                           ) ;
    SC . Bind    ( "name" , s . toUtf8 ( )     ) ;
    if ( SC . Exec ( ) ) while ( SC . Next ( ) ) {
      SUID u = SC . Uuid ( 0 )                   ;
      if ( ! U . contains ( u ) ) U << u         ;
    }                                            ;
  }                                              ;
  return ( U . count ( ) > 0 )                   ;
}
