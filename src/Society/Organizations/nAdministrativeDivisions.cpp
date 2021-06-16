#include <society.h>

N::AdministrativeDivisions:: AdministrativeDivisions ( QWidget * parent , Plan * p )
                           : ItemEditor              (           parent ,        p )
{
  Configure ( ) ;
}

N::AdministrativeDivisions::~AdministrativeDivisions(void)
{
}

void N::AdministrativeDivisions::Configure(void)
{
  ItemTable = PlanTable(Administrations)               ;
  ItemType  = Types::Administrative                    ;
  //////////////////////////////////////////////////////
  setAccessibleName ( "N::AdministrativeDivisions"   ) ;
  setObjectName     ( "N::AdministrativeDivisions"   ) ;
  //////////////////////////////////////////////////////
  NewTreeWidgetItem ( head                           ) ;
  head->setText     ( 0 , tr("Division name"       ) ) ;
  setWindowTitle    ( tr("Administrative divisions") ) ;
  setColumnCount    ( 2                              ) ;
  assignHeaderItems ( head                           ) ;
  setColumnWidth    ( 1 , 2                          ) ;
  //////////////////////////////////////////////////////
  plan -> setFont   ( this                           ) ;
}
