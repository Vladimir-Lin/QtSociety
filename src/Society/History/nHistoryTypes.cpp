#include <society.h>

N::HistoryTypes:: HistoryTypes   ( QWidget * parent , Plan * p )
                : TreeWidget     (           parent ,        p )
                , HistoryManager (                           p )
                , sorting        ( Qt::AscendingOrder          )
{
  Configure ( ) ;
}

N::HistoryTypes::~HistoryTypes (void)
{
}

void N::HistoryTypes::Configure(void)
{
  setAccessibleName       ( "N::HistoryTypes"             ) ;
  setObjectName           ( "N::HistoryTypes"             ) ;
  ///////////////////////////////////////////////////////////
  setFocusPolicy          ( Qt::WheelFocus                ) ;
  setDragDropMode         ( NoDragDrop                    ) ;
  setAlternatingRowColors ( true                          ) ;
  setRootIsDecorated      ( false                         ) ;
  setSelectionMode        ( ExtendedSelection             ) ;
  setSuggestion           ( QSize ( 240 , 320 )           ) ;
  setColumnCount          ( 2                             ) ;
  ///////////////////////////////////////////////////////////
  setWindowTitle          ( tr("History types")           ) ;
  NewTreeWidgetItem       ( head                          ) ;
  head -> setText         ( 0    , tr("Time anchor")      ) ;
  head -> setText         ( 1    , ""                     ) ;
  assignHeaderItems       ( head                          ) ;
  setColumnWidth          ( 1    , 2                      ) ;
  ///////////////////////////////////////////////////////////
  LocalIcons [ "Update" ] = QIcon ( ":/images/update.png" ) ;
  LocalIcons [ "Add"    ] = QIcon ( ":/images/plus.png"   ) ;
  ///////////////////////////////////////////////////////////
  MountClicked            ( 1                             ) ;
  MountClicked            ( 2                             ) ;
  ///////////////////////////////////////////////////////////
  plan->setFont           ( this                          ) ;
}

bool N::HistoryTypes::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true                ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Insert       , New             ( ) ) ;
  LinkAction        ( Paste        , Paste           ( ) ) ;
  LinkAction        ( Refresh      , startup         ( ) ) ;
  LinkAction        ( Rename       , Rename          ( ) ) ;
  LinkAction        ( Search       , Search          ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll       ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone      ( ) ) ;
  LinkAction        ( Language     , Language        ( ) ) ;
  LinkAction        ( Font         , setFont         ( ) ) ;
  return true                                              ;
}

void N::HistoryTypes::run(int T,ThreadData * d)
{
  switch     ( T ) {
    case 10001     :
      List   ( d ) ;
    break          ;
    case 10002     :
      Insert ( d ) ;
    break          ;
  }                ;
}

bool N::HistoryTypes::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::HistoryTypes::List(ThreadData * d)
{
  nDropOut         ( ! IsContinue ( d )    ) ;
  TreeWidgetItems items                      ;
  SqlConnection SC ( plan -> sql )           ;
  if ( SC . open ( FunctionString ) )        {
    UUIDs U                                  ;
    SUID  u                                  ;
    U = SC . Uuids                           (
          PlanTable(HistoryTypes)            ,
          "uuid"                             ,
          SC . OrderBy ( "id" , sorting )  ) ;
    foreach ( u , U )                        {
      NewTreeWidgetItem ( IT )               ;
      QString name                           ;
      name = SC . getName                    (
               PlanTable(Names)              ,
               "uuid"                        ,
               vLanguageId                   ,
               u                           ) ;
      IT -> setData ( 0 , Qt::UserRole , u ) ;
      IT -> setText ( 0 , name             ) ;
      items << IT                            ;
    }                                        ;
    SC . close  (       )                    ;
  } else                                     {
    Alert       ( Error )                    ;
    SC . close  (       )                    ;
  }                                          ;
  SC   . remove (       )                    ;
  if ( items . count ( ) > 0 )               {
    addTopLevelItems ( items )               ;
    Alert            ( Done  )               ;
  }                                          ;
}

