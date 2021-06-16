#include <qtfoundation.h>

#define plan VideoPlan

N::VideoManager:: VideoManager (PurePlan * p)
                : VideoPlan    (           p)
{
}

N::VideoManager::~VideoManager (void)
{
}

QStringList N::VideoManager::M3U(QString filename)
{
  QStringList files                              ;
  QFile F(filename)                              ;
  if (!F.open(QIODevice::ReadOnly)) return files ;
  QByteArray L                                   ;
  do                                             {
    L = F.readLine ()                            ;
    if (L.size()>0)                              {
      QString s = QString::fromUtf8(L)           ;
      if (s.at(0)!='#')                          {
        s = s.replace("\r","")                   ;
        s = s.replace("\n","")                   ;
        files << s                               ;
      }                                          ;
    }                                            ;
  } while (L.size()>0)                           ;
  F . close ()                                   ;
  return files                                   ;
}

TUID N::VideoManager::Duration(QString filename)
{
  QFileInfo Info (filename)                     ;
  TUID      duration   = -1                     ;
  if (!Info.exists()) return duration           ;
  AvDecoder AV                                  ;
  if (!AV.Open(filename)) return duration       ;
  if (AV.FindStream()) duration = AV.Duration() ;
  AV.Close()                                    ;
  return duration                               ;
}

UUIDs N::VideoManager::Clips(SqlConnection & Connection)
{
  QString Q                                       ;
  UUIDs   Uuids                                   ;
  Q = Connection.sql.SelectFrom                   (
        "uuid",PlanTable(Videos),"order by id desc") ;
  SqlLoopNow(Connection,Q)                        ;
    Uuids << Connection.Uuid(0)                   ;
  SqlLoopErr(Connection,Q)                        ;
  SqlLoopEnd(Connection,Q)                        ;
  return  Uuids                                   ;
}

UUIDs N::VideoManager::Clips(SqlConnection & Connection,SUID album)
{
  QString Q                                ;
  UUIDs   Uuids                            ;
  Q = Connection.sql.SelectFrom            (
        "second"                           ,
        PlanTable(AlbumGroups)                ,
          QString("where first = %1"
                  " and t1 = %2"
                  " and t2 = %3"
                  " and relation = %4"
                  " order by position asc ")
          .arg(album                       )
          .arg(Types::Album                )
          .arg(Types::Video                )
          .arg(Groups::Subordination       )
      )                                    ;
  SqlLoopNow(Connection,Q)                 ;
    Uuids << Connection.Uuid(0)            ;
  SqlLoopErr(Connection,Q)                 ;
  SqlLoopEnd(Connection,Q)                 ;
  return  Uuids                            ;
}

NAMEs N::VideoManager::ClipLists(SqlConnection & Connection)
{
  UUIDs Uuids = Clips(Connection)          ;
  NAMEs names                              ;
  SUID  uuid                               ;
  foreach (uuid,Uuids)                     {
    QStringList n = Names(Connection,uuid) ;
    if (n.count()>0) names[uuid] = n[0]    ;
  }                                        ;
  return names                             ;
}

QStringList N::VideoManager::Names(SqlConnection & Connection,SUID Uuid)
{
  QStringList names                                        ;
  UUIDs       Uuids                                        ;
  SUID        uuid                                         ;
  Uuids = Connection.Uuids(PlanTable(NameMaps),"name"         ,
              QString("where uuid = %1 order by prefer asc")
              .arg(Uuid)                                 ) ;
  foreach (uuid,Uuids)                                     {
    QString Q = Connection.sql.SelectFrom                  (
                  "name",PlanTable(Names)                     ,
                  QString("where uuid = %1").arg(uuid)   ) ;
    if (Connection.Fetch(Q)) names << Connection.String(0) ;
  }                                                        ;
  return names                                             ;
}

QString N::VideoManager::Filename(SqlConnection & Connection,SUID clip)
{
  QString filename                  ;
  QString Q                         ;
  Q = Connection.sql.SelectFrom     (
        "filename",PlanTable(Videos)   ,
        Connection.WhereUuid(clip)) ;
  if (Connection.Fetch(Q))          {
    filename = Connection.String(0) ;
  }                                 ;
  return  filename                  ;
}

