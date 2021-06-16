#include <society.h>

N::NationLists:: NationLists (QWidget * parent,Plan * p)
               : TreeDock    (          parent,       p)
{
  Configure ( ) ;
}

N::NationLists::~NationLists (void)
{
}

void N::NationLists::Configure(void)
{
  setAccessibleName               ( "N::NationLists"              ) ;
  setObjectName                   ( "N::NationLists"              ) ;
  ///////////////////////////////////////////////////////////////////
  NewTreeWidgetItem               ( head                          ) ;
  head -> setText                 ( 0 , tr("Nation")              ) ;
  head -> setText                 ( 1 , ""                        ) ;
  ///////////////////////////////////////////////////////////////////
  setWindowTitle                  ( tr("Nations")                 ) ;
  setWindowIcon                   ( QIcon(":/images/nations.png") ) ;
  setDragDropMode                 ( DragDrop                      ) ;
  setRootIsDecorated              ( false                         ) ;
  setAlternatingRowColors         ( true                          ) ;
  setSelectionMode                ( ExtendedSelection             ) ;
  setHorizontalScrollBarPolicy    ( Qt::ScrollBarAsNeeded         ) ;
  setVerticalScrollBarPolicy      ( Qt::ScrollBarAsNeeded         ) ;
  ///////////////////////////////////////////////////////////////////
  setColumnCount                  ( 2                             ) ;
  setAllAlignments                ( head , Qt::AlignCenter        ) ;
  setFont                         ( head , Fonts::ListView        ) ;
  assignHeaderItems               ( head                          ) ;
  setColumnWidth                  ( 1    , 3                      ) ;
  ///////////////////////////////////////////////////////////////////
  setFunction                     ( 1000102 , true                ) ;
  setFunction                     ( 1000103 , true                ) ;
  setFunction                     ( 1000301 , true                ) ;
  setFunction                     ( 1001001 , false               ) ;
  setFunction                     ( 1001002 , false               ) ;
  ///////////////////////////////////////////////////////////////////
  setDropFlag                     ( DropPicture , true            ) ;
  setDropFlag                     ( DropPeople  , true            ) ;
  plan -> setFont                 ( this                          ) ;
  ///////////////////////////////////////////////////////////////////
  PassDragDrop            = false                                   ;
  LocalIcons [ "Update" ] = QIcon ( ":/images/update.png"         ) ;
}

QMimeData * N::NationLists::dragMime (void)
{
  UUIDs Uuids                                    ;
  SUID  uuid  = 0                                ;
  ////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)        {
    QTreeWidgetItem * it = topLevelItem(i)       ;
    if (it->isSelected())                        {
      Uuids << nTreeUuid(it,0)                   ;
    }                                            ;
  }                                              ;
  if (Uuids.count()<=0)                          {
    QTreeWidgetItem * it = currentItem()         ;
    if (NotNull(it))                             {
      uuid = nTreeUuid(it,0)                     ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  if (Uuids.count()<=0 && uuid == 0) return NULL ;
  if (Uuids.count()==1)                          {
    uuid  = Uuids [0]                            ;
    Uuids . clear ( )                            ;
  }                                              ;
  ////////////////////////////////////////////////
  QMimeData * mime = new QMimeData()             ;
  if (Uuids.count()==0)                          {
    setMime ( mime , "nation/uuid"  , uuid  )    ;
  } else                                         {
    setMime ( mime , "nation/uuids" , Uuids )    ;
  }                                              ;
  return mime                                    ;
}

bool N::NationLists::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::NationLists::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return true                                                ;
}

bool N::NationLists::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::NationLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{ Q_UNUSED ( dropIt ) ;
  Q_UNUSED ( mime   ) ;
}

bool N::NationLists::finishDrag(QMouseEvent * event)
{ Q_UNUSED ( event ) ;
  return true        ;
}

bool N::NationLists::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual ( source   , this ) , false ) ;
  return dropHandler(mime)                        ;
}

bool N::NationLists::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{ Q_UNUSED ( source ) ;
  Q_UNUSED ( mime   ) ;
  Q_UNUSED ( pos    ) ;
  Alert    ( Action ) ;
  return true         ;
}

