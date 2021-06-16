#include <society.h>

#pragma message("OrganizationProcessor is about to be obsoleted")

N::OrganizationProcessor:: OrganizationProcessor ( QWidget * parent , Plan * p )
                         : InteractiveEditor     (           parent ,        p )
                         , Object                ( 0 , Types::People           )
{
  Configure ( ) ;
}

N::OrganizationProcessor::~OrganizationProcessor(void)
{
}

void N::OrganizationProcessor::Configure(void)
{
}

void N::OrganizationProcessor::run(int Type,ThreadData * data)
{
}

void N::OrganizationProcessor::Enter(void)
{
  if (Commands.length()<=0) return ;
  Report ( Commands )              ;
  Report ( "\n"     )              ;
  if (Interpret(Commands))         {
    Queues  << Commands            ;
  }                                ;
  Commands = ""                    ;
}

void N::OrganizationProcessor::Help(void)
{
  Report ( tr("clear (clear display messages)") ) ;
  Report ( "\n"                                 ) ;
  Report ( tr("help (get command lists)"      ) ) ;
  Report ( "\n"                                 ) ;
}

bool N::OrganizationProcessor::Interpret(QString command)
{
  QStringList C = command.split(' ')            ;
  int         s = 0                             ;
  if (C.count()==1 && C[0].toLower()=="clear")  {
    s = 10001                                   ;
  } else
  if (C.count()==1 && C[0].toLower()=="help")   {
    s = 10002                                   ;
  }                                             ;
  switch (s)                                    {
    case 10001                                  :
      Clear       (         )                   ;
    return false                                ;
    case 10002                                  :
      Help        (         )                   ;
    return false                                ;
    default                                     :
    return false                                ;
  }                                             ;
  return true                                   ;
}
