#include <qtfoundation.h>

#define plan KnowledgePlan

N::KnowledgeManager:: KnowledgeManager ( PurePlan * p )
                    : KnowledgePlan    (            p )
{
}

N::KnowledgeManager::~KnowledgeManager(void)
{
}

QString N::KnowledgeManager::Name(SqlConnection & Connection,SUID uuid,int language)
{
  return Connection.getName(PlanTable(Names),"uuid",language,uuid) ;
}

bool N::KnowledgeManager::assureName(SqlConnection & Connection,SUID uuid,int language,QString name)
{
  return Connection.assureName(PlanTable(Names),uuid,language,name) ;
}
