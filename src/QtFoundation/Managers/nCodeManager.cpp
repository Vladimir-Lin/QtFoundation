#include <qtfoundation.h>

#define plan CodePlan
#define PLAN (*CodePlan)

N::CodeManager:: CodeManager ( PurePlan * p )
               : CodePlan    (            p )
{
}

N::CodeManager::~CodeManager (void)
{
}

QString N::CodeManager  :: Name     (
          SqlConnection  & SC       ,
          SUID             uuid     ,
          int              language )
{
  return SC . getName    (
           PlanTable(Names) ,
           "uuid"        ,
           language      ,
           uuid        ) ;
}

NAMEs N::CodeManager  :: Names      (
        SqlConnection  & Connection ,
        UUIDs          & Uuids      ,
        int              language   )
{ Q_UNUSED ( language )     ;
  NAMEs N                   ;
  N :: Manager :: UuidNames (
    PLAN                    ,
    Connection              ,
    Uuids                   ,
    N                     ) ;
  return N                  ;
}

bool N::CodeManager :: assureName (
       SqlConnection & Connection ,
       SUID            uuid       ,
       int             language   ,
       QString         name       )
{
  return Connection . assureName (
           PlanTable ( Names )      ,
           uuid                  ,
           language              ,
           name                ) ;
}

QString N::CodeManager::QueryMembers(SqlConnection  & Connection)
{
  return Connection. sql . SelectFrom          (
           Connection.Columns(2,"uuid","name") ,
           PlanTable ( SetMembers )               ,
           Connection . OrderByAsc ( "id" )  ) ;
}

SUID N::CodeManager :: assureMember (
       SqlConnection & Connection   ,
       SUID            uuid         ,
       int             language     ,
       QString         name         ,
       QString         identifier   )
{
  QString Q                                             ;
  if (uuid<=0)                                          {
    uuid = Connection . Unique                          (
             PlanTable(MajorUuid)                          ,
             "uuid"                                     ,
             IdSections::SetMembers                   ) ;
    Connection.assureUuid                               (
      PlanTable(MajorUuid)                                 ,
      uuid                                              ,
      Types::SetMember                                ) ;
    Q = Connection.sql.InsertInto                       (
          PlanTable(SetMembers)                            ,
          2,"uuid","name"                             ) ;
  } else                                                {
    Q = Connection.sql.Update                           (
          PlanTable(SetMembers)                            ,
          "where `uuid` = :UUID",1,"name"             ) ;
  }                                                     ;
  Connection . Prepare ( Q )                            ;
  Connection . Bind    ( "uuid" , uuid                ) ;
  Connection . Bind    ( "name" , identifier.toUtf8() ) ;
  Connection . Exec    (   )                            ;
  assureName ( Connection , uuid , language , name )    ;
  return uuid                                           ;
}

UUIDs N::CodeManager::Members(SqlConnection & Connection)
{
  UUIDs U              ;
  Manager :: Uuids     (
    PLAN               ,
    Connection         ,
    U                  ,
    Types::SetMember ) ;
  return U             ;
}

QString N::CodeManager::QuerySets(SqlConnection & Connection)
{
  return Connection.sql.SelectFrom       (
           Connection . Columns          (
             3                           ,
             "uuid"                      ,
             "type"                      ,
             "uniqueness"              ) ,
           PlanTable(PoSets               ) ,
           Connection.OrderByAsc("id") ) ;
}

SUID N::CodeManager :: assureSet  (
       SqlConnection & Connection ,
       SUID            uuid       ,
       int             language   ,
       QString         name       ,
       int             Type       ,
       int             Uniqueness )
{
  QString Q                                             ;
  if (uuid<=0)                                          {
    uuid = Connection.Unique(PlanTable(MajorUuid),"uuid")  ;
    Connection.assureUuid                               (
      PlanTable(MajorUuid),uuid                            ,
      Types::POSet                                    ) ;
    Q = Connection.sql.InsertInto                       (
          PlanTable(PoSets)                                ,
          3,"uuid","type","uniqueness"                ) ;
  } else                                                {
    Q = Connection.sql.Update                           (
          PlanTable(PoSets)                                ,
          "where `uuid` = :UUID",2,"type","uniqueness") ;
  }                                                     ;
  Connection . Prepare ( Q )                            ;
  Connection . Bind    ( "uuid"       , uuid       )    ;
  Connection . Bind    ( "type"       , Type       )    ;
  Connection . Bind    ( "uniqueness" , Uniqueness )    ;
  Connection . Exec    (   )                            ;
  assureName ( Connection , uuid , language , name )    ;
  return uuid ;
}

UUIDs N::CodeManager::Sets(SqlConnection & Connection)
{
  return Connection.Uuids                (
           PlanTable(PoSets)                ,
           "uuid"                        ,
           Connection.OrderByAsc("id") ) ;
}

