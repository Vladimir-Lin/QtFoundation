#include <qtfoundation.h>

#define plan      DocumentPlan
#define TYPE(T)   Types::T
#define RELATE(R) Groups::R

N::DocumentManager:: DocumentManager ( PurePlan * p )
                   : DocumentPlan    (            p )
{
}

N::DocumentManager::~DocumentManager (void)
{
}

QString N::DocumentManager :: Name       (
          SqlConnection     & Connection ,
          SUID                uuid       ,
          int                 language   )
{
  return Connection . getName (
           PlanTable(Names)   ,
           "uuid"             ,
           language           ,
           uuid             ) ;
}

bool N::DocumentManager :: assureName (
       SqlConnection     & Connection ,
       SUID                uuid       ,
       int                 language   ,
       QString             name       )
{
  return Connection . assureName (
           PlanTable(Names)      ,
           uuid                  ,
           language              ,
           name                ) ;
}

SUID N::DocumentManager::appendDocument(SqlConnection & SC)
{
  SUID uuid                                                           ;
  uuid = SC . Unique ( PlanTable(MajorUuid) , "uuid"                ) ;
  SC . assureUuid    ( PlanTable(MajorUuid) , uuid , TYPE(Document) ) ;
  SC . insertUuid    ( PlanTable(Documents) , uuid , "uuid"         ) ;
  return uuid                                                         ;
}

UUIDs N::DocumentManager :: Documents  (
        SqlConnection     & Connection ,
        Qt::SortOrder       Sorting    )
{
  return Connection.Uuids                     (
           PlanTable(Documents)               ,
           "uuid"                             ,
           Connection.OrderBy("id",Sorting) ) ;
}

UUIDs N::DocumentManager :: Documents  (
        SqlConnection     & Connection ,
        SUID                uuid       )
{
  return Connection . Uuids                               (
           PlanTable(Groups)                              ,
           "second"                                       ,
           QString ( "where t1 = %1 "
                     "and t2 = %2 "
                     "and relation = %3 "
                     "and first = %4 %5"                  )
           . arg ( TYPE   ( Division      )               )
           . arg ( TYPE   ( Document      )               )
           . arg ( RELATE ( Subordination )               )
           . arg ( uuid                                   )
           . arg ( Connection.OrderByDesc("position") ) ) ;
}

UUIDs N::DocumentManager :: Documents  (
        SqlConnection     & Connection ,
        SUID                uuid       ,
        int                 Type       ,
        Qt::SortOrder       Sorting    )
{
  QString Sort                                                         ;
  QString Options                                                      ;
  Sort    = Connection.OrderBy("position",Sorting)                     ;
  Options = QString ( "where t1 = %1 and t2 = %2 and relation = %3 "
                      "and first = %4 %5"                              )
          . arg ( Type                                                 )
          . arg ( TYPE   ( Document      )                             )
          . arg ( RELATE ( Subordination )                             )
          . arg ( uuid                                                 )
          . arg ( Sort                                               ) ;
  return Connection . Uuids ( PlanTable(Groups) , "second" , Options ) ;
}

int N::DocumentManager :: Rating     (
      SqlConnection     & Connection ,
      SUID                uuid       ,
      QString             name       )
{
  int     rating = -1                             ;
  QString Q                                       ;
  Q = Connection.sql.SelectFrom                   (
        "value"                                   ,
        PlanTable(Rating)                         ,
        "where uuid = :UUID and name = :NAME ;" ) ;
  Connection . Prepare ( Q                      ) ;
  Connection . Bind    ( "uuid" , uuid          ) ;
  Connection . Bind    ( "name" , name.toUtf8() ) ;
  IfSqlExec(Connection)                           {
    rating = (int)Connection.Tuid(0)              ;
  }                                               ;
  return rating                                   ;
}

