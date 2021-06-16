#include <qtfoundation.h>

#define plan       NetPlan
#define TYPE(T)    Types::T
#define UuidAndLanguage(language,uuid) QString("where uuid = %1 and language = %2 limit 0,1").arg(uuid).arg(language)
#define OrderIdAsc  "order by id asc"
#define OrderIdDesc "order by id desc"

N::NetworkManager:: NetworkManager ( PurePlan * P )
                  : NetPlan        (            P )
{
}

N::NetworkManager::~NetworkManager(void)
{
}

bool N::NetworkManager::toURLs(QStringList & URIs,QList<QUrl> & URLs)
{
  for (int i=0;i<URIs.count();i++)          {
    QString v    = URIs[i]                  ;
    bool    prot = false                    ;
    v = v.replace("\r","")                  ;
    v = v.replace("\n","")                  ;
    if (v.contains("http://" )) prot = true ;
    if (v.contains("https://")) prot = true ;
    if (v.contains("ftp://"  )) prot = true ;
    if (!prot) v.prepend("http://")         ;
    QUrl url(v)                             ;
    if (url.isValid())                      {
      URLs << url                           ;
    }                                       ;
  }                                         ;
  return ( URLs . count ( ) > 0 )           ;
}

QString N::NetworkManager :: Name       (
          SqlConnection    & Connection ,
          SUID               uuid       )
{
  QString name = ""                                     ;
  QString Q    = Connection.sql.SelectFrom              (
        "name"                                          ,
        PlanTable(Names)                                   ,
        QString("where uuid = %1 limit 0,1").arg(uuid)) ;
  if (Connection.Fetch(Q)) name = Connection.String(0)  ;
  return name                                           ;
}

QString N::NetworkManager :: Name       (
          SqlConnection    & Connection ,
          SUID               uuid       ,
          int                language   )
{
  QString name = ""                        ;
  QString Q    = Connection.sql.SelectFrom (
        "name"                             ,
        PlanTable(Names)                      ,
        UuidAndLanguage(language,uuid) )   ;
  if (Connection.Fetch(Q))                 {
    name = Connection.String(0)            ;
  }                                        ;
  return name                              ;
}

bool N::NetworkManager::LoadNations(SqlConnection & Connection)
{
  QString Q                                    ;
  Q = Connection.sql.SelectFrom                (
        "country,uuid"                         ,
        PlanTable(Nations)                        ,
        OrderIdAsc            )                ;
  SqlLoopNow ( Connection , Q )                ;
    int  country = Connection . Int  (0)       ;
    SUID u       = Connection . Uuid (1)       ;
    Countries [ country ] = u                  ;
  SqlLoopErr ( Connection , Q )                ;
  SqlLoopEnd ( Connection , Q )                ;
  UUIDs Uuids = Countries . values ( )         ;
  SUID  u                                      ;
  int   language = NetPlan->LanguageId         ;
  foreach (u,Uuids)                            {
    QString name = Name(Connection,u,language) ;
    Nations [ u ] = name                       ;
  }                                            ;
  return false                                 ;
}

bool N::NetworkManager::LoadTLDs(SqlConnection & Connection)
{
  QString Q                                ;
  Q = Connection.sql.SelectFrom            (
        "id,uuid,country,tld,nic,comment"  ,
        PlanTable(TLD)                        ,
        OrderIdAsc                       ) ;
  SqlLoopNow ( Connection , Q            ) ;
    int     id      = Connection.Int   (0) ;
    SUID    uuid    = Connection.Uuid  (1) ;
    int     country = Connection.Int   (2) ;
    QString tld     = Connection.String(3) ;
    QString nic     = Connection.String(4) ;
    QString comment = Connection.String(5) ;
    TLDs                << tld             ;
    TldIDs     [ uuid ]  = id              ;
    TldUuids   [ tld  ]  = uuid            ;
    TldNations [ uuid ]  = country         ;
    TldNames   [ uuid ]  = tld             ;
    NICs       [ uuid ]  = nic             ;
    Comments   [ uuid ]  = comment         ;
  SqlLoopErr ( Connection , Q            ) ;
  SqlLoopEnd ( Connection , Q            ) ;
  return false                             ;
}

bool N::NetworkManager::LoadSLDs(SqlConnection & Connection)
{
  QString Q                             ;
  Q = Connection.sql.SelectFrom         (
        "id,uuid,tld,gtld"              ,
        PlanTable(GTLD)                    ,
        OrderIdAsc            )         ;
  SqlLoopNow ( Connection , Q )         ;
    int     id   = Connection.Int   (0) ;
    SUID    uuid = Connection.Uuid  (1) ;
    SUID    tld  = Connection.Uuid  (2) ;
    QString sld  = Connection.String(3) ;
    SLDs              << sld            ;
    SldTLDs  [ uuid ]  = tld            ;
    SldIDs   [ uuid ]  = id             ;
    SldUuids [ sld  ]  = uuid           ;
  SqlLoopErr ( Connection , Q )         ;
  SqlLoopEnd ( Connection , Q )         ;
  return false                          ;
}

void N::NetworkManager::ClearIndex(void)
{
  TLDs       . clear ( ) ;
  SLDs       . clear ( ) ;
  SldTLDs    . clear ( ) ;
  TldUuids   . clear ( ) ;
  SldUuids   . clear ( ) ;
  TldIDs     . clear ( ) ;
  SldIDs     . clear ( ) ;
  TldNations . clear ( ) ;
  Countries  . clear ( ) ;
  TldNames   . clear ( ) ;
  SldNames   . clear ( ) ;
  Nations    . clear ( ) ;
  NICs       . clear ( ) ;
  Comments   . clear ( ) ;
}

bool N::NetworkManager::LoadDomainIndex(SqlConnection & Connection)
{
  ClearIndex  (                 ) ;
  LoadNations ( Connection      ) ;
  LoadTLDs    ( Connection      ) ;
  LoadSLDs    ( Connection      ) ;
  return ( TLDs . count ( ) > 0 ) ;
}

int N::NetworkManager::SiteTotal(SqlConnection & Connection)
{
  int     total = 0             ;
  QString Q                     ;
  Q = Connection.sql.SelectFrom (
        "count(*)"              ,
        PlanTable(SiteDepot)       ,
        ""                    ) ;
  if (Connection.Fetch(Q))      {
    total = Connection . Int(0) ;
  }                             ;
  return total                  ;
}

int N::NetworkManager :: TldCounts  (
      SqlConnection    & Connection ,
      SUID               tld        )
{
  int     total = 0             ;
  QString Q                     ;
  Q = Connection.sql.SelectFrom (
        "count(*)"              ,
        PlanTable(GTLD)            ,
        QString("where tld = %1")
        .arg(tld)             ) ;
  if (Connection.Fetch(Q))      {
    total = Connection . Int(0) ;
  }                             ;
  return total                  ;
}

