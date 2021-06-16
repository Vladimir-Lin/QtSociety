#include <society.h>

#define TABLES(ID) plan->Tables[Tables::ID]

N::IpaEditor:: IpaEditor   ( QWidget * parent , Plan * p )
             :  TreeWidget (           parent ,        p )
{
  Configure ( ) ;
}

N::IpaEditor::~IpaEditor (void)
{
}

QSize N::IpaEditor::SuitableSize(void)
{
  QSize  s = size()             ;
  CUIDs  w = columnWidths()     ;
  int    t = 0                  ;
  if (w.count()<=0) return s    ;
  for (int i=0;i<w.count();i++) {
    t += w[i]                   ;
  }                             ;
  s . setWidth ( t )            ;
  return s                      ;
}

bool N::IpaEditor::FocusIn(void)
{
  connectAction(Menus::Insert    ,this,SLOT(New            ())) ;
  connectAction(Menus::Rename    ,this,SLOT(Rename         ())) ;
  connectAction(Menus::Refresh   ,this,SLOT(List           ())) ;
  connectAction(Menus::Copy      ,this,SLOT(CopyToClipboard())) ;
  connectAction(Menus::SelectNone,this,SLOT(SelectNone     ())) ;
  connectAction(Menus::SelectAll ,this,SLOT(SelectAll      ())) ;
  connectAction(Menus::Delete    ,this,SLOT(Delete         ())) ;
  return true                                                             ;
}

void N::IpaEditor::Configure(void)
{
  NewTreeWidgetItem            ( head                                  ) ;
  head -> setText              ( 0 , tr("Symbol" )                     ) ;
  head -> setText              ( 1 , tr("Name"   )                     ) ;
  head -> setText              ( 2 , tr("Unicode")                     ) ;
  setWindowTitle               ( tr("International Phonetic Alphabet") ) ;
  setDragDropMode              ( NoDragDrop                            ) ;
  setRootIsDecorated           ( false                                 ) ;
  setAlternatingRowColors      ( true                                  ) ;
  setSelectionMode             ( ExtendedSelection                     ) ;
  setColumnCount               ( 3                                     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded                 ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded                 ) ;
  assignHeaderItems            ( head                                  ) ;
  MountClicked                 ( 2                                     ) ;
  plan -> setFont              ( this                                  ) ;
}

QString N::IpaEditor::CharUnicode(ushort schar)
{
  ushort  uc = schar                       ;
  QString U  = QString::number ( uc , 16 ) ;
  QString S  = "0"                         ;
  if (U.length()<4)                        {
    S = S.repeated(4-U.length())           ;
    U = S + U                              ;
  }                                        ;
  return U.toUpper()                       ;
}

QString N::IpaEditor::SymbolUnicode(QString symbol)
{
  if (symbol.length()<=0) return ""   ;
  QStringList ss                      ;
  const ushort * us = symbol.utf16()  ;
  while ((*us)!=0)                    {
    ss << CharUnicode(*us)            ;
    us++                              ;
  }                                   ;
  return ss.join("/")                 ;
}

void N::IpaEditor::List(void)
{
  blockSignals ( true  )                                                    ;
  clear        (       )                                                    ;
  EnterSQL(SC,plan->sql)                                                    ;
    UUIDs   Uuids = SC.Uuids(TABLES(IPA),"uuid","order by id asc")          ;
    SUID    uuid                                                            ;
    QString Q                                                               ;
    foreach (uuid,Uuids)                                                    {
      QString name = SC.getName(TABLES(Names),"uuid",plan->LanguageId,uuid) ;
      QString symbol                                                        ;
      Q = SC.sql.SelectFrom("symbol",TABLES(IPA),SC.WhereUuid(uuid))        ;
      if (SC.Fetch(Q)) symbol = SC . String ( 0 )                           ;
      NewTreeWidgetItem  ( item                                  )          ;
      item->setData      ( 0 , Qt::UserRole,uuid                 )          ;
      item->setFont      ( 0 , plan->fonts[Fonts::IPA] )          ;
      item->setText      ( 0 , symbol                            )          ;
      item->setText      ( 1 , name                              )          ;
      item->setText      ( 2 , SymbolUnicode(symbol)             )          ;
      addTopLevelItem    ( item                                  )          ;
    }                                                                       ;
  LeaveSQL(SC,plan->sql)                                                    ;
  for (int i=0;i<columnCount();i++) resizeColumnToContents ( i )            ;
  blockSignals ( false )                                                    ;
  Alert ( Done )                                                            ;
}

void N::IpaEditor::New(void)
{
  NewTreeWidgetItem( item                 ) ;
  item -> setData  ( 0 , Qt::UserRole , 0 ) ;
  item -> setFont  ( 0 , plan->fonts[Fonts::IPA] ) ;
  addTopLevelItem  ( item                 ) ;
  scrollToItem     ( item                 ) ;
  doubleClicked    ( item , 0             ) ;
}

void N::IpaEditor::Rename(void)
{
  QTreeWidgetItem * item      ;
  item = currentItem()        ;
  if (IsNull(item)) return    ;
  int cc = currentColumn (  ) ;
  doubleClicked ( item , cc ) ;
}

