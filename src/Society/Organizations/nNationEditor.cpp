#include <society.h>

N::NationEditor:: NationEditor ( QWidget * parent , Plan * p )
                : TreeWidget   (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::NationEditor::~NationEditor(void)
{
}

void N::NationEditor::Configure(void)
{
  setAccessibleName                  ( "N::NationEditor"               ) ;
  setObjectName                      ( "N::NationEditor"               ) ;
  ////////////////////////////////////////////////////////////////////////
  NewTreeWidgetItem                  ( head                            ) ;
  head -> setText                    ( 0 , tr("Nation")                ) ;
  head -> setText                    ( 1 , tr("ISO(2)")                ) ;
  head -> setText                    ( 2 , tr("ISO(3)")                ) ;
  head -> setText                    ( 3 , tr("Numeric")               ) ;
  head -> setText                    ( 4 , tr("Code")                  ) ;
  head -> setText                    ( 5 , tr("Position")              ) ;
  head -> setText                    ( 6 , tr("Short name")            ) ;
  head -> setText                    ( 7 , tr("English")               ) ;
  head -> setText                    ( 8 , tr("Native")                ) ;
  head -> setText                    ( 9 , ""                          ) ;
  ////////////////////////////////////////////////////////////////////////
  head -> setToolTip                 ( 1 , tr("ISO-3166 two digits"  ) ) ;
  head -> setToolTip                 ( 2 , tr("ISO-3166 three digits") ) ;
  head -> setToolTip                 ( 3 , tr("ISO-3166 numeric"     ) ) ;
  ////////////////////////////////////////////////////////////////////////
  setWindowTitle                     ( tr("Nations")                   ) ;
  setWindowIcon                      ( QIcon(":/images/nations.png")   ) ;
  setDragDropMode                    ( DragDrop                        ) ;
  setRootIsDecorated                 ( false                           ) ;
  setAlternatingRowColors            ( true                            ) ;
  setSelectionMode                   ( ExtendedSelection               ) ;
  setHorizontalScrollBarPolicy       ( Qt::ScrollBarAsNeeded           ) ;
  setVerticalScrollBarPolicy         ( Qt::ScrollBarAsNeeded           ) ;
  setSuggestion                      ( QSize ( 640 , 480 )             ) ;
  ////////////////////////////////////////////////////////////////////////
  setColumnCount                     ( 10                              ) ;
  setAllAlignments                   ( head , Qt::AlignCenter          ) ;
  setFont                            ( head , Fonts::ListView          ) ;
  assignHeaderItems                  ( head                            ) ;
  setColumnWidth                     ( 9    , 2                        ) ;
  setAlignment                       ( 4                                 ,
                                       Qt::AlignVCenter|Qt::AlignRight ) ;
  setAlignment                       ( 5                                 ,
                                       Qt::AlignVCenter|Qt::AlignRight ) ;
  ////////////////////////////////////////////////////////////////////////
  setFunction                        ( 1000102 , true                  ) ;
  setFunction                        ( 1000103 , true                  ) ;
  setFunction                        ( 1000301 , true                  ) ;
  setFunction                        ( 1001001 , false                 ) ;
  setFunction                        ( 1001002 , false                 ) ;
  ////////////////////////////////////////////////////////////////////////
  setDropFlag                        ( DropPicture , true              ) ;
  setDropFlag                        ( DropPeople  , true              ) ;
  ////////////////////////////////////////////////////////////////////////
  MountClicked                       ( 1                               ) ;
  MountClicked                       ( 2                               ) ;
  ////////////////////////////////////////////////////////////////////////
  plan -> setFont                    ( this                            ) ;
  ////////////////////////////////////////////////////////////////////////
  LocalIcons [ "Add"    ] = QIcon    ( ":/images/plus.png"             ) ;
  LocalIcons [ "Update" ] = QIcon    ( ":/images/update.png"           ) ;
  ////////////////////////////////////////////////////////////////////////
  LocalMsgs  [ NationMsgItems    ] = tr ( "%1 nations"                 ) ;
  LocalMsgs  [ NationMsgPeople   ] = tr ( "%1 peoples"                 ) ;
  LocalMsgs  [ NationMsgPictures ] = tr ( "%1 pictures"                ) ;
  ////////////////////////////////////////////////////////////////////////
  PassDragDrop            = false                                        ;
}

bool N::NationEditor::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  return true                                          ;
}

QMimeData * N::NationEditor::dragMime (void)
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

bool N::NationEditor::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::NationEditor::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return true                                                ;
}

bool N::NationEditor::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::NationEditor::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{ Q_UNUSED ( dropIt ) ;
  Q_UNUSED ( mime   ) ;
}

