#include <qtfoundation.h>

bool N::Acoustics::Load(Sql & sql,QString table,QDir & dir,SUID uuid,AudioData & data)
{
  VideoManager VM(NULL)                      ;
  QString      filename                      ;
  filename = VM.Location(sql,table,dir,uuid) ;
  if (filename.length()<=0) return false     ;
  return N::Acoustics::Load(filename,data)   ;
}

bool N::Acoustics::Load(Sql & sql,QString table,QDir & dir,QString identifier,AudioData & data)
{
  VideoManager VM(NULL)                            ;
  QString      filename                            ;
  filename = VM.Location(sql,table,dir,identifier) ;
  if (filename.length()<=0) return false           ;
  return N::Acoustics::Load(filename,data)         ;
}

bool N::Acoustics::Load(PurePlan & plan,SUID uuid,AudioData & data)
{
  VideoManager VM(&plan)                   ;
  QString      filename                    ;
  filename = VM.Location(plan,uuid)        ;
  if (filename.length()<=0) return false   ;
  return N::Acoustics::Load(filename,data) ;
}

bool N::Acoustics::Load(PurePlan & plan,QString identifier,AudioData & data)
{
  VideoManager VM(&plan)                   ;
  QString      filename                    ;
  filename = VM.Location(plan,identifier)  ;
  if (filename.length()<=0) return false   ;
  return N::Acoustics::Load(filename,data) ;
}