int N::NetworkManager :: TldTotal   (
      SqlConnection    & Connection ,
      SUID               tld        )
{
  int     total = 0             ;
  QString Q                     ;
  Q = Connection.sql.SelectFrom (
        "count(*)"              ,
        PlanTable(SitesTld)        ,
        QString("where tld = %1")
        .arg(tld)             ) ;
  if (Connection.Fetch(Q))      {
    total = Connection . Int(0) ;
  }                             ;
  return total                  ;
}

int N::NetworkManager :: SldCounts  (
      SqlConnection    & Connection ,
      SUID               sld        )
{
  int     total = 0             ;
  QString Q                     ;
  Q = Connection.sql.SelectFrom (
        "count(*)"              ,
        PlanTable(SitesSld)        ,
        QString("where sld = %1")
        .arg(sld)             ) ;
  if (Connection.Fetch(Q))      {
    total = Connection . Int(0) ;
  }                             ;
  return total                  ;
}

TUID N::NetworkManager :: SiteId     (
       SqlConnection    & Connection ,
       SUID               site       )
{
  TUID tuid = 0                      ;
  QString Q                          ;
  Q = Connection.sql.SelectFrom      (
        "id"                         ,
        PlanTable(Sites)                ,
        Connection.WhereUuid(site) ) ;
  if (Connection.Fetch(Q))           {
    tuid = Connection.Tuid(0)        ;
  }                                  ;
  return tuid                        ;
}

SUID N::NetworkManager::LastestTld(SqlConnection & Connection)
{
  SUID luid = 0                              ;
  QString Q                                  ;
  Q = Connection.sql.SelectFrom              (
        "site"                               ,
        PlanTable(SitesTld)                     ,
        QString ( "%1 %2"                    )
        . arg ( Connection.OrderByDesc("id") )
        . arg ( Connection.sql.Limit(0,1)) ) ;
  if (Connection.Fetch(Q))                   {
    luid = Connection.Uuid(0)                ;
  }                                          ;
  return luid                                ;
}

SUID N::NetworkManager::LastestSld(SqlConnection & Connection)
{
  SUID luid = 0                              ;
  QString Q                                  ;
  Q = Connection.sql.SelectFrom              (
        "site"                               ,
        PlanTable(SitesSld)                     ,
        QString ( "%1 %2"                    )
        . arg ( Connection.OrderByDesc("id") )
        . arg ( Connection.sql.Limit(0,1)) ) ;
  if (Connection.Fetch(Q))                   {
    luid = Connection.Uuid(0)                ;
  }                                          ;
  return luid                                ;
}

bool N::NetworkManager :: InsertTldMap (
       SqlConnection    & Connection   ,
       SUID               site         ,
       SUID               tld          )
{
  QString Q                            ;
  Q = Connection.sql.InsertInto        (
        PlanTable(SitesTld)               ,
        2                              ,
        "site"                         ,
        "tld"                        ) ;
  Connection . Prepare ( Q           ) ;
  Connection . Bind    ( "site",site ) ;
  Connection . Bind    ( "tld" ,tld  ) ;
  return Connection.Exec()             ;
}

bool N::NetworkManager :: InsertSldMap (
       SqlConnection    & Connection   ,
       SUID               site         ,
       SUID               sld          )
{
  QString Q                            ;
  Q = Connection.sql.InsertInto        (
        PlanTable(SitesSld)               ,
        2                              ,
        "site"                         ,
        "sld"                        ) ;
  Connection . Prepare ( Q           ) ;
  Connection . Bind    ( "site",site ) ;
  Connection . Bind    ( "sld" ,sld  ) ;
  return Connection.Exec()             ;
}

QString N::NetworkManager::Reverse(QString name)
{
  int     len = name . length () ;
  QString R                      ;
  while (len>0)                  {
    len --                       ;
    R . append ( name.at(len) )  ;
  }                              ;
  return  R                      ;
}

void N::NetworkManager::PrepareDomainMap (void)
{
  QString LDN                       ;
  QString ldn                       ;
  RLDs     . clear()                ;
  UuidRlds . clear()                ;
  foreach (LDN,SLDs)                {
    ldn  = "."                      ;
    ldn += LDN                      ;
    ldn  = Reverse(ldn)             ;
    ldn  = ldn.toLower ()           ;
    RLDs           << ldn           ;
    UuidRlds [ldn]  = SldUuids[LDN] ;
  }                                 ;
  foreach (LDN,TLDs)                {
    ldn  = "."                      ;
    ldn += LDN                      ;
    ldn  = Reverse(ldn)             ;
    ldn  = ldn.toLower ()           ;
    RXDs           << ldn           ;
    UuidRxds [ldn]  = TldUuids[LDN] ;
  }                                 ;
}

SUID N::NetworkManager::UrlTLD(QUrl & url)
{
  QString host  = url.host (      ) ;
  QString rhost = Reverse  ( host ) ;
  QString ldn                       ;
  foreach (ldn,RXDs)                {
    int     len   = ldn.length(   ) ;
    QString chost = rhost.left(len) ;
    chost = chost . toLower   (   ) ;
    if (ldn==chost)                 {
      return UuidRxds[ldn]          ;
    }                               ;
  }                                 ;
  return 0                          ;
}

SUID N::NetworkManager::UrlSLD(QUrl & url)
{
  QString host  = url.host (      ) ;
  QString rhost = Reverse  ( host ) ;
  QString ldn                       ;
  foreach (ldn,RLDs)                {
    int     len   = ldn.length(   ) ;
    QString chost = rhost.left(len) ;
    chost = chost . toLower   (   ) ;
    if (ldn==chost)                 {
      return UuidRlds[ldn]          ;
    }                               ;
  }                                 ;
  return 0                          ;
}

SUID N::NetworkManager :: DomainUuid (
       SqlConnection    & Connection ,
       QUrl             & url        )
{
  SUID    site = 0                                 ;
  QString host = url.host ( )                      ;
  QString rhost                                    ;
  QString Q                                        ;
  if (host.length()<=0) return 0                   ;
  host = host.toLower ( )                          ;
  rhost = Reverse     ( host )                     ;
  if (rhost.length()<=0) return 0                  ;
  QByteArray R = rhost.toUtf8()                    ;
  Q = Connection.sql.SelectFrom                    (
        "uuid"                                     ,
        PlanTable(Domains)                            ,
        Connection.sql.Where(2,"length","rindex")) ;
  Connection . Prepare ( Q                       ) ;
  Connection . Bind    ( "length",R    .size  () ) ;
  Connection . Bind    ( "rindex",R              ) ;
  IfSqlExec ( Connection )                         {
    site = Connection . Uuid ( 0 )                 ;
  }                                                ;
  return site                                      ;
}

