#include <qtfoundation.h>

N::ParamentQuery:: ParamentQuery ( int type,int variety,QString scope )
                 : Type          (     type                           )
                 , Variety       (              variety               )
                 , Scope         (                              scope )
{
}

N::ParamentQuery:: ParamentQuery ( void                   )
                 : Type          ( Dynamic::Functionality )
                 , Variety       ( 1                      )
                 , Scope         ( ""                     )
{
}

N::ParamentQuery:: ParamentQuery  (const ParamentQuery & query)
{
  ME = query ;
}

N::ParamentQuery::~ParamentQuery(void)
{
}

N::ParamentQuery & N::ParamentQuery::operator = (const ParamentQuery & query)
{
  nMemberCopy ( query , Type    ) ;
  nMemberCopy ( query , Variety ) ;
  nMemberCopy ( query , Scope   ) ;
  return ME                       ;
}

TUID N::ParamentQuery::Value(SqlConnection & SC,QString table,QString name)
{
  QString Q                                     ;
  TUID    value = 0                             ;
  Q = SC.sql.SelectFrom                         (
        "value" , table                         ,
        SC.sql.Where                            (
          4 , "type"  , "variety"               ,
              "scope" , "name"              ) ) ;
  SC . Prepare ( Q                            ) ;
  SC . Bind    ( "type"    , Type             ) ;
  SC . Bind    ( "variety" , Variety          ) ;
  SC . Bind    ( "scope"   , Scope . toUtf8() ) ;
  SC . Bind    ( "name"    , name  . toUtf8() ) ;
  IfSqlExec(SC)                                 {
    value = SC.Tuid(0)                          ;
  }                                             ;
  return value                                  ;
}

double N::ParamentQuery::Floating(SqlConnection & SC,QString table,QString name)
{
  QString Q                                     ;
  double  floating = 0                          ;
  Q = SC.sql.SelectFrom                         (
        "floating" , table                      ,
        SC.sql.Where                            (
          4 , "type"  , "variety"               ,
              "scope" , "name"              ) ) ;
  SC . Prepare ( Q                            ) ;
  SC . Bind    ( "type"    , Type             ) ;
  SC . Bind    ( "variety" , Variety          ) ;
  SC . Bind    ( "scope"   , Scope . toUtf8() ) ;
  SC . Bind    ( "name"    , name  . toUtf8() ) ;
  IfSqlExec(SC)                                 {
    floating = SC.Value(0).toDouble()           ;
  }                                             ;
  return floating                               ;
}

QVariant N::ParamentQuery::Data(SqlConnection & SC,QString table,QString name)
{
  QString  Q                                    ;
  QVariant data                                 ;
  Q = SC.sql.SelectFrom                         (
        "data" , table                          ,
        SC.sql.Where                            (
          4 , "type"  , "variety"               ,
              "scope" , "name"              ) ) ;
  SC . Prepare ( Q                            ) ;
  SC . Bind    ( "type"    , Type             ) ;
  SC . Bind    ( "variety" , Variety          ) ;
  SC . Bind    ( "scope"   , Scope . toUtf8() ) ;
  SC . Bind    ( "name"    , name  . toUtf8() ) ;
  IfSqlExec(SC)                                 {
    data = SC.Value(0)                          ;
  }                                             ;
  return data                                   ;
}

QString N::ParamentQuery::String(SqlConnection & SC,QString table,QString name)
{
  QVariant   data = Data(SC,table,name) ;
  QByteArray bata = data.toByteArray()  ;
  return QString::fromUtf8(bata)        ;
}

TUID N::ParamentQuery::Value(SqlConnection & SC,QString table,SUID uuid)
{
  QString Q                  ;
  TUID    value = 0          ;
  Q = SC.sql.SelectFrom      (
        "value"              ,
        table                ,
        SC.WhereUuid(uuid) ) ;
  if (SC.Fetch(Q))           {
    value = SC.Tuid(0)       ;
  }                          ;
  return  value              ;
}

double N::ParamentQuery::Floating(SqlConnection & SC,QString table,SUID uuid)
{
  QString Q                           ;
  double  floating = 0                ;
  Q = SC.sql.SelectFrom               (
        "floating"                    ,
        table                         ,
        SC.WhereUuid(uuid) )          ;
  if (SC.Fetch(Q))                    {
    floating = SC.Value(0).toDouble() ;
  }                                   ;
  return floating                     ;
}

QVariant N::ParamentQuery::Data(SqlConnection & SC,QString table,SUID uuid)
{
  QString  Q                 ;
  QVariant data              ;
  Q = SC.sql.SelectFrom      (
        "data"               ,
        table                ,
        SC.WhereUuid(uuid) ) ;
  if (SC.Fetch(Q))           {
    data = SC.Value(0)       ;
  }                          ;
  return   data              ;
}

QString N::ParamentQuery::String(SqlConnection & SC,QString table,SUID uuid)
{
  QVariant   data = Data(SC,table,uuid) ;
  QByteArray bata = data.toByteArray()  ;
  return QString::fromUtf8(bata)        ;
}
