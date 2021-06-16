#include <society.h>

N::OrganizationEdit:: OrganizationEdit ( QWidget * parent , Plan * p )
                    : TreeDock         (           parent ,        p )
                    , Group            (                             )
                    , dropAction       ( false                       )
{
  Configure ( ) ;
}

N::OrganizationEdit::~OrganizationEdit(void)
{
}

QMimeData * N::OrganizationEdit::dragMime (void)
{
  QMimeData * mime = standardMime("organization") ;
  nKickOut ( IsNull(mime) , NULL )                ;
  return mime                                     ;
}

bool N::OrganizationEdit::hasItem(void)
{
  UUIDs Uuids = selectedUuids(0) ;
  return ( Uuids.count() > 0 )   ;
}

bool N::OrganizationEdit::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())           ;
  nKickOut ( IsNull(atItem) , false )                       ;
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton),false) ;
  dragPoint = event->pos()                                  ;
  nKickOut (!atItem->isSelected(),false)                    ;
  nKickOut (!PassDragDrop,true)                             ;
  return true                                               ;
}

bool N::OrganizationEdit::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false) ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::OrganizationEdit::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::OrganizationEdit::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::OrganizationEdit::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::OrganizationEdit::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

void N::OrganizationEdit::Configure(void)
{
  first             = 0                                     ;
  second            = 0                                     ;
  t1                = Types::People                         ;
  t2                = Types::None                           ;
  relation          = Groups::Subordination                 ;
  position          = 0                                     ;
  membership        = 0                                     ;
  LimitValues [ 0 ] =  0                                    ;
  LimitValues [ 1 ] = 48                                    ;
  LimitValues [ 2 ] =  0                                    ;
  PassDragDrop      = false                                 ;
  ///////////////////////////////////////////////////////////
  setActivate             ( 0 , false               )       ;
  setActivate             ( 1 , true                )       ;
  ///////////////////////////////////////////////////////////
  NewTreeWidgetItem       ( head                    )       ;
  setWindowTitle          ( tr("Organizations"    ) )       ;
  head->setText           ( 0 , tr("Organization" ) )       ;
  head->setText           ( 1 , tr("Identifier"   ) )       ;
  head->setText           ( 2 , ""                  )       ;
  ///////////////////////////////////////////////////////////
  setFocusPolicy          ( Qt::WheelFocus          )       ;
  setDragDropMode         ( DragDrop                )       ;
  setAlternatingRowColors ( true                    )       ;
  setRootIsDecorated      ( false                   )       ;
  setSelectionMode        ( ExtendedSelection       )       ;
  setColumnCount          ( 3                       )       ;
  assignHeaderItems       ( head                    )       ;
  setColumnHidden         ( 1 , true                )       ;
  MountClicked            ( 2                       )       ;
  ///////////////////////////////////////////////////////////
  setDropFlag             ( DropText         , true )       ;
  setDropFlag             ( DropDocument     , true )       ;
  setDropFlag             ( DropPeople       , true )       ;
  setDropFlag             ( DropCommodity    , true )       ;
  setDropFlag             ( DropOrganization , true )       ;
  plan -> setFont         ( this                    )       ;
  ///////////////////////////////////////////////////////////
  LocalIcons [ "Update" ] = QIcon ( ":/images/update.png" ) ;
  ///////////////////////////////////////////////////////////
  nConnect ( this , SIGNAL(arrangeItems())                  ,
             this , SLOT  (ArrangeItems())                ) ;
}

void N::OrganizationEdit::run(int T,ThreadData * d)
{
  switch                ( T ) {
    case 10001                :
      ListOrganizations ( d ) ;
    break                     ;
    case 10002                :
      Delete            ( d ) ;
    break                     ;
    case 10003                :
      DropIn            ( d ) ;
    break                     ;
  }                           ;
}

bool N::OrganizationEdit::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false               ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Refresh      , List            ( ) ) ;
  LinkAction        ( Insert       , New             ( ) ) ;
  LinkAction        ( Delete       , Delete          ( ) ) ;
  LinkAction        ( Paste        , Paste           ( ) ) ;
  LinkAction        ( Export       , Export          ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone      ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll       ( ) ) ;
  LinkAction        ( GoHome       , PageHome        ( ) ) ;
  LinkAction        ( GoEnd        , PageEnd         ( ) ) ;
  LinkAction        ( PreviousPage , PageUp          ( ) ) ;
  LinkAction        ( NextPage     , PageDown        ( ) ) ;
  LinkAction        ( Language     , Language        ( ) ) ;
  LinkAction        ( Font         , setFont         ( ) ) ;
  return true                                              ;
}