SUID N::NetworkManager :: InsertDomain (
       SqlConnection    & Connection   ,
       QUrl             & url          )
{
  SUID    site = 0                                    ;
  QString host = url.host ( )                         ;
  QString rhost                                       ;
  QString Q                                           ;
  host  = host.toLower (      )                       ;
  nKickOut ( host  . length() <= 0 , 0 )              ;
  rhost = Reverse      ( host )                       ;
  nKickOut ( rhost . length() <= 0 , 0 )              ;
  QByteArray R = rhost.toUtf8()                       ;
  site  = Connection . Unique                         (
            PlanTable(MajorUuid)                         ,
            "uuid"                                  ) ;
  Connection.assureUuid                               (
    PlanTable(MajorUuid)                                 ,
    site                                              ,
    TYPE(InternetDomain)                            ) ;
  Q = Connection.sql.InsertInto                       (
        PlanTable(Domains)                               ,
        4                                             ,
        "uuid"                                        ,
        "length"                                      ,
        "domain"                                      ,
        "rindex"                                    ) ;
  Connection.Prepare( Q                             ) ;
  Connection.Bind   ( "uuid"   , site               ) ;
  Connection.Bind   ( "length" , R     . size   ( ) ) ;
  Connection.Bind   ( "domain" , host  . toUtf8 ( ) ) ;
  Connection.Bind   ( "rindex" , R                  ) ;
  Connection.Exec   (                               ) ;
  return site                                         ;
}

SUID N::NetworkManager :: UrlUuid    (
       SqlConnection    & Connection ,
       QUrl             & url        )
{
  SUID    site = 0                                ;
  QString host = url.host ( )                     ;
  QString rhost                                   ;
  QString Q                                       ;
  /////////////////////////////////////////////////
  nKickOut ( host  . length() <= 0 , 0 )          ;
  host = host.toLower ( )                         ;
  rhost = Reverse     ( host )                    ;
  nKickOut ( rhost . length() <= 0 , 0 )          ;
  /////////////////////////////////////////////////
  QByteArray R = rhost.toUtf8()                   ;
  Q = Connection.sql.SelectFrom                   (
        "uuid"                                    ,
        PlanTable(SiteDepot)                         ,
        Connection.sql.Where                      (
          2                                       ,
          "length"                                ,
          "rindex"                            ) ) ;
  Connection . Prepare ( Q                      ) ;
  Connection . Bind    ( "length" , R . size () ) ;
  Connection . Bind    ( "rindex" , R           ) ;
  IfSqlExec(Connection)                           {
    site = Connection . Uuid ( 0 )                ;
  }                                               ;
  /////////////////////////////////////////////////
  return site                                     ;
}

SUID N::NetworkManager :: UrlUuidCache (
       SqlConnection    & Connection   ,
       QUrl             & url          )
{
  QString host = url.host ( )      ;
  SUID    site = 0                 ;
  host = host.toLower()            ;
  if (SitesCache.contains(host))   {
    site = SitesCache[host]        ;
  } else                           {
    site = UrlUuid(Connection,url) ;
    if (site>0)                    {
      SitesCache[host] = site      ;
    }                              ;
  }                                ;
  return site                      ;
}

SUID N::NetworkManager :: InsertUrl  (
       SqlConnection    & Connection ,
       QUrl             & url        )
{
  SUID    site = 0                                 ;
  QString host = url.host ( )                      ;
  QString rhost                                    ;
  QString Q                                        ;
  //////////////////////////////////////////////////
  host  = host.toLower (      )                    ;
  nKickOut ( host  . length() <= 0 , 0 )           ;
  rhost = Reverse      ( host )                    ;
  nKickOut ( rhost . length() <= 0 , 0 )           ;
  //////////////////////////////////////////////////
  QByteArray R = rhost.toUtf8()                    ;
  site  = Connection . Unique                      (
            PlanTable(MajorUuid)                      ,
            "uuid"                               ) ;
  //////////////////////////////////////////////////
  if (!Connection.assureUuid                       (
         PlanTable(MajorUuid)                         ,
         site                                      ,
         TYPE(DomainName)                      ) ) {
    return 0                                       ;
  }                                                ;
  //////////////////////////////////////////////////
  bool correct = true                              ;
  Q = Connection.sql.InsertInto                    (
        PlanTable(SiteDepot)                          ,
        4                                          ,
        "uuid"                                     ,
        "length"                                   ,
        "url"                                      ,
        "rindex"                                 ) ;
  Connection.Prepare( Q                          ) ;
  Connection.Bind   ( "uuid"   , site            ) ;
  Connection.Bind   ( "length" , R    . size  () ) ;
  Connection.Bind   ( "url"    , host . toUtf8() ) ;
  Connection.Bind   ( "rindex" , R               ) ;
  if ( ! Connection.Exec ( ) ) correct = false     ;
  //////////////////////////////////////////////////
  Q = Connection.sql.InsertInto                    (
        PlanTable(Sites)                              ,
        2                                          ,
        "uuid"                                     ,
        "length"                                 ) ;
  Connection.Prepare( Q                          ) ;
  Connection.Bind   ( "uuid"   , site            ) ;
  Connection.Bind   ( "length" , R    . size  () ) ;
  if ( ! Connection.Exec ( ) ) correct = false     ;
  //////////////////////////////////////////////////
  return correct ? site : 0                        ;
}

SUID N::NetworkManager :: NewBookmark (
       SqlConnection    & Connection  ,
       QString            name        )
{
  SUID uuid                  ;
  uuid = Connection.Unique   (
           PlanTable(MajorUuid) ,
           "uuid"          ) ;
  Connection.assureUuid      (
    PlanTable(MajorUuid)        ,
    uuid                     ,
    TYPE(Division)         ) ;
  Connection.assureUuid      (
    PlanTable(Divisions)        ,
    uuid                     ,
    TYPE(URL)              ) ;
  Connection.assureName      (
    PlanTable(Names)            ,
    uuid                     ,
    NetPlan->LanguageId      ,
    name                   ) ;
  return uuid                ;
}

bool N::NetworkManager :: assureName (
       SqlConnection    & Connection ,
       SUID               uuid       ,
       int                language   ,
       QString            name       )
{
  return Connection.assureName (
           PlanTable(Names)       ,
           uuid                ,
           language            ,
           name              ) ;
}