QByteArray N::CodeManager :: LoadFile   (
             SqlConnection & Connection ,
             SUID            uuid       )
{
  QString    Q                       ;
  QByteArray F                       ;
  Q = Connection.sql.SelectFrom      (
        "file",PlanTable(FileDepot)     ,
        Connection.WhereUuid(uuid) ) ;
  if (Connection.Fetch(Q))           {
    F = Connection.ByteArray(0)      ;
  }                                  ;
  return     F                       ;
}

bool N::CodeManager :: LoadFile   (
       SqlConnection & Connection ,
       SUID            uuid       ,
       QByteArray    & Body       )
{
  QString    Q                       ;
  Q = Connection.sql.SelectFrom      (
        "file",PlanTable(FileDepot)     ,
        Connection.WhereUuid(uuid) ) ;
  if (Connection.Fetch(Q))           {
    Body = Connection.ByteArray(0)   ;
  } else return false                ;
  return true                        ;
}

bool N::CodeManager :: isDirectory (int attribute)
{
  nKickOut ( isAttribute(attribute,FaFile) , false ) ;
  return     isAttribute(attribute,FaDir )           ;
}

bool N::CodeManager :: isAttribute (
      int              attribute   ,
      FileAttributes   fa          )
{
  int fav = (int)fa                     ;
  return ( ( attribute & fav ) == fav ) ;
}

int N::CodeManager :: SetAttribute (
      int             attribute    ,
      FileAttributes  fa           )
{
  return ( attribute | fa ) ;
}

int N::CodeManager :: FileAttribute(QFileInfo & FI)
{
  int Attribute = 0                                ;
  if (FI.isFile      ()) Attribute |= FaFile       ;
  if (FI.isDir       ()) Attribute |= FaDir        ;
  if (FI.isExecutable()) Attribute |= FaExecutable ;
  if (FI.isHidden    ()) Attribute |= FaHidden     ;
  if (FI.isReadable  ()) Attribute |= FaReadable   ;
  if (FI.isWritable  ()) Attribute |= FaWritable   ;
  if (FI.isSymLink   ()) Attribute |= FaSymLink    ;
  if (FI.isRoot      ()) Attribute |= FaRoot       ;
  if (FI.isRelative  ()) Attribute |= FaRelative   ;
  return Attribute                                 ;
}

bool N::CodeManager :: GetAttributes (
       SqlConnection & Connection    ,
       SUID            uuid          ,
       int           & attributes    ,
       int           & size          )
{
  QString Q                          ;
  Q = Connection.sql.SelectFrom      (
        "size,attribute"             ,
        PlanTable(Files)                ,
        Connection.WhereUuid(uuid) ) ;
  if (Connection.Fetch(Q))           {
    size       = Connection.Int(0)   ;
    attributes = Connection.Int(1)   ;
  } else return false                ;
  return true                        ;
}

SUID N::CodeManager :: FindFile  (
       SqlConnection & SC        ,
       int             attribute ,
       QByteArray    & data      )
{
  int     fsize    = data.size()               ;
  int     checksum = nChecksum(data)           ;
  SUID    uuid     = 0                         ;
  SUID    iuid                                 ;
  UUIDs   Uuids                                ;
  QString Q                                    ;
  //////////////////////////////////////////////
  SC.Uuids                                     (
    Uuids                                      ,
    PlanTable(Files)                              ,
    "uuid"                                     ,
    QString("where size = %1 "
            "and checksum = %2 "
            "and attribute = %3 "
            "order by id desc"                 )
    . arg ( fsize                              )
    . arg ( checksum                           )
    . arg ( attribute                      ) ) ;
  //////////////////////////////////////////////
  nKickOut ( Uuids.count() <= 0 , 0 )          ;
  //////////////////////////////////////////////
  for (int i=0;uuid<=0 && i<Uuids.count();i++) {
    iuid = Uuids[i]                            ;
    Q = SC.sql.SelectFrom                      (
          "file"                               ,
          PlanTable(FileDepot)                    ,
          SC.WhereUuid(iuid)                 ) ;
    if (SC.Fetch(Q))                           {
      QByteArray d = SC . ByteArray ( 0 )      ;
      if (d==data) uuid = iuid                 ;
    }                                          ;
  }                                            ;
  //////////////////////////////////////////////
  return uuid                                  ;
}

SUID N::CodeManager :: addEmpty (
       SqlConnection & SC       ,
       QFileInfo     & FI       )
{
  QString Q                                           ;
  int     Attribute = FileAttribute ( FI )            ;
  SUID    uuid = SC.Unique(PlanTable(MajorUuid),"uuid")  ;
  SC . assureUuid(PlanTable(MajorUuid),uuid,Types::File) ;
  /////////////////////////////////////////////////////
  Q = SC.sql.InsertInto                               (
        PlanTable(Files)                                 ,
        6                                             ,
        "uuid"                                        ,
        "size"                                        ,
        "checksum"                                    ,
        "attribute"                                   ,
        "createtime"                                  ,
        "lastmodified"                              ) ;
  SC . Prepare ( Q                                  ) ;
  SC . Bind    ( "uuid"         , uuid              ) ;
  SC . Bind    ( "size"         , 0                 ) ;
  SC . Bind    ( "checksum"     , 0                 ) ;
  SC . Bind    ( "attribute"    , Attribute         ) ;
  SC . Bind    ( "createtime"   , FI.created     () ) ;
  SC . Bind    ( "lastmodified" , FI.lastModified() ) ;
  SC . Exec    (                                    ) ;
  return uuid                                         ;
}