void N::OrganizationEdit::ArrangeItems (void)
{
  SuitableColumns ( ) ;
  reportItems     ( ) ;
}

UUIDs N::OrganizationEdit::LoadUuids(SqlConnection & SC)
{
  UUIDs Uuids                                            ;
  if (activate(0) && first <=0)                          {
    Uuids = SC . Uuids                                   (
             PlanTable(Organizations)                    ,
             "uuid"                                      ,
             SC.OrderByAsc("id")                       ) ;
  } else
  if (activate(1) && second<=0)                          {
    return SC . Uuids                                    (
             PlanTable(Organizations)                    ,
             "uuid"                                      ,
             SC.OrderByAsc("id")                       ) ;
  } else                                                 {
  ////////////////////////////////////////////////////////
    QString Q                                            ;
    if (activate(0))                                     {
      Q = SC . sql . SelectFrom                          (
            "second"                                     ,
            PlanTable(Groups)                            ,
            QString ( "%1 %2"                            )
            .arg(SC.sql.Where                            (
                   4                                     ,
                   "first"                               ,
                   "t1"                                  ,
                   "t2"                                  ,
                   "relation"                          ) )
            .arg(SC.OrderByAsc("position"))            ) ;
     SC . Prepare ( Q                                  ) ;
      SC . Bind    ( "first"    , first                ) ;
      SC . Bind    ( "t1"       , t1                   ) ;
      SC . Bind    ( "t2"       , Types::Organization  ) ;
      SC . Bind    ( "relation" , relation             ) ;
    } else
    if (activate(1))                                     {
      Q = SC . sql . SelectFrom                          (
            "first"                                      ,
            PlanTable(Groups)                            ,
            QString ( "%1 %2"                            )
            .arg(SC.sql.Where                            (
                   4                                     ,
                   "second"                              ,
                   "t1"                                  ,
                   "t2"                                  ,
                   "relation"                          ) )
            .arg(SC.OrderByAsc("position"))            ) ;
      SC . Prepare ( Q                                 ) ;
      SC . Bind    ( "second"   , second               ) ;
      SC . Bind    ( "t1"       , Types::Organization  ) ;
      SC . Bind    ( "t2"       , t2                   ) ;
      SC . Bind    ( "relation" , relation             ) ;
    }                                                    ;
    if (SC.Exec()) while (SC.Next()) Uuids << SC.Uuid(0) ;
  };
  ////////////////////////////////////////////////////////
  FixUuids ( Uuids )                                     ;
  return Uuids                                           ;
}

void N::OrganizationEdit::LoadNames(SqlConnection & SC,UUIDs & Uuids)
{
  int  index = 0                                   ;
  SUID uuid  = 0                                   ;
  GroupItems     GI ( plan )                       ;
  FinanceManager FM ( plan )                       ;
  foreach (uuid,Uuids)                             {
    QStringList Names = GI.Names(SC,uuid)          ;
    if (Names.count()>0)                           {
      QString           id                         ;
      QTreeWidgetItem * it                         ;
      it = addItem(Names[0],uuid,0)                ;
      id = FM.FetchIdentifier(SC,uuid)             ;
      if (id.length()>0)                           {
        it->setText(1,id)                          ;
      }                                            ;
    }                                              ;
    index++                                        ;
    if (nMod(index,100)) DoProcessEvents           ;
  }                                                ;
}

SUID N::OrganizationEdit::AppendUuid(SqlConnection & SC)
{
  GroupItems GI ( plan )                                                   ;
  UUIDs      U                                                             ;
  SUID       u                                                             ;
  u  = SC.Unique  ( PlanTable(MajorUuid    ),"uuid"               )        ;
  SC . assureUuid ( PlanTable(MajorUuid    ),u,Types::Organization)        ;
  SC . insertUuid ( PlanTable(Organizations),u,"uuid"             )        ;
  if (activate(0))                                                         {
    U << u                                                                 ;
    GI . Join ( SC , first , t1 , Types::Organization , relation , 0 , U ) ;
  } else
  if (activate(1))                                                         {
    U << second                                                            ;
    GI . Join ( SC , u     , Types::Organization , t2 , relation , 0 , U ) ;
  }                                                                        ;
  return u                                                                 ;
}

