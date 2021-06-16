#include <qtfoundation.h>

QScriptValue N::JsFeederAttachment(QScriptContext * context,QScriptEngine * engine)
{
  Q_UNUSED ( context )                                 ;
  N::MtFeeder * api       = new N::MtFeeder ( engine ) ;
  QString       MtApiPath = ""                         ;
  QScriptValue  global                                 ;
  QScriptValue  path                                   ;
  global = engine -> globalObject (             )      ;
  path   = global  . property     ( "MtApiPath" )      ;
  if ( path . isString ( ) )                           {
    MtApiPath = path . toString ( )                    ;
  }                                                    ;
  api -> Load ( MtApiPath )                            ;
  api -> Scripts . Creators [ "MtJS" ] = JsToolCreate  ;
  return engine -> newQObject ( api )                  ;
}

N::JsTools:: JsTools     ( QObject * parent )
           : QObject     (           parent )
           , QScriptable (                  )
           , Thread      ( 0 , false        )
           , Enabler     (                  )
           , geolocation ( NULL             )
           , pcap        ( NULL             )
{
  Configure ( ) ;
}

N::JsTools::~JsTools (void)
{
}

bool N::JsTools::SetEnabled(int Id,bool enable)
{
  return N::Enabler::setEnabled ( Id , enable ) ;
}

bool N::JsTools::IsEnabled(int Id)
{
  return N::Enabler::isEnabled ( Id ) ;
}

bool N::JsTools::SetEnabled(QString Id,bool enable)
{
  return N::Enabler::setEnabled ( Id , enable ) ;
}

bool N::JsTools::IsEnabled(QString Id)
{
  return N::Enabler::isEnabled ( Id ) ;
}

void N::JsTools::setInt(int id,int value)
{
  IntInt [ id ] = value ;
}

void N::JsTools::setInt(QString id,int value)
{
  IntStr [ id ] = value ;
}

int N::JsTools::Int(int id)
{
  return IntInt [ id ] ;
}

int N::JsTools::Int(QString id)
{
  return IntStr [ id ] ;
}

void N::JsTools::setDouble(int id,double value)
{
  DoubleInt [ id ] = value ;
}

void N::JsTools::setDouble(QString id,double value)
{
  DoubleStr [ id ] = value ;
}

double N::JsTools::Double(int id)
{
  return DoubleInt [ id ] ;
}

double N::JsTools::Double(QString id)
{
  return DoubleStr [ id ] ;
}

void N::JsTools::setString(int id,QString value)
{
  StringInt [ id ] = value ;
}

void N::JsTools::setString(QString id,QString value)
{
  StringStr [ id ] = value ;
}

QString N::JsTools::String(int id)
{
  return StringInt [ id ] ;
}

QString N::JsTools::String(QString id)
{
  return StringStr [ id ] ;
}

void N::JsTools::toStrings(QString id,QString ids,QString splitter)
{
  StringStrList [ id ] = StringStr [ ids ] . split ( splitter ) ;
}

void N::JsTools::setStrings(QString id,QStringList ids)
{
  StringStrList [ id ] = ids ;
}

int N::JsTools::Strings(QString id)
{
  return StringStrList [ id ] . count ( ) ;
}

QString N::JsTools::Strings(QString id,int position)
{
  return StringStrList [ id ] [ position ] ;
}

void N::JsTools::PrepareStrings(QString id)
{
  StringStrList [ id ] = QStringList ( ) ;
}

void N::JsTools::JoinStrings(QString id,QString str)
{
  StringStrList [ id ] << str ;
}

void N::JsTools::setByteArray(int id,QByteArray value)
{
  DataInt [ id ] = value ;
}

void N::JsTools::setByteArray(QString id,QByteArray value)
{
  DataStr [ id ] = value ;
}

QByteArray N::JsTools::ByteArray(int id)
{
  return DataInt [ id ] ;
}

QByteArray N::JsTools::ByteArray(QString id)
{
  return DataStr [ id ] ;
}

QDateTime N::JsTools::currentDateTime (void)
{
  return QDateTime::currentDateTime ( ) ;
}

void N::JsTools::setDateTime(int id,QDateTime dt)
{
  DateTimeInt [ id ] = dt ;
}

void N::JsTools::setDateTime(QString id,QDateTime dt)
{
  DateTimeStr [ id ] = dt ;
}

QDateTime N::JsTools::getDateTime(int id)
{
  return DateTimeInt [ id ] ;
}

QDateTime N::JsTools::getDateTime(QString id)
{
  return DateTimeStr [ id ] ;
}

long long N::JsTools::Elapsed(int t1,int t2)
{
  return DateTimeInt [ t1 ] . msecsTo ( DateTimeInt [ t2 ] ) ;
}

long long N::JsTools::Elapsed(QString t1,QString t2)
{
  return DateTimeStr [ t1 ] . msecsTo ( DateTimeStr [ t2 ] ) ;
}