SUID N::CodeManager :: assureFile (
       SqlConnection & SC         ,
       QString         filename   )
{
  QFileInfo FI(filename)                                    ;
  int       Attribute = FileAttribute ( FI )                ;
  SUID      uuid      = 0                                   ;
  QString   Q                                               ;
  if (FI.isDir())                                           {
    uuid = addEmpty(SC,FI)                                  ;
  } else
  if (FI.isFile())                                          {
    QByteArray data                                         ;
    if (File::toByteArray(filename,data))                   {
      uuid = FindFile ( SC , Attribute , data )             ;
      if (uuid<=0)                                          {
        int checksum = nChecksum   ( data )                 ;
        int length   = data . size (      )                 ;
        uuid = SC.Unique(PlanTable(MajorUuid),"uuid")          ;
        SC.assureUuid(PlanTable(MajorUuid)                     ,
                      uuid,Types::File                    ) ;
        SC . transaction ( )                                ;
        /////////////////////////////////////////////////////
        Q = SC.sql.InsertInto                               (
              PlanTable(Files)                                 ,
              6                                             ,
              "uuid"                                        ,
              "size"                                        ,
              "checksum"                                    ,
              "attribute"                                   ,
              "createtime"                                  ,
              "lastmodified"                              ) ;
        SC . Prepare ( Q                                  ) ;
        SC . Bind    ( "uuid"         , uuid              ) ;
        SC . Bind    ( "size"         , length            ) ;
        SC . Bind    ( "checksum"     , checksum          ) ;
        SC . Bind    ( "attribute"    , Attribute         ) ;
        SC . Bind    ( "createtime"   , FI.created     () ) ;
        SC . Bind    ( "lastmodified" , FI.lastModified() ) ;
        SC . Exec    (                                    ) ;
        /////////////////////////////////////////////////////
        Q = SC.sql.InsertInto                               (
              PlanTable(FileDepot)                             ,
              3                                             ,
              "uuid"                                        ,
              "length"                                      ,
              "file"                                      ) ;
        SC . Prepare ( Q                                  ) ;
        SC . Bind    ( "uuid"         , uuid              ) ;
        SC . Bind    ( "length"       , length            ) ;
        SC . Bind    ( "file"         , data              ) ;
        SC . Exec    (                                    ) ;
        /////////////////////////////////////////////////////
        SC . commit  (                                    ) ;
      }                                                     ;
    } else                                                  {
      uuid = addEmpty(SC,FI)                                ;
    }                                                       ;
  } else                                                    {
  }                                                         ;
  return uuid                                               ;
}

SUID N::CodeManager :: assureFile   (
       SqlConnection & SC           ,
       int             attribute    ,
       QDateTime       Created      ,
       QDateTime       LastModified ,
       QByteArray    & Body         ,
       bool            Find         )
{
  SUID      uuid = 0                                 ;
  QString   Q                                        ;
  if (Body.size()>0)                                 {
    uuid = FindFile ( SC , attribute , Body )        ;
    if (uuid<=0)                                     {
      int checksum = nChecksum   ( Body )            ;
      int length   = Body . size (      )            ;
      uuid = SC.Unique(PlanTable(MajorUuid),"uuid")     ;
      SC.assureUuid(PlanTable(MajorUuid)                ,
                    uuid,Types::File               ) ;
      SC . transaction ( )                           ;
      ////////////////////////////////////////////////
      Q = SC.sql.InsertInto                          (
            PlanTable(Files)                            ,
            6                                        ,
            "uuid"                                   ,
            "size"                                   ,
            "checksum"                               ,
            "attribute"                              ,
            "createtime"                             ,
            "lastmodified"                         ) ;
      SC . Prepare ( Q                             ) ;
      SC . Bind    ( "uuid"         , uuid         ) ;
      SC . Bind    ( "size"         , length       ) ;
      SC . Bind    ( "checksum"     , checksum     ) ;
      SC . Bind    ( "attribute"    , attribute    ) ;
      SC . Bind    ( "createtime"   , Created      ) ;
      SC . Bind    ( "lastmodified" , LastModified ) ;
      SC . Exec    (                               ) ;
      ////////////////////////////////////////////////
      Q = SC.sql.InsertInto                          (
            PlanTable(FileDepot)                        ,
            3                                        ,
            "uuid"                                   ,
            "length"                                 ,
            "file"                                 ) ;
      SC . Prepare ( Q                             ) ;
      SC . Bind    ( "uuid"         , uuid         ) ;
      SC . Bind    ( "length"       , length       ) ;
      SC . Bind    ( "file"         , Body         ) ;
      SC . Exec    (                               ) ;
      ////////////////////////////////////////////////
      SC . commit  (                               ) ;
    }                                                ;
  } else                                             {
    if (Find)                                        {
      uuid = FindFile ( SC , attribute , Body )      ;
      if (uuid>0) return uuid                        ;
    }                                                ;
    uuid = SC.Unique(PlanTable(MajorUuid),"uuid")       ;
    SC . assureUuid                                  (
      PlanTable(MajorUuid)                              ,
      uuid                                           ,
      Types::File                                  ) ;
    //////////////////////////////////////////////////
    Q = SC.sql.InsertInto                            (
          PlanTable(Files)                              ,
          6                                          ,
          "uuid"                                     ,
          "size"                                     ,
          "checksum"                                 ,
          "attribute"                                ,
          "createtime"                               ,
          "lastmodified"                           ) ;
    SC . Prepare ( Q                               ) ;
    SC . Bind    ( "uuid"         , uuid           ) ;
    SC . Bind    ( "size"         , 0              ) ;
    SC . Bind    ( "checksum"     , 0              ) ;
    SC . Bind    ( "attribute"    , attribute      ) ;
    SC . Bind    ( "createtime"   , Created        ) ;
    SC . Bind    ( "lastmodified" , LastModified   ) ;
    SC . Exec    (                                 ) ;
  }                                                  ;
  return uuid                                        ;
}