QString N::VideoManager :: Filename (SUID uuid)
{
  QString filename = ""                        ;
  nKickOut ( uuid <= 0 , filename )            ;
  SqlConnection SC ( plan -> sql )             ;
  if ( SC . open ( FunctionString ) )          {
    filename = Filename          ( SC , uuid ) ;
    filename = plan -> VideoFile ( filename  ) ;
    SC . close ( )                             ;
  }                                            ;
  SC . remove ( )                              ;
  return filename                              ;
}

QSize N::VideoManager::Dimension(SqlConnection & Connection,SUID clip)
{
  QSize Dim ( 0 , 0 )                 ;
  QString Q                           ;
  Q = Connection.sql.SelectFrom       (
        "width,height",PlanTable(Videos) ,
        Connection.WhereUuid(clip))   ;
  if (Connection.Fetch(Q))            {
    Dim.setWidth (Connection.Int(0))  ;
    Dim.setHeight(Connection.Int(1))  ;
  }                                   ;
  return Dim                          ;
}

QString N::VideoManager::Find(QStringList Depots,QString filename,qint64 filesize)
{
  QFileInfo FX                                  ;
  QString   filepath = ""                       ;
  bool      exists = false                      ;
  for (int i=0;!exists && i<Depots.count();i++) {
    QDir    dp (Depots[i])                      ;
    QString f = dp.absoluteFilePath(filename)   ;
    FX.setFile(f)                               ;
    if (FX.exists())                            {
      exists = true                             ;
      filepath = f                              ;
    }                                           ;
  }                                             ;
  if ( exists) exists = (FX.size() == filesize) ;
  if (!exists) filepath = ""                    ;
  return filepath                               ;
}

bool N::VideoManager::AlbumClip(SqlConnection & Connection,SUID album,SUID clip)
{
  int     position = 0;
  QString Q ;
  Q = Connection.sql.SelectFrom (
        "position"              ,
        PlanTable(AlbumGroups)     ,
        QString("where first = %1 and t1 = %2 and t2 = %3 and relation = %4 order by position desc limit 0,1")
          .arg(album)
          .arg(Types::Album)
          .arg(Types::Video)
          .arg(Groups::Subordination)
      );
  if (Connection.Fetch(Q)) position = Connection.Int(0) ;
  position++                    ;
  Q = Connection.sql.InsertInto (
        PlanTable(AlbumGroups)     ,
          6                     ,
          "first"               ,
          "second"              ,
          "t1"                  ,
          "t2"                  ,
          "relation"            ,
          "position"          ) ;
  Connection.Prepare(Q);
  Connection.Bind("first",album);
  Connection.Bind("second",clip);
  Connection.Bind("t1",Types::Album);
  Connection.Bind("t2",Types::Video);
  Connection.Bind("relation",Groups::Subordination);
  Connection.Bind("position",position);
  return Connection.Exec() ;
}

bool N::VideoManager::RemoveClip(SqlConnection & Connection,SUID album,SUID clip)
{
  QString Q ;
  Q = Connection . sql . DeleteFrom (
        PlanTable(AlbumGroups)         ,
          QString("where first = %1 and second = %2 and t1 = %3 and t2 = %4 and relation = %5")
          .arg(album)
          .arg(clip)
          .arg(Types::Album)
          .arg(Types::Video)
          .arg(Groups::Subordination)
      );
  return Connection.Query(Q) ;
}

bool N::VideoManager::AlbumOrders(SqlConnection & Connection,SUID album,UUIDs & clips)
{
  QString Q ;
  SUID    clip ;
  int     position = 1 ;
  Q = Connection.sql.Update(
        PlanTable(AlbumGroups),
        "where first = :FIRST and second = :SECOND and t1 = :T1 and t2 = :T2 and relation = :RELATION",
        1,"position") ;
  foreach (clip,clips) {
    Connection.Prepare(Q);
    Connection.Bind("first",album);
    Connection.Bind("second",clip);
    Connection.Bind("t1",Types::Album);
    Connection.Bind("t2",Types::Video);
    Connection.Bind("relation",Groups::Subordination);
    Connection.Bind("position",position);
    Connection.Exec();
    position++;
  };
  return true ;
}