void N::JsTools::Configure(void)
{
  codec = QTextCodec::codecForLocale ( )                          ;
  setEnabled ( "Explain" , true )                                 ;
  /////////////////////////////////////////////////////////////////
  Engines [ -1 ] = qobject_cast<QScriptEngine *>(parent())        ;
  /////////////////////////////////////////////////////////////////
  Scripts . Creators [ "MtJS"          ] = JsToolCreate           ;
  Scripts . Creators [ "FTP"           ] = FtpAttachement         ;
  Scripts . Creators [ "SQL"           ] = SqlAttachment          ;
  Scripts . Creators [ "File"          ] = FileAttachment         ;
  Scripts . Creators [ "Directory"     ] = DirectoryAttachment    ;
  Scripts . Creators [ "Socket"        ] = SocketAttachment       ;
  Scripts . Creators [ "Settings"      ] = SettingsAttachment     ;
  Scripts . Creators [ "ByteArray"     ] = ByteArrayAttachment    ;
  Scripts . Creators [ "ScriptStrings" ] = ScriptCreateString     ;
  Scripts . Creators [ "Http"          ] = AttacheHttp            ;
  Scripts . Creators [ "Xml"           ] = AttacheXml             ;
  Scripts . Creators [ "Mutex"         ] = MutexAttachment        ;
  Scripts . Creators [ "Transformer"   ] = TransformerAttachment  ;
  Scripts . Creators [ "MtAPI"         ] = JsFeederAttachment     ;
  Scripts . Creators [ "Dukascopy"     ] = DukascopyCreate        ;
  Scripts . Creators [ "RpcServer"     ] = RPC::ServerAttachement ;
  Scripts . Creators [ "RpcClient"     ] = RPC::ClientAttachement ;
}

QScriptEngine * N::JsTools::GetEngine(int id)
{
  if ( ! Engines . contains ( -1 ) ) return NULL                             ;
  if ( Engines . contains ( id ) ) return Engines [ id ]                     ;
  ////////////////////////////////////////////////////////////////////////////
  QScriptValue     global   = Engines [ -1 ] -> globalObject (             ) ;
  QScriptValue     source   = global          . property     ( "Original"  ) ;
  QScriptValue     appMtAPI = global          . property     ( "AppDir"    ) ;
  QScriptValue     appDir   = global          . property     ( "MtApiPath" ) ;
  QString          script   = source          . toString     (             ) ;
  ////////////////////////////////////////////////////////////////////////////
  if ( script.length() <= 0 ) return NULL                                    ;
  ////////////////////////////////////////////////////////////////////////////
  QScriptEngine *  Engine = new QScriptEngine    (      )                    ;
  QScriptValue     mtjs   = Engine -> newQObject ( this )                    ;
  QScriptValue     func                                                      ;
  QScriptValue     g                                                         ;
  QScriptValue     src      ( script                  )                      ;
  QScriptValue     AppDir   ( appDir   . toString ( ) )                      ;
  QScriptValue     AppMtAPI ( appMtAPI . toString ( ) )                      ;
  QScriptValue     TID      ( id                      )                      ;
  ////////////////////////////////////////////////////////////////////////////
  Scripts . AttachCreators         ( *Engine                               ) ;
  func    = Engine -> evaluate     ( script                                ) ;
  g       = Engine -> globalObject (                                       ) ;
  g       . setProperty            ( "gMtJS"     , mtjs                    ) ;
  g       . setProperty            ( "Original"  , src                     ) ;
  g       . setProperty            ( "MtApiPath" , AppMtAPI                ) ;
  g       . setProperty            ( "AppDir"    , AppDir                  ) ;
  g       . setProperty            ( "ThreadId"  , TID                     ) ;
  ////////////////////////////////////////////////////////////////////////////
  Engines [ id ] = Engine                                                    ;
  return Engines [ id ]                                                      ;
}

void N::JsTools::toConsole(QString msg)
{
  QByteArray M = codec->fromUnicode(msg) ;
  if ( M  .size ( ) <= 0 ) return        ;
  ::printf ( M . constData ( ) )         ;
}

void N::JsTools::toConsoleLn(QString msg)
{
  msg += "\r\n"     ;
  toConsole ( msg ) ;
}

void N::JsTools::setRoot(QString rootpath)
{
  Bin = QDir ( rootpath ) ;
}

void N::JsTools::setMtAPI(QString mtapi)
{
  MtAPI = mtapi ;
}

void N::JsTools::run(int type,N::ThreadData * data)
{
  switch ( type )                                {
    case 10001                                   :
      RunScript ( data->Arguments , data -> Id ) ;
    break                                        ;
    case 10002                                   :
      download  ( data->Arguments , data -> Id ) ;
    break                                        ;
  }                                              ;
}