void N::IpaEditor::doubleClicked (QTreeWidgetItem * item,int column)
{
  SUID uuid = nTreeUuid ( item , 0)      ;
  QLineEdit * line                       ;
  switch (column)                        {
    case 0                               :
      line = setLineEdit                 (
        item,column                      ,
        SIGNAL(editingFinished())        ,
        SLOT  (symbolChanged  ()) )      ;
      line->setFont(plan->fonts[Fonts::IPA]) ;
      line->setFocus(Qt::TabFocusReason) ;
    break                                ;
    case 1                               :
      if (uuid<=0) return                ;
      line = setLineEdit                 (
        item,column                      ,
        SIGNAL(editingFinished())        ,
        SLOT  (nameChanged    ()) )      ;
      line->setFocus(Qt::TabFocusReason) ;
    break                                ;
  }                                      ;
}

void N::IpaEditor::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return                  ;
  SUID uuid    = nTreeUuid(ItemEditing,ItemColumn) ;
  QString name = ItemEditing->text(0)              ;
  removeItemWidget (ItemEditing,ItemColumn)        ;
  if (uuid<=0 && name.length()<=0)                 {
    int index = indexOfTopLevelItem(ItemEditing)   ;
    takeTopLevelItem(index)                        ;
  }                                                ;
  ItemEditing = NULL                               ;
  ItemWidget  = NULL                               ;
  ItemColumn  = -1                                 ;
}

void N::IpaEditor::symbolChanged(void)
{
  if (IsNull(ItemEditing)) return                          ;
  if (IsNull(ItemWidget )) return                          ;
  QLineEdit * line    = Casting(QLineEdit,ItemWidget)      ;
  SUID        uuid    = nTreeUuid(ItemEditing,0)           ;
  QString     name    = ""                                 ;
  bool        success = false                              ;
  if (NotNull(line)) name = line->text()                   ;
  if (name.length()>0)                                     {
    EnterSQL ( SC , plan->sql )                            ;
      QString Q                                            ;
      if (uuid>0)                                          {
        Q = SC.sql.Update(TABLES(IPA)                      ,
              "where uuid = :UUID",1,"symbol"       )      ;
      } else                                               {
        uuid = SC.Unique(TABLES(MajorUuid)                 ,
                         "uuid",9001                )      ;
        SC.assureUuid(TABLES(MajorUuid)                    ,
                      uuid,Types::IPA     )      ;
        Q = SC.sql.InsertInto(TABLES(IPA)                  ,
              2,"uuid","symbol"                     )      ;
      }                                                    ;
      SC.Prepare ( Q                        )              ;
      SC.Bind    ( "uuid"   , uuid          )              ;
      SC.Bind    ( "symbol" , name.toUtf8() )              ;
      if (SC.Exec())                                       {
        ItemEditing -> setData ( ItemColumn                ,
                                 Qt::UserRole,uuid       ) ;
        ItemEditing -> setText ( ItemColumn , name       ) ;
        ItemEditing -> setText ( 2 , SymbolUnicode(name) ) ;
        success = true                                     ;
      }                                                    ;
    LeaveSQL ( SC , plan->sql )                            ;
  }                                                        ;
  removeOldItem ( )                                        ;
  if (success)                                             {
    Alert ( Done  )                                        ;
  } else                                                   {
    Alert ( Error )                                        ;
  }                                                        ;
}

void N::IpaEditor::nameChanged(void)
{
  if (IsNull(ItemEditing)) return                      ;
  if (IsNull(ItemWidget )) return                      ;
  QLineEdit * line    = Casting(QLineEdit,ItemWidget)  ;
  SUID        uuid    = nTreeUuid(ItemEditing,0)       ;
  QString     name    = ""                             ;
  bool        success = false                          ;
  if (NotNull(line)) name = line->text()               ;
  if (uuid>0 && name.length()>0)                       {
    EnterSQL ( SC , plan->sql )                        ;
      QString Q                                        ;
      Q = SC.sql.SelectFrom("name",TABLES(Names)       ,
            QString("where uuid = %1 and language = %2")
            .arg(uuid).arg(plan->LanguageId)         ) ;
      if (SC.Fetch(Q))                                 {
        Q = SC.NameUpdate(TABLES(Names))               ;
      } else                                           {
        Q = SC.NameSyntax(TABLES(Names))               ;
      }                                                ;
      if (SC.insertName(Q,uuid,plan->LanguageId,name)) {
        ItemEditing -> setText ( ItemColumn , name )   ;
        success = true                                 ;
      }                                                ;
    LeaveSQL ( SC , plan->sql )                        ;
  }                                                    ;
  removeOldItem ( )                                    ;
  if (success)                                         {
    Alert ( Done  )                                    ;
  } else                                               {
    Alert ( Error )                                    ;
  }                                                    ;
}

bool N::IpaEditor::Menu(QPoint)
{
  nScopedMenu ( mm , this )     ;
  QAction    * aa               ;
  mm.add(101,tr("New"        )) ;
  mm.add(102,tr("Rename"     )) ;
  mm.add(103,tr("Refresh"    )) ;
  mm.addSeparator (           ) ;
  mm.add(201,tr("Copy"       )) ;
  mm.add(202,tr("Select all" )) ;
  mm.add(203,tr("Select none")) ;
  mm.setFont(plan)              ;
  aa = mm.exec()                ;
  if (IsNull(aa)) return true   ;
  switch (mm[aa])               {
    case 101                    :
      New             ( )       ;
    break                       ;
    case 102                    :
      Rename          ( )       ;
    break                       ;
    case 103                    :
      List            ( )       ;
    break                       ;
    case 201                    :
      CopyToClipboard ( )       ;
    break                       ;
    case 202                    :
      SelectAll       ( )       ;
    break                       ;
    case 203                    :
      SelectNone      ( )       ;
    break                       ;
  }                             ;
  return true                   ;
}
