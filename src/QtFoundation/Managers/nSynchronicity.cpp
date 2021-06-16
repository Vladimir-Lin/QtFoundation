#include <QtFoundation>

N::Synchronicity:: Synchronicity ( QObject * parent )
                 : QObject       (           parent )
                 , QScriptable   (                  )
                 , watcher       ( NULL             )
                 , started       ( false            )
                 , waiting       ( false            )
{
  Configure ( ) ;
}

N::Synchronicity::~Synchronicity (void)
{
  if ( NotNull(watcher) ) delete watcher ;
}

void N::Synchronicity::Configure (void)
{
  lastest = nTimeNow                          ;
  values [ "filesChanged" ] = 180             ;
  values [ "Waiting"      ] = 600             ;
  nConnect ( this , SIGNAL ( Updated  ( ) )   ,
             this , SLOT   ( Redirect ( ) ) ) ;
}

int N::Synchronicity::addDir(const QString dir)
{
  if ( ! pathes . contains ( dir ) ) {
    pathes << dir                    ;
  }                                  ;
  return pathes . count ( )          ;
}

bool N::Synchronicity::addPath(const QString & path)
{
  if ( IsNull(watcher) ) return false ;
  return watcher -> addPath ( path )  ;
}

QStringList N::Synchronicity::addPaths(const QStringList & paths)
{
  if ( IsNull(watcher) ) return QStringList() ;
  return watcher -> addPaths ( paths )        ;
}

bool N::Synchronicity::removePath(const QString & path)
{
  if ( IsNull(watcher) ) return false   ;
  return watcher -> removePath ( path ) ;
}

QStringList N::Synchronicity::removePaths(const QStringList & paths)
{
  if ( IsNull(watcher) ) return QStringList() ;
  return watcher -> removePaths ( paths )     ;
}

void N::Synchronicity::run(int Type,ThreadData * data)
{
  switch ( Type )     {
    case 10001        :
      addWatchers ( ) ;
    break             ;
    case 10002        :
      Waiting     ( ) ;
    break             ;
  }                   ;
}

void N::Synchronicity::startup(bool enabled)
{
  if ( ! enabled )                                         {
    delete watcher                                         ;
    watcher = NULL                                         ;
    started = false                                        ;
    return                                                 ;
  }                                                        ;
  if ( NotNull(watcher) ) return                           ;
  //////////////////////////////////////////////////////////
  watcher = new QFileSystemWatcher ( this )                ;
  //////////////////////////////////////////////////////////
  start ( 10001 )                                          ;
}

void N::Synchronicity::filesChanged (const QString & path)
{
  if ( ! started ) return                        ;
  if ( ! waiting )                               {
    int ds                                       ;
    ds = lastest.secsTo(nTimeNow)                ;
    if ( ds < values [ "filesChanged" ] ) return ;
  }                                              ;
  QFileInfo f ( path )                           ;
  if ( ! f . exists ( ) ) return                 ;
  lastest = nTimeNow                             ;
  if ( waiting ) return                          ;
  waiting = true                                 ;
  start ( 10002 )                                ;
}

void N::Synchronicity::addWatchers (void)
{
  QFileInfoList t                                          ;
  QFileInfoList f                                          ;
  QFileInfo     i                                          ;
  QString       p                                          ;
  QDir          d                                          ;
  //////////////////////////////////////////////////////////
  foreach ( p , pathes )                                   {
    d  = QDir ( p )                                        ;
    f  = File::FileInfos ( d )                             ;
    t  . append ( f )                                      ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  foreach ( i , t )                                        {
    if ( i . isDir ( ) )                                   {
      p = i . absoluteFilePath ( )                         ;
      addPath ( p )                                        ;
    }                                                      ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  nConnect ( watcher , SIGNAL(fileChanged     (QString))   ,
             this    , SLOT  (filesChanged    (QString)) ) ;
  nConnect ( watcher , SIGNAL(directoryChanged(QString))   ,
             this    , SLOT  (filesChanged    (QString)) ) ;
  started = true                                           ;
}

void N::Synchronicity::Waiting (void)
{
  bool completed = false                   ;
  while ( ! completed )                    {
    int ds = lastest . secsTo ( nTimeNow ) ;
    if ( ds > values [ "Waiting" ] )       {
      completed = true                     ;
    } else                                 {
      Time::msleep(1000)                   ;
    }                                      ;
  }                                        ;
  emit Updated ( )                         ;
}

void N::Synchronicity::Redirect (void)
{
  emit DropOut ( ) ;
}

int N::Synchronicity::Value(QString key)
{
  return values [ key ] ;
}

int N::Synchronicity::setValue(QString key,int value)
{
  values [ key ] = value ;
  return values [ key ]  ;
}

QScriptValue N::SyncAttachment(QScriptContext * context,QScriptEngine * engine)
{
  N::Synchronicity * sync = new N::Synchronicity ( engine ) ;
  return engine -> newQObject                    ( sync   ) ;
}
