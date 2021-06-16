#include <qtfoundation.h>

#define plan TaskPlan

N::TaskManager:: TaskManager ( PurePlan * p )
               : TaskPlan    (            p )
{
}

N::TaskManager::~TaskManager (void)
{
}

void N::TaskManager::Clear(void)
{
  Tasks        . clear ( ) ;
  Names        . clear ( ) ;
  Behaviors    . clear ( ) ;
  Status       . clear ( ) ;
  Roots        . clear ( ) ;
  Flags        . clear ( ) ;
  Parents      . clear ( ) ;
  Predecessors . clear ( ) ;
  Successors   . clear ( ) ;
  Entries      . clear ( ) ;
  Heirs        . clear ( ) ;
}

UUIDs N::TaskManager::TopLevels(void)
{
  UUIDs Uuids                                  ;
  SUID  t                                      ;
  //////////////////////////////////////////////
  foreach (t,Tasks)                            {
    bool root = true                           ;
    if (Parents     .contains(t)) root = false ;
    if (Predecessors.contains(t)) root = false ;
    if (Entries     .contains(t)) root = false ;
    if (root) Uuids << t                       ;
  }                                            ;
  //////////////////////////////////////////////
  return Uuids                                 ;
}

UUIDs N::TaskManager::Frameworks(void)
{
  UUIDs  Uuids                             ;
  SUID   uuid                              ;
  //////////////////////////////////////////
  foreach (uuid,Tasks)                     {
    if (Behaviors[uuid]==Tasks::Framework) {
      Uuids << uuid                        ;
    }                                      ;
  }                                        ;
  //////////////////////////////////////////
  return Uuids                             ;
}

UUIDs N::TaskManager::Predecessor(void)
{
  UUIDs  Uuids                                        ;
  SUID   uuid                                         ;
  /////////////////////////////////////////////////////
  foreach (uuid,Tasks)                                {
    switch (Behaviors[uuid])                          {
      case Tasks::Standalone                          :
        if (!Successors.contains(uuid)) Uuids << uuid ;
      break                                           ;
      case Tasks::Framework                           :
        if (!Successors.contains(uuid)) Uuids << uuid ;
      break                                           ;
      case Tasks::Decision                            :
        Uuids << uuid                                 ;
      break                                           ;
    }                                                 ;
  }                                                   ;
  /////////////////////////////////////////////////////
  return Uuids                                        ;
}

