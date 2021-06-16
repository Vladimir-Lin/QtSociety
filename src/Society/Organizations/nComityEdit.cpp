#include <society.h>

N::ComityEdit:: ComityEdit ( QWidget * parent , Plan * p )
              : ItemEditor (           parent ,        p )
{
  Configure ( ) ;
}

N::ComityEdit::~ComityEdit(void)
{
}

void N::ComityEdit::Configure(void)
{
  ItemTable = PlanTable(Comity)                      ;
  ItemType  = Types::TypeOfOrganization              ;
  ////////////////////////////////////////////////////
  setAccessibleName ( "N::ComityEdit"              ) ;
  setObjectName     ( "N::ComityEdit"              ) ;
  ////////////////////////////////////////////////////
  NewTreeWidgetItem ( head                         ) ;
  head->setText     ( 0 , tr("Type"              ) ) ;
  setWindowTitle    ( tr("Types of organizations") ) ;
  setColumnCount    ( 2                            ) ;
  assignHeaderItems ( head                         ) ;
  setColumnWidth    ( 1 , 2                        ) ;
  ////////////////////////////////////////////////////
  plan -> setFont   ( this                         ) ;
  setDropFlag       ( DropOrganization , true      ) ;
}

bool N::ComityEdit::dropOrganizations(QWidget * source,QPointF pos,const UUIDs & Uuids)
{
  if ( source == this ) return true               ;
  QTreeWidgetItem * it = itemAt ( pos.toPoint() ) ;
  if (IsNull(it)) return true                     ;
  GroupItems GI ( plan )                          ;
  SUID u = nTreeUuid ( it , 0 )                   ;
  if (u<=0) return true                           ;
  SqlConnection SC ( plan -> sql )                ;
  if ( SC . open ( FunctionString ) )             {
    UUIDs U = Uuids                               ;
    GI . Join  ( SC                               ,
                 u                                ,
                 ItemType                         ,
                 Types  :: Organization           ,
                 Groups :: Subordination          ,
                 0                                ,
                 U                              ) ;
    SC . close (      )                           ;
  }                                               ;
  SC . remove  (      )                           ;
  Alert        ( Done )                           ;
  return true                                     ;
}
