#include <qtfoundation.h>

N::Parallel:: Parallel  ( PurePlan * p )
            : Computing (              )
            , plan      (            p )
{
  if ( NULL != plan )                             {
    Data . Controller = & ( plan -> canContinue ) ;
    plan -> addDestroyer  ( this                ) ;
  }                                               ;
}

N::Parallel::~Parallel(void)
{
}

bool N::Parallel::Recycling(void)
{
  return true ;
}

bool N::Parallel::Destructor(void)
{
  delete this ;
  return true ;
}

void N::Parallel::ReportError (int level,int code)
{
  if ( NULL == plan ) return                  ;
  QString m                                   ;
  if ( ErrorString . contains ( code ) )      {
    m = ErrorString [ code ]                  ;
  } else                                      {
    m = QString ( "Error %1" ) . arg ( code ) ;
  }                                           ;
  plan -> Debug ( level , m )                 ;
}