QString N::VideoManager::Location(Sql & sql,QString table,QDir & dir,SUID uuid)
{
  QString filename = ""                                      ;
  QString suffix   = ""                                      ;
  ConnectSQL(SC,sql)                                         ;
    QString Q                                                ;
    Q = SC.sql.SelectFrom("suffix",table,SC.WhereUuid(uuid)) ;
    if (SC.Fetch(Q)) suffix = SC.String(0)                   ;
    if (suffix.length()>0)                                   {
      filename = QString("%1.%2").arg(uuid).arg(suffix)      ;
      filename = dir.absoluteFilePath(filename)              ;
      if (filename.length()>0)                               {
        QFileInfo FI(filename)                               ;
        if (!FI.exists()) filename = ""                      ;
      }                                                      ;
    }                                                        ;
  DisconnectSQL(SC)                                          ;
  return filename                                            ;
}

QString N::VideoManager::Location(Sql & sql,QString table,QDir & dir,QString identifier)
{
  QString filename = ""                                             ;
  QString suffix   = ""                                             ;
  ConnectSQL(SC,sql)                                                ;
    QString Q                                                       ;
    SUID    uuid = 0                                                ;
    if (SC.sql.isSQLITE())                                          {
      Q = SC.sql.SelectFrom                                         (
            "suffix,uuid"                                           ,
            table                                                   ,
            QString("where identifier like '%1'").arg(identifier) ) ;
    } else                                                          {
      Q = SC.sql.SelectFrom                                         (
            "suffix,uuid"                                           ,
            table                                                   ,
            QString("where identifier = '%1'").arg(identifier)    ) ;
    }                                                               ;
    if (SC.Fetch(Q))                                                {
      suffix = SC.String (0)                                        ;
      uuid   = SC.Uuid   (1)                                        ;
    }                                                               ;
    if (suffix.length()>0)                                          {
      filename = QString("%1.%2").arg(uuid).arg(suffix)             ;
      filename = dir.absoluteFilePath(filename)                     ;
      if (filename.length()>0)                                      {
        QFileInfo FI(filename)                                      ;
        if (!FI.exists()) filename = ""                             ;
      }                                                             ;
    }                                                               ;
  DisconnectSQL(SC)                                                 ;
  return filename                                                   ;
}

QString N::VideoManager::Location(PurePlan & p,SUID uuid)
{
  QString filename = ""                                 ;
  QString suffix   = ""                                 ;
  ConnectSQL(SC,p.sql)                               ;
    QString Q                                           ;
    Q = SC.sql.SelectFrom                               (
          "suffix"                                      ,
          p.Tables[Tables::AudioFiles]                  ,
          SC.WhereUuid(uuid)                          ) ;
    if (SC.Fetch(Q)) suffix = SC.String(0)              ;
    if (suffix.length()>0 && uuid>0)                    {
      filename = QString("%1.%2").arg(uuid).arg(suffix) ;
      filename = p.AudioFile(filename)                  ;
      if (filename.length()>0)                          {
        QFileInfo FI(filename)                          ;
        if (!FI.exists()) filename = ""                 ;
      }                                                 ;
    }                                                   ;
  DisconnectSQL(SC)                                     ;
  return filename                                       ;
}