SUID N::CodeManager :: addVersion (
       SqlConnection & SC         ,
       SUID            sourceUuid ,
       QString         version    )
{
  QString Q                          ;
  SUID    vuid = SC.Unique           (
                   PlanTable(MajorUuid) ,
                   "uuid"          ) ;
  SC . assureUuid                    (
    PlanTable(MajorUuid)                ,
    vuid                             ,
    Types::Version                 ) ;
  ////////////////////////////////////
  Q = SC.sql.InsertInto              (
        PlanTable(SourceVersion)        ,
        3                            ,
        "uuid"                       ,
        "source"                     ,
        "name"                     ) ;
  SC . Prepare ( Q                 ) ;
  SC . Bind("uuid",vuid            ) ;
  SC . Bind("source",sourceUuid    ) ;
  SC . Bind("name",version.toUtf8()) ;
  SC . Exec(                       ) ;
  ////////////////////////////////////
  return vuid                        ;
}

bool N::CodeManager :: attachVersion (
       SqlConnection & SC            ,
       SUID            versionUuid   ,
       SUID            fileUuid      ,
       QString         path          ,
       int             subversion    )
{
  nKickOut ( versionUuid <= 0 , false )   ;
  nKickOut ( fileUuid    <= 0 , false )   ;
  QString Q                               ;
  Q = SC.sql.InsertInto                   (
        PlanTable(SourceMaps)                ,
        4                                 ,
        "uuid"                            ,
        "file"                            ,
        "subversion"                      ,
        "path"                          ) ;
  SC.Prepare(Q)                           ;
  SC.Bind( "uuid"       , versionUuid   ) ;
  SC.Bind( "file"       , fileUuid      ) ;
  SC.Bind( "subversion" , subversion    ) ;
  SC.Bind( "path"       , path.toUtf8() ) ;
  return SC.Exec()                        ;
}

UUIDs N::CodeManager :: SourceVersions (
        SqlConnection & SC             ,
        SUID            sourceUuid     )
{
  UUIDs   Uuids                                      ;
  QString Q                                          ;
  Q = SC.sql.SelectFrom                              (
      "uuid"                                         ,
      PlanTable(SourceVersion)                          ,
      QString ( "where source = %1 order by id desc" )
      .arg    ( sourceUuid )                       ) ;
  SqlLoopNow ( SC , Q )                              ;
    SUID uuid = SC . Uuid   ( 0 )                    ;
    if (uuid>0 && !Uuids.contains(uuid))             {
      Uuids << uuid                                  ;
    }                                                ;
  SqlLoopErr ( SC , Q )                              ;
  SqlLoopEnd ( SC , Q )                              ;
  return Uuids                                       ;
}

UUIDs N::CodeManager :: SourceUrls (
        SqlConnection & SC         ,
        SUID            sourceUuid )
{
  QString Q                               ;
  UUIDs   Uuids                           ;
  Q = SC.sql.SelectFrom                   (
        "second"                          ,
        PlanTable(Groups)                    ,
        QString("where first = %1"
                " and t1 = %2"
                " and t2 = %3"
                " and relation = %4"
                " order by position asc ;")
        .arg(sourceUuid                   )
        .arg(Types::Source                )
        .arg(Types::URL                   )
        .arg(Groups::Subordination        )
      )                                   ;
  SqlLoopNow ( SC , Q )                   ;
    SUID vuid = SC.Uuid(0)                ;
    if (vuid>0 && !Uuids.contains(vuid))  {
      Uuids << vuid                       ;
    }                                     ;
  SqlLoopErr ( SC , Q )                   ;
  SqlLoopEnd ( SC , Q )                   ;
  return Uuids                            ;
}