bool N::NationEditor::finishDrag(QMouseEvent * event)
{ Q_UNUSED ( event ) ;
  return true        ;
}

bool N::NationEditor::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual ( source   , this ) , false ) ;
  return dropHandler(mime)                        ;
}

bool N::NationEditor::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{ Q_UNUSED ( source ) ;
  Q_UNUSED ( mime   ) ;
  Q_UNUSED ( pos    ) ;
  Alert    ( Action ) ;
  return true         ;
}

bool N::NationEditor::dropPeople(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  nKickOut ( source == this , false )   ;
  QPoint pos = psf.toPoint()            ;
  QTreeWidgetItem * item = itemAt(pos)  ;
  nKickOut ( IsNull(item)   , false )   ;
  SUID uuid = nTreeUuid(item,0)         ;
  nKickOut ( uuid <= 0      , false )   ;
  ///////////////////////////////////////
  VarArgs args                          ;
  args << uuid                          ;
  args << Types::People                 ;
  args << Groups::Subordination         ;
  args << LocalMsgs [ NationMsgPeople ] ;
  toVariants ( Uuids , args )           ;
  start      ( 10002 , args )           ;
  ///////////////////////////////////////
  return true                           ;
}

bool N::NationEditor::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  nKickOut ( source == this , false )     ;
  QPoint pos = psf.toPoint()              ;
  QTreeWidgetItem * item = itemAt(pos)    ;
  nKickOut ( IsNull(item)   , false )     ;
  SUID uuid = nTreeUuid(item,0)           ;
  nKickOut ( uuid <= 0      , false )     ;
  /////////////////////////////////////////
  VarArgs args                            ;
  args << uuid                            ;
  args << Types::Picture                  ;
  args << Groups::Subordination           ;
  args << LocalMsgs [ NationMsgPictures ] ;
  toVariants ( Uuids , args )             ;
  start      ( 10002 , args )             ;
  /////////////////////////////////////////
  return true                             ;
}

void N::NationEditor::run(int T,ThreadData * d)
{
  nDropOut         ( ! IsContinue ( d ) ) ;
  switch           ( T                  ) {
    case 10001                            :
      startLoading (                    ) ;
      List         ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
    case 10002                            :
      startLoading (                    ) ;
      DropIn       ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
    case 10003                            :
      startLoading (                    ) ;
      Iso3166      ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
  }                                       ;
}

void N::NationEditor::ListItem (
       SqlConnection   & SC    ,
       QTreeWidgetItem * it    ,
       SUID              uu    )
{
  QString Q                                            ;
  QString n                                            ;
  //////////////////////////////////////////////////////
  n = SC . getName                                     (
        PlanTable(Names)                               ,
        "uuid"                                         ,
        vLanguageId                                    ,
        uu                                           ) ;
  it -> setData ( 0 , Qt::UserRole , uu )              ;
  it -> setText ( 0 , n                 )              ;
  //////////////////////////////////////////////////////
  Q = SC . sql . SelectFrom                            (
        "id,country,position,native,english,shortname" ,
        PlanTable(Nations)                             ,
        SC . WhereUuid ( uu )                        ) ;
  if ( SC . Fetch ( Q ) )                              {
    int     id        = SC . Int    ( 0 )              ;
    int     country   = SC . Int    ( 1 )              ;
    int     position  = SC . Int    ( 2 )              ;
    QString native    = SC . String ( 3 )              ;
    QString english   = SC . String ( 4 )              ;
    QString shortname = SC . String ( 5 )              ;
    ////////////////////////////////////////////////////
    it -> setText ( 4 , QString::number(country)     ) ;
    it -> setText ( 5 , QString::number(position)    ) ;
    it -> setText ( 6 , shortname                    ) ;
    it -> setText ( 7 , english                      ) ;
    it -> setText ( 8 , native                       ) ;
    it -> setData ( 9 , Qt::UserRole , id            ) ;
    setAlignments ( it                               ) ;
  }                                                    ;
  //////////////////////////////////////////////////////
  Q = SC . sql . SelectFrom                            (
        "`two`,`three`,`numeric`"                      ,
        PlanTable(ISO3166)                             ,
        SC . WhereUuid ( uu )                        ) ;
  if ( SC . Fetch ( Q ) )                              {
    QString two     = SC . String ( 0 )                ;
    QString three   = SC . String ( 1 )                ;
    QString numeric = SC . String ( 2 )                ;
    it -> setText ( 1 , two     )                      ;
    it -> setText ( 2 , three   )                      ;
    it -> setText ( 3 , numeric )                      ;
  }                                                    ;
}