QString N::VideoManager::Location(PurePlan & p,QString identifier)
{
  QString filename = ""                                        ;
  QString suffix   = ""                                        ;
  ConnectSQL(SC,p.sql)                                         ;
    QString Q                                                  ;
    SUID    uuid = 0                                           ;
    if (SC.sql.isSQLITE())                                     {
      Q = SC.sql.SelectFrom                                    (
            "suffix,uuid"                                      ,
            p.Tables[Tables::AudioFiles]                       ,
       QString("where identifier like '%1'").arg(identifier) ) ;
    } else                                                     {
      Q = SC.sql.SelectFrom                                    (
            "suffix,uuid"                                      ,
            p.Tables[Tables::AudioFiles]                       ,
       QString("where identifier = '%1'").arg(identifier) )    ;
    }                                                          ;
    if (SC.Fetch(Q))                                           {
      suffix = SC.String (0)                                   ;
      uuid   = SC.Uuid   (1)                                   ;
    }                                                          ;
    if (suffix.length()>0 && uuid>0)                           {
      filename = QString("%1.%2").arg(uuid).arg(suffix)        ;
      filename = p.AudioFile(filename)                         ;
      if (filename.length()>0)                                 {
        QFileInfo FI(filename)                                 ;
        if (!FI.exists()) filename = ""                        ;
      }                                                        ;
    }                                                          ;
  DisconnectSQL(SC)                                            ;
  return filename                                              ;
}

QString N::VideoManager::Location(SqlConnection & SC,SUID uuid)
{
  QString filename = ""                               ;
  QString suffix   = ""                               ;
  QString Q                                           ;
  Q = SC.sql.SelectFrom                               (
        "suffix"                                      ,
        PlanTable(AudioFiles)                            ,
        SC.WhereUuid(uuid)                          ) ;
  if (SC.Fetch(Q)) suffix = SC.String(0)              ;
  if (suffix.length()>0 && uuid>0)                    {
    filename = QString("%1.%2").arg(uuid).arg(suffix) ;
    filename = VideoPlan -> AudioFile(filename)       ;
    if (filename.length()>0)                          {
      QFileInfo FI(filename)                          ;
      if (!FI.exists()) filename = ""                 ;
    }                                                 ;
  }                                                   ;
  return filename                                     ;
}

SUID N::VideoManager::AppendAlbum(SqlConnection & SC,int language,QString name)
{
  int     position = 0                            ;
  QString Q                                       ;
  SUID uuid = SC.Unique(PlanTable(MajorUuid),"uuid") ;
  SC.assureUuid                                   (
    PlanTable(MajorUuid)                             ,
    uuid                                          ,
    Types::Album                                ) ;
  SC.assureUuid                                   (
    PlanTable(Divisions)                             ,
    uuid                                          ,
    Types::Album                                ) ;
  SC.assureName                                   (
    PlanTable(Names)                                 ,
    uuid                                          ,
    language                                      ,
    name                                        ) ;
  /////////////////////////////////////////////////
  Q = SC.sql.SelectFrom                           (
        "position"                                ,
        PlanTable(Divisions)                         ,
        QString("where type = %1 "
                "order by position desc "
                "limit 0,1"                       )
        .arg(Types::Album                     ) ) ;
  if (SC.Fetch(Q)) position = SC.Int(0)           ;
  position++                                      ;
  /////////////////////////////////////////////////
  Q = SC.sql.Update                               (
        PlanTable(Divisions)                         ,
        SC.sql.Where(1,"uuid")                    ,
        1                                         ,
        "position"                              ) ;
  SC . Prepare ( Q                              ) ;
  SC . Bind    ( "uuid"     , uuid              ) ;
  SC . Bind    ( "position" , position          ) ;
  SC . Exec    (                                ) ;
  return uuid                                     ;
}

bool N::VideoManager::FindClipsBySize(SqlConnection & SC,qint64 s,UUIDs & u)
{
  QString Q                                     ;
  Q = SC . sql . SelectFrom                     (
        "uuid"                                  ,
        PlanTable(Videos)                       ,
        QString("where `filesize` = %1").arg(s) ,
        SC . OrderByAsc ( "id" )              ) ;
  SqlLoopNow ( SC , Q )                         ;
    SUID x = SC . Uuid ( 0 )                    ;
    if ( ! u . contains ( x ) ) u << x          ;
  SqlLoopErr ( SC , Q )                         ;
  SqlLoopEnd ( SC , Q )                         ;
  return ( u . count ( ) > 0 )                  ;
}