SUID N::NetworkManager :: insertPage (
       SqlConnection    & Connection ,
       QUrl             & url        )
{
  SUID    site   = 0                                      ;
  SUID    ruid   = 0                                      ;
  QString host   = url . host ( ) . toLower ( )           ;
  QString path   = URI ( url )                            ;
  QString scheme = url . scheme ( )                       ;
  QString page                                            ;
  QString Q                                               ;
  /////////////////////////////////////////////////////////
  if (path.length()<=0) path = "/"                        ;
  if (scheme.length()<=0) scheme = "http"                 ;
  page = host + path                                      ;
  if (page . length ( ) <= 0) return false                ;
  site = UrlUuid(Connection,url)                          ;
  if (site<=0) site = InsertUrl(Connection,url)           ;
  if (site<=0) return 0                                   ;
  /////////////////////////////////////////////////////////
  SUID TLD = UrlTLD ( url )                               ;
  SUID SLD = UrlSLD ( url )                               ;
  /////////////////////////////////////////////////////////
  ruid = Connection.Unique(PlanTable(MajorUuid),"uuid")      ;
  Connection.assureUuid                                   (
    PlanTable(MajorUuid)                                     ,
    ruid                                                  ,
    TYPE(URL)                                           ) ;
  /////////////////////////////////////////////////////////
  Q = Connection.sql.InsertInto                           (
        PlanTable(WwwPages)                                 ,
        2                                                 ,
        "uuid"                                            ,
        "page"                                          ) ;
  Connection . Prepare ( Q                              ) ;
  Connection . Bind    ( "uuid"     , ruid              ) ;
  Connection . Bind    ( "page"     , page   . toUtf8() ) ;
  Connection . Exec    (                                ) ;
  /////////////////////////////////////////////////////////
  Q = Connection.sql.InsertInto                           (
        PlanTable(WwwURIs)                                   ,
        2                                                 ,
        "uuid"                                            ,
        "uri"                                           ) ;
  Connection . Prepare ( Q                              ) ;
  Connection . Bind    ( "uuid"     , ruid              ) ;
  Connection . Bind    ( "uri"      , path   . toUtf8() ) ;
  Connection . Exec    (                                ) ;
  /////////////////////////////////////////////////////////
  Q = Connection.sql.InsertInto                           (
        PlanTable(SitePages)                                 ,
        2                                                 ,
        "site"                                            ,
        "page"                                          ) ;
  Connection . Prepare ( Q                              ) ;
  Connection . Bind    ( "page"     , ruid              ) ;
  Connection . Bind    ( "site"     , site              ) ;
  Connection . Exec    (                                ) ;
  /////////////////////////////////////////////////////////
  Q = Connection.sql.InsertInto                           (
        PlanTable(WebPages)                                  ,
        1                                                 ,
        "uuid"                                          ) ;
  Connection . Prepare ( Q                              ) ;
  Connection . Bind    ( "uuid"     , ruid              ) ;
  Connection . Exec    (                                ) ;
  /////////////////////////////////////////////////////////
  Q = Connection.sql.InsertInto                           (
        PlanTable(PageProtocols)                             ,
        2                                                 ,
        "uuid"                                            ,
        "protocol"                                      ) ;
  Connection . Prepare ( Q                              ) ;
  Connection . Bind    ( "uuid"     , ruid              ) ;
  Connection . Bind    ( "protocol" , scheme . toUtf8() ) ;
  Connection . Exec    (                                ) ;
  /////////////////////////////////////////////////////////
  Q = Connection.sql.InsertInto                           (
        PlanTable(PageTLDs)                                  ,
        2                                                 ,
        "uuid"                                            ,
        "tld"                                           ) ;
  Connection . Prepare ( Q                              ) ;
  Connection . Bind    ( "uuid"     , ruid              ) ;
  Connection . Bind    ( "tld"      , TLD               ) ;
  Connection . Exec    (                                ) ;
  /////////////////////////////////////////////////////////
  Q = Connection.sql.InsertInto                           (
        PlanTable(PageSLDs)                                  ,
        2                                                 ,
        "uuid"                                            ,
        "sld"                                           ) ;
  Connection . Prepare ( Q                              ) ;
  Connection . Bind    ( "uuid"     , ruid              ) ;
  Connection . Bind    ( "sld"      , SLD               ) ;
  Connection . Exec    (                                ) ;
  /////////////////////////////////////////////////////////
  return ruid                                             ;
}