bool N::NationLists::dropPeople(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  nKickOut ( source == this , false )  ;
  QPoint pos = psf.toPoint()           ;
  QTreeWidgetItem * item = itemAt(pos) ;
  nKickOut ( IsNull(item)   , false )  ;
  SUID uuid = nTreeUuid(item,0)        ;
  nKickOut ( uuid <= 0      , false )  ;
  return JoinPeople ( uuid , Uuids  )  ;
}

bool N::NationLists::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  nKickOut ( source == this , false )  ;
  QPoint pos = psf.toPoint()           ;
  QTreeWidgetItem * item = itemAt(pos) ;
  nKickOut ( IsNull(item)   , false )  ;
  SUID uuid = nTreeUuid(item,0)        ;
  nKickOut ( uuid <= 0      , false )  ;
  return JoinPictures ( uuid , Uuids ) ;
}

bool N::NationLists::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Export       , Export      ( ) ) ;
  LinkAction        ( Copy         , Copy        ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  return true                                          ;
}

void N::NationLists::setCheckable(QTreeWidgetItem * item)
{
  if ( isFunction ( 1001001 ) )                 {
    item -> setCheckState ( 0 , Qt::Unchecked ) ;
    return                                      ;
  }                                             ;
  if ( isFunction ( 1001002 ) )                 {
    item -> setCheckState ( 0 , Qt::Checked   ) ;
    return                                      ;
  }                                             ;
}

void N::NationLists::run(int T,ThreadData * d)
{
  nDropOut         ( ! IsContinue ( d ) ) ;
  switch           ( T                  ) {
    case 10001                            :
      startLoading (                    ) ;
      List         (                    ) ;
      stopLoading  (                    ) ;
    break                                 ;
    case 10002                            :
      startLoading (                    ) ;
      JoinPeople   ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
    case 10003                            :
      startLoading (                    ) ;
      JoinPictures (d                   ) ;
      stopLoading  (                    ) ;
    break                                 ;
  }                                       ;
}

bool N::NationLists::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

bool N::NationLists::List(void)
{
  emit clearItems       (                      ) ;
  emit assignEnabling   ( false                ) ;
  emit OnBusy           (                      ) ;
  EnterSQL              ( SC , plan -> sql     ) ;
    SUID  u                                      ;
    UUIDs U = SC . Uuids                         (
                PlanTable(Nations)               ,
                "uuid"                           ,
                SC . OrderByAsc ( "position" ) ) ;
    foreach             ( u , U                ) {
      NewTreeWidgetItem ( IT                   ) ;
      QString name = SC . getName                (
                       PlanTable(Names)          ,
                       "uuid"                    ,
                       vLanguageId               ,
                       u                       ) ;
      IT -> setData     ( 0 , Qt::UserRole , u ) ;
      IT -> setText     ( 0 , name             ) ;
      setCheckable      ( IT                   ) ;
      addTopLevelItem   ( IT                   ) ;
    }                                            ;
  LeaveSQL              ( SC , plan -> sql     ) ;
  emit GoRelax          (                      ) ;
  emit assignEnabling   ( true                 ) ;
  emit AutoFit          (                      ) ;
  Alert                 ( Done                 ) ;
  return true                                    ;
}

void N::NationLists::Export(void)
{
  QString filename = QFileDialog::getSaveFileName (
    this , tr("Export nations" )                  ,
    plan->User.absolutePath() , "*.txt"         ) ;
  if (filename.length()<=0) return                ;
  QFile F ( filename )                            ;
  if (!F.open(QIODevice::WriteOnly)) return       ;
  for (int i=0;i<topLevelItemCount();i++)         {
    QTreeWidgetItem * it = topLevelItem(i)        ;
    QString R                                     ;
    R = it->text(0) + "\r\n"                      ;
    F . write ( R . toUtf8 ( ) )                  ;
  }                                               ;
  F . close ( )                                   ;
}

