#include <qtfoundation.h>

#define plan    MimePlan
#define TYPE(T) Types::T

N::MimeManager:: MimeManager (PurePlan * p)
               : MimePlan    (           p)
{
}

N::MimeManager::~MimeManager(void)
{
}

int N::MimeManager::count(void)
{
  return MimeTypes . count ( ) ;
}

bool N::MimeManager::contains(QString mime)
{
  for (int i=0;i<count();i++)           {
    if (MimeTypes[i]==mime) return true ;
  }                                     ;
  return false                          ;
}

bool N::MimeManager::contains(int MimeId)
{
  for (int i=0;i<count();i++)                {
    if (MimeTypes[i].id==MimeId) return true ;
  }                                          ;
  return false                               ;
}

QStringList N::MimeManager::supports(void)
{
  QStringList s               ;
  for (int i=0;i<count();i++) {
    s << MimeTypes[i].mime    ;
  }                           ;
  return      s               ;
}

QStringList N::MimeManager::supports(QString catalog)
{
  QStringList s                      ;
  QString     l = catalog.toLower()  ;
  for (int i=0;i<count();i++)        {
    QString c = MimeTypes[i].catalog ;
    if (l==c.toLower())              {
      s << MimeTypes[i].mime         ;
    }                                ;
  }                                  ;
  return      s                      ;
}

N::MimeType & N::MimeManager::operator [] (QString mime)
{
  for (int i=0;i<count();i++) {
    if (MimeTypes[i]==mime)   {
      return MimeTypes[i]     ;
    }                         ;
  }                           ;
  return Default              ;
}

N::MimeType & N::MimeManager::operator [] (int MimeId)
{
  for (int i=0;i<count();i++)    {
    if (MimeTypes[i].id==MimeId) {
      return MimeTypes[i]        ;
    }                            ;
  }                              ;
  return Default                 ;
}

bool N::MimeManager::at(QString mime,MimeType & mt)
{
  for (int i=0;i<count();i++) {
    if (MimeTypes[i]==mime)   {
      mt = MimeTypes[i]       ;
      return true             ;
    }                         ;
  }                           ;
  return false                ;
}

bool N::MimeManager::at(int MimeId,MimeType & mt)
{
  for (int i=0;i<count();i++)    {
    if (MimeTypes[i].id==MimeId) {
      mt = MimeTypes[i]          ;
      return true                ;
    }                            ;
  }                              ;
  return false                   ;
}

