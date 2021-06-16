#include <qtfoundation.h>

N::RpcLogHandler:: RpcLogHandler    ( PurePlan * p )
                 : XmlRpcLogHandler (              )
                 , plan             (            p )
{
}

N::RpcLogHandler::~RpcLogHandler (void)
{
}

void N::RpcLogHandler::log(int level,const char * msg)
{
  if ( IsNull ( plan ) ) return       ;
  if ( IsNull ( msg  ) ) return       ;
  QString m = QString::fromUtf8(msg)  ;
  plan -> Debug         ( level , m ) ;
  plan -> processEvents (           ) ;
}
