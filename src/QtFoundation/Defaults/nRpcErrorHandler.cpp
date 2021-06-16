#include <qtfoundation.h>

N::RpcErrorHandler:: RpcErrorHandler    ( PurePlan * p )
                   : XmlRpcErrorHandler (              )
                   , plan               (            p )
{
}

N::RpcErrorHandler::~RpcErrorHandler (void)
{
}

void N::RpcErrorHandler::error(const char * msg)
{
  if ( IsNull ( plan ) ) return                                  ;
  if ( IsNull ( msg  ) ) return                                  ;
  QString m = QString::fromUtf8(msg)                             ;
  plan -> Debug         ( XmlRpcLogHandler::getVerbosity() , m ) ;
  plan -> processEvents (                                      ) ;
}