bool N::DocumentManager :: setRating  (
       SqlConnection     & Connection ,
       SUID                uuid       ,
       QString             name       ,
       int                 rating     )
{
  QString Q                                         ;
  Q = Connection.sql.SelectFrom                     (
        "value"                                     ,
        PlanTable(Rating)                           ,
        "where uuid = :UUID and name = :NAME ;" )   ;
  Connection . Prepare ( Q                      )   ;
  Connection . Bind    ( "uuid" , uuid          )   ;
  Connection . Bind    ( "name" , name.toUtf8() )   ;
  IfSqlExec(Connection)                             {
    Q = Connection.sql.Update                       (
          PlanTable(Rating)                         ,
          "where uuid = :UUID and name = :NAME"     ,
          1,"value"                               ) ;
  } else                                            {
    Q = Connection.sql.InsertInto                   (
          PlanTable(Rating)                         ,
          3                                         ,
          "uuid"                                    ,
          "name"                                    ,
          "value"                                 ) ;
  }                                                 ;
  Connection . Prepare ( Q                        ) ;
  Connection . Bind    ( "uuid"  , uuid           ) ;
  Connection . Bind    ( "name"  , name.toUtf8()  ) ;
  Connection . Bind    ( "value" , rating         ) ;
  return Connection . Exec    (                   ) ;
}

SUID N::DocumentManager::acquirePlainText(SqlConnection & Connection)
{
  SUID uuid = Connection.Unique(PlanTable(MajorUuid),"uuid",4840)    ;
  Connection . assureUuid(PlanTable(MajorUuid),uuid,TYPE(PlainText)) ;
  Connection . insertUuid(PlanTable(TextDepot),uuid,"uuid"         ) ;
  Connection . insertUuid(PlanTable(PlainText),uuid,"uuid"         ) ;
  return uuid                                                        ;
}

QByteArray N::DocumentManager :: PlainText  (
             SqlConnection     & Connection ,
             SUID                uuid       )
{
  QString    Q                       ;
  QByteArray Body                    ;
  Q = Connection.sql.SelectFrom      (
        "text"                       ,
        PlanTable(TextDepot)         ,
        Connection.WhereUuid(uuid) ) ;
  if (Connection.Fetch(Q))           {
    Body = Connection.ByteArray(0)   ;
  }                                  ;
  return Body                        ;
}

bool N::DocumentManager :: PlainText  (
       SqlConnection     & Connection ,
       SUID                uuid       ,
       QByteArray        & Body       )
{
  QString    Q                       ;
  Q = Connection.sql.SelectFrom      (
        "text"                       ,
        PlanTable(TextDepot)         ,
        Connection.WhereUuid(uuid) ) ;
  if (Connection.Fetch(Q))           {
    Body = Connection.ByteArray(0)   ;
  } else return false                ;
  return true                        ;
}

int N::DocumentManager :: PlainTextSize (
      SqlConnection     & Connection    ,
      SUID                uuid          )
{
  int     s = 0                     ;
  QString Q                         ;
  Q = Connection.sql.SelectFrom     (
        "size"                      ,
        PlanTable(PlainText)        ,
        Connection.WhereUuid(uuid)) ;
  if (Connection.Fetch(Q))          {
    s = Connection.Int(0)           ;
  }                                 ;
  return s                          ;
}

QString N::DocumentManager :: PlainEncoding (
          SqlConnection     & Connection    ,
          SUID                uuid          )
{
  QString s = "UTF-8"                       ;
  QString Q                                 ;
  Q = Connection.sql.SelectFrom             (
        "encoding"                          ,
        PlanTable(PlainText)                ,
        Connection.WhereUuid(uuid))         ;
  if (Connection.Fetch(Q))                  {
    int e = Connection.Int(0)               ;
    Q = Connection.sql.SelectFrom           (
          "name" , PlanTable(Encodings)     ,
          QString("where id = %1").arg(e) ) ;
    if (Connection.Fetch(Q))                {
      s = Connection.String(0)              ;
    }                                       ;
  }                                         ;
  return s                                  ;
}

bool N::DocumentManager :: emptyPlainText (
       SqlConnection     & SC             ,
       SUID                uuid           )
{
  QString Q                           ;
  Q = SC.sql.Update                   (
        PlanTable(PlainText)          ,
        SC.sql.Where(1,"uuid")        ,
        3                             ,
        "encoding","size","checksum") ;
  SC . Prepare ( Q                  ) ;
  SC . Bind    ( "uuid"     , uuid  ) ;
  SC . Bind    ( "encoding" , 1     ) ;
  SC . Bind    ( "size"     , 0     ) ;
  SC . Bind    ( "checksum" , 0     ) ;
  return SC . Exec    (             ) ;
}

