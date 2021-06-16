#include <qtfoundation.h>

QByteArray N::Cpp::       ExportToCpp (
             QByteArray & Data        ,
             QString      DataName    ,
             bool         ExportSize  ,
             QString      DataSize    )
{
  QByteArray  Body                               ;
  QStringList Lines                              ;
  QString     Line                               ;
  ////////////////////////////////////////////////
  int             t = Data.size()                ;
  unsigned char * d                              ;
  d = (unsigned char *)Data.data()               ;
  ////////////////////////////////////////////////
  if (ExportSize)                                {
    Line = QString("int           %1 = %2 ;"     )
            .arg(DataSize                        )
            .arg(t                             ) ;
    Lines << Line                                ;
  }                                              ;
  ////////////////////////////////////////////////
  QString space = " "                            ;
  Line  = QString("unsigned char %1 [ %2 ] ="    )
          .arg(DataName                          )
          .arg(t                               ) ;
  Line += space.repeated(79-Line.length())       ;
  Line += "{"                                    ;
  Lines << Line                                  ;
  ////////////////////////////////////////////////
  int     i = 0                                  ;
  char    x[17] = "0123456789ABCDEF"             ;
  while (i<t)                                    {
    QList<int>  V                                ;
    QStringList S                                ;
    V . clear ( )                                ;
    if ((i+15)<=t)                               {
      for (int j=0;j<16;j++,i++) V << d[i]       ;
    } else                                       {
      while (i<t)                                {
        V << d[i]                                ;
        i++                                      ;
      }                                          ;
    }                                            ;
    for (int i=0;i<V.count();i++)                {
      int v = V[i]                               ;
      int H = v / 16                             ;
      int L = v % 16                             ;
      S << QString("0x%1%2").arg(x[H]).arg(x[L]) ;
    }                                            ;
    if (V.count()>0)                             {
      Line = S.join(",")                         ;
      if (V.count()>=16) Line += ","             ;
      Lines << Line                              ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  Line = QString("}%1;").arg(space.repeated(78)) ;
  Lines << Line                                  ;
  Line = ""                                      ;
  Lines << Line                                  ;
  ////////////////////////////////////////////////
  Line = Lines.join   ("\n")                     ;
  Body = Line .toUtf8 (    )                     ;
  ////////////////////////////////////////////////
  return Body                                    ;
}

bool N::Cpp::  ExportToCpp (
       QString source      ,
       QString filename    ,
       QString DataName    ,
       bool    ExportSize  ,
       QString DataSize    )
{
  QByteArray Data                                    ;
  QFile F(source)                                    ;
  if (F.open(QIODevice::ReadOnly))                   {
    Data = F.readAll()                               ;
    F.close()                                        ;
  }                                                  ;
  nKickOut ( Data.size() <= 0 , false )              ;
  QByteArray Body                                    ;
  Body = N::Cpp::ExportToCpp                         (
           Data                                      ,
           DataName                                  ,
           ExportSize                                ,
           DataSize                                ) ;
  nKickOut ( Body.size() <= 0 , false )              ;
  QString ss = QString("// Data source from %1\n"    )
               .arg(source)                          ;
  Body . insert ( 0 , ss . toUtf8 ( ) )              ;
  QFile Z(filename)                                  ;
  nKickOut ( !Z.open(QIODevice::WriteOnly) , false ) ;
  Z.write  ( Body                                  ) ;
  Z.close  (                                       ) ;
  QFileInfo ZI(filename)                             ;
  return ZI.exists()                                 ;
}

QByteArray N::Cpp::       FixedToCpp  (
             QByteArray & Data        ,
             int          Type        ,
             QString      DataName    ,
             bool         ExportSize  ,
             QString      DataSize    )
{
  QByteArray           Body                                    ;
  QStringList          Lines                                   ;
  QString              Line                                    ;
  QString              space = " "                             ;
  int                  t   = Data.size()                       ;
  short              * dsi = (short              *)Data.data() ;
  unsigned short     * dsu = (unsigned short     *)Data.data() ;
  int                * dii = (int                *)Data.data() ;
  unsigned int       * diu = (unsigned int       *)Data.data() ;
  long long          * dli = (long long          *)Data.data() ;
  unsigned long long * dlu = (unsigned long long *)Data.data() ;
  switch (Type)                                                {
    case N::Cpp::Short                                         :
      t /= 2                                                   ;
      if (ExportSize)                                          {
        Line = QString("int           %1 = %2 ;"               )
                .arg(DataSize                                  )
                .arg(t                                       ) ;
        Lines << Line                                          ;
      }                                                        ;
      Line  = QString("short %1 [ %2 ] ="                      )
              .arg(DataName                                    )
              .arg(t                                         ) ;
      Line += space.repeated(79-Line.length())                 ;
      Line += "{"                                              ;
      Lines << Line                                            ;
      for (int i=0;i<t;i++)                                    {
        Line = QString::number(dsi[i])                         ;
        if ((i+1)<t) Line += " ,"                              ;
        Lines << Line                                          ;
      }                                                        ;
    break                                                      ;
    case N::Cpp::UShort                                        :
      t /= 2                                                   ;
      if (ExportSize)                                          {
        Line = QString("int           %1 = %2 ;"               )
                .arg(DataSize                                  )
                .arg(t                                       ) ;
        Lines << Line                                          ;
      }                                                        ;
      Line  = QString("unsigned short %1 [ %2 ] ="             )
              .arg(DataName                                    )
              .arg(t                                         ) ;
      Line += space.repeated(79-Line.length())                 ;
      Line += "{"                                              ;
      Lines << Line                                            ;
      for (int i=0;i<t;i++)                                    {
        Line = QString::number(dsu[i])                         ;
        if ((i+1)<t) Line += " ,"                              ;
        Lines << Line                                          ;
      }                                                        ;
    break                                                      ;
    case N::Cpp::Integer                                       :
      t /= 4                                                   ;
      if (ExportSize)                                          {
        Line = QString("int           %1 = %2 ;"               )
                .arg(DataSize                                  )
                .arg(t                                       ) ;
        Lines << Line                                          ;
      }                                                        ;
      Line  = QString("int %1 [ %2 ] ="                        )
              .arg(DataName                                    )
              .arg(t                                         ) ;
      Line += space.repeated(79-Line.length())                 ;
      Line += "{"                                              ;
      Lines << Line                                            ;
      for (int i=0;i<t;i++)                                    {
        Line = QString::number(dii[i])                         ;
        if ((i+1)<t) Line += " ,"                              ;
        Lines << Line                                          ;
      }                                                        ;
    break                                                      ;
    case N::Cpp::UInt                                          :
      t /= 4                                                   ;
      if (ExportSize)                                          {
        Line = QString("int           %1 = %2 ;"               )
                .arg(DataSize                                  )
                .arg(t                                       ) ;
        Lines << Line                                          ;
      }                                                        ;
      Line  = QString("unsigned int %1 [ %2 ] ="               )
              .arg(DataName                                    )
              .arg(t                                         ) ;
      Line += space.repeated(79-Line.length())                 ;
      Line += "{"                                              ;
      Lines << Line                                            ;
      for (int i=0;i<t;i++)                                    {
        Line = QString::number(diu[i])                         ;
        if ((i+1)<t) Line += " ,"                              ;
        Lines << Line                                          ;
      }                                                        ;
    break                                                      ;
    case N::Cpp::LongLong                                      :
      t /= 8                                                   ;
      if (ExportSize)                                          {
        Line = QString("int           %1 = %2 ;"               )
                .arg(DataSize                                  )
                .arg(t                                       ) ;
        Lines << Line                                          ;
      }                                                        ;
      Line  = QString("long long %1 [ %2 ] ="                  )
              .arg(DataName                                    )
              .arg(t                                         ) ;
      Line += space.repeated(79-Line.length())                 ;
      Line += "{"                                              ;
      Lines << Line                                            ;
      for (int i=0;i<t;i++)                                    {
        Line = QString::number(dli[i])                         ;
        if ((i+1)<t) Line += " ,"                              ;
        Lines << Line                                          ;
      }                                                        ;
    break                                                      ;
    case N::Cpp::ULongLong                                     :
      t /= 8                                                   ;
      if (ExportSize)                                          {
        Line = QString("int           %1 = %2 ;"               )
                .arg(DataSize                                  )
                .arg(t                                       ) ;
        Lines << Line                                          ;
      }                                                        ;
      Line  = QString("unsigned long long %1 [ %2 ] ="         )
              .arg(DataName                                    )
              .arg(t                                         ) ;
      Line += space.repeated(79-Line.length())                 ;
      Line += "{"                                              ;
      Lines << Line                                            ;
      for (int i=0;i<t;i++)                                    {
        Line = QString::number(dlu[i])                         ;
        if ((i+1)<t) Line += " ,"                              ;
        Lines << Line                                          ;
      }                                                        ;
    break                                                      ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  Line = QString("}%1;").arg(space.repeated(78))               ;
  Lines << Line                                                ;
  Line = ""                                                    ;
  Lines << Line                                                ;
  //////////////////////////////////////////////////////////////
  Line = Lines.join   ("\n")                                   ;
  Body = Line .toUtf8 (    )                                   ;
  //////////////////////////////////////////////////////////////
  return Body                                                  ;
}

QByteArray N::Cpp::       FloatingToCpp (
             QByteArray & Data          ,
             int          Type          ,
             QString      DataName      ,
             bool         isPrecision   ,
             int          Precision     ,
             bool         ExportSize    ,
             QString      DataSize      )
{
  QByteArray  Body                                   ;
  QStringList Lines                                  ;
  QString     Line                                   ;
  QString     space = " "                            ;
  int         t     = Data.size()                    ;
  int         pc    = Precision                      ;
  float       * dsf = (float       *)Data.data()     ;
  double      * dsd = (double      *)Data.data()     ;
  long double * dld = (long double *)Data.data()     ;
  switch (Type)                                      {
    case N::Cpp::Float                               :
      t /=  4                                        ;
      if (ExportSize)                                {
        Line = QString("int           %1 = %2 ;"     )
                .arg(DataSize                        )
                .arg(t                             ) ;
        Lines << Line                                ;
      }                                              ;
      Line  = QString("float %1 [ %2 ] ="            )
              .arg(DataName                          )
              .arg(t                               ) ;
      Line += space.repeated(79-Line.length())       ;
      Line += "{"                                    ;
      Lines << Line                                  ;
      for (int i=0;i<t;i++)                          {
        if (isPrecision)                             {
          Line = QString::number(dsf[i],'f',pc)      ;
        } else                                       {
          Line = QString("%1").arg(dsf[i])           ;
        }                                            ;
        if ((i+1)<t) Line += " ,"                    ;
        Lines << Line                                ;
      }                                              ;
    break                                            ;
    case N::Cpp::Double                              :
      t /=  8                                        ;
      if (ExportSize)                                {
        Line = QString("int           %1 = %2 ;"     )
                .arg(DataSize                        )
                .arg(t                             ) ;
        Lines << Line                                ;
      }                                              ;
      Line  = QString("double %1 [ %2 ] ="           )
              .arg(DataName                          )
              .arg(t                               ) ;
      Line += space.repeated(79-Line.length())       ;
      Line += "{"                                    ;
      Lines << Line                                  ;
      for (int i=0;i<t;i++)                          {
        if (isPrecision)                             {
          Line = QString::number(dsd[i],'f',pc)      ;
        } else                                       {
          Line = QString("%1").arg(dsd[i])           ;
        }                                            ;
        if ((i+1)<t) Line += " ,"                    ;
        Lines << Line                                ;
      }                                              ;
    break                                            ;
    case N::Cpp::LDouble                             :
      t /= 16                                        ;
      if (ExportSize)                                {
        Line = QString("int           %1 = %2 ;"     )
                .arg(DataSize                        )
                .arg(t                             ) ;
        Lines << Line                                ;
      }                                              ;
      Line  = QString("long double %1 [ %2 ] ="      )
              .arg(DataName                          )
              .arg(t                               ) ;
      Line += space.repeated(79-Line.length())       ;
      Line += "{"                                    ;
      Lines << Line                                  ;
      for (int i=0;i<t;i++)                          {
        if (isPrecision)                             {
          Line = QString::number(dld[i],'f',pc)      ;
        } else                                       {
          Line = QString("%1").arg((double)dld[i])   ;
        }                                            ;
        if ((i+1)<t) Line += " ,"                    ;
        Lines << Line                                ;
      }                                              ;
    break                                            ;
  }                                                  ;
  ////////////////////////////////////////////////////
  Line = QString("}%1;").arg(space.repeated(78))     ;
  Lines << Line                                      ;
  Line = ""                                          ;
  Lines << Line                                      ;
  ////////////////////////////////////////////////////
  Line = Lines.join   ("\n")                         ;
  Body = Line .toUtf8 (    )                         ;
  ////////////////////////////////////////////////////
  return Body                                        ;
}