bool N::CodeManager :: DeleteSourceUrl (
       SqlConnection & SC              ,
       SUID            sourceUuid      ,
       SUID            url             )
{
  QString Q                          ;
  Q = SC.sql.DeleteFrom              (
        PlanTable(Groups)               ,
        QString("where first = %1 "
                "and second = %2 "
                "and t1 = %3 "
                "and t2 = %4 "
                "and relation = %5 ;")
        .arg(sourceUuid              )
        .arg(url                     )
        .arg(Types::Source           )
        .arg(Types::URL              )
        .arg(Groups::Subordination   )
      )                              ;
  return SC . Query ( Q )            ;
}

bool N::CodeManager :: UpdateUrlPositions (
       SqlConnection & SC                 ,
       SUID            sourceUuid         ,
       UUIDs         & Uuids              )
{
  int     position = 1                                            ;
  SUID    uuid                                                    ;
  QString Q                                                       ;
  foreach (uuid,Uuids)                                            {
    Q = SC.sql.Update                                             (
          PlanTable(Groups)                                          ,
          "where first = :FIRST and second = :SECOND "
          "and t1 = :T1 and t2 = :T2 and relation = :RELATION"    ,
          1 , "position"                                        ) ;
    SC . Prepare ( Q                                            ) ;
    SC . Bind    ( "first"    , sourceUuid                      ) ;
    SC . Bind    ( "second"   , uuid                            ) ;
    SC . Bind    ( "t1"       , Types::Source                   ) ;
    SC . Bind    ( "t2"       , Types::URL                      ) ;
    SC . Bind    ( "relation" , Groups::Subordination           ) ;
    SC . Bind    ( "position" , position                        ) ;
    SC . Exec    (                                              ) ;
    position++                                                    ;
  }                                                               ;
  return true                                                     ;
}

UUIDs N::CodeManager::DataTypes(SqlConnection & SC)
{
  return SC.Uuids(PlanTable(DataTypes),"uuid","order by id asc");
}

NAMEs N::CodeManager :: TypeNames (
        SqlConnection & SC        ,
        UUIDs         & Uuids     )
{
  QString Q                          ;
  SUID    uuid                       ;
  NAMEs   Names                      ;
  foreach (uuid,Uuids)               {
    Q = SC.sql.SelectFrom            (
          "name" , PlanTable(DataTypes) ,
          SC.WhereUuid(uuid)       ) ;
    if (SC.Fetch(Q))                 {
      Names[uuid] = SC.String(0)     ;
    }                                ;
  }                                  ;
  return Names                       ;
}

SUID N::CodeManager :: findDefine (
       SqlConnection & SC         ,
       QString         Define     )
{
  QString Q                         ;
  SUID    uuid = 0                  ;
  Q = SC.sql.SelectFrom             (
        "uuid"                      ,
        PlanTable(CppDefines)          ,
        "where length = :LENGTH "
        "and name = :NAME"        ) ;
  SC.Prepare ( Q )                  ;
  SC.Bind("length",Define.length()) ;
  SC.Bind("name"  ,Define.toUtf8()) ;
  IfSqlExec ( SC )                  {
    uuid = SC . Uuid ( 0 )          ;
  }                                 ;
  return uuid                       ;
}

SUID N::CodeManager :: assureDefine (
       SqlConnection & SC           ,
       QString         Define       )
{
  SUID uuid = findDefine(SC,Define)          ;
  nKickOut ( uuid > 0 , uuid )               ;
  QString Q                                  ;
  uuid = SC.Unique(PlanTable(MajorUuid),"uuid") ;
  SC.assureUuid                              (
    PlanTable(MajorUuid)                        ,
    uuid                                     ,
    Types::Component                       ) ;
  insertComponent                            (
    SC                                       ,
    uuid                                     ,
    Cpp::Define                              ,
    100079                                 ) ;
  insertPreprocessor                         (
    SC                                       ,
    uuid                                     ,
    Cpp::Define                            ) ;
  Q = SC.sql.InsertInto                      (
        PlanTable(CppDefines)                   ,
        3                                    ,
        "uuid"                               ,
        "length"                             ,
        "name"                             ) ;
  SC.Prepare ( Q                           ) ;
  SC.Bind    ( "uuid"   , uuid             ) ;
  SC.Bind    ( "length" , Define.length()  ) ;
  SC.Bind    ( "name"   , Define.toUtf8()  ) ;
  SC.Exec    (                             ) ;
  return uuid                                ;
}