bool N::DocumentManager :: savePlainText (
       SqlConnection     & SC            ,
       SUID                uuid          ,
       QString           & text          )
{
  QString    Q                            ;
  QByteArray Body     = text.toUtf8(    ) ;
  int        length   = Body.size  (    ) ;
  int        checksum = nChecksum  (Body) ;
  bool       correct  = true              ;
  /////////////////////////////////////////
  SC . transaction ( )                    ;
  /////////////////////////////////////////
  Q = SC.sql.Update                       (
        PlanTable(TextDepot)              ,
        SC.sql.Where(1,"uuid")            ,
        2                                 ,
        "length"                          ,
        "text"                          ) ;
  SC . Prepare ( Q                      ) ;
  SC . Bind    ( "uuid"     , uuid      ) ;
  SC . Bind    ( "length"   , length    ) ;
  SC . Bind    ( "text"     , Body      ) ;
  if (!SC.Exec()) correct = false         ;
  /////////////////////////////////////////
  Q = SC.sql.Update                       (
        PlanTable(PlainText)              ,
        SC.sql.Where(1,"uuid")            ,
        3                                 ,
        "encoding"                        ,
        "size"                            ,
        "checksum"                      ) ;
  SC . Prepare ( Q                      ) ;
  SC . Bind    ( "uuid"     , uuid      ) ;
  SC . Bind    ( "encoding" , 1         ) ;
  SC . Bind    ( "size"     , length    ) ;
  SC . Bind    ( "checksum" , checksum  ) ;
  if (!SC.Exec()) correct = false         ;
  /////////////////////////////////////////
  SC . commit ( )                         ;
  /////////////////////////////////////////
  return correct                          ;
}

bool N::DocumentManager :: savePlainText (
       SqlConnection     & SC            ,
       SUID                uuid          ,
       QString             encoding      ,
       QByteArray        & Body          )
{
  QString Q                               ;
  int     length   = Body.size  (    )    ;
  int     checksum = nChecksum  (Body)    ;
  bool    correct  = true                 ;
  int     codec    = 1                    ;
  /////////////////////////////////////////
  Q = SC.sql.SelectFrom                   (
        "id",PlanTable(Encodings)         ,
        QString ( "where name = '%1'"     )
        .arg    ( encoding            ) ) ;
  if (SC.Fetch(Q)) codec = SC.Int(0)      ;
  /////////////////////////////////////////
  SC . transaction ( )                    ;
  /////////////////////////////////////////
  Q = SC.sql.Update                       (
        PlanTable(TextDepot)              ,
        SC.sql.Where(1,"uuid")            ,
        2                                 ,
        "length"                          ,
        "text"                          ) ;
  SC . Prepare ( Q                      ) ;
  SC . Bind    ( "uuid"     , uuid      ) ;
  SC . Bind    ( "length"   , length    ) ;
  SC . Bind    ( "text"     , Body      ) ;
  if (!SC.Exec()) correct = false         ;
  /////////////////////////////////////////
  Q = SC.sql.Update                       (
        PlanTable(PlainText)              ,
        SC.sql.Where(1,"uuid")            ,
        3                                 ,
        "encoding"                        ,
        "size"                            ,
        "checksum"                      ) ;
  SC . Prepare ( Q                      ) ;
  SC . Bind    ( "uuid"     , uuid      ) ;
  SC . Bind    ( "encoding" , codec     ) ;
  SC . Bind    ( "size"     , length    ) ;
  SC . Bind    ( "checksum" , checksum  ) ;
  if (!SC.Exec()) correct = false         ;
  /////////////////////////////////////////
  SC . commit ( )                         ;
  /////////////////////////////////////////
  return correct                          ;
}