void N::NationEditor::List(ThreadData * d)
{
  nDropOut            ( ! IsContinue ( d )           )                       ;
  ////////////////////////////////////////////////////////////////////////////
  emit assignEnabling ( false                        )                       ;
  emit OnBusy         (                              )                       ;
  Bustle              (                              )                       ;
  ////////////////////////////////////////////////////////////////////////////
  SqlConnection SC    ( plan -> sql                  )                       ;
  if                  ( SC . open ( FunctionString ) )                       {
    UUIDs U                                                                  ;
    if ( SC . Uuids   ( U                                                    ,
                        PlanTable(Nations)                                   ,
                        "uuid"                                               ,
                        SC . OrderByAsc ( "id" )                         ) ) {
      for (int i = 0 ; IsContinue ( d ) && ( i < U . count ( ) ) ; i++     ) {
        SUID u = U [ i ]                                                     ;
        NewTreeWidgetItem ( IT          )                                    ;
        ListItem          ( SC , IT , u )                                    ;
        addTopLevelItem   ( IT          )                                    ;
      }                                                                      ;
    }                                                                        ;
    SC . close        (                              )                       ;
  }                                                                          ;
  SC   . remove       (                              )                       ;
  ////////////////////////////////////////////////////////////////////////////
  Vacancy             (                              )                       ;
  emit GoRelax        (                              )                       ;
  emit assignEnabling ( true                         )                       ;
  ////////////////////////////////////////////////////////////////////////////
  QString m                                                                  ;
  m = QString ( LocalMsgs [ NationMsgItems ] ) . arg ( topLevelItemCount() ) ;
  alert            ( "Done" , m                   )                          ;
  ////////////////////////////////////////////////////////////////////////////
  emit AutoFit        (                              )                       ;
}