void N::NationLists::Copy(void)
{
  QTreeWidgetItem * item = currentItem()     ;
  if (IsNull(item)) return                   ;
  if (currentColumn()<0) return              ;
  QString text = item->text(currentColumn()) ;
  qApp->clipboard()->setText(text)           ;
}

void N::NationLists::JoinPeople(ThreadData * d)
{
  nDropOut ( ! IsContinue ( d )             )        ;
  nDropOut ( d -> Arguments . count ( ) < 2 )        ;
  ////////////////////////////////////////////////////
  SUID  uuid                                         ;
  UUIDs Uuids                                        ;
  uuid = d -> Arguments [  0 ] . toULongLong    ( )  ;
  for (int i=1;i<d->Arguments.count();i++)           {
    Uuids << d -> Arguments [ i ] . toULongLong ( )  ;
  }                                                  ;
  ////////////////////////////////////////////////////
  QTreeWidgetItem * item = uuidAtItem(uuid,0)        ;
  nDropOut ( IsNull(item) )                          ;
  ////////////////////////////////////////////////////
  QString           title = item->text(0)            ;
  plan -> showMessage( tr("%1 people join nation %2" )
    .arg(Uuids.count()).arg(title)                 ) ;
  plan -> processEvents (                          ) ;
  ////////////////////////////////////////////////////
  Bustle  ( )                                        ;
  SqlConnection Connection ( plan->sql )             ;
  GroupItems GI(plan)                                ;
  if (Connection.open(FunctionString) )              {
    UUIDs Guids = Uuids                              ;
    GI . AutoMap    = true                           ;
    GI . GroupTable = GI . LookTable                 (
                        Types::Name                  ,
                        Types::Picture               ,
                        Groups::Subordination      ) ;
    GI . Join                                        (
      Connection                                     ,
      uuid                                           ,
      Types::Nation                                  ,
      Types::People                                  ,
      Groups::Subordination,0,Guids                ) ;
    Connection . close ( )                           ;
  }                                                  ;
  Connection . remove ( )                            ;
  Vacancy ( )                                        ;
  ////////////////////////////////////////////////////
  plan->showMessage                                  (
    tr("%1 people join nation %2 finished"           )
    .arg(Uuids.count()).arg(title)                 ) ;
  ////////////////////////////////////////////////////
  Alert ( Done )                                     ;
}

bool N::NationLists::JoinPeople(SUID uuid,const UUIDs & Uuids)
{
  VarArgs args                ;
  args << uuid                ;
  toVariants ( Uuids , args ) ;
  start      ( 10002 , args ) ;
  return true                 ;
}

void N::NationLists::JoinPictures(ThreadData * d)
{
  nDropOut ( ! IsContinue ( d )             )          ;
  nDropOut ( d -> Arguments . count ( ) < 2 )          ;
  //////////////////////////////////////////////////////
  SUID  uuid                                           ;
  UUIDs Uuids                                          ;
  uuid = d -> Arguments [  0 ] . toULongLong    ( )    ;
  for (int i=1;i<d->Arguments.count();i++)             {
    Uuids << d -> Arguments [ i ] . toULongLong ( )    ;
  }                                                    ;
  //////////////////////////////////////////////////////
  QTreeWidgetItem * item = uuidAtItem(uuid,0)          ;
  nDropOut ( IsNull(item) )                            ;
  //////////////////////////////////////////////////////
  QString           title = item->text(0)              ;
  plan -> showMessage( tr("%1 pictures join nation %2" )
    .arg(Uuids.count()).arg(title)                   ) ;
  plan -> processEvents (                            ) ;
  //////////////////////////////////////////////////////
  Bustle  ( )                                          ;
  SqlConnection Connection ( plan->sql )               ;
  GroupItems GI(plan)                                  ;
  if (Connection.open(FunctionString) )                {
    UUIDs Guids = Uuids                                ;
    GI . AutoMap    = true                             ;
    GI . GroupTable = GI . LookTable                   (
                        Types::Name                    ,
                        Types::Picture                 ,
                        Groups::Subordination        ) ;
    GI . Join                                          (
      Connection                                       ,
      uuid                                             ,
      Types::Nation                                    ,
      Types::Picture                                   ,
      Groups::Subordination,0,Guids                  ) ;
    Connection . close ( )                             ;
  }                                                    ;
  Connection . remove ( )                              ;
  Vacancy ( )                                          ;
  //////////////////////////////////////////////////////
  plan->showMessage                                    (
    tr("%1 pictures join nation %2 finished"           )
    .arg(Uuids.count()).arg(title)                   ) ;
  //////////////////////////////////////////////////////
  Alert ( Done )                                       ;
}