bool N::DocumentManager :: savePlainText (
       SqlConnection     & SC            ,
       SUID                uuid          ,
       int                 encoding      ,
       QByteArray        & Body          )
{
  QString Q                               ;
  int        length   = Body.size  (    ) ;
  int        checksum = nChecksum  (Body) ;
  bool       correct  = true              ;
  int        codec    = encoding          ;
  /////////////////////////////////////////
  SC . transaction ( )                    ;
  /////////////////////////////////////////
  Q = SC.sql.Update                       (
        PlanTable(TextDepot)              ,
        SC.sql.Where(1,"uuid")            ,
        2                                 ,
        "length"                          ,
        "text"                          ) ;
  SC . Prepare ( Q                      ) ;
  SC . Bind    ( "uuid"     , uuid      ) ;
  SC . Bind    ( "length"   , length    ) ;
  SC . Bind    ( "text"     , Body      ) ;
  if (!SC.Exec()) correct = false         ;
  /////////////////////////////////////////
  Q = SC.sql.Update                       (
        PlanTable(PlainText)              ,
        SC.sql.Where(1,"uuid")            ,
        3                                 ,
        "encoding"                        ,
        "size"                            ,
        "checksum"                      ) ;
  SC . Prepare ( Q                      ) ;
  SC . Bind    ( "uuid"     , uuid      ) ;
  SC . Bind    ( "encoding" , codec     ) ;
  SC . Bind    ( "size"     , length    ) ;
  SC . Bind    ( "checksum" , checksum  ) ;
  if (!SC.Exec()) correct = false         ;
  /////////////////////////////////////////
  SC . commit ( )                         ;
  /////////////////////////////////////////
  return correct                          ;
}

SUID N::DocumentManager :: FindPlainText (
       SqlConnection     & Connection    ,
       QByteArray        & Body          )
{
  QString Q                           ;
  UUIDs   Uuids                       ;
  SUID    uuid     = 0                ;
  int     checksum = nChecksum(Body)  ;
  int     length   = Body.size()      ;
  /////////////////////////////////////
  Q = Connection.sql.SelectFrom       (
        "uuid"                        ,
        PlanTable(PlainText)          ,
        QString("where size = %1 "
                "and checksum = %2 "
                "order by id desc"    )
        .arg(length).arg(checksum)  ) ;
  SqlLoopNow(Connection,Q)            ;
    Uuids << Connection.Uuid(0)       ;
  SqlLoopErr(Connection,Q)            ;
  SqlLoopEnd(Connection,Q)            ;
  /////////////////////////////////////
  nKickOut ( Uuids.count() <= 0 , 0 ) ;
  /////////////////////////////////////
  foreach (uuid,Uuids)                {
    QByteArray BP                     ;
    BP . clear ( )                    ;
    Q = Connection.sql.SelectFrom     (
          "text"                      ,
          PlanTable(TextDepot)        ,
          Connection.WhereUuid(uuid)) ;
    if (Connection.Fetch(Q))          {
      BP = Connection.ByteArray(0)    ;
      if (BP==Body) return uuid       ;
    }                                 ;
  }                                   ;
  /////////////////////////////////////
  return 0                            ;
}

int N::DocumentManager :: JoinDocument (
      SqlConnection     & SC           ,
      SUID                document     ,
      SUID                fileuuid     ,
      int                 dtype        ,
      QString             name         )
{
  QString Q                                    ;
  int     prefer = 0                           ;
  //////////////////////////////////////////////
  Q = SC.sql.SelectFrom                        (
        "prefer"                               ,
         PlanTable(DocumentGroups)             ,
         QString("where document = %1 "
                 "order by prefer desc "
                 "limit 0,1").arg(document)  ) ;
  if (SC.Fetch(Q)) prefer = SC.Int(0)          ;
  prefer++                                     ;
  //////////////////////////////////////////////
  Q = SC.sql.InsertInto                        (
        PlanTable(DocumentGroups)              ,
        5                                      ,
        "document"                             ,
        "fileuuid"                             ,
        "extension"                            ,
        "prefer"                               ,
        "version"                            ) ;
  SC . Prepare ( Q                           ) ;
  SC . Bind    ( "document"  , document      ) ;
  SC . Bind    ( "fileuuid"  , fileuuid      ) ;
  SC . Bind    ( "extension" , dtype         ) ;
  SC . Bind    ( "prefer"    , prefer        ) ;
  SC . Bind    ( "version"   , name.toUtf8() ) ;
  SC . Exec    (                             ) ;
  //////////////////////////////////////////////
  return  prefer                               ;
}