bool N::MimeManager::load(SqlConnection & Connection)
{
  QString Q                                              ;
  ////////////////////////////////////////////////////////
  MimeTypes . clear()                                    ;
  Q = Connection.sql.SelectFrom                          (
        Connection.Columns(3,"mimeid","uuid","name")     ,
        PlanTable(MimeTypes)                                ,
        Connection.OrderByAsc("mimeid")                ) ;
  SqlLoopNow ( Connection , Q   )                        ;
    MimeType M                                           ;
    int      MimeId                                      ;
    SUID     uuid                                        ;
    QString  Mime                                        ;
    MimeId = Connection.Int   (0)                        ;
    uuid   = Connection.Uuid  (1)                        ;
    Mime   = Connection.String(2)                        ;
    M.id   = MimeId                                      ;
    M.uuid = uuid                                        ;
    M      = Mime                                        ;
    MimeTypes << M                                       ;
  SqlLoopErr ( Connection , Q   )                        ;
  SqlLoopEnd ( Connection , Q   )                        ;
  ////////////////////////////////////////////////////////
  for (int i=0;i<count();i++)                            {
    QString S                                            ;
    S = QString ( "where mimeid = %1 %2"                 )
        . arg   ( MimeTypes[i].id                        )
        . arg   ( Connection.OrderByAsc("extensionid") ) ;
    Q = Connection.sql.SelectFrom                        (
          "name"                                         ,
          PlanTable(Extensions)                             ,
          S                     )                        ;
    SqlLoopNow ( Connection , Q )                        ;
      QString N                                          ;
      N = Connection.String(0)                           ;
      MimeTypes[i] << N                                  ;
    SqlLoopErr ( Connection , Q )                        ;
    SqlLoopEnd ( Connection , Q )                        ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  Q = Connection.sql.SelectFrom                          (
        "extensionid,name"                               ,
        PlanTable(Extensions)                               ,
        Connection.OrderByAsc("extensionid")         )   ;
  SqlLoopNow ( Connection , Q )                          ;
    int     eid                                          ;
    QString name                                         ;
    eid  = Connection . Int    (0)                       ;
    name = Connection . String (1)                       ;
    Extensions    << eid                                 ;
    Suffixes[eid]  = name                                ;
  SqlLoopErr ( Connection , Q )                          ;
  SqlLoopEnd ( Connection , Q )                          ;
  return true                                            ;
}

N::MimeType N::MimeManager  :: append     (
               SqlConnection & Connection ,
               QString         mime       )
{
  QString  Q                                          ;
  MimeType M                                          ;
  /////////////////////////////////////////////////////
  Q = Connection.sql.SelectFrom                       (
        "mimeid"                                      ,
        PlanTable(MimeTypes)                             ,
        QString ( "%1 %2"                             )
        . arg ( Connection.OrderByDesc("mimeid")      )
        . arg ( Connection.sql.Limit(0,1)         ) ) ;
  if (!Connection.Fetch(Q)) return M                  ;
  /////////////////////////////////////////////////////
  M.id   = Connection.Int(0)                          ;
  M.id++                                              ;
  M.uuid = Connection.Unique(PlanTable(MajorUuid),"uuid");
  M      = mime                                       ;
  Connection.assureUuid(PlanTable(MajorUuid)             ,
    M.uuid  , TYPE(MimeTypes)                       ) ;
  /////////////////////////////////////////////////////
  Q = Connection.sql.InsertInto                       (
        PlanTable(MimeTypes)                             ,
        5                                             ,
        "mimeid"                                      ,
        "uuid"                                        ,
        "name"                                        ,
        "catalog"                                     ,
        "mimename"                                  ) ;
  Connection.Prepare(Q)                               ;
  Connection.Bind ("mimeid"  ,M.id                  ) ;
  Connection.Bind ("uuid"    ,M.uuid                ) ;
  Connection.Bind ("name"    ,M.mime   .toUtf8()    ) ;
  Connection.Bind ("catalog" ,M.catalog.toUtf8()    ) ;
  Connection.Bind ("mimename",M.name   .toUtf8()    ) ;
  Connection.Exec ()                                  ;
  return M                                            ;
}

QStringList N::MimeManager :: Launcher   (
              SqlConnection & Connection ,
              int             extension  )
{
  QStringList App                                        ;
  QString     Q                                          ;
  Q = Connection.sql.SelectFrom                          (
        "name,paraments"                                 ,
        PlanTable(Applications)                             ,
        QString("where extension = %1").arg(extension) ) ;
  if (Connection.Fetch(Q))                               {
    App << Connection.String(0)                          ;
    App << Connection.String(1)                          ;
  }                                                      ;
  return App                                             ;
}

QString N::MimeManager :: documentMime (
          SqlConnection & Connection   ,
          SUID            uuid         ,
          int             type         ,
          TUID            publish      )
{
  QString mime   = ""                       ;
  QString Q                                 ;
  Q = Connection.sql.SelectFrom             (
        "mimetype"                          ,
        PlanTable(DocumentMimes)               ,
        QString("where uuid = %1 "
                "and type = %2 "
                "and publish = %3"          )
        .arg(uuid).arg(type).arg(publish) ) ;
  if ( Connection . Fetch ( Q ) )           {
    mime = Connection . String ( 0 )        ;
  }                                         ;
  return  mime                              ;
}

bool N::MimeManager :: documentMime (
       SqlConnection & Connection   ,
       SUID            uuid         ,
       int             type         ,
       TUID            publish      ,
       MimeType      & mime         )
{
  QString Q                                 ;
  Q = Connection.sql.SelectFrom             (
        "mimeid"                            ,
        PlanTable(DocumentMimes)               ,
        QString("where uuid = %1 "
                "and type = %2 "
                "and publish = %3"          )
        .arg(uuid).arg(type).arg(publish) ) ;
  if ( Connection . Fetch ( Q ) )           {
    int mimeid = Connection . Int ( 0 )     ;
    if (at(mimeid,mime)) return true        ;
  }                                         ;
  return false                              ;
}

bool N::MimeManager :: assureDocumentMime (
       SqlConnection & Connection         ,
       SUID            uuid               ,
       int             type               ,
       TUID            publish            ,
       QString         mime               )
{
  nKickOut ( !contains(mime) , false )                                ;
  QString  Q                                                          ;
  MimeType M                                                          ;
  nKickOut ( !at(mime,M)     , false )                                ;
  /////////////////////////////////////////////////////////////////////
  bool exists = false                                                 ;
  Q = Connection.sql.SelectFrom                                       (
        "id"                                                          ,
        PlanTable(DocumentMimes)                                         ,
        QString ( "where uuid = %1 and publish = %2"                  )
        .arg(uuid).arg(publish)                                     ) ;
  if (Connection.Fetch(Q)) exists = true                              ;
  /////////////////////////////////////////////////////////////////////
  if (exists)                                                         {
    Q = Connection.sql.Update                                         (
          PlanTable(DocumentMimes)                                       ,
          Connection.sql.Where(2,"uuid","publish")                    ,
          5                                                           ,
          "type"                                                      ,
          "mimeid"                                                    ,
          "mimetype"                                                  ,
          "catalog"                                                   ,
          "mimename"                                                ) ;
  } else                                                              {
    Q = Connection.sql.InsertInto                                     (
          PlanTable(DocumentMimes)                                       ,
          7                                                           ,
          "uuid"                                                      ,
          "type"                                                      ,
          "publish"                                                   ,
          "mimeid"                                                    ,
          "mimetype"                                                  ,
          "catalog"                                                   ,
          "mimename"                                                ) ;
  }                                                                   ;
  /////////////////////////////////////////////////////////////////////
  Connection        . Prepare ( Q                                   ) ;
  Connection        . Bind    ( "uuid"     , uuid                   ) ;
  Connection        . Bind    ( "type"     , (SUID)type             ) ;
  Connection        . Bind    ( "publish"  , publish                ) ;
  Connection        . Bind    ( "mimeid"   , M . id                 ) ;
  Connection        . Bind    ( "mimetype" , M . mime    . toUtf8() ) ;
  Connection        . Bind    ( "catalog"  , M . catalog . toUtf8() ) ;
  Connection        . Bind    ( "mimename" , M . name    . toUtf8() ) ;
  return Connection . Exec    (                                     ) ;
}

bool N::MimeManager    :: MimeUuids  (
       SqlConnection    & Connection ,
       QString            mimetype   ,
       UUIDs            & Uuids      ,
       enum Qt::SortOrder Sorting    )
{
  QString Q                                         ;
  Q = Connection.sql.SelectFrom                     (
        "uuid"                                      ,
        PlanTable(DocumentMimes)                       ,
        QString ( "where mimetype = '%1' %2"        )
       . arg ( mimetype                             )
       . arg ( Connection.OrderBy("id",Sorting) ) ) ;
  SqlLoopNow ( Connection , Q )                     ;
    Uuids << Connection . Uuid ( 0 )                ;
  SqlLoopErr ( Connection , Q )                     ;
  SqlLoopEnd ( Connection , Q )                     ;
  return ( Uuids . count ( ) > 0 )                  ;
}

bool N::MimeManager    :: MimeLikeUuids (
       SqlConnection    & Connection    ,
       QString            mimetype      ,
       UUIDs            & Uuids         ,
       enum Qt::SortOrder Sorting       )
{
  QString Q                                         ;
  Q = Connection.sql.SelectFrom                     (
        "uuid"                                      ,
        PlanTable(DocumentMimes)                       ,
        QString ( "where mimetype like '%1' %2"     )
       . arg ( mimetype                             )
       . arg ( Connection.OrderBy("id",Sorting) ) ) ;
  SqlLoopNow ( Connection , Q )                     ;
    Uuids << Connection . Uuid ( 0 )                ;
  SqlLoopErr ( Connection , Q )                     ;
  SqlLoopEnd ( Connection , Q )                     ;
  return ( Uuids . count ( ) > 0 )                  ;
}