void N::NationEditor::DropIn(ThreadData * d)
{
  nDropOut ( ! IsContinue ( d )             )                      ;
  nDropOut ( d -> Arguments . count ( ) < 5 )                      ;
  SUID    uu = d -> Arguments [ 0 ] . toULongLong ( )              ;
  int     t2 = d -> Arguments [ 1 ] . toInt       ( )              ;
  int     rl = d -> Arguments [ 2 ] . toInt       ( )              ;
  QString ns = d -> Arguments [ 3 ] . toString    ( )              ;
  UUIDs   U                                                        ;
  //////////////////////////////////////////////////////////////////
  for (int i = 4 ; i < d -> Arguments . count ( ) ; i++ )          {
    U << d -> Arguments [ i ] . toULongLong ( )                    ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                                 ;
  if ( SC . open ( FunctionString ) )                              {
    GroupItems GI ( plan )                                         ;
    GI . AutoMap    = true                                         ;
    GI . GroupTable = GI . LookTable ( Types::Nation , t2 , rl )   ;
    GI . Join ( SC , uu , Types::Nation , t2 , rl , 0 , U , true ) ;
    SC . close  ( )                                                ;
  }                                                                ;
  SC   . remove ( )                                                ;
  //////////////////////////////////////////////////////////////////
  QString m = QString ( ns ) . arg ( U . count ( ) )               ;
  alert ( "Done" , m )                                             ;
}

void N::NationEditor::Iso3166(ThreadData * d)
{
  nDropOut ( ! IsContinue ( d )             )                      ;
  //////////////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                                 ;
  if ( SC . open ( FunctionString ) )                              {
    QString Q                                                      ;
    UUIDs   U                                                      ;
    SUID    u                                                      ;
    ////////////////////////////////////////////////////////////////
    if ( SC . Uuids ( U                                            ,
                      PlanTable(ISO3166)                           ,
                      "uuid"                                       ,
                      SC . OrderByAsc ( "two" ) )                ) {
      int id = 1                                                   ;
      foreach ( u , U )                                            {
        Q = SC . sql . Update                                      (
              PlanTable(ISO3166)                                   ,
              SC.sql.Where(1,"uuid")                               ,
              1                                                    ,
              "id"                                               ) ;
        SC . Prepare ( Q           )                               ;
        SC . Bind    ( "uuid" , u  )                               ;
        SC . Bind    ( "id"   , id )                               ;
        SC . Exec    (             )                               ;
        id ++                                                      ;
      }                                                            ;
      Q = QString ( "alter table `%1` auto_increment = %2"         )
          . arg   ( PlanTable(ISO3166)                             )
          . arg   ( id                                           ) ;
      SC . Query  ( Q                                            ) ;
    }                                                              ;
    SC . close  ( )                                                ;
  }                                                                ;
  SC   . remove ( )                                                ;
  //////////////////////////////////////////////////////////////////
  Alert ( Done )                                                   ;
}

bool N::NationEditor::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::NationEditor::singleClicked(QTreeWidgetItem *,int)
{
  Alert ( Action ) ;
}

void N::NationEditor::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * le = NULL                                                      ;
  SUID        uu = nTreeUuid ( item , 0 )                                    ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( column )                                                          {
    case 0                                                                   :
      le   = setLineEdit                                                     (
               item                                                          ,
               column                                                        ,
               SIGNAL ( returnPressed ( ) )                                  ,
               SLOT   ( nameFinished  ( ) )                                ) ;
      le   -> setFocus ( Qt::TabFocusReason )                                ;
    break                                                                    ;
    case 1                                                                   :
      if ( uu > 0 )                                                          {
        le = setLineEdit                                                     (
               item                                                          ,
               column                                                        ,
               SIGNAL ( returnPressed ( ) )                                  ,
               SLOT   ( twoFinished   ( ) )                                ) ;
        le -> setFocus ( Qt::TabFocusReason )                                ;
      } else                                                                 {
        Alert ( Error )                                                      ;
      }                                                                      ;
    break                                                                    ;
    case 2                                                                   :
      if ( uu > 0 )                                                          {
        le = setLineEdit                                                     (
               item                                                          ,
               column                                                        ,
               SIGNAL ( returnPressed ( ) )                                  ,
               SLOT   ( threeFinished ( ) )                                ) ;
        le -> setFocus ( Qt::TabFocusReason )                                ;
      } else                                                                 {
        Alert ( Error )                                                      ;
      }                                                                      ;
    break                                                                    ;
    case 3                                                                   :
      if ( uu > 0 )                                                          {
        le = setLineEdit                                                     (
               item                                                          ,
               column                                                        ,
               SIGNAL ( returnPressed   ( ) )                                ,
               SLOT   ( numericFinished ( ) )                              ) ;
        le -> setFocus ( Qt::TabFocusReason )                                ;
      } else                                                                 {
        Alert ( Error )                                                      ;
      }                                                                      ;
    break                                                                    ;
    case 4                                                                   :
      if ( uu > 0 )                                                          {
        le = setLineEdit                                                     (
               item                                                          ,
               column                                                        ,
               SIGNAL ( returnPressed ( ) )                                  ,
               SLOT   ( codeFinished  ( ) )                                ) ;
        le -> setFocus ( Qt::TabFocusReason )                                ;
      } else                                                                 {
        Alert ( Error )                                                      ;
      }                                                                      ;
    break                                                                    ;
    case 5                                                                   :
      if ( uu > 0 )                                                          {
        le = setLineEdit                                                     (
               item                                                          ,
               column                                                        ,
               SIGNAL ( returnPressed    ( ) )                               ,
               SLOT   ( positionFinished ( ) )                             ) ;
        le -> setFocus ( Qt::TabFocusReason )                                ;
      } else                                                                 {
        Alert ( Error )                                                      ;
      }                                                                      ;
    break                                                                    ;
    case 6                                                                   :
      if ( uu > 0 )                                                          {
        le = setLineEdit                                                     (
               item                                                          ,
               column                                                        ,
               SIGNAL ( returnPressed ( ) )                                  ,
               SLOT   ( shortFinished ( ) )                                ) ;
        le -> setFocus ( Qt::TabFocusReason )                                ;
      } else                                                                 {
        Alert ( Error )                                                      ;
      }                                                                      ;
    break                                                                    ;
    case 7                                                                   :
      if ( uu > 0 )                                                          {
        le = setLineEdit                                                     (
               item                                                          ,
               column                                                        ,
               SIGNAL ( returnPressed   ( ) )                                ,
               SLOT   ( englishFinished ( ) )                              ) ;
        le -> setFocus ( Qt::TabFocusReason )                                ;
      } else                                                                 {
        Alert ( Error )                                                      ;
      }                                                                      ;
    break                                                                    ;
    case 8                                                                   :
      if ( uu > 0 )                                                          {
        le = setLineEdit                                                     (
               item                                                          ,
               column                                                        ,
               SIGNAL ( returnPressed  ( ) )                                 ,
               SLOT   ( nativeFinished ( ) )                               ) ;
        le -> setFocus ( Qt::TabFocusReason )                                ;
      } else                                                                 {
        Alert ( Error )                                                      ;
      }                                                                      ;
    break                                                                    ;
  }                                                                          ;
}