bool N::JsTools::Start(QString command)
{
  QStringList s = N::CommandTokens ( command )    ;
  QString     p                                   ;
  QString     l                                   ;
  N::VarArgs  args                                ;
  if ( s . count ( ) <= 0 ) return false          ;
  p = s . first   (   )                           ;
  s . takeAt      ( 0 )                           ;
  l = p . toLower (   )                           ;
  /////////////////////////////////////////////////
  if ( "start" == l && ( s  .count ( ) > 1 ) )    {
    args << s                                     ;
    start ( 10001 , args )                        ;
    return true                                   ;
  }                                               ;
  /////////////////////////////////////////////////
  if ( "download" == l && ( s  .count ( ) > 1 ) ) {
    args << s                                     ;
    start ( 10002 , args )                        ;
    return true                                   ;
  }                                               ;
  /////////////////////////////////////////////////
  return false                                    ;
}

int N::JsTools::Execute(QString command)
{
  QStringList s = N::CommandTokens ( command ) ;
  QString     p                                ;
  if ( s . count ( ) <= 0 ) return 0           ;
  p = s . first ( )                            ;
  s . takeAt    (0)                            ;
  if ( s.count() <= 0 )                        {
    return QProcess::execute ( p     )         ;
  }                                            ;
  return   QProcess::execute ( p , s )         ;
}

bool N::JsTools::Go(QString command)
{
  QStringList s = N::CommandTokens ( command ) ;
  QString     p                                ;
  if ( s . count ( ) <= 0 ) return false       ;
  p = s . first ( )                            ;
  s . takeAt    (0)                            ;
  if ( s.count() <= 0 )                        {
    return QProcess::startDetached ( p     )   ;
  }                                            ;
  return   QProcess::startDetached ( p , s )   ;
}

void N::JsTools::download(N::VarArgs & args,int Id)
{
  QString url                                                       ;
  QString filename                                                  ;
  int     t = 60 * 1000                                             ;
  if ( args     . count  ( ) >  0 ) url      = args[0].toString ( ) ;
  if ( args     . count  ( ) >  1 ) filename = args[1].toString ( ) ;
  if ( args     . count  ( ) >  2 ) t        = args[2].toInt    ( ) ;
  if ( url      . length ( ) <= 0 ) return                          ;
  if ( filename . length ( ) <= 0 ) return                          ;
  ///////////////////////////////////////////////////////////////////
  QtCURL FTP                                                        ;
  QUrl   u       ( url       )                                      ;
  QFile  f       ( filename  )                                      ;
  FTP . download ( u , f , t )                                      ;
}

void N::JsTools::RunScript(N::VarArgs & args,int Id)
{
  if ( args . count ( ) <= 0 ) return                       ;
  QString callback = args [ 0 ] . toString ( )              ;
  if ( callback . length ( ) <= 0    ) return               ;
  QScriptEngine * e = GetEngine ( Id )                      ;
  if ( NULL == e ) return                                   ;
  QScriptValue global = e      -> globalObject (          ) ;
  QScriptValue silent = global  . property     ( callback ) ;
  if ( silent . isFunction ( ) )                            {
    QScriptValueList args                                   ;
    silent  . call           ( QScriptValue ( ) , args )    ;
    e      -> collectGarbage (                         )    ;
  }                                                         ;
}

void N::JsTools::setGlobal(QString key,QScriptValue value)
{
  QScriptEngine * e = GetEngine ( -1 )        ;
  if ( NULL == e ) return                     ;
  QScriptValue global = e -> globalObject ( ) ;
  global . setProperty ( key , value )        ;
}

void N::JsTools::setGlobal(int Id,QString key,QScriptValue value)
{
  QScriptEngine * e = GetEngine ( Id )        ;
  if ( NULL == e ) return                     ;
  QScriptValue global = e -> globalObject ( ) ;
  global . setProperty ( key , value )        ;
}

void N::JsTools::msleep(int msecs)
{
  N::Time::msleep ( msecs ) ;
}

void N::JsTools::sleep(int msecs)
{
  N::Time::sleep ( msecs ) ;
}

QScriptValue N::JsToolCreate(QScriptContext * context,QScriptEngine * engine)
{
  QScriptValue global                                            ;
  QScriptValue appDir                                            ;
  QScriptValue appMtAPI                                          ;
  ////////////////////////////////////////////////////////////////
  global    = engine -> globalObject (                         ) ;
  appDir    = global  . property     ( "AppDir"                ) ;
  appMtAPI  = global  . property     ( "MtApiPath"             ) ;
  ////////////////////////////////////////////////////////////////
  N::JsTools * js = new N::JsTools   ( engine                  ) ;
  js -> setRoot                      ( appDir   . toString ( ) ) ;
  js -> setMtAPI                     ( appMtAPI . toString ( ) ) ;
  return engine -> newQObject        ( js                      ) ;
}