bool N::DocumentManager :: UpdateVersion (
       SqlConnection     & SC            ,
       SUID                document      ,
       SUID                fileuuid      ,
       QString             name          )
{
  QString Q                                    ;
  Q = SC.sql.Update                            (
         PlanTable(DocumentGroups)             ,
         "where document = :DOCUMENT "
         "and fileuuid = :FILEUUID"            ,
         1,"version"                         ) ;
  SC . Prepare ( Q                           ) ;
  SC . Bind    ( "document"  , document      ) ;
  SC . Bind    ( "fileuuid"  , fileuuid      ) ;
  SC . Bind    ( "version"   , name.toUtf8() ) ;
  return SC . Exec    (                      ) ;
}

SUID N::DocumentManager :: FindKeyword (
       SqlConnection     & Connection  ,
       int                 language    ,
       QString             text        )
{
  QString Q                                           ;
  SUID    kuid = 0                                    ;
  SUID    uuid                                        ;
  UUIDs   Nuids                                       ;
  /////////////////////////////////////////////////////
  Q = Connection . sql . SelectFrom                   (
        "uuid"                                        ,
        PlanTable(Names)                              ,
        "where language = :LANGUAGE "
        "and name = :NAME "
        "order by id desc;"                         ) ;
  Connection . Prepare ( Q                          ) ;
  Connection . Bind    ( "language" , language      ) ;
  Connection . Bind    ( "name"     , text.toUtf8() ) ;
  IfSqlLoop(Connection)                               {
    Nuids << Connection . Uuid ( 0 )                  ;
  }                                                   ;
  /////////////////////////////////////////////////////
  foreach (uuid,Nuids)                                {
    Q = Connection . sql . SelectFrom                 (
          "uuid"                                      ,
          PlanTable(KeywordNames)                     ,
          QString("where name = %1").arg(uuid)      ) ;
    if (Connection.Fetch(Q))                          {
      kuid = Connection.Uuid(0)                       ;
      return kuid                                     ;
    }                                                 ;
  }                                                   ;
  /////////////////////////////////////////////////////
  return 0                                            ;
}