void N::HistoryTypes::Insert(ThreadData * d)
{
  nDropOut                       ( ! IsContinue ( d )                      ) ;
  nDropOut                       ( d -> Arguments . count ( ) <= 0         ) ;
  ////////////////////////////////////////////////////////////////////////////
  QStringList A                                                              ;
  for (int i = 0 ; i < d -> Arguments . count ( ) ; i++ )                    {
    A << d -> Arguments [ i ] . toString ( )                                 ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  emit OnBusy                    (                                         ) ;
  EnterSQL                       ( SC , plan -> sql                        ) ;
    QString S                                                                ;
    foreach                      ( S , A                                   ) {
      NewTreeWidgetItem          ( IT                                      ) ;
      SUID u = appendHistoryType ( SC                                      ) ;
      SC  . assureName           ( PlanTable(Names) , u , vLanguageId , S  ) ;
      IT -> setData              ( 0 , Qt::UserRole , u                    ) ;
      IT -> setText              ( 0 , S                                   ) ;
      addTopLevelItem            ( IT                                      ) ;
    }                                                                        ;
    Alert                        ( Done                                    ) ;
  ErrorSQL                       ( SC , plan -> sql                        ) ;
    Alert                        ( Error                                   ) ;
  LeaveSQL                       ( SC , plan -> sql                        ) ;
  ////////////////////////////////////////////////////////////////////////////
  emit GoRelax                   (                                         ) ;
}

void N::HistoryTypes::List(void)
{
  List ( &Data ) ;
}

void N::HistoryTypes::New(void)
{
  NewItem ( 0 ) ;
}

void N::HistoryTypes::Rename(void)
{
  RenameItem ( 0 ) ;
}

void N::HistoryTypes::Paste(void)
{
  QString S = nClipboardText          ;
  if ( S . length ( ) <= 0 )          {
    Alert ( Error )                   ;
    return                            ;
  }                                   ;
  /////////////////////////////////////
  QStringList L = S . split ( "\n" )  ;
  L = File::PurifyLines ( L )         ;
  if ( L . count ( ) <= 0 )           {
    Alert ( Error )                   ;
    return                            ;
  }                                   ;
  /////////////////////////////////////
  QStringList E = columnStrings ( 0 ) ;
  QStringList A                       ;
  A = L - E                           ;
  if ( A . count ( ) <= 0 )           {
    Alert ( Error )                   ;
    return                            ;
  }                                   ;
  /////////////////////////////////////
  VarArgs args                        ;
  args <= A                           ;
  start ( 10002 , args )              ;
}

void N::HistoryTypes::Search(void)
{
  #pragma message("Search History Types need to be completed")
  Alert ( Error ) ;
}

void N::HistoryTypes::singleClicked(QTreeWidgetItem * item,int column)
{ Q_UNUSED ( item   ) ;
  Q_UNUSED ( column ) ;
  Alert    ( Action ) ;
}

void N::HistoryTypes::doubleClicked(QTreeWidgetItem * item,int column)
{
  nDropOut ( IsNull(item) )                    ;
  QLineEdit * le = NULL                        ;
  switch ( column )                            {
    case 0                                     :
      le  = setLineEdit                        (
              item                             ,
              0                                ,
              SIGNAL ( returnPressed   ( ) )   ,
              SLOT   ( editingFinished ( ) ) ) ;
      le -> setFocus ( Qt::TabFocusReason    ) ;
    break                                      ;
  }                                            ;
}

void N::HistoryTypes::removeOldItem(void)
{
  TreeWidget :: removeOldItem ( true , 0 ) ;
}

bool N::HistoryTypes::assureItem (
       QString           name    ,
       QTreeWidgetItem * item    ,
       int               column  )
{
  bool c = false                             ;
  SUID u = nTreeUuid ( item , column)        ;
  EnterSQL ( SC , plan->sql )                ;
    if ( u <= 0 )                            {
      u = appendHistoryType ( SC )           ;
    }                                        ;
    if ( u > 0 )                             {
      SC . assureName ( PlanTable(Names)     ,
                        u                    ,
                        vLanguageId          ,
                        name               ) ;
    }                                        ;
    Alert ( Done )                           ;
  LeaveSQL ( SC , plan->sql )                ;
  if ( u > 0 )                               {
    item -> setText ( 0 , name             ) ;
    item -> setData ( 0 , Qt::UserRole , u ) ;
    c = true                                 ;
  }                                          ;
  return c                                   ;
}

void N::HistoryTypes::editingFinished(void)
{
  QLineEdit * le = Casting ( QLineEdit , ItemWidget ) ;
  if ( NotNull ( le ) )                               {
    QString name = le -> text ( )                     ;
    if ( name . length ( ) > 0 )                      {
      assureItem ( name , ItemEditing , ItemColumn )  ;
    }                                                 ;
  }                                                   ;
  removeOldItem (      )                              ;
  Alert         ( Done )                              ;
}

void N::HistoryTypes::Translation(void)
{
  UUIDs U = itemUuids ( 0                   ) ;
  emit Translations   ( windowTitle ( ) , U ) ;
}

void N::HistoryTypes::EditNames(QTreeWidgetItem * item)
{
  emit FullNames ( nTreeUuid ( item , 0 ) ) ;
}

bool N::HistoryTypes::Menu(QPoint pos)
{
  nScopedMenu     ( mm , this )                                              ;
  QAction         * aa                                                       ;
  QMenu           * ms                                                       ;
  QMenu           * ma                                                       ;
  QMenu           * mp                                                       ;
  QTreeWidgetItem * item = currentItem ( )                                   ;
  ////////////////////////////////////////////////////////////////////////////
  mm . add          ( 101 , LocalIcons [ "Update" ] , tr("Refresh")        ) ;
  mm . add          ( 102 , LocalIcons [ "Add"    ] , tr("Insert" )        ) ;
  mm . add          ( 103 , tr("Paste"  )                                  ) ;
  nIfSafe           ( item                                                 ) {
    mm . add        ( 106 , tr("Rename" )                                  ) ;
    mm . add        ( 107 , tr("Edit names" )                              ) ;
  }                                                                          ;
  mm . add          ( 301 , tr("Search"    )                               ) ;
  mm . addSeparator (                                                      ) ;
  ////////////////////////////////////////////////////////////////////////////
  ma = mm . addMenu ( tr("Adjustments")                                    ) ;
  mp = mm . addMenu ( tr("Selection"  )                                    ) ;
  ms = mm . addMenu ( tr("Sorting"    )                                    ) ;
  ////////////////////////////////////////////////////////////////////////////
  AdjustMenu        ( mm , ma                                              ) ;
  SelectionsMenu    ( mm , mp                                              ) ;
  SortingMenu       ( mm , ms                                              ) ;
  mm . addSeparator ( ma                                                   ) ;
  mm . add          ( ma , 201 , tr("Multilingual translations")           ) ;
  ////////////////////////////////////////////////////////////////////////////
  mm . setFont      ( plan                                                 ) ;
  if (plan->Booleans["Desktop"])                                             {
    pos = QCursor::pos()                                                     ;
  } else                                                                     {
    pos = mapToGlobal ( pos )                                                ;
  }                                                                          ;
  aa = mm . exec      ( pos                                                ) ;
  if (!plan->Booleans["Desktop"])                                            {
    allowGesture = true                                                      ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  nKickOut     ( IsNull        (      aa ) , true )                          ;
  nKickOut     ( RunSorting    ( mm , aa ) , true )                          ;
  nKickOut     ( RunAdjustment ( mm , aa ) , true )                          ;
  nKickOut     ( RunSelections ( mm , aa ) , true )                          ;
  ////////////////////////////////////////////////////////////////////////////
  switch       ( mm [ aa ]                        )                          {
    nFastCast  ( 101 , List        (      )       )                          ;
    nFastCast  ( 102 , New         (      )       )                          ;
    nFastCast  ( 103 , Paste       (      )       )                          ;
    nFastCast  ( 106 , Rename      (      )       )                          ;
    nFastCast  ( 107 , EditNames   ( item )       )                          ;
    nFastCast  ( 201 , Translation (      )       )                          ;
    nFastCast  ( 301 , Search      (      )       )                          ;
  }                                                                          ;
  return true                                                                ;
}