bool N::VideoManager::ClipSuffix (
       SqlConnection & SC        ,
       SUID            u         ,
       QString       & s         )
{
  QString Q                        ;
  Q = SC . sql . SelectFrom        (
        "suffix"                   ,
        PlanTable(Videos)          ,
        SC . WhereUuid  ( u    )   ,
        SC . OrderByAsc ( "id" ) ) ;
  if ( SC . Fetch ( Q ) )          {
    s = SC . String ( 0 )          ;
    return ( s . length ( ) > 0 )  ;
  }                                ;
  return false                     ;
}

bool N::VideoManager::ClipsFilename (
       SqlConnection & SC           ,
       QString         f            ,
       UUIDs         & u            )
{
  QFileInfo F ( f )                                    ;
  UUIDs     U                                          ;
  //////////////////////////////////////////////////////
  if ( ! F . exists ( )                 ) return false ;
  qint64 s = F . size ( )                              ;
  if ( s <= 0                           ) return false ;
  if ( ! FindClipsBySize ( SC , s , U ) ) return false ;
  //////////////////////////////////////////////////////
  SUID x                                               ;
  foreach ( x , U )                                    {
    if ( ! u . contains ( x ) )                        {
      QString suffix                                   ;
      if ( ClipSuffix ( SC , x , suffix ) )            {
        QString r = F . suffix ( )                     ;
        suffix = suffix . toLower ( )                  ;
        r      = r      . toLower ( )                  ;
        if ( r == suffix ) u << x                      ;
      }                                                ;
    }                                                  ;
  }                                                    ;
  //////////////////////////////////////////////////////
  return ( u . count ( ) > 0 )                         ;
}

bool N::VideoManager::ClipToAlbums (
       SqlConnection & SC          ,
       SUID            clip        ,
       UUIDs         & albums      )
{
  QString Q                              ;
  Q = SC . sql . SelectFrom              (
        "first"                          ,
        PlanTable(AlbumGroups)           ,
          QString("where `second` = %1"
                  " and `t1` = %2"
                  " and `t2` = %3"
                  " and `relation` = %4" )
          .arg(clip                      )
          .arg(Types::Album              )
          .arg(Types::Video              )
          .arg(Groups::Subordination     )
      )                                  ;
  SqlLoopNow ( SC , Q )                  ;
    SUID u = SC . Uuid ( 0 )             ;
    if ( ! albums . contains ( u ) )     {
      albums << u                        ;
    }                                    ;
  SqlLoopErr ( SC , Q )                  ;
  SqlLoopEnd ( SC , Q )                  ;
  return ( albums . count ( ) > 0 )      ;
}

bool N::VideoManager::ClipsToAlbums (
       SqlConnection & SC           ,
       UUIDs         & clips        ,
       UUIDs         & albums       )
{
  SUID c                             ;
  foreach ( c , clips )              {
    ClipToAlbums ( SC , c , albums ) ;
  }                                  ;
  return ( albums . count ( ) > 0 )  ;
}

bool N::VideoManager::ImageToAlbums (
       SqlConnection & SC           ,
       SUID            image        ,
       UUIDs         & albums       )
{
  QString Q                              ;
  Q = SC . sql . SelectFrom              (
        "first"                          ,
        PlanTable(AlbumGroups)           ,
          QString("where `second` = %1"
                  " and `t1` = %2"
                  " and `t2` = %3"
                  " and `relation` = %4" )
          .arg(image                     )
          .arg(Types::Album              )
          .arg(Types::Picture            )
          .arg(Groups::Subordination     )
      )                                  ;
  SqlLoopNow ( SC , Q )                  ;
    SUID u = SC . Uuid ( 0 )             ;
    if ( ! albums . contains ( u ) )     {
      albums << u                        ;
    }                                    ;
  SqlLoopErr ( SC , Q )                  ;
  SqlLoopEnd ( SC , Q )                  ;
  return ( albums . count ( ) > 0 )      ;
}

bool N::VideoManager::ImagesToAlbums (
       SqlConnection & SC            ,
       UUIDs         & images        ,
       UUIDs         & albums        )
{
  SUID i                              ;
  foreach ( i , images )              {
    ImageToAlbums ( SC , i , albums ) ;
  }                                   ;
  return ( albums . count ( ) > 0 )   ;
}