SUID N::DocumentManager :: assureKeyword (
       SqlConnection     & Connection    ,
       int                 language      ,
       QString             text          )
{
  QString Q                                                  ;
  SUID    kuid = 0                                           ;
  SUID    nuid = 0                                           ;
  SUID    uuid                                               ;
  UUIDs   Nuids                                              ;
  ////////////////////////////////////////////////////////////
  Q = Connection . sql . SelectFrom                          (
        "uuid"                                               ,
        PlanTable(Names)                                     ,
        "where language = :LANGUAGE and name = :NAME ;"    ) ;
  Connection . Prepare ( Q                          )        ;
  Connection . Bind    ( "language" , language      )        ;
  Connection . Bind    ( "name"     , text.toUtf8() )        ;
  IfSqlLoop(Connection)                                      {
    Nuids << Connection . Uuid ( 0 )                         ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  foreach (uuid,Nuids)                                       {
    Q = Connection.sql.SelectFrom                            (
          "uuid"                                             ,
          PlanTable(KeywordNames)                            ,
          QString("where name = %1").arg(uuid)             ) ;
    if (Connection.Fetch(Q))                                 {
      kuid = Connection.Uuid(0)                              ;
      return kuid                                            ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  nuid = Connection.Unique(PlanTable(MajorUuid),"uuid",8769) ;
  Connection.assureUuid                                      (
    PlanTable(MajorUuid)                                     ,
    nuid                                                     ,
    TYPE(Name)                                             ) ;
  ////////////////////////////////////////////////////////////
  kuid = Connection.Unique(PlanTable(MajorUuid),"uuid",8999) ;
  Connection.assureUuid                                      (
    PlanTable(MajorUuid)                                     ,
    kuid                                                     ,
    TYPE(Keyword)                                          ) ;
  ////////////////////////////////////////////////////////////
  Connection.assureName(PlanTable(Names),nuid,language,text) ;
  Connection.insertUuid(PlanTable(Keywords),kuid,"uuid")     ;
  ////////////////////////////////////////////////////////////
  Q = Connection . sql . InsertInto                          (
        PlanTable(KeywordNames)                              ,
        2                                                    ,
        "uuid"                                               ,
        "name"                                             ) ;
  Connection . Prepare ( Q             )                     ;
  Connection . Bind    ( "uuid" , kuid )                     ;
  Connection . Bind    ( "name" , nuid )                     ;
  Connection . Exec    (               )                     ;
  return kuid                                                ;
}

SUID N::DocumentManager :: assureSentence (
       SqlConnection     & Connection     ,
       int                 language       ,
       QString             text           )
{
  QString Q                                   ;
  SUID    uuid = 0                            ;
  /////////////////////////////////////////////
  Q = Connection . sql . SelectFrom           (
        "uuid"                                ,
        PlanTable(Sentences)                  ,
        "where name = :NAME ;")               ;
  Connection.Prepare(Q)                       ;
  Connection.Bind("name",text.toUtf8())       ;
  if (Connection.Exec() && Connection.Next()) {
    uuid = Connection.Uuid(0)                 ;
    return uuid                               ;
  }                                           ;
  /////////////////////////////////////////////
  uuid = Connection.Unique                    (
           PlanTable(MajorUuid)               ,
           "uuid",8371                      ) ;
  Connection.assureUuid                       (
    PlanTable(MajorUuid)                      ,
    uuid                                      ,
    TYPE(Sentence)                          ) ;
  /////////////////////////////////////////////
  Q = Connection.NameSyntax                   (
        PlanTable(Sentences)                  ,
        "uuid,language,name"                ) ;
  Connection.insertName(Q,uuid,language,text) ;
  /////////////////////////////////////////////
  return uuid                                 ;
}

SUID N::DocumentManager :: assureParagraph (
       SqlConnection     & Connection      ,
       int                 language        ,
       QString             text            )
{
  QString Q                                   ;
  SUID    uuid = 0                            ;
  /////////////////////////////////////////////
  Q = Connection . sql . SelectFrom           (
        "uuid"                                ,
        PlanTable(Paragraphs)                 ,
        "where name = :NAME ;")               ;
  Connection.Prepare(Q)                       ;
  Connection.Bind("name",text.toUtf8())       ;
  if (Connection.Exec() && Connection.Next()) {
    uuid = Connection.Uuid(0)                 ;
    return uuid                               ;
  }                                           ;
  /////////////////////////////////////////////
  uuid = Connection.Unique                    (
           PlanTable(MajorUuid)               ,
           "uuid",8371                      ) ;
  Connection.assureUuid                       (
    PlanTable(MajorUuid)                      ,
    uuid                                      ,
    TYPE(Paragraph)                         ) ;
  /////////////////////////////////////////////
  Q = Connection.NameSyntax                   (
        PlanTable(Paragraphs)                 ,
        "uuid,language,name"                ) ;
  Connection.insertName(Q,uuid,language,text) ;
  /////////////////////////////////////////////
  return uuid                                 ;
}

SUID N::DocumentManager :: assureTerminology (
       SqlConnection     & Connection        ,
       int                 language          ,
       QString             text              )
{
  QString Q                                                  ;
  SUID    nuid = 0                                           ;
  SUID    uuid                                               ;
  UUIDs   Nuids                                              ;
  ////////////////////////////////////////////////////////////
  Q = Connection . sql . SelectFrom                          (
        "uuid" , PlanTable(Names)                            ,
        "where language = :LANGUAGE and name = :NAME ;"    ) ;
  Connection . Prepare ( Q                          )        ;
  Connection . Bind    ( "language" , language      )        ;
  Connection . Bind    ( "name"     , text.toUtf8() )        ;
  IfSqlLoop(Connection)                                      {
    Nuids << Connection . Uuid ( 0 )                         ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  foreach (uuid,Nuids)                                       {
    Q = Connection.sql.SelectFrom                            (
          "type",PlanTable(MajorUuid)                        ,
          Connection.WhereUuid(uuid)                       ) ;
    if (Connection.Fetch(Q))                                 {
      SUID type = Connection.Uuid(0)                         ;
      if (type==TYPE(Terminology)) return uuid               ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  nuid = Connection.Unique(PlanTable(MajorUuid),"uuid",6156) ;
  Connection.assureUuid                                      (
    PlanTable(MajorUuid)                                     ,
    nuid                                                     ,
    TYPE(Terminology)                                      ) ;
  Connection.assureUuid( PlanTable(Terminology) , nuid , 0 ) ;
  ////////////////////////////////////////////////////////////
  Connection.assureName(PlanTable(Names),nuid,language,text) ;
  return nuid                                                ;
}

SUID N::DocumentManager :: assureKnowledge (
       SqlConnection     & Connection      ,
       int                 language        ,
       QString             text            )
{
  QString Q                                                  ;
  SUID    nuid = 0                                           ;
  SUID    uuid                                               ;
  UUIDs   Nuids                                              ;
  ////////////////////////////////////////////////////////////
  Q = Connection . sql . SelectFrom                          (
        "uuid"                                               ,
        PlanTable(Names)                                     ,
        "where language = :LANGUAGE and name = :NAME ;"    ) ;
  Connection . Prepare ( Q                          )        ;
  Connection . Bind    ( "language" , language      )        ;
  Connection . Bind    ( "name"     , text.toUtf8() )        ;
  IfSqlLoop(Connection)                                      {
    Nuids << Connection . Uuid ( 0 )                         ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  foreach (uuid,Nuids)                                       {
    Q = Connection.sql.SelectFrom                            (
          "type",PlanTable(MajorUuid)                        ,
          Connection.WhereUuid(uuid)                       ) ;
    if (Connection.Fetch(Q))                                 {
      SUID type = Connection.Uuid(0)                         ;
      if (type==TYPE(Knowledge)) return uuid                 ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  nuid = Connection.Unique(PlanTable(MajorUuid),"uuid",6164) ;
  Connection.assureUuid                                      (
    PlanTable(MajorUuid)                                     ,
    nuid                                                     ,
    TYPE(Knowledge)                                        ) ;
  Connection.insertUuid(PlanTable(Knowledge),nuid,"uuid"   ) ;
  ////////////////////////////////////////////////////////////
  Connection.assureName(PlanTable(Names),nuid,language,text) ;
  return nuid                                                ;
}

bool N::DocumentManager :: GetXml     (
       SqlConnection     & Connection ,
       SUID                uuid       ,
       QByteArray        & Body       )
{
  QString Q                          ;
  Q = Connection.sql.SelectFrom      (
        "xml"                        ,
        PlanTable(XmlDepot)          ,
        Connection.WhereUuid(uuid) ) ;
  if (Connection.Fetch(Q))           {
    Body = Connection.ByteArray(0)   ;
  }                                  ;
  return ( Body . size ( ) > 0 )     ;
}

bool N::DocumentManager :: UpdateXml (
       SqlConnection     & SC        ,
       SUID                uuid      ,
       int                 type      ,
       QByteArray        & Body      )
{
  QString Q                                     ;
  int     checksum = 0                          ;
  int     size     = Body.size()                ;
  if ( size > 0 )                               {
    checksum = qChecksum ( Body.data() , size ) ;
  }                                             ;
  ///////////////////////////////////////////////
  Q = SC . sql . SelectFrom                     (
        "id"                                    ,
        PlanTable(XmlDepot)                     ,
        SC . WhereUuid ( uuid )               ) ;
  if (SC.Fetch(Q))                              {
    Q = SC . sql . Update                       (
          PlanTable(XmlDepot)                   ,
          SC.sql.Where(1,"uuid")                ,
          2                                     ,
          "length"                              ,
          "xml"                               ) ;
  } else                                        {
    Q = SC . sql . InsertInto                   (
          PlanTable(XmlDepot)                   ,
          3                                     ,
          "uuid"                                ,
          "length"                              ,
          "xml"                               ) ;
  }                                             ;
  SC . Prepare ( Q                 )            ;
  SC . Bind    ( "uuid"     , uuid )            ;
  SC . Bind    ( "length"   , size )            ;
  SC . Bind    ( "xml"      , Body )            ;
  SC . Exec    (                   )            ;
  ///////////////////////////////////////////////
  Q = SC . sql . SelectFrom                     (
        "id"                                    ,
        PlanTable(XML)                          ,
        SC . WhereUuid(uuid)                  ) ;
  if (SC.Fetch(Q))                              {
    Q = SC . sql . Update                       (
          PlanTable(XML)                        ,
          SC . sql . Where ( 1 , "uuid" )       ,
          3                                     ,
          "type"                                ,
          "size"                                ,
          "checksum"                          ) ;
  } else                                        {
    Q = SC . sql . InsertInto                   (
          PlanTable(XML)                        ,
          4                                     ,
          "uuid"                                ,
          "type"                                ,
          "size"                                ,
          "checksum"                          ) ;
  }                                             ;
  SC . Prepare     ( Q                     )    ;
  SC . Bind        ( "uuid"     , uuid     )    ;
  SC . Bind        ( "type"     , type     )    ;
  SC . Bind        ( "size"     , size     )    ;
  SC . Bind        ( "checksum" , checksum )    ;
  return SC . Exec (                       )    ;
}

int N::DocumentManager :: EncodingId (
      QString             codec      ,
      UMAPs             & Encodings  )
{
  nKickOut ( ! Encodings . contains ( codec ) , 0 ) ;
  return Encodings [ codec ]                        ;
}

bool N::DocumentManager :: getEncodings (
       SqlConnection     & Connection   ,
       UMAPs             & Encodings    )
{
  QString Q                                  ;
  Q = Connection.sql.SelectFrom              (
        "id,name"                            ,
        PlanTable(Encodings)                 ,
        Connection.OrderByAsc("id")        ) ;
  SqlLoopNow ( Connection , Q )              ;
    int     id   = Connection . Int    ( 0 ) ;
    QString name = Connection . String ( 1 ) ;
    Encodings [ name ] = id                  ;
  SqlLoopErr ( Connection , Q )              ;
  SqlLoopEnd ( Connection , Q )              ;
  return ( Encodings . count ( ) > 0 )       ;
}

bool N::DocumentManager :: getEncodings (
       SqlConnection     & Connection   ,
       NAMEs             & Encodings    )
{
  QString Q                                  ;
  Q = Connection . sql . SelectFrom          (
        "`id`,`name`"                        ,
        PlanTable(Encodings)                 ,
        Connection.OrderByAsc("id")        ) ;
  SqlLoopNow ( Connection , Q )              ;
    int     id   = Connection . Int    ( 0 ) ;
    QString name = Connection . String ( 1 ) ;
    Encodings [ id ] = name                  ;
  SqlLoopErr ( Connection , Q )              ;
  SqlLoopEnd ( Connection , Q )              ;
  return ( Encodings . count ( ) > 0 )       ;
}

bool N::DocumentManager :: assureEncoding (
       SqlConnection     & Connection     ,
       SUID                uuid           ,
       TUID                publish        ,
       QString             encoding       )
{
  QString Q                                                     ;
  int     codec = 0                                             ;
  Q = Connection.sql.SelectFrom                                 (
        "id"                                                    ,
        PlanTable(Encodings)                                    ,
        QString("where name = '%1' ;").arg(encoding)          ) ;
  if (Connection.Fetch(Q)) codec = Connection.Int(0)            ;
  return assureEncoding(Connection,uuid,publish,codec,encoding) ;
}

bool N::DocumentManager :: assureEncoding (
       SqlConnection     & Connection     ,
       SUID                uuid           ,
       TUID                publish        ,
       int                 codec          ,
       QString             encoding       )
{
  QString Q                                                      ;
  bool    exists = false                                         ;
  ////////////////////////////////////////////////////////////////
  Q = Connection.sql.SelectFrom                                  (
        "id"                                                     ,
        PlanTable(DocumentCodecs)                                ,
        QString ( "where uuid = %1 and publish = %2"             )
        . arg   ( uuid                                           )
        . arg   ( publish                                    ) ) ;
  if (Connection.Fetch(Q)) exists = true                         ;
  ////////////////////////////////////////////////////////////////
  if (exists)                                                    {
    Q = Connection.sql.Update                                    (
          PlanTable(DocumentCodecs)                              ,
          Connection.sql.Where(2,"uuid","publish")               ,
          2                                                      ,
          "codec"                                                ,
          "encoding"                                           ) ;
  } else                                                         {
    Q = Connection.sql.InsertInto                                (
          PlanTable(DocumentCodecs)                              ,
          4                                                      ,
          "uuid"                                                 ,
          "publish"                                              ,
          "codec"                                                ,
          "encoding"                                           ) ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  Connection        . Prepare ( Q                              ) ;
  Connection        . Bind    ( "uuid"     , uuid              ) ;
  Connection        . Bind    ( "publish"  , publish           ) ;
  Connection        . Bind    ( "codec"    , codec             ) ;
  Connection        . Bind    ( "encoding" , encoding.toUtf8() ) ;
  return Connection . Exec    (                                ) ;
}
