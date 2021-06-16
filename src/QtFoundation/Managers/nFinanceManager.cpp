#include <qtfoundation.h>

#define plan FinancePlan

N::FinanceManager:: FinanceManager ( PurePlan * p )
                  : FinancePlan    (            p )
{
}

N::FinanceManager::~FinanceManager(void)
{
}

SUID N::FinanceManager::FindIdentifier(SqlConnection & SC,QString name)
{
  QString Q                             ;
  SUID    uuid  = 0                     ;
  Q = SC.sql.SelectFrom                 (
        "uuid"                          ,
        PlanTable(Identifiers)             ,
        SC.sql.Where(1,"name")        ) ;
  SC . Prepare ( Q                    ) ;
  SC . Bind    ( "name",name.toUtf8() ) ;
  IfSqlExec(SC) uuid = SC.Uuid(0)       ;
  return uuid                           ;
}

bool N::FinanceManager::assureIdentifier(SqlConnection & SC,SUID uuid,QString name)
{
  QString Q                                  ;
  int     id = -1                            ;
  Q = SC.sql.SelectFrom                      (
        "id" , PlanTable(Identifiers)           ,
        SC.sql.Where(2,"uuid","name")   )    ;
  SC . Prepare ( Q                      )    ;
  SC . Bind    ( "uuid" , uuid          )    ;
  SC . Bind    ( "name" , name.toUtf8() )    ;
  IfSqlExec(SC) id = SC.Int(0)               ;
  if (id<0)                                  {
    Q = SC.sql.InsertInto                    (
          PlanTable(Identifiers)                ,
          2,"uuid","name"                  ) ;
    SC . Prepare ( Q                       ) ;
    SC . Bind    ( "uuid" , uuid           ) ;
    SC . Bind    ( "name" , name.toUtf8()  ) ;
    SC . Exec    (                         ) ;
  } else                                     {
    int nid = 1                              ;
    Q = SC.sql.SelectFrom                    (
          "id"                               ,
          PlanTable(Identifiers)                ,
          "order by id desc limit 0,1"     ) ;
    if (SC.Fetch(Q)) nid = SC.Int(0)         ;
    nid ++                                   ;
    Q = SC.sql.Update                        (
          PlanTable(Identifiers)                ,
          QString("where id = %1").arg(id)   ,
          1,"id"                           ) ;
    SC . Prepare ( Q                       ) ;
    SC . Bind    ( "id" , nid              ) ;
    SC . Exec    (                         ) ;
  }                                          ;
  return true                                ;
}

QString N::FinanceManager::FetchIdentifier(SqlConnection & SC,SUID uuid)
{
  QString Q                            ;
  QString name = ""                    ;
  Q = SC.sql.SelectFrom                (
        "name"                         ,
        PlanTable(Identifiers)            ,
        QString("where uuid = %1 "
          "order by id desc limit 0,1" )
          .arg(uuid)                 ) ;
  if (SC.Fetch(Q)) name = SC.String(0) ;
  return name                          ;
}

SUID N::FinanceManager::appendCommodity(SqlConnection & SC)
{
  SUID uuid                                                ;
  uuid = SC.Unique(PlanTable(MajorUuid),"uuid"              ) ;
  SC.assureUuid(PlanTable(MajorUuid  ),uuid,Types::Commodity) ;
  SC.insertUuid(PlanTable(Commodities),uuid,"uuid"          ) ;
  return uuid                                              ;
}

SUID N::FinanceManager:: appendNamemap (
        SqlConnection  & SC            ,
        SUID             uuid          ,
        int              language      ,
        QString          name          )
{
  QString Q                                                        ;
  int     prefer = -1                                              ;
  SUID    nuid   = SC.Unique(PlanTable(MajorUuid),"uuid")             ;
  SC.assureUuid(PlanTable(MajorUuid),nuid,Types::Name)                ;
  SC.assureName(PlanTable(Names),nuid,language,name)                  ;
  Q = SC.sql.SelectFrom("prefer",PlanTable(NameMaps)                  ,
           QString("where uuid = %1 order by prefer desc limit 0,1")
           .arg(uuid)                                            ) ;
  if (SC.Fetch(Q)) prefer = SC.Int(0)                              ;
  prefer ++                                                        ;
  Q = SC.sql.InsertInto(PlanTable(NameMaps),3,"uuid","name","prefer") ;
  SC . Prepare ( Q )                                               ;
  SC . Bind    ( "uuid"   , uuid   )                               ;
  SC . Bind    ( "name"   , nuid   )                               ;
  SC . Bind    ( "prefer" , prefer )                               ;
  SC . Exec    (   )                                               ;
  return nuid                                                      ;
}
