#include <society.h>

N::ProfessionLists:: ProfessionLists ( QWidget * parent , Plan * p )
                   : ItemEditor      (           parent ,        p )
{
  Configure ( ) ;
}

N::ProfessionLists::~ProfessionLists(void)
{
}

void N::ProfessionLists::Configure(void)
{
  ItemTable = PlanTable(Occupations)         ;
  ItemType  = Types::Occupation              ;
  ////////////////////////////////////////////
  setAccessibleName ( "N::ProfessionLists" ) ;
  setObjectName     ( "N::ProfessionLists" ) ;
  ////////////////////////////////////////////
  NewTreeWidgetItem ( head                 ) ;
  head -> setText   ( 0 , tr("Occupation") ) ;
  head -> setText   ( 1 , ""               ) ;
  setWindowTitle    ( tr("Occupations")    ) ;
  setColumnCount    ( 2                    ) ;
  assignHeaderItems ( head                 ) ;
  setColumnWidth    ( 1 , 2                ) ;
  ////////////////////////////////////////////
  plan -> setFont   ( this                 ) ;
}