NAMEs N::NetworkManager::Bookmarks(SqlConnection & Connection)
{
  NAMEs Name                                               ;
  UUIDs Uuids                                              ;
  SUID  uuid                                               ;
  //////////////////////////////////////////////////////////
  Uuids = Connection.Uuids                                 (
            PlanTable(Divisions)                              ,
            "uuid"                                         ,
            QString("where type = %1 order by position asc")
            .arg( TYPE ( URL ) )                         ) ;
  //////////////////////////////////////////////////////////
  foreach (uuid,Uuids)                                     {
    QString name = Connection.getName                      (
                     PlanTable(Names)                         ,
                     "uuid"                                ,
                     NetPlan->LanguageId                   ,
                     uuid                                ) ;
    Name[uuid] = name                                      ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  return Name                                              ;
}

QUrl N::NetworkManager :: PageUrl    (
       SqlConnection    & Connection ,
       SUID               uuid       )
{
  QString Q                                   ;
  QString page   = ""                         ;
  QString scheme = ""                         ;
  Q = Connection.sql.SelectFrom               (
        "page"                                ,
        PlanTable(WwwPages)                      ,
        Connection.WhereUuid(uuid)          ) ;
  if (Connection.Fetch(Q))                    {
    page   = Connection . String ( 0 )        ;
  }                                           ;
  Q = Connection.sql.SelectFrom               (
        "protocol"                            ,
        PlanTable(PageProtocols)                 ,
        Connection.WhereUuid(uuid)          ) ;
  if (Connection.Fetch(Q))                    {
    scheme = Connection . String ( 0 )        ;
  }                                           ;
  if (scheme.length()<=0)                     {
    page = "http"   "://" + page              ;
  } else                                      {
    page = scheme + "://" + page              ;
  }                                           ;
  page = page.replace("\n","")                ;
  page = page.replace("\r","")                ;
  return QUrl ( page )                        ;
}

QString N::NetworkManager :: PageURI    (
          SqlConnection    & Connection ,
          SUID               uuid       )
{
  QString Q                         ;
  QString uri = ""                  ;
  Q = Connection.sql.SelectFrom     (
        "uri"                       ,
        PlanTable(WwwURIs)             ,
        Connection.WhereUuid(uuid)) ;
  if (Connection.Fetch(Q))          {
    uri = Connection . String ( 0 ) ;
  }                                 ;
  return uri                        ;
}

SUID N::NetworkManager :: PageUuid   (
       SqlConnection    & Connection ,
       QUrl             & url        )
{
  SUID    uuid = 0                            ;
  QString host = url . host ( ) . toLower ( ) ;
  QString path = URI ( url )                  ;
  QString page                                ;
  QString Q                                   ;
  if (path.length()<=0) path = "/"            ;
  page = host + path                          ;
  if (page . length ( ) <= 0) return 0        ;
  Q = Connection.sql.SelectFrom               (
        "uuid"                                ,
        PlanTable(WwwPages)                      ,
        Connection.sql.Where(1,"page")        ,
        Connection.OrderByDesc("id")          ,
        Connection.sql.Limit(0,1)           ) ;
  Connection.Prepare(Q                      ) ;
  Connection.Bind   ("page",page.toUtf8()   ) ;
  IfSqlExec(Connection)                       {
    uuid  = Connection . Uuid   ( 0 )         ;
  } else                                      {
    NetPlan->Debug(55,QString("[%1] not found").arg(page) );
  }                                           ;
  return uuid                                 ;
}

SUID N::NetworkManager :: PageUuid   (
       SqlConnection    & Connection ,
       QString            Page       )
{
  QString Q                             ;
  Q = Connection.sql.SelectFrom         (
        "uuid"                          ,
        PlanTable(WwwPages)                ,
        Connection.sql.Where(1,"page")  ,
        Connection.OrderByDesc("id")    ,
        Connection.sql.Limit(0,1)     ) ;
  Connection.Prepare(Q)                 ;
  Connection.Bind("page",Page.toUtf8()) ;
  IfSqlExec(Connection)                 {
    return Connection.Uuid(0)           ;
  } else                                {
    NetPlan->Debug(55,QString("[%1] not found").arg(Page) );
  }                                     ;
  return 0                              ;
}

bool N::NetworkManager :: PageUuids   (
       SqlConnection    & Connection  ,
       QList<QUrl>      & URLs        ,
       UUIDs            & Uuids       ,
       bool               processEvent)
{
  for (int i=0;i<URLs.count();i++)  {
    QUrl u = URLs[i]                ;
    SUID p = PageUuid(Connection,u) ;
    if (p>=0)                       {
      if (!Uuids.contains(p))       {
        Uuids << p                  ;
      }                             ;
    }                               ;
    if (processEvent)               {
      NetPlan->processEvents()      ;
    }                               ;
  }                                 ;
  return ( Uuids.count() > 0 )      ;
}

bool N::NetworkManager :: PageLike   (
       SqlConnection    & Connection ,
       QString            Where      ,
       QString            Page       ,
       QString            Item       ,
       UUIDs            & Uuids      ,
       bool             & Keep       )
{
  QString Q                                     ;
  Q = Connection.sql.SelectFrom                 (
        "uuid"                                  ,
        PlanTable(WwwPages)                        ,
        Where                                 ) ;
  Connection . Prepare ( Q                    ) ;
  Connection . Bind    ( Item , Page.toUtf8() ) ;
  if (Connection.Exec())                        {
    while (Keep && Connection.Next())           {
      SUID uuid = Connection.Uuid(0)            ;
      if (!Uuids.contains(uuid)) Uuids << uuid  ;
    }                                           ;
  }                                             ;
  return ( Uuids.count() > 0 )                  ;
}

SUID N::NetworkManager :: assurePage (
       SqlConnection    & Connection ,
       QUrl             & url        )
{
  SUID uuid = PageUuid (Connection,url) ;
  if (uuid>0) return uuid               ;
  return insertPage    (Connection,url) ;
}

QString N::NetworkManager :: PageTitle  (
          SqlConnection    & Connection ,
          SUID               uuid       ,
          TUID               publish    )
{
  QString Q                                          ;
  QString title = ""                                 ;
  Q = Connection.sql.SelectFrom                      (
        "title"                                      ,
        PlanTable(PageTitles)                           ,
        QString ( "where uuid = %1 and publish = %2" )
        .arg(uuid).arg(publish)                    ) ;
  if (Connection.Fetch(Q))                           {
    title = Connection . String ( 0 )                ;
  }                                                  ;
  return title                                       ;
}

bool N::NetworkManager :: assureTitle (
       SqlConnection    & Connection  ,
       SUID               uuid        ,
       TUID               publish     ,
       QString            title       )
{
  QString Q                                                  ;
  ////////////////////////////////////////////////////////////
  Q = Connection.sql.ReplaceInto                             (
        PlanTable(PageTitles)                                   ,
        3                                                    ,
        "uuid"                                               ,
        "publish"                                            ,
        "title"                                            ) ;
  ////////////////////////////////////////////////////////////
  Connection        . Prepare ( Q                          ) ;
  Connection        . Bind    ( "uuid"    , uuid           ) ;
  Connection        . Bind    ( "publish" , publish        ) ;
  Connection        . Bind    ( "title"   , title.toUtf8() ) ;
  return Connection . Exec    (                            ) ;
}

int N::NetworkManager :: HttpStatus (
      SqlConnection    & Connection ,
      SUID               uuid       )
{
  int     retcode = 0                ;
  QString Q                          ;
  Q = Connection.sql.SelectFrom      (
        "retcode"                    ,
        PlanTable(HttpStatus)           ,
        Connection.WhereUuid(uuid) ) ;
  if (Connection.Fetch(Q))           {
    retcode = Connection . Int ( 0 ) ;
  }                                  ;
  return retcode                     ;
}

bool N::NetworkManager :: assureHttpStatus (
       SqlConnection    & Connection       ,
       SUID               uuid             ,
       int                retcode          )
{
  QString Q                                           ;
  /////////////////////////////////////////////////////
  Q = Connection.sql.ReplaceInto                      (
        PlanTable(HttpStatus)                            ,
        2                                             ,
        "uuid"                                        ,
        "retcode"                                   ) ;
  /////////////////////////////////////////////////////
  Connection        . Prepare ( Q                   ) ;
  Connection        . Bind    ( "uuid"    , uuid    ) ;
  Connection        . Bind    ( "retcode" , retcode ) ;
  return Connection . Exec    (                     ) ;
}

bool N::NetworkManager :: FromPage   (
       SqlConnection    & Connection ,
       SUID               page       ,
       UUIDs            & Uuids      )
{
  QString Q                                        ;
  Q = Connection.sql.SelectFrom                    (
        "uuid"                                     ,
        PlanTable(PageParent)                         ,
        QString("where fromPage = %1").arg(page) ) ;
  SqlLoopNow ( Connection , Q )                    ;
    SUID uuid = Connection . Uuid ( 0 )            ;
    if (!Uuids.contains(uuid))                     {
      Uuids << uuid                                ;
    }                                              ;
  SqlLoopErr ( Connection , Q )                    ;
  SqlLoopEnd ( Connection , Q )                    ;
  return ( Uuids . count ( ) > 0 )                 ;
}

bool N::NetworkManager :: FromPage   (
       SqlConnection    & Connection ,
       SUID               page       ,
       UUIDs            & Uuids      ,
       bool             & KeepGo     )
{
  QString Q                                        ;
  Q = Connection.sql.SelectFrom                    (
        "uuid"                                     ,
        PlanTable(PageParent)                         ,
        QString("where fromPage = %1").arg(page) ) ;
  if (Connection.Query(Q))                         {
    while (KeepGo && Connection.Next())            {
      SUID uuid = Connection.Uuid(0)               ;
      if (!Uuids.contains(uuid)) Uuids << uuid     ;
    }                                              ;
  }                                                ;
  return ( Uuids . count ( ) > 0 )                 ;
}

bool N::NetworkManager :: FromSite   (
       SqlConnection    & Connection ,
       SUID               site       ,
       UUIDs            & Uuids      ,
       bool             & KeepGo     )
{
  QString Q                                    ;
  Q = Connection.sql.SelectFrom                (
        "page"                                 ,
        PlanTable(SitePages)                      ,
        QString("where site = %1").arg(site) ) ;
  if (Connection.Query(Q))                     {
    while (KeepGo && Connection.Next())        {
      SUID uuid = Connection.Uuid(0)           ;
      if (!Uuids.contains(uuid)) Uuids << uuid ;
    }                                          ;
  }                                            ;
  return ( Uuids . count ( ) > 0 )             ;
}

SUID N::NetworkManager :: PageParent (
       SqlConnection    & Connection ,
       SUID               uuid       )
{
  QString Q                          ;
  SUID    fromPage = 0               ;
  Q = Connection.sql.SelectFrom      (
        "fromPage"                   ,
        PlanTable(PageParent)           ,
        Connection.WhereUuid(uuid) ) ;
  if (Connection.Fetch(Q))           {
    fromPage = Connection . Uuid(0)  ;
  }                                  ;
  return fromPage                    ;
}

bool N::NetworkManager :: insertParent (
       SqlConnection    & Connection   ,
       SUID               uuid         ,
       SUID               fromPage     )
{
  QString Q                     ;
  Q = Connection.sql.InsertInto (
        "uuid,fromPage"         ,
        PlanTable(PageParent)      ,
        QString ( "%1,%2"       )
        . arg   ( uuid          )
        . arg   ( fromPage  ) ) ;
  return Connection . Query (Q) ;
}

bool N::NetworkManager :: updateParent (
       SqlConnection    & Connection   ,
       SUID               uuid         ,
       SUID               fromPage     )
{
  QString Q                            ;
  Q = Connection.sql.Update            (
        PlanTable(PageParent)             ,
        Connection.sql.Where(1,"uuid") ,
        1                              ,
        "fromPage"                   ) ;
  Connection.Prepare ( Q             ) ;
  Connection.Bind("uuid"    ,uuid    ) ;
  Connection.Bind("fromPage",fromPage) ;
  return Connection.Exec()             ;
}

bool N::NetworkManager :: assureParent (
       SqlConnection    & Connection   ,
       SUID               uuid         ,
       SUID               fromPage     )
{
  QString Q                                               ;
  bool    exists = false                                  ;
  /////////////////////////////////////////////////////////
  Q = Connection.sql.SelectFrom                           (
        "fromPage"                                        ,
        PlanTable(PageParent)                                ,
        Connection.WhereUuid(uuid)                      ) ;
  /////////////////////////////////////////////////////////
  if (Connection.Fetch(Q)) exists = true                  ;
  return exists ? updateParent (Connection,uuid,fromPage  )
                : insertParent (Connection,uuid,fromPage) ;
}

QString N::NetworkManager::URI(QUrl & url)
{
  QString f = url.toString (   )  ;
  QString h = url.host     (   )  ;
  if (h.length()<=0) return "/"   ;
  int     i = f.indexOf    ( h )  ;
  if (i<0) return f               ;
  i = f.length() - h.length() - i ;
  if (i<=0) return "/"            ;
  return f . right ( i )          ;
}

SUID N::NetworkManager :: Username   (
       SqlConnection    & Connection ,
       QString            name       )
{
  nKickOut ( name . length ( ) <= 0 , 0 ) ;
  QString Q                               ;
  SUID    uuid = 0                        ;
  Q = Connection.sql.SelectFrom           (
        "uuid"                            ,
        PlanTable(Usernames)                 ,
        Connection.sql.Where(1,"name")  ) ;
  Connection . Prepare(Q)                 ;
  Connection . Bind("name",name.toUtf8()) ;
  IfSqlExec(Connection)                   {
    uuid = Connection . Uuid ( 0 )        ;
  }                                       ;
  return uuid                             ;
}

SUID N::NetworkManager :: assureUsername (
       SqlConnection    & Connection     ,
       QString            name           )
{
  nKickOut ( name . length ( ) <= 0 , 0 ) ;
  SUID    uuid = 0                        ;
  uuid = Username(Connection,name)        ;
  nKickOut ( uuid > 0 , uuid )            ;
  QString Q                               ;
  uuid = Connection.Unique                (
           PlanTable(MajorUuid)              ,
           "uuid"                         ,
           9010                         ) ;
  Connection.assureUuid                   (
    PlanTable(MajorUuid)                     ,
    uuid                                  ,
    TYPE(Username)                      ) ;
  Q = Connection.sql.InsertInto           (
        PlanTable(Usernames)                 ,
        2                                 ,
        "uuid"                            ,
        "name"                          ) ;
  Connection.Prepare(Q)                   ;
  Connection.Bind("uuid",uuid         )   ;
  Connection.Bind("name",name.toUtf8())   ;
  Connection.Exec()                       ;
  return uuid                             ;
}

SUID N::NetworkManager :: EMail      (
       SqlConnection    & Connection ,
       QString            mail       )
{
  nKickOut ( mail.length() <= 0 , 0 ) ;
  QStringList M = mail.split('@')     ;
  if (M.count()!=2) return 0          ;
  return EMail(Connection,M[0],M[1])  ;
}

SUID N::NetworkManager :: EMail      (
       SqlConnection    & Connection ,
       QString            account    ,
       QString            domain     )
{
  nKickOut ( account.length() <= 0 , 0   )      ;
  QString ds = QString("http://%1").arg(domain) ;
  QUrl    url (ds)                              ;
  return EMail(Connection,account,url)          ;
}

SUID N::NetworkManager :: EMail      (
       SqlConnection    & Connection ,
       QString            account    ,
       QUrl               url        )
{
  nKickOut ( account.length() <= 0 , 0   ) ;
  SUID site = UrlUuid (Connection,url    ) ;
  nKickOut ( site             <= 0 , 0   ) ;
  SUID name = Username(Connection,account) ;
  nKickOut ( name             <= 0 , 0   ) ;
  SUID    uuid = 0                         ;
  QString Q                                ;
  Q = Connection.sql.SelectFrom            (
        "uuid"                             ,
        PlanTable(EMails)                     ,
        Connection.sql.Where               (
          2                                ,
          "name"                           ,
          "domain"                     ) ) ;
  Connection.Prepare(Q)                    ;
  Connection.Bind("name"  ,name)           ;
  Connection.Bind("domain",site)           ;
  IfSqlExec(Connection)                    {
    uuid = Connection . Uuid ( 0 )         ;
  }                                        ;
  return uuid                              ;
}

SUID N::NetworkManager :: assureEMail (
       SqlConnection    & Connection  ,
       QString            account     ,
       QString            domain      )
{
  nKickOut ( account.length() <= 0 , 0   )      ;
  QString ds = QString("http://%1").arg(domain) ;
  QUrl    url (ds)                              ;
  return assureEMail(Connection,account,url)    ;
}

SUID N::NetworkManager :: assureEMail (
       SqlConnection    & Connection  ,
       QString            account     ,
       QUrl               url         )
{
  SUID uuid = EMail(Connection,account,url)           ;
  nKickOut ( uuid > 0 , uuid )                        ;
  SUID site = UrlUuid (Connection,url    )            ;
  nKickOut ( site             <= 0 , 0   )            ;
  SUID name = assureUsername(Connection,account)      ;
  nKickOut ( name             <= 0 , 0   )            ;
  QString Q                                           ;
  QString alias = account                             ;
  QString email                                       ;
  email = QString("%1@%2").arg(alias).arg(url.host()) ;
  uuid = Connection.Unique                            (
           PlanTable(MajorUuid)                          ,
           "uuid"                                     ,
           9201                                     ) ;
  Connection.assureUuid                               (
    PlanTable(MajorUuid)                                 ,
    uuid                                              ,
    TYPE(EMail)                                     ) ;
  Q = Connection.sql.InsertInto                       (
        PlanTable(EMails)                                ,
        5                                             ,
        "uuid"                                        ,
        "name"                                        ,
        "domain"                                      ,
        "alias"                                       ,
        "email"                                     ) ;
  Connection.Prepare(Q)                               ;
  Connection.Bind("uuid"  ,uuid          )            ;
  Connection.Bind("name"  ,name          )            ;
  Connection.Bind("domain",site          )            ;
  Connection.Bind("alias" ,alias.toUtf8())            ;
  Connection.Bind("email" ,email.toUtf8())            ;
  Connection.Exec()                                   ;
  return uuid                                         ;
}

int N::NetworkManager :: PatternId  (
      SqlConnection    & Connection ,
      SUID               type       ,
      QString            name       )
{
  QString Q                                          ;
  int     id = 0                                     ;
  Q = Connection.sql.SelectFrom                      (
        "position"                                   ,
        PlanTable(UrlPatterns)                          ,
        "where uuid = :UUID "
        "and name = :NAME "
        "order by position desc limit 0,1"         ) ;
  Connection . Prepare ( Q                         ) ;
  Connection . Bind    ( "uuid" , type             ) ;
  Connection . Bind    ( "name" , name   .toUtf8() ) ;
  IfSqlExec(Connection) id = Connection.Int(0)       ;
  return id                                          ;
}

bool N::NetworkManager :: InsertPattern (
       SqlConnection    & Connection    ,
       SUID               type          ,
       int                position      ,
       QString            name          )
{
  QString Q                                              ;
  Q = Connection.sql.InsertInto                          (
        PlanTable(UrlPatterns)                              ,
        3                                                ,
        "uuid","position","name"                       ) ;
  Connection . Prepare ( Q                             ) ;
  Connection . Bind    ( "uuid"     , type             ) ;
  Connection . Bind    ( "position" , position         ) ;
  Connection . Bind    ( "name"     , name   .toUtf8() ) ;
  return Connection . Exec    (                        ) ;
}

bool N::NetworkManager :: UpdatePattern (
       SqlConnection    & Connection    ,
       SUID               type          ,
       int                position      ,
       QString            name          )
{
  QString Q                                              ;
  Q = Connection.sql.Update                              (
        PlanTable(UrlPatterns)                              ,
        Connection.sql.Where(2,"uuid","position")        ,
        1                                                ,
        "name"                                         ) ;
  Connection . Prepare ( Q                             ) ;
  Connection . Bind    ( "uuid"     , type             ) ;
  Connection . Bind    ( "position" , position         ) ;
  Connection . Bind    ( "name"     , name   .toUtf8() ) ;
  return Connection . Exec    (                        ) ;
}

bool N::NetworkManager :: UpdatePattern (
       SqlConnection    & Connection    ,
       SUID               type          ,
       int                position      ,
       QString            name          ,
       QString            pattern       )
{
  QString Q                                              ;
  Q = Connection.sql.Update                              (
        PlanTable(UrlPatterns)                              ,
        Connection.sql.Where(3,"uuid","position","name") ,
        1                                                ,
        "pattern"                                      ) ;
  Connection . Prepare ( Q                             ) ;
  Connection . Bind    ( "uuid"     , type             ) ;
  Connection . Bind    ( "position" , position         ) ;
  Connection . Bind    ( "name"     , name   .toUtf8() ) ;
  Connection . Bind    ( "pattern"  , pattern.toUtf8() ) ;
  return Connection . Exec    (                        ) ;
}

bool N::NetworkManager :: UrlPatterns (
       SqlConnection    & SC          ,
       SUID               type        ,
       QString            name        ,
       QStringList      & patterns    )
{
  QString Q                               ;
  Q = SC.sql.SelectFrom                   (
        "pattern"                         ,
        PlanTable(UrlPatterns)               ,
        QString ( "%1 %2"                 )
        .arg(SC.sql.Where(2,"uuid","name"))
        .arg(SC.OrderByDesc("position"))) ;
  SC . Prepare ( Q                      ) ;
  SC . Bind    ( "uuid" , type          ) ;
  SC . Bind    ( "name" , name.toUtf8() ) ;
  if (SC.Exec()) while (SC.Next())        {
    patterns << SC . String ( 0 )         ;
  }                                       ;
  return ( patterns.count()>0 )           ;
}

bool N::NetworkManager :: DeletePatterns (
       SqlConnection    & SC             ,
       SUID               type           ,
       int                position       ,
       QString            name           )
{
  QString Q                                   ;
  Q = SC.sql.DeleteFrom                       (
        PlanTable(UrlPatterns)                   ,
        SC.sql.Where                          (
          3,"uuid","position","name"      ) ) ;
  SC . Prepare ( Q                          ) ;
  SC . Bind    ( "uuid"     , type          ) ;
  SC . Bind    ( "position" , position      ) ;
  SC . Bind    ( "name"     , name.toUtf8() ) ;
  return SC . Exec    (                     ) ;
}

QString N::NetworkManager :: SelectPatterns (
          SqlConnection    & Connection     ,
          Qt::SortOrder      Sorting        )
{
  return Connection.sql.SelectFrom                          (
           "position,name,pattern"                          ,
           PlanTable(UrlPatterns)                              ,
           Connection.OrderBy("name,position,id",Sorting) ) ;
}

bool N::NetworkManager :: assignBody (
       SqlConnection    & SC         ,
       SUID               uuid       ,
       QByteArray       & Body       )
{
  QString Q                            ;
  int     bs = Body.size()             ;
  Q = SC.sql.SelectFrom                (
        "id"                           ,
        PlanTable(ImportBodies)           ,
        SC.WhereUuid(uuid)           ) ;
  if (!SC.Fetch(Q))                    {
    Q = SC.sql.InsertInto              (
          PlanTable(ImportBodies)         ,
          3                            ,
          "uuid"                       ,
          "length"                     ,
          "body"                     ) ;
  } else                               {
    Q = SC.sql.Update                  (
          PlanTable(ImportBodies)         ,
          SC.sql.Where(1,"uuid")       ,
          2                            ,
          "length"                     ,
          "body"                     ) ;
  }                                    ;
  SC . Prepare     ( Q               ) ;
  SC . Bind        ( "uuid"   , uuid ) ;
  SC . Bind        ( "length" , bs   ) ;
  SC . Bind        ( "body"   , Body ) ;
  return SC . Exec (                 ) ;
}

bool N::NetworkManager :: fetchBody (
       SqlConnection    & SC         ,
       SUID               uuid       ,
       QByteArray       & Body       )
{
  QString Q                  ;
  Q = SC.sql.SelectFrom      (
        "body"               ,
        PlanTable(ImportBodies) ,
        SC.WhereUuid(uuid) ) ;
  if (SC.Fetch(Q))           {
    Body = SC.ByteArray(0)   ;
  } else return false        ;
  return true                ;
}

bool N::NetworkManager :: deleteBody (
       SqlConnection    & SC         ,
       SUID               uuid       )
{
  QString Q                  ;
  Q = SC.sql.DeleteFrom      (
        PlanTable(ImportBodies) ,
        SC.WhereUuid(uuid) ) ;
  return SC . Query ( Q    ) ;
}

bool N::NetworkManager :: assignHeader (
       SqlConnection    & SC           ,
       SUID               uuid         ,
       QByteArray       & Header       )
{
  QString Q                              ;
  int     bs = Header.size()             ;
  Q = SC.sql.SelectFrom                  (
        "id"                             ,
        PlanTable(WwwHeaders)               ,
        SC.WhereUuid(uuid)             ) ;
  if (!SC.Fetch(Q))                      {
    Q = SC.sql.InsertInto                (
          PlanTable(WwwHeaders)             ,
          3                              ,
          "uuid"                         ,
          "length"                       ,
          "header"                     ) ;
  } else                                 {
    Q = SC.sql.Update                    (
          PlanTable(WwwHeaders)             ,
          SC.sql.Where(1,"uuid")         ,
          2                              ,
          "length"                       ,
          "header"                     ) ;
  }                                      ;
  SC . Prepare     ( Q                 ) ;
  SC . Bind        ( "uuid"   , uuid   ) ;
  SC . Bind        ( "length" , bs     ) ;
  SC . Bind        ( "header" , Header ) ;
  return SC . Exec (                   ) ;
}

bool N::NetworkManager :: FromTable  (
       SqlConnection    & Connection ,
       QString            Table      ,
       Binary           & binary     ,
       UUIDs            & Uuids      )
{
  QString     Q                                           ;
  QString     Where                                       ;
  QStringList Keys                                        ;
  KMAPs       KVs                                         ;
  nKickOut (!binary.SqlWhere(Where,Keys,KVs,"") , false ) ;
  Q = Connection.sql.SelectFrom                           (
        "uuid"                                            ,
        Table                                             ,
        Where                                           ) ;
  Connection . BindAll ( Q , Keys , KVs )                 ;
  IfSqlLoop(Connection) Uuids << Connection.Uuid(0)       ;
  return true                                             ;
}

bool N::NetworkManager :: FromProtocol (
       SqlConnection    & Connection   ,
       Binary           & binary       ,
       UUIDs            & Uuids        )
{
  return FromTable(Connection,PlanTable(PageProtocols),binary,Uuids) ;
}

bool N::NetworkManager :: FromPage   (
       SqlConnection    & Connection ,
       Binary           & binary     ,
       UUIDs            & Uuids      )
{
  return FromTable(Connection,PlanTable(WwwPages),binary,Uuids) ;
}

bool N::NetworkManager :: FromTitle  (
       SqlConnection    & Connection ,
       Binary           & binary     ,
       UUIDs            & Uuids      )
{
  return FromTable(Connection,PlanTable(PageTitles),binary,Uuids) ;
}

bool N::NetworkManager :: DnsUuids   (
       SqlConnection    & Connection ,
       UUIDs            & Uuids      ,
       enum Qt::SortOrder Sorting    )
{
  return ( Connection . Uuids                         (
             Uuids                                    ,
             PlanTable(DNS)                              ,
             "uuid"                                   ,
             Connection.OrderBy("id",Sorting) ) > 0 ) ;
}

bool N::NetworkManager :: DnsAddress (
       SqlConnection    & Connection ,
       SUID               uuid       ,
       QHostAddress     & host       )
{
  QString Q                           ;
  host . clear ( )                    ;
  Q = Connection . sql . SelectFrom   (
        "ip"                          ,
        PlanTable(DNS)                   ,
        Connection.WhereUuid(uuid) )  ;
  if (Connection.Fetch(Q))            {
    QString ip = Connection.String(0) ;
    host . setAddress ( ip )          ;
  } else return false                 ;
  return ( ! host . isNull ( ) )      ;
}

bool N::NetworkManager    :: DnsAddresses (
       SqlConnection       & Connection   ,
       UUIDs               & Uuids        ,
       QList<QHostAddress> & Addresses    )
{
  SUID uuid                               ;
  foreach (uuid,Uuids)                    {
    QHostAddress host                     ;
    if (DnsAddress(Connection,uuid,host)) {
      Addresses << host                   ;
    }                                     ;
  }                                       ;
  return ( Addresses . count ( ) > 0 )    ;
}