bool N::CodeManager :: insertGroup (
       SqlConnection & SC          ,
       SUID            first       ,
       SUID            second      ,
       int             t1          ,
       int             t2          ,
       int             relation    )
{
  QString Q                              ;
  Q = SC.sql.InsertInto                  (
        PlanTable(Groups)                   ,
        5                                ,
        "first"                          ,
        "second"                         ,
        "t1"                             ,
        "t2"                             ,
        "relation"                     ) ;
  SC . Prepare ( Q                     ) ;
  SC . Bind    ( "first"    , first    ) ;
  SC . Bind    ( "second"   , second   ) ;
  SC . Bind    ( "t1"       , t1       ) ;
  SC . Bind    ( "t2"       , t2       ) ;
  SC . Bind    ( "relation" , relation ) ;
  return SC . Exec (                   ) ;
}

SUID N::CodeManager :: assureDefine (
       SqlConnection & SC           ,
       SUID            file         ,
       QString         Define       )
{
  SUID uuid = assureDefine(SC,Define) ;
  nKickOut ( uuid <= 0 , 0 )          ;
  insertGroup                         (
    SC                                ,
    file                              ,
    uuid                              ,
    Types::File                       ,
    Types::Component                  ,
    Groups::Contains                ) ;
  return uuid                         ;
}

SUID N::CodeManager :: findInclude (
       SqlConnection & SC          ,
       QString         Include     )
{
  QString Q                          ;
  SUID    uuid = 0                   ;
  Q = SC.sql.SelectFrom              (
        "uuid"                       ,
        PlanTable(Includes)             ,
        "where length = :LENGTH "
        "and name = :NAME"         ) ;
  SC.Prepare ( Q )                   ;
  SC.Bind("length",Include.length()) ;
  SC.Bind("name"  ,Include.toUtf8()) ;
  IfSqlExec ( SC )                   {
    uuid = SC . Uuid ( 0 )           ;
  }                                  ;
  return uuid                        ;
}

SUID N::CodeManager :: assureInclude (
       SqlConnection & SC            ,
       QString         Include       )
{
  SUID uuid = findInclude(SC,Include)        ;
  nKickOut ( uuid > 0 , uuid )               ;
  QString Q                                  ;
  uuid = SC.Unique(PlanTable(MajorUuid),"uuid") ;
  SC.assureUuid                              (
    PlanTable(MajorUuid)                        ,
    uuid                                     ,
    Types::Component                       ) ;
  insertComponent                            (
    SC                                       ,
    uuid                                     ,
    Cpp::Include                             ,
    100079                                 ) ;
  Q = SC.sql.InsertInto                      (
        PlanTable(Includes)                     ,
        3                                    ,
        "uuid"                               ,
        "length"                             ,
        "name"                             ) ;
  SC.Prepare ( Q                           ) ;
  SC.Bind    ( "uuid"   , uuid             ) ;
  SC.Bind    ( "length" , Include.length() ) ;
  SC.Bind    ( "name"   , Include.toUtf8() ) ;
  SC.Exec    (                             ) ;
  return uuid                                ;
}

SUID N::CodeManager :: assureInclude (
       SqlConnection & SC            ,
       SUID            file          ,
       QString         Include       )
{
  SUID uuid = assureInclude(SC,Include) ;
  nKickOut ( uuid <= 0 , 0 )            ;
  insertGroup                           (
    SC                                  ,
    file                                ,
    uuid                                ,
    Types::File                         ,
    Types::Component                    ,
    Groups::Contains                  ) ;
  return uuid                           ;
}

SUID N::CodeManager :: findNamespace (
       SqlConnection & SC            ,
       QString         Namespace     )
{
  QString Q                            ;
  SUID    uuid = 0                     ;
  Q = SC.sql.SelectFrom                (
        "uuid"                         ,
        PlanTable(Namespaces)             ,
        "where length = :LENGTH "
        "and name = :NAME"           ) ;
  SC.Prepare ( Q )                     ;
  SC.Bind("length",Namespace.length()) ;
  SC.Bind("name"  ,Namespace.toUtf8()) ;
  IfSqlExec ( SC )                     {
    uuid = SC . Uuid ( 0 )             ;
  }                                    ;
  return uuid                          ;
}

SUID N::CodeManager :: assureNamespace (
       SqlConnection & SC              ,
       QString         Namespace       )
{
  SUID uuid = findNamespace(SC,Namespace)      ;
  nKickOut ( uuid > 0 , uuid )                 ;
  QString Q                                    ;
  uuid = SC.Unique(PlanTable(MajorUuid),"uuid")   ;
  SC.assureUuid                                (
    PlanTable(MajorUuid)                          ,
    uuid                                       ,
    Types::Component                         ) ;
  insertComponent                              (
    SC                                         ,
    uuid                                       ,
    Cpp::Namespace                             ,
    100081                                   ) ;
  Q = SC.sql.InsertInto                        (
        PlanTable(Namespaces)                     ,
        3                                      ,
        "uuid"                                 ,
        "length"                               ,
        "name"                               ) ;
  SC.Prepare ( Q                             ) ;
  SC.Bind    ( "uuid"   , uuid               ) ;
  SC.Bind    ( "length" , Namespace.length() ) ;
  SC.Bind    ( "name"   , Namespace.toUtf8() ) ;
  SC.Exec    (                               ) ;
  return uuid                                  ;
}