void N::NationEditor::Insert(void)
{
  removeOldItem     (                       ) ;
  NewTreeWidgetItem ( IT                    ) ;
  setAlignments     ( IT                    ) ;
  IT -> setData     ( 0  , Qt::UserRole , 0 ) ;
  addTopLevelItem   ( IT                    ) ;
  scrollToItem      ( IT                    ) ;
  doubleClicked     ( IT , 0                ) ;
}

void N::NationEditor::nameFinished(void)
{
  SUID        u = nTreeUuid ( ItemEditing , 0          )   ;
  QLineEdit * l = Casting   ( QLineEdit   , ItemWidget )   ;
  if ( IsNull ( l ) )                                      {
    removeOldItem (       )                                ;
    Alert         ( Error )                                ;
    return                                                 ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  QString n = l -> text ( )                                ;
  if ( n . length ( ) > 0 )                                {
    SqlConnection SC ( plan -> sql )                       ;
    if ( SC . open ( FunctionString ) )                    {
      if ( u <= 0 )                                        {
        QString Q                                          ;
        int     id       = 0                               ;
        int     country  = 0                               ;
        int     position = 0                               ;
        ////////////////////////////////////////////////////
        u = SC . Unique                                    (
              PlanTable(MajorUuid)                         ,
              "uuid"                                       ,
              Unify::NationHead                          ) ;
        SC . assureUuid                                    (
          PlanTable(MajorUuid)                             ,
          u                                                ,
          Types::Nation                                  ) ;
        ////////////////////////////////////////////////////
        Q = SC . sql . SelectFrom                          (
              "id"                                         ,
              PlanTable(Nations)                           ,
              SC . OrderByDesc ( "id" )                    ,
              SC . sql . Limit ( 0 , 1 )                 ) ;
        if ( SC . Fetch ( Q ) ) id = SC . Int ( 0 )        ;
        id++                                               ;
        ////////////////////////////////////////////////////
        Q = SC . sql . SelectFrom                          (
              "country"                                    ,
              PlanTable(Nations)                           ,
              SC . OrderByDesc ( "country" )               ,
              SC . sql . Limit ( 0 , 1 )                 ) ;
        if ( SC . Fetch ( Q ) ) country = SC . Int ( 0 )   ;
        country++                                          ;
        ////////////////////////////////////////////////////
        Q = SC . sql . SelectFrom                          (
              "position"                                   ,
              PlanTable(Nations)                           ,
              SC . OrderByDesc ( "position" )              ,
              SC . sql . Limit ( 0 , 1 )                 ) ;
        if ( SC . Fetch ( Q ) ) position = SC . Int ( 0 )  ;
        position++                                         ;
        ////////////////////////////////////////////////////
        Q = SC . sql . InsertInto                          (
              PlanTable(Nations)                           ,
              4                                            ,
              "id"                                         ,
              "uuid"                                       ,
              "country"                                    ,
              "position"                                 ) ;
        SC . Prepare ( Q                                 ) ;
        SC . Bind    ( "id"       , id                   ) ;
        SC . Bind    ( "uuid"     , u                    ) ;
        SC . Bind    ( "country"  , country              ) ;
        SC . Bind    ( "position" , position             ) ;
        SC . Exec    (                                   ) ;
        ////////////////////////////////////////////////////
        SC . assureName                                    (
          PlanTable(Names)                                 ,
          u                                                ,
          vLanguageId                                      ,
          n                                              ) ;
        ////////////////////////////////////////////////////
        QString c = QString::number ( country  )           ;
        QString p = QString::number ( position )           ;
        ItemEditing -> setText ( 0 , n                   ) ;
        ItemEditing -> setText ( 4 , c                   ) ;
        ItemEditing -> setText ( 5 , p                   ) ;
        ItemEditing -> setData ( 0 , Qt::UserRole , u    ) ;
        ItemEditing -> setData ( 8 , Qt::UserRole , id   ) ;
      } else                                               {
        SC . assureName                                    (
          PlanTable(Names)                                 ,
          u                                                ,
          vLanguageId                                      ,
          n                                              ) ;
        ItemEditing -> setText ( 0 , n                   ) ;
      }                                                    ;
      SC . close  (          )                             ;
    }                                                      ;
    SC   . remove (          )                             ;
    Alert         ( Done     )                             ;
  } else                                                   {
    Alert         ( Error    )                             ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  removeOldItem   ( true , 0 )                             ;
}

void N::NationEditor::twoFinished(void)
{
  SUID        u = nTreeUuid ( ItemEditing , 0          ) ;
  QLineEdit * l = Casting   ( QLineEdit   , ItemWidget ) ;
  if ( IsNull ( l ) || ( u <= 0 ) )                      {
    removeOldItem (       )                              ;
    Alert         ( Error )                              ;
    return                                               ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  QString n = l -> text ( )                              ;
  if ( n . length ( ) >= 2 )                             {
    n = n . left    ( 2 )                                ;
    n = n . toUpper (   )                                ;
    SqlConnection SC ( plan -> sql )                     ;
    if ( SC . open ( FunctionString ) )                  {
      QString Q                                          ;
      QString three   = ItemEditing -> text ( 2 )        ;
      QString numeric = ItemEditing -> text ( 3 )        ;
      Q = SC . sql . ReplaceInto                         (
            PlanTable(ISO3166)                           ,
            4                                            ,
            "uuid"                                       ,
            "two"                                        ,
            "three"                                      ,
            "numeric"                                  ) ;
      SC . Prepare ( Q                                 ) ;
      SC . Bind    ( "uuid"    , u                     ) ;
      SC . Bind    ( "two"     , n       . toUtf8 ( )  ) ;
      SC . Bind    ( "three"   , three   . toUtf8 ( )  ) ;
      SC . Bind    ( "numeric" , numeric . toUtf8 ( )  ) ;
      SC . Exec    (                                   ) ;
      ItemEditing -> setText ( 1 , n                   ) ;
      SC . close   (                                   ) ;
    }                                                    ;
    SC   . remove  (                                   ) ;
    Alert          ( Done                              ) ;
  } else                                                 {
    Alert          ( Error                             ) ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  removeOldItem ( )                                      ;
}

void N::NationEditor::threeFinished(void)
{
  SUID        u = nTreeUuid ( ItemEditing , 0          ) ;
  QLineEdit * l = Casting   ( QLineEdit   , ItemWidget ) ;
  if ( IsNull ( l ) || ( u <= 0 ) )                      {
    removeOldItem (       )                              ;
    Alert         ( Error )                              ;
    return                                               ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  QString n = l -> text ( )                              ;
  if ( n . length ( ) >= 3 )                             {
    n = n . left    ( 3 )                                ;
    n = n . toUpper (   )                                ;
    SqlConnection SC ( plan -> sql )                     ;
    if ( SC . open ( FunctionString ) )                  {
      QString Q                                          ;
      QString two     = ItemEditing -> text ( 1 )        ;
      QString numeric = ItemEditing -> text ( 3 )        ;
      Q = SC . sql . ReplaceInto                         (
            PlanTable(ISO3166)                           ,
            4                                            ,
            "uuid"                                       ,
            "two"                                        ,
            "three"                                      ,
            "numeric"                                  ) ;
      SC . Prepare ( Q                                 ) ;
      SC . Bind    ( "uuid"    , u                     ) ;
      SC . Bind    ( "two"     , two     . toUtf8 ( )  ) ;
      SC . Bind    ( "three"   , n       . toUtf8 ( )  ) ;
      SC . Bind    ( "numeric" , numeric . toUtf8 ( )  ) ;
      SC . Exec    (                                   ) ;
      ItemEditing -> setText ( 2 , n                   ) ;
      SC . close   (                                   ) ;
    }                                                    ;
    SC   . remove  (                                   ) ;
    Alert          ( Done                              ) ;
  } else                                                 {
    Alert          ( Error                             ) ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  removeOldItem ( )                                      ;
}

void N::NationEditor::numericFinished(void)
{
  SUID        u = nTreeUuid ( ItemEditing , 0          ) ;
  QLineEdit * l = Casting   ( QLineEdit   , ItemWidget ) ;
  if ( IsNull ( l ) || ( u <= 0 ) )                      {
    removeOldItem (       )                              ;
    Alert         ( Error )                              ;
    return                                               ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  QString n = l -> text ( )                              ;
  if ( n . length ( ) >= 3 )                             {
    n = n . left    ( 3 )                                ;
    n = n . toUpper (   )                                ;
    SqlConnection SC ( plan -> sql )                     ;
    if ( SC . open ( FunctionString ) )                  {
      QString Q                                          ;
      QString two   = ItemEditing -> text ( 1 )          ;
      QString three = ItemEditing -> text ( 2 )          ;
      Q = SC . sql . ReplaceInto                         (
            PlanTable(ISO3166)                           ,
            4                                            ,
            "uuid"                                       ,
            "two"                                        ,
            "three"                                      ,
            "numeric"                                  ) ;
      SC . Prepare ( Q                                 ) ;
      SC . Bind    ( "uuid"    , u                     ) ;
      SC . Bind    ( "two"     , two     . toUtf8 ( )  ) ;
      SC . Bind    ( "three"   , three   . toUtf8 ( )  ) ;
      SC . Bind    ( "numeric" , n       . toUtf8 ( )  ) ;
      SC . Exec    (                                   ) ;
      ItemEditing -> setText ( 3 , n                   ) ;
      SC . close   (                                   ) ;
    }                                                    ;
    SC   . remove  (                                   ) ;
    Alert          ( Done                              ) ;
  } else                                                 {
    Alert          ( Error                             ) ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  removeOldItem ( )                                      ;
}

void N::NationEditor::codeFinished(void)
{
  SUID        u = nTreeUuid ( ItemEditing , 0          ) ;
  QLineEdit * l = Casting   ( QLineEdit   , ItemWidget ) ;
  if ( IsNull ( l ) || ( u <= 0 ) )                      {
    removeOldItem (       )                              ;
    Alert         ( Error )                              ;
    return                                               ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  QString n  = l -> text ( )                             ;
  bool    ok = false                                     ;
  int     c  = n . toInt ( &ok )                         ;
  if ( ok )                                              {
    SqlConnection SC ( plan -> sql )                     ;
    if ( SC . open ( FunctionString ) )                  {
      QString Q                                          ;
      Q = SC . sql . Update                              (
            PlanTable(Nations)                           ,
            SC . sql . Where ( 1 , "uuid" )              ,
            1                                            ,
            "country"                                  ) ;
      SC . Prepare ( Q                                 ) ;
      SC . Bind    ( "uuid"    , u                     ) ;
      SC . Bind    ( "country" , c                     ) ;
      if ( SC . Exec ( ) )                               {
        QString m = QString::number ( c )                ;
        ItemEditing -> setText ( 4 , m                 ) ;
        Alert      ( Done                              ) ;
      } else                                             {
        Alert      ( Error                             ) ;
      }                                                  ;
      SC . close   (                                   ) ;
    }                                                    ;
    SC   . remove  (                                   ) ;
  } else                                                 {
    Alert          ( Error                             ) ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  removeOldItem    (                                   ) ;
}

void N::NationEditor::positionFinished(void)
{
  SUID        u = nTreeUuid ( ItemEditing , 0          ) ;
  QLineEdit * l = Casting   ( QLineEdit   , ItemWidget ) ;
  if ( IsNull ( l ) || ( u <= 0 ) )                      {
    removeOldItem (       )                              ;
    Alert         ( Error )                              ;
    return                                               ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  QString n  = l -> text ( )                             ;
  bool    ok = false                                     ;
  int     p  = n . toInt ( &ok )                         ;
  if ( ok )                                              {
    SqlConnection SC ( plan -> sql )                     ;
    if ( SC . open ( FunctionString ) )                  {
      QString Q                                          ;
      Q = SC . sql . Update                              (
            PlanTable(Nations)                           ,
            SC . sql . Where ( 1 , "uuid" )              ,
            1                                            ,
            "position"                                 ) ;
      SC . Prepare ( Q                                 ) ;
      SC . Bind    ( "uuid"     , u                    ) ;
      SC . Bind    ( "position" , p                    ) ;
      if ( SC . Exec ( ) )                               {
        QString m = QString::number ( p )                ;
        ItemEditing -> setText ( 5 , m                 ) ;
        Alert      ( Done                              ) ;
      } else                                             {
        Alert      ( Error                             ) ;
      }                                                  ;
      SC . close   (                                   ) ;
    }                                                    ;
    SC   . remove  (                                   ) ;
  } else                                                 {
    Alert          ( Error                             ) ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  removeOldItem    (                                   ) ;
}

void N::NationEditor::shortFinished(void)
{
  SUID        u = nTreeUuid ( ItemEditing , 0          ) ;
  QLineEdit * l = Casting   ( QLineEdit   , ItemWidget ) ;
  if ( IsNull ( l ) || ( u <= 0 ) )                      {
    removeOldItem (       )                              ;
    Alert         ( Error )                              ;
    return                                               ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  QString n = l -> text ( )                              ;
  SqlConnection SC ( plan -> sql )                       ;
  if ( SC . open ( FunctionString ) )                    {
    QString Q                                            ;
    Q = SC . sql . Update                                (
          PlanTable(Nations)                             ,
          SC . sql . Where ( 1 , "uuid" )                ,
          1                                              ,
          "shortname"                                  ) ;
    SC . Prepare ( Q                                   ) ;
    SC . Bind    ( "uuid"      , u                     ) ;
    SC . Bind    ( "shortname" , n . toUtf8 ( )        ) ;
    if ( SC . Exec ( ) )                                 {
      ItemEditing -> setText ( 6 , n                   ) ;
      Alert      ( Done                                ) ;
    } else                                               {
      Alert      ( Error                               ) ;
    }                                                    ;
    SC . close   (                                     ) ;
  }                                                      ;
  SC   . remove  (                                     ) ;
  ////////////////////////////////////////////////////////
  removeOldItem  (                                     ) ;
}

void N::NationEditor::englishFinished(void)
{
  SUID        u = nTreeUuid ( ItemEditing , 0          ) ;
  QLineEdit * l = Casting   ( QLineEdit   , ItemWidget ) ;
  if ( IsNull ( l ) || ( u <= 0 ) )                      {
    removeOldItem (       )                              ;
    Alert         ( Error )                              ;
    return                                               ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  QString n = l -> text ( )                              ;
  SqlConnection SC ( plan -> sql )                       ;
  if ( SC . open ( FunctionString ) )                    {
    QString Q                                            ;
    Q = SC . sql . Update                                (
          PlanTable(Nations)                             ,
          SC . sql . Where ( 1 , "uuid" )                ,
          1                                              ,
          "english"                                    ) ;
    SC . Prepare ( Q                                   ) ;
    SC . Bind    ( "uuid"    , u                       ) ;
    SC . Bind    ( "english" , n . toUtf8 ( )          ) ;
    if ( SC . Exec ( ) )                                 {
      ItemEditing -> setText ( 7 , n                   ) ;
      Alert      ( Done                                ) ;
    } else                                               {
      Alert      ( Error                               ) ;
    }                                                    ;
    SC . close   (                                     ) ;
  }                                                      ;
  SC   . remove  (                                     ) ;
  ////////////////////////////////////////////////////////
  removeOldItem  (                                     ) ;
}

void N::NationEditor::nativeFinished(void)
{
  SUID        u = nTreeUuid ( ItemEditing , 0          ) ;
  QLineEdit * l = Casting   ( QLineEdit   , ItemWidget ) ;
  if ( IsNull ( l ) || ( u <= 0 ) )                      {
    removeOldItem (       )                              ;
    Alert         ( Error )                              ;
    return                                               ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  QString n = l -> text ( )                              ;
  SqlConnection SC ( plan -> sql )                       ;
  if ( SC . open ( FunctionString ) )                    {
    QString Q                                            ;
    Q = SC . sql . Update                                (
          PlanTable(Nations)                             ,
          SC . sql . Where ( 1 , "uuid" )                ,
          1                                              ,
          "native"                                     ) ;
    SC . Prepare ( Q                                   ) ;
    SC . Bind    ( "uuid"   , u                        ) ;
    SC . Bind    ( "native" , n . toUtf8 ( )           ) ;
    if ( SC . Exec ( ) )                                 {
      ItemEditing -> setText ( 8 , n                   ) ;
      Alert      ( Done                                ) ;
    } else                                               {
      Alert      ( Error                               ) ;
    }                                                    ;
    SC . close   (                                     ) ;
  }                                                      ;
  SC   . remove  (                                     ) ;
  ////////////////////////////////////////////////////////
  removeOldItem  (                                     ) ;
}

bool N::NationEditor::Menu(QPoint pos)
{
  nScopedMenu           ( mm  , this                    ) ;
  QTreeWidgetItem * item   = itemAt ( pos               ) ;
  QMenu           * ma                                    ;
  QMenu           * ms                                    ;
  QAction         * aa                                    ;
  /////////////////////////////////////////////////////////
  mm     . add          ( 401                             ,
                          LocalIcons [ "Add"    ]         ,
                          tr("Add nation")              ) ;
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
  /////////////////////////////////////////////////////////
  mm     . addSeparator ( ma                            ) ;
  mm     . add          ( ma , 201 , tr("Translations") ) ;
  /////////////////////////////////////////////////////////
  mm     . addSeparator ( ms                            ) ;
  mm     . add          ( ms , 202 , tr("Ordering ISO") ) ;
  /////////////////////////////////////////////////////////
  mm     . setFont      ( plan                          ) ;
  aa     = mm . exec    (                               ) ;
  /////////////////////////////////////////////////////////
  if ( IsNull        (      aa ) ) return true            ;
  if ( RunAdjustment ( mm , aa ) ) return true            ;
  if ( RunSorting    ( mm , aa ) ) return true            ;
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
    case 202                                              :
      start ( 10003 )                                     ;
    break                                                 ;
    case 301                                              :
      if ( NotNull ( item ) )                             {
        QString name = item -> text ( 0 )                 ;
        SUID    xuid = nTreeUuid ( item , 0 )             ;
        emit ListPeople ( name , xuid , Types::Nation )   ;
      }                                                   ;
    break                                                 ;
    case 401                                              :
      Insert ( )                                          ;
    break                                                 ;
    default                                               :
    break                                                 ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  return true                                             ;
}