bool N::OrganizationEdit :: AppendName (
       SqlConnection & Connection      ,
       SUID            uuid            ,
       QString         name            )
{
  QString Q                                            ;
  GroupItems GI(plan)                                  ;
  SUID nuid = Connection.Unique                        (
                PlanTable(MajorUuid) , "uuid"        ) ;
  Connection.assureUuid                                (
    PlanTable(MajorUuid) , nuid,Types::Name)           ;
  Q = Connection.sql.ReplaceInto                       (
        PlanTable(Names) , 3,"uuid","language","name") ;
  Connection . insertName (Q,nuid,vLanguageId,name   ) ;
  GI.InsertNameMap(Connection,uuid,nuid)               ;
  return true                                          ;
}

void N::OrganizationEdit::ListOrganizations (ThreadData * d)
{
  emit clearItems     (                ) ;
  emit assignEnabling ( false          ) ;
  EnterSQL            ( SC , plan->sql ) ;
    UUIDs Uuids                          ;
    Uuids = LoadUuids ( SC             ) ;
    LoadNames         ( SC , Uuids     ) ;
  LeaveSQL            ( SC , plan->sql ) ;
  emit assignEnabling ( true           ) ;
  emit arrangeItems   (                ) ;
  Alert               ( Done           ) ;
}

void N::OrganizationEdit::Delete(ThreadData * d)
{
}

bool N::OrganizationEdit::List(void)
{
  if ( 1 == running ) {
    Alert ( Error   ) ;
    return false      ;
  }                   ;
  start    ( 10001 )  ;
  return true         ;
}

void N::OrganizationEdit::doubleClicked (
       QTreeWidgetItem * item           ,
       int               column         )
{
  nDropOut ( column > 1 )              ;
  QLineEdit * line                     ;
  removeOldItem()                      ;
  line = setLineEdit                   (
           item                        ,
           column                      ,
           SIGNAL(editingFinished())   ,
           SLOT  (editingFinished()) ) ;
  line->setFocus(Qt::TabFocusReason)   ;
}

void N::OrganizationEdit::New(void)
{
  NewTreeWidgetItem ( IT                   ) ;
  IT -> setData     ( 0 , Qt::UserRole , 0 ) ;
  setAlignments     ( IT                   ) ;
  addTopLevelItem   ( IT                   ) ;
  scrollToItem      ( IT                   ) ;
  doubleClicked     ( IT , 0               ) ;
}

void N::OrganizationEdit::editingFinished(void)
{
  nDropOut ( IsNull(ItemEditing) )                      ;
  QLineEdit * line = Casting  (QLineEdit  ,ItemWidget)  ;
  if (IsNull(line)) return                              ;
  ItemEditing -> setText ( ItemColumn , line->text() )  ;
  ///////////////////////////////////////////////////////
  SUID    uuid   = nTreeUuid(ItemEditing,0)             ;
  QString name   = line->text()                         ;
  int     column = ItemColumn                           ;
  EnterSQL ( SC , plan->sql )                           ;
    switch (column)                                     {
      case 0                                            :
        if (uuid<=0 && name.length()>0)                 {
          uuid = AppendUuid(SC)                         ;
          ItemEditing->setData(0,Qt::UserRole,uuid)     ;
        }                                               ;
        if (uuid>0 && name.length()>0)                  {
          AppendName(SC,uuid,name)                      ;
        }                                               ;
        removeOldItem()                                 ;
      break                                             ;
      case 1                                            :
        if (uuid>0 && name.length()>0)                  {
          FinanceManager FM(plan)                       ;
          FM.assureIdentifier(SC,uuid,name)             ;
        }                                               ;
        removeOldItem()                                 ;
      break                                             ;
    }                                                   ;
  LeaveSQL ( SC , plan->sql )                           ;
  Alert    ( Done           )                           ;
}

