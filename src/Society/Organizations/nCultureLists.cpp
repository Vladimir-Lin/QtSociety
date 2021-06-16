#include <society.h>

N::CultureLists:: CultureLists (QWidget * parent,Plan * p)
                : TreeWidget   (          parent,       p)
                , sorting      (Qt::AscendingOrder       )
{
  Configure ( ) ;
}

N::CultureLists::~CultureLists (void)
{
}

QSize N::CultureLists::sizeHint(void) const
{
  return QSize ( 240 , 240 ) ;
}

void N::CultureLists::Configure(void)
{
  //////////////////////////////////////////////////////
  setFocusPolicy          ( Qt::WheelFocus           ) ;
  setDragDropMode         ( DragDrop                 ) ;
  setAlternatingRowColors ( true                     ) ;
  setRootIsDecorated      ( false                    ) ;
  setSelectionMode        ( ExtendedSelection        ) ;
  setColumnCount          ( 1                        ) ;
  //////////////////////////////////////////////////////
  setWindowTitle          ( tr("Culture lists")      ) ;
  NewTreeWidgetItem       ( head                     ) ;
  head -> setText         ( 0    , tr("Kulturkreis") ) ;
  assignHeaderItems       ( head                     ) ;
  MountClicked            ( 1                        ) ;
  MountClicked            ( 2                        ) ;
  plan->setFont           ( this                     ) ;
}

bool N::CultureLists::FocusIn(void)
{
  AssignAction ( Label      , windowTitle     ( ) ) ;
  LinkAction   ( Insert     , New             ( ) ) ;
  LinkAction   ( Paste      , Paste           ( ) ) ;
  LinkAction   ( Refresh    , startup         ( ) ) ;
  LinkAction   ( Rename     , Rename          ( ) ) ;
  LinkAction   ( Search     , Search          ( ) ) ;
  LinkAction   ( Copy       , CopyToClipboard ( ) ) ;
  LinkAction   ( SelectAll  , SelectAll       ( ) ) ;
  LinkAction   ( SelectNone , SelectNone      ( ) ) ;
  return true                                       ;
}

void N::CultureLists::run(int type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch (type)     {
    case 10001      :
      List ( )      ;
    break           ;
  }                 ;
}

bool N::CultureLists::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::CultureLists::List(void)
{
  TreeWidgetItems items                  ;
  clear            (           )         ;
  SqlConnection SC ( plan->sql )         ;
  if (SC.open("CultureLists","List"))    {
    UUIDs Uuids                          ;
    SUID  u                              ;
    Uuids = SC.Uuids                     (
              PlanTable(Cultures)        ,
              "uuid"                     ,
              SC.OrderBy("id",sorting) ) ;
    foreach (u,Uuids)                    {
      NewTreeWidgetItem ( IT )           ;
      QString name                       ;
      name = SC.getName                  (
               PlanTable(Names)          ,
               "uuid"                    ,
               vLanguageId               ,
               u                       ) ;
      IT -> setData ( 0,Qt::UserRole,u ) ;
      IT -> setText ( 0,name           ) ;
      items << IT                        ;
    }                                    ;
    SC.close (       )                   ;
  } else                                 {
    Alert    ( Error )                   ;
    SC.close (       )                   ;
  }                                      ;
  SC.remove()                            ;
  if (items.count()>0)                   {
    addTopLevelItems ( items )           ;
    Alert            ( Done  )           ;
  }                                      ;
}

void N::CultureLists::New(void)
{
  NewItem ( 0 ) ;
}

void N::CultureLists::Rename(void)
{
  RenameItem ( 0 ) ;
}

void N::CultureLists::Paste(void)
{
  QString S = nClipboardText                                   ;
  if (S.length()<=0)                                           {
    Alert ( Error )                                            ;
    return                                                     ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  QStringList L = S.split("\n")                                ;
  L = File::PurifyLines(L)                                     ;
  if (L.count()<=0)                                            {
    Alert ( Error )                                            ;
    return                                                     ;
  }                                                            ;
  QStringList E = columnStrings ( 0 )                          ;
  QStringList A                                                ;
  A = L - E                                                    ;
  if (A.count()<=0)                                            {
    Alert ( Error )                                            ;
    return                                                     ;
  }                                                            ;
  //////////////////////////////////////////////////////////////
  emit OnBusy ( )                                              ;
  EnterSQL ( SC , plan->sql )                                  ;
    foreach (S,A)                                              {
      NewTreeWidgetItem ( IT )                                 ;
      SUID u = SC.Unique(PlanTable(MajorUuid),"uuid",1977)     ;
      SC.assureUuid(PlanTable(MajorUuid),u,Types::Culture)     ;
      SC.insertUuid (PlanTable(Cultures),u,"uuid")             ;
      SC.assureName(PlanTable(Names),u,vLanguageId,S)          ;
      IT->setData(0,Qt::UserRole,u)                            ;
      IT->setText(0,S)                                         ;
      addTopLevelItem(IT)                                      ;
    }                                                          ;
    Alert  ( Done           )                                  ;
  ErrorSQL ( SC , plan->sql )                                  ;
    Alert  ( Error          )                                  ;
  LeaveSQL ( SC , plan->sql )                                  ;
  //////////////////////////////////////////////////////////////
  emit GoRelax (      )                                        ;
}

void N::CultureLists::Search(void)
{
  Alert ( Error ) ;
}

void N::CultureLists::singleClicked(QTreeWidgetItem * item,int column)
{ Q_UNUSED ( item   ) ;
  Q_UNUSED ( column ) ;
  Alert    ( Click  ) ;
}

void N::CultureLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  nDropOut ( IsNull(item) )              ;
  QLineEdit * le = NULL                  ;
  switch ( column )                      {
    case 0                               :
      le = setLineEdit                   (
             item                        ,
             0                           ,
             SIGNAL(returnPressed  ())   ,
             SLOT  (editingFinished()) ) ;
      le->setFocus(Qt::TabFocusReason)   ;
    break                                ;
  }                                      ;
}