bool N::NationLists::JoinPictures(SUID uuid,const UUIDs & Uuids)
{
  VarArgs args                ;
  args << uuid                ;
  toVariants ( Uuids , args ) ;
  start      ( 10003 , args ) ;
  return true                 ;
}

bool N::NationLists::Menu(QPoint pos)
{
  nScopedMenu           ( mm  , this                    ) ;
  QTreeWidgetItem * item   = itemAt ( pos               ) ;
  QMenu           * ma                                    ;
  QMenu           * ms                                    ;
  QAction         * aa                                    ;
  /////////////////////////////////////////////////////////
  mm     . add          ( 101                             ,
                          LocalIcons [ "Update" ]         ,
                          tr("Refresh")                 ) ;
  if                    ( NotNull ( item )              ) {
    mm   . addSeparator (                               ) ;
    if                  ( isFunction ( 1000102 )        ) {
      mm . add          ( 102 , tr("Full names")        ) ;
    }                                                     ;
    if                  ( isFunction ( 1000103 )        ) {
      mm . add          ( 103 , tr("Gallery")           ) ;
    }                                                     ;
    if                  ( isFunction ( 1000301 )        ) {
      mm . add          ( 301 , tr("People" )           ) ;
    }                                                     ;
  }                                                       ;
  mm     . addSeparator (                               ) ;
  /////////////////////////////////////////////////////////
  ma     = mm . addMenu ( tr("Adjustments")             ) ;
  ms     = mm . addMenu ( tr("Sorting"    )             ) ;
  /////////////////////////////////////////////////////////
  AdjustMenu            ( mm , ma                       ) ;
  SortingMenu           ( mm , ms                       ) ;
  DockingMenu           ( mm                            ) ;
  /////////////////////////////////////////////////////////
  mm     . addSeparator ( ma                            ) ;
  mm     . add          ( ma , 201 , tr("Translations") ) ;
  /////////////////////////////////////////////////////////
  mm     . setFont      ( plan                          ) ;
  aa     = mm . exec    (                               ) ;
  /////////////////////////////////////////////////////////
  if ( IsNull        (      aa ) ) return true            ;
  if ( RunAdjustment ( mm , aa ) ) return true            ;
  if ( RunSorting    ( mm , aa ) ) return true            ;
  if ( RunDocking    ( mm , aa ) ) return true            ;
  /////////////////////////////////////////////////////////
  QString name                                            ;
  UUIDs   U                                               ;
  /////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                    {
    case 101                                              :
      startup ( )                                         ;
    break                                                 ;
    case 102                                              :
      if ( NotNull ( item ) )                             {
        SUID xuid = nTreeUuid ( item , 0 )                ;
        emit FullNames ( xuid )                           ;
      }                                                   ;
    break                                                 ;
    case 103                                              :
      if ( NotNull ( item ) )                             {
        QString name = item -> text ( 0 )                 ;
        SUID    xuid = nTreeUuid ( item , 0 )             ;
        emit Gallery ( name , xuid )                      ;
      }                                                   ;
    break                                                 ;
    case 201                                              :
      name = windowTitle (          )                     ;
      U    = itemUuids   ( 0        )                     ;
      emit Translations  ( name , U )                     ;
    break                                                 ;
    case 301                                              :
      if ( NotNull ( item ) )                             {
        QString name = item -> text ( 0 )                 ;
        SUID    xuid = nTreeUuid ( item , 0 )             ;
        emit ListPeople ( name , xuid , Types::Nation )   ;
      }                                                   ;
    break                                                 ;
    default                                               :
    break                                                 ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  return true                                             ;
}