void N::OrganizationEdit::Delete(void)
{
  QTreeWidgetItem * item = currentItem ( ) ;
  if (IsNull(item)) return                 ;
  //////////////////////////////////////////
  bool correct = false                     ;
  //////////////////////////////////////////
  if ((activate(0) && first>0))            {
    correct = true                         ;
  }                                        ;
  //////////////////////////////////////////
  if ((activate(1) && second>0))           {
    correct = true                         ;
  }                                        ;
  //////////////////////////////////////////
  SUID       u = nTreeUuid ( item , 0 )    ;
  GroupItems GI ( plan           )         ;
  EnterSQL      ( SC , plan->sql )         ;
    UUIDs U                                ;
    if ((activate(0) && first >0))         {
      U << u                               ;
      GI . Detach                          (
        SC                                 ,
        first                              ,
        t1                                 ,
        Types::Organization                ,
        relation                           ,
        U                                ) ;
    } else
    if ((activate(1) && second>0))         {
      U << second                          ;
      GI . Detach                          (
        SC                                 ,
        u                                  ,
        Types::Organization                ,
        t2                                 ,
        relation                           ,
        U                                ) ;
    }                                      ;
  //////////////////////////////////////////
  LeaveSQL      ( SC , plan->sql )         ;
  //////////////////////////////////////////
  takeItem      ( item           )         ;
  reportItems   (                )         ;
  Alert         ( Done           )         ;
}

void N::OrganizationEdit::Paste(QString text)
{
  nDropOut ( text.length() <= 0 )            ;
  QStringList sl = text.split("\n")          ;
  nDropOut ( sl.count()    <= 0 )            ;
  EnterSQL ( SC , plan->sql )                ;
    for (int i=0;i<sl.count();i++)           {
      QString s = sl[i]                      ;
      s = s.replace("\r","")                 ;
      s = s.replace("\n","")                 ;
      if (s.length()>0)                      {
        SUID uuid = AppendUuid(SC)           ;
        if (uuid>0 && AppendName(SC,uuid,s)) {
          addItem (s,uuid,0)                 ;
        }                                    ;
      }                                      ;
    }                                        ;
  LeaveSQL ( SC , plan->sql )                ;
  Alert    ( Done           )                ;
}

void N::OrganizationEdit::Paste(void)
{
  Paste ( nClipboardText ) ;
}

void N::OrganizationEdit::Export(void)
{
  QString filename = QFileDialog::getSaveFileName (
                       this                       ,
                       tr("Export organizations") ,
                       plan->Temporary("")        ,
                       "*.*\n"
                       "*.txt"
                     )                            ;
  nDropOut ( filename.length() <= 0 )             ;
  QFile F  ( filename )                           ;
  if (!F.open(QIODevice::WriteOnly)) return       ;
  for (int i=0;i<topLevelItemCount();i++)         {
    QTreeWidgetItem * item = topLevelItem(i)      ;
    QString n = item->text(0)                     ;
    if (!isColumnHidden(1))                       {
      n += "    "                                 ;
      n += item->text(1)                          ;
    }                                             ;
    n += "\r\n"                                   ;
    F.write(n.toUtf8())                           ;
  }                                               ;
  F.close()                                       ;
  Alert ( Done )                                  ;
}

void N::OrganizationEdit::Copy(void)
{
  QStringList s                           ;
  for (int i=0;i<topLevelItemCount();i++) {
    if (topLevelItem(i)->isSelected())    {
      s << topLevelItem(i)->text(0)       ;
    }                                     ;
  }                                       ;
  nDropOut      ( s.count() <= 0 )        ;
  nSetClipboard ( s.join("\r\n") )        ;
}

void N::OrganizationEdit::CopyIt(void)
{
  Copy ( ) ;
}

void N::OrganizationEdit::DropIn(ThreadData * d)
{
}

bool N::OrganizationEdit :: dropText (
       QWidget       * source      ,
       QPointF         pos         ,
       const QString & text        )
{
  dropAction = true  ;
  Paste(text)        ;
  dropAction = false ;
  return true        ;
}

bool N::OrganizationEdit :: dropDocuments (
       QWidget     * source               ,
       QPointF       psf                  ,
       const UUIDs & Uuids                )
{
  nKickOut ( source == this , true )   ;
  //////////////////////////////////////
  QPoint pos = psf.toPoint()           ;
  QTreeWidgetItem * item = itemAt(pos) ;
  if (IsNull(item))                    {
    Alert ( Error )                    ;
    return false                       ;
  }                                    ;
  dropAction = true                    ;
  GroupItems GI( plan      )           ;
  EnterSQL ( SC , plan->sql )          ;
    UUIDs PUIDs = Uuids                ;
    SUID  uuid  = nTreeUuid(item,0)    ;
    GI.Join                            (
      SC                               ,
      uuid                             ,
      Types::Organization    ,
      Types::Document        ,
      Groups::Subordination  ,
      0,PUIDs                        ) ;
  LeaveSQL ( SC , plan->sql )          ;
  Alert    ( Done           )          ;
  dropAction = false                   ;
  return true                          ;
}