void N::CultureLists::removeOldItem(void)
{
  TreeWidget :: removeOldItem ( true , 0 ) ;
}

void N::CultureLists::editingFinished(void)
{
  SUID        u  = nTreeUuid(ItemEditing,ItemColumn)                 ;
  QLineEdit * le = Casting(QLineEdit,ItemWidget)                     ;
  if (NotNull(le))                                                   {
    QString name = le->text()                                        ;
    if (name.length()>0)                                             {
      AbstractGui::Mutex.lock  ()                                    ;
      SqlConnection SC ( plan->sql )                                 ;
      if (SC.open("CultureLists","editingFinished"))                 {
        if (u<=0)                                                    {
          u = SC.Unique(PlanTable(MajorUuid),"uuid",1977)            ;
          if (!SC.assureUuid(PlanTable(MajorUuid),u,Types::Culture)) {
            u = 0                                                    ;
          }                                                          ;
          if (u>0)                                                   {
            if (!SC.insertUuid (PlanTable(Cultures),u,"uuid"))       {
              u = 0                                                  ;
            }                                                        ;
          }                                                          ;
        }                                                            ;
        if (u>0)                                                     {
          SC.assureName(PlanTable(Names),u,vLanguageId,name)         ;
        }                                                            ;
        Alert ( Done )                                               ;
        SC.close()                                                   ;
      }                                                              ;
      SC.remove()                                                    ;
      AbstractGui::Mutex.unlock()                                    ;
      if (u>0)                                                       {
        ItemEditing->setText(0,name          )                       ;
        ItemEditing->setData(0,Qt::UserRole,u)                       ;
      }                                                              ;
    }                                                                ;
  }                                                                  ;
  removeOldItem ( )                                                  ;
}

void N::CultureLists::Translation(void)
{
  UUIDs Uuids = itemUuids ( 0 )              ;
  emit Translations ( windowTitle() , Uuids) ;
}

bool N::CultureLists::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                    ;
  QAction * aa                                 ;
  QMenu   * ms                                 ;
  QMenu   * ma                                 ;
  QMenu   * mp                                 ;
  QTreeWidgetItem * item = currentItem ( )     ;
  mm . add          ( 101 , tr("Refresh")    ) ;
  mm . add          ( 102 , tr("Insert" )    ) ;
  mm . add          ( 103 , tr("Paste"  )    ) ;
  nIfSafe(item)                                {
    mm . add        ( 106 , tr("Rename" )    ) ;
  }                                            ;
  mm . add          ( 301 , tr("Search"    ) ) ;
  mm . addSeparator (                        ) ;
  ma = mm . addMenu ( tr("Adjustments")      ) ;
  mp = mm . addMenu ( tr("Selection"  )      ) ;
  ms = mm . addMenu ( tr("Sorting"    )      ) ;
  AdjustMenu        ( mm , ma                ) ;
  SelectionsMenu    ( mm , mp                ) ;
  SortingMenu       ( mm , ms                ) ;
  mm . addSeparator ( ma                     ) ;
  mm . add          ( ma , 201 , tr("Multilingual translations") ) ;
  mm . setFont      ( plan                   ) ;
  if (plan->Booleans["Desktop"])               {
    pos = QCursor::pos()                       ;
  } else                                       {
    pos = mapToGlobal ( pos )                  ;
  }                                            ;
  aa = mm.exec      ( pos                    ) ;
  if (!plan->Booleans["Desktop"])              {
    allowGesture = true                        ;
  }                                            ;
  nKickOut          ( IsNull(aa) , true      ) ;
  if ( RunSorting    ( mm , aa ) ) return true ;
  if ( RunAdjustment ( mm , aa ) ) return true ;
  if ( RunSelections ( mm , aa ) ) return true ;
  switch (mm[aa])                              {
    nFastCast  ( 101 , List           ( ) )    ;
    nFastCast  ( 102 , New            ( ) )    ;
    nFastCast  ( 103 , Paste          ( ) )    ;
    nFastCast  ( 106 , Rename         ( ) )    ;
    nFastCast  ( 201 , Translation    ( ) )    ;
    nFastCast  ( 301 , Search         ( ) )    ;
  }                                            ;
  return true                                  ;
}