UUIDs N::TaskManager::Successor(void)
{
  UUIDs  Uuids                                          ;
  SUID   uuid                                           ;
  ///////////////////////////////////////////////////////
  foreach (uuid,Tasks)                                  {
    switch (Behaviors[uuid])                            {
      case Tasks::Standalone                            :
        if (!Predecessors.contains(uuid)) Uuids << uuid ;
      break                                             ;
      case Tasks::Framework                             :
        if (!Predecessors.contains(uuid)) Uuids << uuid ;
      break                                             ;
      case Tasks::Decision                              :
        Uuids << uuid                                   ;
      break                                             ;
    }                                                   ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  return Uuids                                          ;
}

UUIDs N::TaskManager::Frame(SUID uuid)
{
  UUIDs Uuids                                        ;
  UUIDs frame                                        ;
  SUID  t                                            ;
  ////////////////////////////////////////////////////
  foreach (t,Tasks) if (Parents[t]==uuid) frame << t ;
  foreach (t,frame)                                  {
    bool root = true                                 ;
    if (Predecessors.contains(t)) root = false       ;
    if (Entries     .contains(t)) root = false       ;
    if (root) Uuids << t                             ;
  }                                                  ;
  ////////////////////////////////////////////////////
  return Uuids                                       ;
}

bool N::TaskManager::isFramework(SUID uuid)
{
  return Parents.contains(uuid) ;
}

bool N::TaskManager::isPredecessor(SUID uuid)
{
  return Predecessors.contains(uuid) ;
}

bool N::TaskManager::isSuccessor(SUID uuid)
{
  return Successors.contains(uuid) ;
}

bool N::TaskManager::Load(SqlConnection & Connection,SUID project)
{
  Clear ( )                                                       ;
  /////////////////////////////////////////////////////////////////
  Tasks = (project>0) ? ProjectTasks ( Connection , project     ) :
                        ListTasks    ( Connection               ) ;
  Names = LoadNames ( Connection , Tasks , TaskPlan->LanguageId ) ;
  TaskBehaviors     ( Connection , Tasks                        ) ;
  TaskRelations     ( Connection , Tasks                        ) ;
  return ( Tasks.count() > 0 )                                    ;
}

UUIDs N::TaskManager::ListProjects(SqlConnection & Connection)
{
  return Connection.Uuids                            (
           PlanTable(Projects)                       ,
           "uuid"                                    ,
           QString("where type = %1 order by id desc")
           .arg(Project::TASK)                     ) ;
}

UUIDs N::TaskManager::ListTasks(SqlConnection & Connection)
{
  return Connection.Uuids(PlanTable(Tasks),"uuid","order by id asc") ;
}

UUIDs N::TaskManager::ProjectTasks(SqlConnection & Connection,SUID project)
{
  QString Q = QString ( "where first = %1 "
                        "and t1 = %2 "
                        "and t2 = %3 "
                        "and relation = %4 "
                        "order by position asc"         )
              .arg(project                              )
              .arg(Types::Project                       )
              .arg(Types::Task                          )
              .arg(Groups::Subordination              ) ;
  return Connection.Uuids(PlanTable(Groups),"second",Q) ;
}

int N::TaskManager::TaskBehaviors(SqlConnection & Connection,UUIDs & Uuids)
{
  QString Q                                    ;
  SUID    uuid                                 ;
  //////////////////////////////////////////////
  foreach (uuid,Uuids)                         {
    Q = Connection.sql.SelectFrom              (
          "`behavior`,`status`,`flags`"        ,
          PlanTable(Tasks)                     ,
          Connection.WhereUuid(uuid)         ) ;
    if (Connection.Fetch(Q))                   {
      Behaviors [uuid] = Connection . Int  (0) ;
      Status    [uuid] = Connection . Int  (1) ;
      Flags     [uuid] = Connection . Uuid (2) ;
    }                                          ;
  }                                            ;
  //////////////////////////////////////////////
  return Uuids . count ( )                     ;
}

int N::TaskManager::TaskRelations(SqlConnection & Connection,UUIDs & Uuids)
{
  QString Q                                                                    ;
  SUID    uuid                                                                 ;
  //////////////////////////////////////////////////////////////////////////////
  foreach ( uuid , Uuids ) if (Behaviors.contains(uuid))                       {
    // relation syntax is
    // first `related` second
    // meaning, if there is a `first`,`second`,`relation` item
    // then "second is the relation of first" ,
    //       second is relation
    //       first  is reverse
    // example: 123456 , 654321 , root
    // then =>  654321 is root , 123456 is child of 654321
    Q = Connection . sql . SelectFrom                                          (
          "first,second,relation"                                              ,
          PlanTable(TaskRelations)                                             ,
          QString("where first = %1 or second = %2 ;").arg(uuid).arg(uuid)   ) ;
    switch (Behaviors[uuid])                                                   {
      case Tasks::Undecided                                                    :
      break                                                                    ;
      //////////////////////////////////////////////////////////////////////////
      //                           Standalone tasks                           //
      //////////////////////////////////////////////////////////////////////////
      case Tasks::Standalone                                                   :
        ////////////////////////////////////////////////////////////////////////
        // This syntax search all possible relations,
        // not just uuid , but also its related UUIDs
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        //                     Build standalone relations                     //
        ////////////////////////////////////////////////////////////////////////
        // a standalone task has four possible relations :
        // 1. Prerequiste : pre-requirement task for this specific task
        //                  it is a procedural structure
        // 2. Heir        : tasks following after this task
        //                  it is a procedural structure
        // 3. Root        : the starting task of a framework task or a major task
        //                  it is a hierarchical structure
        // 4. Child       : the child task of a framework task or a major task
        //                  it is a hierarchical structure
        ////////////////////////////////////////////////////////////////////////
        SqlLoopNow ( Connection , Q )                                          ;
          SUID first    = Connection . Uuid (0)                                ;
          SUID second   = Connection . Uuid (1)                                ;
          int  relation = Connection . Int  (2)                                ;
          //////////////////////////////////////////////////////////////////////
          switch (relation)                                                    {
            case Tasks::Prerequiste                                            :
              // Decision node can have multiple connections
              if (Behaviors[first ]==Tasks::Decision                          ||
                  Behaviors[second]==Tasks::Decision  )                        {
                bool f = Behaviors[first ]==Tasks::Decision                    ;
                bool s = Behaviors[second]==Tasks::Decision                    ;
                if (f && s)                                                    {
                  UUIDs ff = Entries . values ( first  )                       ;
                  UUIDs ss = Heirs   . values ( second )                       ;
                  if (!ff.contains(second)) Entries . insert (first ,second)   ;
                  if (!ss.contains(first )) Heirs   . insert (second,first )   ;
                } else
                if (f)                                                         {
                  UUIDs ff = Entries.values(first)                             ;
                  if (!ff.contains(second)) Entries.insert(first,second)       ;
                  Successors[second] = first                                   ;
                } else
                if (s)                                                         {
                  UUIDs ss = Heirs.values(second)                              ;
                  if (!ss.contains(first)) Heirs.insert(second,first)          ;
                  Predecessors[second] = first                                 ;
                }                                                              ;
              } else                                                           {
                Predecessors[first ] = second                                  ;
                Successors  [second] = first                                   ;
              }                                                                ;
            break                                                              ;
            ////////////////////////////////////////////////////////////////////
            case Tasks::Heir                                                   :
              // Decision node can have multiple connections
              if (Behaviors[first ]==Tasks::Decision                          ||
                  Behaviors[second]==Tasks::Decision  )                        {
                bool f = Behaviors[first ]==Tasks::Decision                    ;
                bool s = Behaviors[second]==Tasks::Decision                    ;
                if (f && s)                                                    {
                  UUIDs ss = Heirs   . values ( first  )                       ;
                  UUIDs ff = Entries . values ( second )                       ;
                  if (!ss.contains(second)) Heirs   . insert ( first ,second ) ;
                  if (!ff.contains(first )) Entries . insert ( second,first  ) ;
                } else
                if (f)                                                         {
                  UUIDs ss = Heirs   . values ( first  )                       ;
                  if (!ss.contains(second)) Heirs   . insert ( first ,second ) ;
                  Predecessors[second] = first                                 ;
                } else
                if (s)                                                         {
                  UUIDs ff = Entries . values ( second )                       ;
                  if (!ff.contains(first )) Entries . insert ( second ,first ) ;
                  Successors[second] = first                                   ;
                }                                                              ;
              } else                                                           {
                Predecessors[second] = first                                   ;
                Successors  [first ] = second                                  ;
              }                                                                ;
            break                                                              ;
            case Tasks::Root                                                   :
              Roots       [first] = true                                       ;
              Parents     [first] = second                                     ;
            break                                                              ;
            case Tasks::Child                                                  :
              Roots       [first] = false                                      ;
              Parents     [first] = second                                     ;
            break                                                              ;
          }                                                                    ;
        SqlLoopErr ( Connection , Q )                                          ;
        SqlLoopEnd ( Connection , Q )                                          ;
        ////////////////////////////////////////////////////////////////////////
      break                                                                    ;
      //////////////////////////////////////////////////////////////////////////
      //                           Framework tasks                            //
      //////////////////////////////////////////////////////////////////////////
      case Tasks::Framework                                                    :
        SqlLoopNow ( Connection , Q )                                          ;
          SUID first    = Connection . Uuid (0)                                ;
          SUID second   = Connection . Uuid (1)                                ;
          int  relation = Connection . Int  (2)                                ;
          switch (relation)                                                    {
            case Tasks::Prerequiste                                            :
              if (Behaviors[first ]==Tasks::Decision                          ||
                  Behaviors[second]==Tasks::Decision                         ) {
                bool f = Behaviors[first ]==Tasks::Decision                    ;
                bool s = Behaviors[second]==Tasks::Decision                    ;
                if (f && s)                                                    {
                  UUIDs ff = Entries . values ( first  )                       ;
                  UUIDs ss = Heirs   . values ( second )                       ;
                  if (!ff.contains(second)) Entries . insert( first  , second) ;
                  if (!ss.contains(first )) Heirs   . insert( second , first ) ;
                } else
                if (f)                                                         {
                  UUIDs ff = Entries.values(first)                             ;
                  if (!ff.contains(second)) Entries . insert( first , second ) ;
                  Successors[second] = first                                   ;
                } else
                if (s)                                                         {
                  UUIDs ss = Heirs.values(second)                              ;
                  if (!ss.contains(first)) Heirs.insert(second,first)          ;
                  Predecessors[second] = first                                 ;
                }                                                              ;
              } else                                                           {
                Predecessors[first ] = second                                  ;
                Successors  [second] = first                                   ;
              }                                                                ;
            break                                                              ;
            case Tasks::Heir                                                   :
              if (Behaviors[first ]==Tasks::Decision                          ||
                  Behaviors[second]==Tasks::Decision                         ) {
                bool f = Behaviors[first ]==Tasks::Decision                    ;
                bool s = Behaviors[second]==Tasks::Decision                    ;
                if (f && s)                                                    {
                  UUIDs ss = Heirs   . values ( first  )                       ;
                  UUIDs ff = Entries . values ( second )                       ;
                  if (!ss.contains(second)) Heirs.insert(first,second)         ;
                  if (!ff.contains(first)) Entries.insert(second,first)        ;
                } else
                if (f)                                                         {
                  UUIDs ss = Heirs.values(first)                               ;
                  if (!ss.contains(second)) Heirs.insert(first,second)         ;
                  Predecessors[second] = first                                 ;
                } else
                if (s)                                                         {
                  UUIDs ff = Entries.values(second)                            ;
                  if (!ff.contains(first)) Entries.insert(second,first)        ;
                  Successors[second] = first                                   ;
                }                                                              ;
              } else                                                           {
                Predecessors[second] = first                                   ;
                Successors  [first ] = second                                  ;
              }                                                                ;
            break                                                              ;
            case Tasks::Root                                                   :
              Roots       [first] = true                                       ;
              Parents     [first] = second                                     ;
            break                                                              ;
            case Tasks::Child                                                  :
              Roots       [first] = false                                      ;
              Parents     [first] = second                                     ;
            break                                                              ;
          }                                                                    ;
        SqlLoopErr ( Connection , Q )                                          ;
        SqlLoopEnd ( Connection , Q )                                          ;
        ////////////////////////////////////////////////////////////////////////
      break                                                                    ;
      //////////////////////////////////////////////////////////////////////////
      //                           Decision tasks                             //
      //////////////////////////////////////////////////////////////////////////
      case Tasks::Decision                                                     :
        SqlLoopNow ( Connection , Q )                                          ;
          SUID first    = Connection . Uuid (0)                                ;
          SUID second   = Connection . Uuid (1)                                ;
          int  relation = Connection . Int  (2)                                ;
          switch (relation)                                                    {
            case Tasks::Prerequiste                                            :
              if (Behaviors[first ]==Tasks::Decision                          ||
                  Behaviors[second]==Tasks::Decision                         ) {
                bool f = Behaviors[first ]==Tasks::Decision                    ;
                bool s = Behaviors[second]==Tasks::Decision                    ;
                if (f && s)                                                    {
                  UUIDs ff = Entries . values ( first  )                       ;
                  UUIDs ss = Heirs   . values ( second )                       ;
                  if (!ff.contains(second)) Entries . insert ( first ,second ) ;
                  if (!ss.contains(first )) Heirs   . insert ( second,first  ) ;
                } else
                if (f)                                                         {
                  UUIDs ff = Entries.values(first)                             ;
                  if (!ff.contains(second)) Entries.insert(first,second)       ;
                  Successors[second] = first                                   ;
                } else
                if (s)                                                         {
                  UUIDs ss = Heirs.values(second)                              ;
                  if (!ss.contains(first)) Heirs.insert(second,first)          ;
                  Predecessors[second] = first                                 ;
                }                                                              ;
              } else                                                           {
                Predecessors[first ] = second                                  ;
                Successors  [second] = first                                   ;
              }                                                                ;
            break                                                              ;
            case Tasks::Heir                                                   :
              if (Behaviors[first ]==Tasks::Decision                          ||
                  Behaviors[second]==Tasks::Decision                         ) {
                  bool f = Behaviors[first ]==Tasks::Decision                  ;
                  bool s = Behaviors[second]==Tasks::Decision                  ;
                if (f && s)                                                    {
                  UUIDs ss = Heirs   . values ( first  )                       ;
                  UUIDs ff = Entries . values ( second )                       ;
                  if (!ss.contains(second)) Heirs   . insert (first  ,second ) ;
                  if (!ff.contains(first )) Entries . insert (second ,first  ) ;
                } else
                if (f)                                                         {
                  UUIDs ss = Heirs.values(first)                               ;
                  if (!ss.contains(second)) Heirs.insert(first,second)         ;
                  Predecessors[second] = first                                 ;
                } else
                if (s)                                                         {
                  UUIDs ff = Entries.values(second)                            ;
                  if (!ff.contains(first)) Entries.insert(second,first)        ;
                  Successors[second] = first                                   ;
                }                                                              ;
              } else                                                           {
                Predecessors[second] = first                                   ;
                Successors  [first ] = second                                  ;
              }                                                                ;
            break                                                              ;
            case Tasks::Root                                                   :
              Roots       [first] = true                                       ;
              Parents     [first] = second                                     ;
            break                                                              ;
            case Tasks::Child                                                  :
              Roots       [first] = false                                      ;
              Parents     [first] = second                                     ;
            break                                                              ;
          }                                                                    ;
        SqlLoopErr ( Connection , Q )                                          ;
        SqlLoopEnd ( Connection , Q )                                          ;
      break                                                                    ;
      case Tasks::Obsolete                                                     :
      break                                                                    ;
    }                                                                          ;
  }                                                                            ;
  //////////////////////////////////////////////////////////////////////////////
  return Uuids . count ( )                                                     ;
}

NAMEs N::TaskManager::LoadNames(SqlConnection & Connection,UUIDs & Uuids,int language)
{
  NAMEs N                          ;
  SUID  uuid                       ;
  foreach ( uuid , Uuids )         {
    N[uuid] = Connection . getName (
                PlanTable(Names)   ,
                "uuid"             ,
                language           ,
                uuid             ) ;
  }                                ;
  return N                         ;
}

bool N::TaskManager::assureName(SqlConnection & Connection,SUID uuid,int language,QString name)
{
  QString Q                                                 ;
  Q = Connection . sql . SelectFrom("name",PlanTable(Names) ,
        QString("where uuid = %1 and language = %2"         )
        .arg(uuid).arg(language)                          ) ;
  if (Connection.Fetch(Q))                                  {
    Q = Connection.NameUpdate(PlanTable(Names))             ;
  } else                                                    {
    Q = Connection.NameSyntax(PlanTable(Names))             ;
  }                                                         ;
  return Connection.insertName(Q,uuid,language,name)        ;
}

bool N::TaskManager::UpdateStatus(SqlConnection & SC,SUID u,int s)
{
  QString Q                         ;
  Q = SC . sql . Update             (
        PlanTable(Tasks)            ,
        "where uuid = :UUID"        ,
        1                           ,
        "status"                  ) ;
  SC . Prepare     ( Q            ) ;
  SC . Bind        ( "uuid"   , u ) ;
  SC . Bind        ( "status" , s ) ;
  return SC . Exec (              ) ;
}

bool N::TaskManager::UpdateBehavior(SqlConnection & SC,SUID u,int b)
{
  QString Q                           ;
  Q = SC . sql . Update               (
        PlanTable(Tasks)              ,
        "where uuid = :UUID"          ,
        1                             ,
        "behavior"                  ) ;
  SC . Prepare     ( Q              ) ;
  SC . Bind        ( "uuid"     , u ) ;
  SC . Bind        ( "behavior" , b ) ;
  return SC . Exec (                ) ;
}

SUID N::TaskManager::NewTask(SqlConnection & SC,int behavior,int status,SUID flags)
{
  QString Q                                                     ;
  SUID    uuid                                                  ;
  uuid = SC . Unique ( PlanTable(MajorUuid) , "uuid" , 713128 ) ;
  SC . assureUuid ( PlanTable(MajorUuid) , uuid , Types::Task ) ;
  Q = SC . sql . InsertInto                                     (
        PlanTable(Tasks)                                        ,
        4                                                       ,
        "uuid"                                                  ,
        "behavior"                                              ,
        "status"                                                ,
        "flags"                                               ) ;
  SC . Prepare ( Q                     )                        ;
  SC . Bind    ( "uuid"     , uuid     )                        ;
  SC . Bind    ( "behavior" , behavior )                        ;
  SC . Bind    ( "status"   , status   )                        ;
  SC . Bind    ( "flags"    , flags    )                        ;
  SC . Exec    (                       )                        ;
  return uuid                                                   ;
}