bool N::OrganizationEdit :: dropPeople (
       QWidget     * source            ,
       QPointF       psf               ,
       const UUIDs & Uuids             )
{
  nKickOut ( source == this , true )   ;
  //////////////////////////////////////
  QPoint pos = psf.toPoint()           ;
  QTreeWidgetItem * item = itemAt(pos) ;
  if (IsNull(item))                    {
    Alert ( Error )                    ;
    return false                       ;
  }                                    ;
  dropAction = true                    ;
  GroupItems GI ( plan      )          ;
  EnterSQL ( SC , plan->sql )          ;
    UUIDs PUIDs = Uuids                ;
    SUID  uuid  = nTreeUuid(item,0)    ;
    GI.Join                            (
      SC                               ,
      uuid                             ,
      Types::Organization    ,
      Types::People          ,
      Groups::Subordination  ,
      0,PUIDs                        ) ;
  LeaveSQL ( SC , plan->sql )          ;
  Alert    ( Done           )          ;
  dropAction = false                   ;
  return true                          ;
}

bool N::OrganizationEdit :: dropCommodities (
       QWidget     * source                 ,
       QPointF       psf                    ,
       const UUIDs & Uuids                  )
{
  nKickOut ( source == this , true )   ;
  //////////////////////////////////////
  QPoint pos = psf.toPoint()           ;
  QTreeWidgetItem * item = itemAt(pos) ;
  if (IsNull(item))                    {
    Alert ( Error )                    ;
    return false                       ;
  }                                    ;
  //////////////////////////////////////
  dropAction = true                    ;
  GroupItems GI( plan      )           ;
  //////////////////////////////////////
  EnterSQL ( SC , plan->sql )          ;
    UUIDs PUIDs = Uuids                ;
    SUID  uuid  = nTreeUuid(item,0)    ;
    GI.Join                            (
      SC                               ,
      uuid                             ,
      Types::Organization              ,
      Types::Commodity                 ,
      Groups::Subordination            ,
      0,PUIDs                        ) ;
  LeaveSQL ( SC , plan->sql )          ;
  //////////////////////////////////////
  Alert    ( Done           )          ;
  dropAction = false                   ;
  return true                          ;
}

bool N::OrganizationEdit :: dropOrganizations (
       QWidget     * source                   ,
       QPointF       psf                      ,
       const UUIDs & Uuids                    )
{
  nKickOut ( source==this , true )     ;
  //////////////////////////////////////
  QPoint pos = psf.toPoint()           ;
  QTreeWidgetItem * item = itemAt(pos) ;
  if (IsNull(item))                    {
    Alert ( Error )                    ;
    return false                       ;
  }                                    ;
  //////////////////////////////////////
  dropAction = true                    ;
  GroupItems GI( plan      )           ;
  //////////////////////////////////////
  EnterSQL ( SC , plan->sql )          ;
    UUIDs PUIDs = Uuids                ;
    SUID  uuid  = nTreeUuid(item,0)    ;
    GI.Join                            (
      SC                               ,
      uuid                             ,
      Types::Organization              ,
      Types::Organization              ,
      Groups::Contains                 ,
      0,PUIDs                        ) ;
  LeaveSQL ( SC , plan->sql )          ;
  //////////////////////////////////////
  Alert    ( Done           )          ;
  dropAction = false                   ;
  return true                          ;
}