SUID N::CodeManager :: assureNamespace (
       SqlConnection & SC              ,
       SUID            file            ,
       QString         Namespace       )
{
  SUID uuid = assureNamespace(SC,Namespace) ;
  nKickOut ( uuid <= 0 , 0 )                ;
  insertGroup                               (
    SC                                      ,
    file                                    ,
    uuid                                    ,
    Types::File                             ,
    Types::Component                        ,
    Groups::Contains                      ) ;
  return uuid                               ;
}

SUID N::CodeManager :: findEnum (
       SqlConnection & SC       ,
       QString         Enum     )
{
  QString Q                       ;
  SUID    uuid = 0                ;
  Q = SC.sql.SelectFrom           (
        "uuid"                    ,
        PlanTable(Enumerations)      ,
        "where length = :LENGTH "
        "and name = :NAME"      ) ;
  SC.Prepare ( Q )                ;
  SC.Bind("length",Enum.length()) ;
  SC.Bind("name"  ,Enum.toUtf8()) ;
  IfSqlExec ( SC )                {
    uuid = SC . Uuid ( 0 )        ;
  }                               ;
  return uuid                     ;
}

SUID N::CodeManager :: assureEnum (
       SqlConnection & SC         ,
       QString         Enum       )
{
  SUID uuid = findEnum(SC,Enum)              ;
  nKickOut ( uuid > 0 , uuid )               ;
  QString Q                                  ;
  uuid = SC.Unique(PlanTable(MajorUuid),"uuid") ;
  SC.assureUuid                              (
    PlanTable(MajorUuid)                        ,
    uuid                                     ,
    Types::Component                       ) ;
  insertComponent                            (
    SC                                       ,
    uuid                                     ,
    Cpp::Enumeration                         ,
    100081                                 ) ; // need to modify to 100079
  Q = SC.sql.InsertInto                      (
        PlanTable(Enumerations)                 ,
        3                                    ,
        "uuid"                               ,
        "length"                             ,
        "name"                             ) ;
  SC.Prepare ( Q                           ) ;
  SC.Bind    ( "uuid"   , uuid             ) ;
  SC.Bind    ( "length" , Enum.length()    ) ;
  SC.Bind    ( "name"   , Enum.toUtf8()    ) ;
  SC.Exec    (                             ) ;
  return uuid                                ;
}

SUID N::CodeManager :: assureEnum (
       SqlConnection & SC         ,
       SUID            file       ,
       QString         Enum       )
{
  SUID uuid = assureEnum(SC,Enum) ;
  nKickOut ( uuid <= 0 , 0 )      ;
  insertGroup                     (
    SC                            ,
    file                          ,
    uuid                          ,
    Types::File                   ,
    Types::Component              ,
    Groups::Contains            ) ;
  return uuid                     ;
}

bool N::CodeManager :: insertPreprocessor (
       SqlConnection & SC                 ,
       SUID            uuid               ,
       int             Type               )
{
  QString Q                          ;
  Q = SC.sql.InsertInto              (
        PlanTable(Preprocessors)        ,
        2                            ,
        "uuid"                       ,
        "type"                     ) ;
  SC . Prepare     ( Q             ) ;
  SC . Bind        ( "uuid" , uuid ) ;
  SC . Bind        ( "type" , Type ) ;
  return SC . Exec (               ) ;
}

bool N::CodeManager :: insertComponent (
       SqlConnection & SC              ,
       SUID            uuid            ,
       int             Type            ,
       int             PDL             )
{
  QString Q                          ;
  Q = SC.sql.InsertInto              (
        PlanTable(Codes)                ,
        3                            ,
        "uuid"                       ,
        "type"                       ,
        "pdl"                      ) ;
  SC . Prepare     ( Q             ) ;
  SC . Bind        ( "uuid" , uuid ) ;
  SC . Bind        ( "type" , Type ) ;
  SC . Bind        ( "pdl"  , PDL  ) ;
  return SC . Exec (               ) ;
}