bool N::OrganizationEdit::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                     ;
  QTreeWidgetItem * item = itemAt  ( pos )                      ;
  QMenu           * me                                          ;
  QMenu           * ms                                          ;
  QMenu           * ma                                          ;
  QMenu           * mc                                          ;
  QAction         * aa                                          ;
  QMenu           * mp       = NULL                             ;
  SpinBox         * spb      = NULL                             ;
  SpinBox         * spp      = NULL                             ;
  int               StartId  = 0                                ;
  int               PageSize = 0                                ;
  ///////////////////////////////////////////////////////////////
  if ( LimitValues [ 2 ] > 0 )                                  {
    mp = PageMenu ( mm                                          ,
                    tr("Total %1 organizations")                ,
                    StartId                                     ,
                    PageSize                                  ) ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  if ( NotNull ( item ) )                                       {
    me = mm . addMenu ( tr("Contents")                        ) ;
    mm . add          ( me  , 911 , tr("Edit"        )        ) ;
    mm . add          ( me  , 901 , tr("Documents"   )        ) ;
    mm . add          ( me  , 902 , tr("People"      )        ) ;
    mm . add          ( me  , 903 , tr("Commodities" )        ) ;
    mm . add          ( me  , 904 , tr("Name lists"  )        ) ;
    mm . add          ( me  , 905 , tr("Bookmarks"   )        ) ;
    mm . add          ( me  , 906 , tr("Subsidiaries")        ) ;
    if ( isFunction ( 3721771 ) )                               {
      mm . add        ( me  , 801 , tr("Trading Goods")       ) ;
    }                                                           ;
    if ( isFunction ( 3721772 ) )                               {
      mm . add        ( me  , 802 , tr("Cargo"       )        ) ;
    }                                                           ;
    mm . addSeparator (                              )          ;
  }                                                             ;
  mm . add ( 101 , tr("Refresh"                      )        ) ;
  mm . addSeparator (                                         ) ;
  mm . add ( 201 , tr("New organization"             )        ) ;
  if (NotNull(item))                                            {
    if ((activate(0) && first>0) || (activate(1) && second>0) ) {
      mm . add ( 202 , tr("Detach organization"      )        ) ;
    }                                                           ;
  }                                                             ;
  mm . add ( 301 , tr("Save organization as ..."     )        ) ;
  mm . addSeparator (                                )          ;
  mm . add ( 501 , tr("Identifier"),true, !isColumnHidden(1)  ) ;
  mc = mm .addMenu  ( tr("Selections")               )          ;
  ma = mm .addMenu  ( tr("Adjustment")               )          ;
  ms = mm .addMenu  ( tr("Sorting"   )               )          ;
  SelectionsMenu    ( mm , mc                        )          ;
  AdjustMenu        ( mm , ma                        )          ;
  SortingMenu       ( mm , ms                        )          ;
  mm . addSeparator (                                )          ;
  DockingMenu       ( mm                             )          ;
  mm . setFont(plan)                                            ;
  aa = mm.exec()                                                ;
  if            ( IsNull ( aa )      )                          {
    PageChanged ( StartId , PageSize )                          ;
    return true                                                 ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  if ( RunAdjustment ( mm , aa ) ) return true                  ;
  if ( RunSorting    ( mm , aa ) ) return true                  ;
  if ( RunSelections ( mm , aa ) ) return true                  ;
  if ( RunDocking    ( mm , aa ) ) return true                  ;
  ///////////////////////////////////////////////////////////////
  switch (mm[aa])                                               {
    case 101                                                    :
      List       ( )                                            ;
    break                                                       ;
    case 201                                                    :
      New        ( )                                            ;
    break                                                       ;
    case 202                                                    :
      Delete     ( )                                            ;
    break                                                       ;
    case 301                                                    :
      Export     ( )                                            ;
    break                                                       ;
    case 501                                                    :
      setColumnHidden  ( 1 , aa->isChecked() ? false : true )   ;
      if (!aa->isChecked()) resizeColumnToContents(1)           ;
    break                                                       ;
    case 801                                                    :
      emit Trading     ( item->text(0) , nTreeUuid(item,0) )    ;
    break                                                       ;
    case 802                                                    :
      emit Cargo       ( item->text(0) , nTreeUuid(item,0) )    ;
    break                                                       ;
    case 901                                                    :
      emit Documents   ( item->text(0) , nTreeUuid(item,0) )    ;
    break                                                       ;
    case 902                                                    :
      emit People      ( item->text(0) , nTreeUuid(item,0) )    ;
    break                                                       ;
    case 903                                                    :
      emit Commodities ( item->text(0) , nTreeUuid(item,0) )    ;
    break                                                       ;
    case 904                                                    :
      emit EditNames   ( item->text(0) , nTreeUuid(item,0) )    ;
    break                                                       ;
    case 905                                                    :
      emit Bookmarks   ( item->text(0) , nTreeUuid(item,0) )    ;
    break                                                       ;
    case 906                                                    :
      emit Subsidiaries                                         (
                         item->text(0)                          ,
                         nTreeUuid(item,0)                      ,
                         Types::Organization                  ) ;
    break                                                       ;
    case 911                                                    :
      emit Edit        ( item->text(0) , nTreeUuid(item,0) )    ;
    break                                                       ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  RunPageMenu ( mm , aa , StartId , PageSize )                  ;
  PageChanged (           StartId , PageSize )                  ;
  ///////////////////////////////////////////////////////////////
  return true                                                   ;
}