UUIDs N::CodeManager :: SelectFiles (
        SqlConnection & SC          ,
        QStringList   & Filters     )
{
  UUIDs   Uuids                                 ;
  SUID    uuid                                  ;
  QString path                                  ;
  QString Q                                     ;
  int     index = 0                             ;
  int     count = 0                             ;
  do                                            {
    Q = SC.sql.SelectFrom                       (
          "uuid,file,path"                      ,
          PlanTable(SourceMaps)                    ,
          QString                               (
            "order by id asc limit %1,10000 ;"  )
          .arg(index)                         ) ;
    index += 10000                              ;
    count  =    0                               ;
    SqlLoopNow ( SC , Q )                       ;
      uuid = SC . Uuid   ( 1    )               ;
      path = SC . String ( 2    )               ;
      QFileInfo FI       ( path )               ;
      QString   suffix = FI.suffix()            ;
      suffix  = suffix . toLower  ()            ;
      if (Filters.contains(suffix))             {
        if (!Uuids.contains(uuid))              {
          Uuids << uuid                         ;
        }                                       ;
      }                                         ;
      count++                                   ;
    SqlLoopErr ( SC , Q )                       ;
    SqlLoopEnd ( SC , Q )                       ;
    if ((index%1000000)==0)                     {
      CodePlan->Talk                            (
        QString("Distill %1 files"              )
        .arg(Uuids.count())                   ) ;
    }                                           ;
  } while (count>0)                             ;
  return Uuids                                  ;
}

UUIDs N::CodeManager::ListPDLs(SqlConnection & SC)
{
  QString Q                        ;
  UUIDs   Uuids                    ;
  Q = SC.sql.SelectFrom            (
        "uuid"                     ,
        PlanTable(PDL)                ,
        "order by rank,id asc ;" ) ;
  SqlLoopNow ( SC , Q )            ;
    SUID uuid = SC . Uuid (0)      ;
    Uuids << uuid                  ;
  SqlLoopErr ( SC , Q )            ;
  SqlLoopEnd ( SC , Q )            ;
  return  Uuids                    ;
}

RMAPs N::CodeManager :: PdlIdentifications (
        SqlConnection & SC                 ,
        UUIDs         & PDLs               )
{
  QString Q                    ;
  RMAPs   rmaps                ;
  SUID    uuid                 ;
  foreach (uuid,PDLs)          {
    Q = SC.sql.SelectFrom      (
          "identification"     ,
          PlanTable(PDL)          ,
          SC.WhereUuid(uuid) ) ;
    if (SC.Fetch(Q))           {
      int id = SC . Int (0)    ;
      rmaps[uuid] = id         ;
    }                          ;
  }                            ;
  return  rmaps                ;
}

NAMEs N::CodeManager::ListExtensions(SqlConnection & SC)
{
  QString Q                            ;
  NAMEs   Names                        ;
  Q = SC.sql.SelectFrom                (
        "extensionid,name"             ,
        PlanTable(Extensions)             ,
        "order by extensionid asc ;" ) ;
  SqlLoopNow ( SC , Q )                ;
    int     eid = SC . Int    ( 0 )    ;
    QString ext = SC . String ( 1 )    ;
    if (eid>0) Names[eid] = ext        ;
  SqlLoopErr ( SC , Q )                ;
  SqlLoopEnd ( SC , Q )                ;
  return  Names                        ;
}

bool N::CodeManager :: AddPdlExtension (
       SqlConnection & SC              ,
       int             pdl             ,
       int             extension       )
{
  QString Q                                ;
  Q = SC.sql.InsertInto                    (
        PlanTable(PdlExtensions)              ,
        2                                  ,
        "pdl"                              ,
        "extension"                      ) ;
  SC . Prepare ( Q                       ) ;
  SC . Bind    ( "pdl"       , pdl       ) ;
  SC . Bind    ( "extension" , extension ) ;
  return SC . Exec ( )                     ;
}

bool N::CodeManager :: DeletePdlExtension (
       SqlConnection & SC                 ,
       int             pdl                ,
       int             extension          )
{
  QString Q ;
  Q = SC.sql.DeleteFrom                              (
        PlanTable(PdlExtensions)                        ,
        QString("where pdl = %1 and extension = %2 ;")
        .arg(pdl                                     )
        .arg(extension                           ) ) ;
  return SC . Query ( Q )                            ;
}

QStringList N::CodeManager :: Extensions (
              SqlConnection & SC         ,
              int             pdl        )
{
  QString     Q                                   ;
  QStringList L                                   ;
  CUIDs       C                                   ;
  int         i                                   ;
  Q = SC.sql.SelectFrom                           (
        "extension"                               ,
        PlanTable(PdlExtensions)                     ,
        QString("where pdl = %1 order by id asc ;")
        .arg(pdl)                               ) ;
  SqlLoopNow ( SC , Q )                           ;
    C << SC . Int ( 0 )                           ;
  SqlLoopErr ( SC , Q )                           ;
  SqlLoopEnd ( SC , Q )                           ;
  foreach (i,C)                                   {
    Q = SC.sql.SelectFrom                         (
          "name"                                  ,
          PlanTable(Extensions)                      ,
          QString("where extensionid = %1 ;"      )
          .arg(i)                               ) ;
    if (SC.Fetch(Q)) L << SC . String ( 0 )       ;
  }                                               ;
  return L                                        ;
}

UUIDs N::CodeManager::Platforms(SqlConnection & SC)
{
  return SC.Uuids(PlanTable(Platforms),"uuid","order by id asc") ;
}
