#include <society.h>

N::Kinships:: Kinships ( QWidget * parent , Plan * p )
            : TreeDock (           parent ,        p )
{
  Configure ( ) ;
}

N::Kinships::~Kinships(void)
{
}

void N::Kinships::Configure(void)
{
  NewTreeWidgetItem            ( head                    ) ;
  head -> setText              ( 0 , tr("Name"         ) ) ;
  head -> setText              ( 1 , tr("Sexuality"    ) ) ;
  head -> setText              ( 2 , tr("Consanguinity") ) ;
  setWindowTitle               ( tr("Kinships")          ) ;
  setDragDropMode              ( NoDragDrop              ) ;
  setRootIsDecorated           ( false                   ) ;
  setAlternatingRowColors      ( true                    ) ;
  setSelectionMode             ( SingleSelection         ) ;
  setColumnCount               ( 3                       ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded   ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded   ) ;
  assignHeaderItems            ( head                    ) ;
  plan -> setFont              ( this                    ) ;
  MountClicked                 ( 2                       ) ;
}

bool N::Kinships::FocusIn(void)
{
  AssignAction ( Label   , windowTitle     ( ) ) ;
  LinkAction   ( Refresh , startup         ( ) ) ;
  LinkAction   ( Insert  , Insert          ( ) ) ;
  LinkAction   ( Copy    , CopyToClipboard ( ) ) ;
  LinkAction   ( Export  , Export          ( ) ) ;
  return true                                    ;
}

bool N::Kinships::startup(void)
{
  clear                   (                   ) ;
  Genders         . clear (                   ) ;
  Relations       . clear (                   ) ;
  Sexualities     . clear (                   ) ;
  Consanguinities . clear (                   ) ;
  EnterSQL         ( SC , plan->sql           ) ;
    QString Q                                   ;
    UUIDs Uuids                                 ;
    NAMEs Names                                 ;
    SUID  uuid                                  ;
    Genders   = SC.Uuids                        (
                  PlanTable(Gender)             ,
                  "uuid"                        ,
                  "order by id asc"           ) ;
    Relations = SC.Uuids                        (
                  PlanTable(Consanguinities)    ,
                  "uuid"                        ,
                  "order by id asc"           ) ;
    Uuids     = SC.Uuids                        (
                  PlanTable(Kinships)           ,
                  "uuid"                        ,
                  "order by id asc"           ) ;
    foreach (uuid,Genders)                      {
      Sexualities[uuid] = SC.getName            (
                            PlanTable(Names)    ,
                            "uuid"              ,
                            plan->LanguageId    ,
                            uuid              ) ;
    }                                           ;
    foreach (uuid,Relations)                    {
      Consanguinities[uuid] = SC.getName        (
                            PlanTable(Names)    ,
                            "uuid"              ,
                            plan->LanguageId    ,
                            uuid              ) ;
    }                                           ;
    foreach (uuid,Uuids)                        {
      Names[uuid] = SC.getName                  (
                            PlanTable(Names)    ,
                            "uuid"              ,
                            plan->LanguageId    ,
                            uuid              ) ;
    }                                           ;
    foreach (uuid,Uuids)                        {
      NewTreeWidgetItem(item)                   ;
      SUID guid                                 ;
      SUID cuid                                 ;
      Q = SC.sql.SelectFrom                     (
            "gender,consanguinity"              ,
            PlanTable(Kinships)                 ,
            SC.WhereUuid(uuid)                ) ;
      if (SC.Fetch(Q))                          {
        guid = SC . Uuid ( 0 )                  ;
        cuid = SC . Uuid ( 1 )                  ;
      }                                         ;
      item -> setData(0,Qt::UserRole,uuid)      ;
      item -> setData(1,Qt::UserRole,guid)      ;
      item -> setData(2,Qt::UserRole,cuid)      ;
      item -> setText(0,Names          [uuid] ) ;
      item -> setText(1,Sexualities    [guid] ) ;
      item -> setText(2,Consanguinities[cuid] ) ;
      addTopLevelItem(item)                     ;
    }                                           ;
  LeaveSQL         ( SC , plan->sql          )  ;
  SuitableColumns  (                         )  ;
  Alert            ( Done                    )  ;
  return true                                   ;
}

void N::Kinships::Insert(void)
{
  NewTreeWidgetItem(item)                     ;
  item->setData(0,Qt::UserRole,0)             ;
  item->setData(1,Qt::UserRole,Genders[0])    ;
  item->setData(2,Qt::UserRole,Relations[0])  ;
  addTopLevelItem(item)                       ;
  doubleClicked(item,0)                       ;
}

void N::Kinships::Export(void)
{
  QString filename = QFileDialog::getSaveFileName  (
                       this                        ,
                       tr("Save kinships")         ,
                       plan->Temporary("")         ,
                       tr("Text file (*.txt)")   ) ;
  if (filename.length()<=0) return                 ;
  QFile F(filename)                                ;
  if (!F.open(QIODevice::WriteOnly)) return        ;
  for (int i=0;i<topLevelItemCount();i++)          {
    QTreeWidgetItem * item  = topLevelItem(i)      ;
    QString full                                   ;
    full  = item->text(0)                          ;
    full += " "                                    ;
    full += item->text(1)                          ;
    full += " "                                    ;
    full += item->text(2)                          ;
    full += "\n"                                   ;
    F.write(full.toUtf8())                         ;
  }                                                ;
  F.close()                                        ;
  Alert(Done)                                      ;
}

bool N::Kinships::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                     ;
  QMdiSubWindow * mdi    = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget   * dock   = Casting ( QDockWidget   , parent() ) ;
  QAction    * aa                                               ;
  mm . add ( 101 , tr("Refresh") )                              ;
  mm . add ( 102 , tr("Insert" ) )                              ;
  mm . add ( 103 , tr("Copy"   ) )                              ;
  mm . add ( 104 , tr("Export" ) )                              ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))  ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))  ;
  mm.setFont ( plan    )                                        ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
    nFastCast ( 101 , startup         ( ) )                     ;
    nFastCast ( 102 , Insert          ( ) )                     ;
    nFastCast ( 103 , CopyToClipboard ( ) )                     ;
    nFastCast ( 104 , Export          ( ) )                     ;
    case 1000001                                                :
      emit attachMdi ( this , 0 )                               ;
    break                                                       ;
    case 1000002                                                :
      emit attachDock                                           (
        this , windowTitle()                                    ,
        Qt::RightDockWidgetArea                                 ,
        Qt::LeftDockWidgetArea  | Qt::RightDockWidgetArea       |
        Qt::TopDockWidgetArea   | Qt::BottomDockWidgetArea    ) ;
    break                                                       ;
    default                                                     :
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}

void N::Kinships::ListGender(QComboBox * combo)
{
  combo -> clear ( ) ;
  N::AddItems(*combo,Genders,Sexualities) ;
}

void N::Kinships::ListConsanguinities(QComboBox * combo)
{
  combo -> clear ( )                                    ;
  N::AddItems(*combo,Relations,Consanguinities) ;
}

void N::Kinships::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                               ;
  QComboBox * cc                                 ;
  SUID        uuid = nTreeUuid(item,0)           ;
  SUID        guid = nTreeUuid(item,1)           ;
  SUID        cuid = nTreeUuid(item,2)           ;
  SUID        puid = nTreeUuid(item,3)           ;
  switch (column)                                {
    case 0                                       :
      line = setLineEdit                         (
               item,column                       ,
               SIGNAL(editingFinished())         ,
               SLOT  (nameFinished   ()) )       ;
      line->setFocus( Qt::TabFocusReason )       ;
    break                                        ;
    case 1                                       :
      cc   = setComboBox                         (
               item,column                       ,
               SIGNAL(activated(int))            ,
               SLOT(genderFinished(int)) )       ;
      ListGender(cc)                             ;
      if (guid>0) (*cc) <= guid                  ;
      cc->setEditable(true)                      ;
      cc->showPopup()                            ;
    break                                        ;
    case 2                                       :
      cc   = setComboBox                         (
               item,column                       ,
               SIGNAL(activated(int))            ,
               SLOT(consanguinityFinished(int))) ;
      ListConsanguinities(cc)                    ;
      if (cuid>0) (*cc) <= cuid                  ;
      cc->setEditable(true)                      ;
      cc->showPopup()                            ;
    break                                        ;
  }                                              ;
}

void N::Kinships::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                   ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget)  ;
  if (IsNull(line)) return                          ;
  SUID    uuid = nTreeUuid(ItemEditing,0)           ;
  QString name = line->text()                       ;
  DocumentManager DM ( plan )                      ;
  EnterSQL         ( SC , plan->sql          )      ;
    QString Q                                       ;
    if (uuid<=0)                                    {
      SUID guid = Genders  [0]                      ;
      SUID cuid = Relations[0]                      ;
      uuid = SC.Unique(PlanTable(MajorUuid)         ,
                       "uuid",9784                ) ;
      SC.assureUuid(PlanTable(MajorUuid),uuid       ,
                    Types::Kinship      ) ;
      Q = SC.sql.InsertInto(PlanTable(Kinships)     ,
                            3                       ,
                            "uuid"                  ,
                            "gender"                ,
                            "consanguinity"       ) ;
      SC . Prepare ( Q                      )       ;
      SC . Bind    ( "uuid"          , uuid )       ;
      SC . Bind    ( "gender"        , guid )       ;
      SC . Bind    ( "consanguinity" , cuid )       ;
      SC . Exec    (                        )       ;
      ItemEditing->setData(0,Qt::UserRole,uuid    ) ;
      ItemEditing->setData(1,Qt::UserRole,guid    ) ;
      ItemEditing->setData(2,Qt::UserRole,cuid    ) ;
      ItemEditing->setText(0,name)                  ;
      ItemEditing->setText(1,Sexualities    [guid]) ;
      ItemEditing->setText(2,Consanguinities[cuid]) ;
    } else                                          {
      DM.assureName(SC,uuid,plan->LanguageId,name)  ;
      ItemEditing->setText(0,name)                  ;
    }                                               ;
  LeaveSQL         ( SC , plan->sql          )      ;
  removeOldItem    ( true , 0                )      ;
  Alert            ( Done                    )      ;
}

void N::Kinships::genderFinished(int)
{
  if (IsNull(ItemEditing)) return                  ;
  QComboBox * cc = Casting(QComboBox,ItemWidget)   ;
  if (IsNull(cc)) return                           ;
  SUID    uuid    = nTreeUuid(ItemEditing,0)       ;
  SUID    guid    = N::GetUuid(cc)          ;
  EnterSQL         ( SC , plan->sql          )     ;
    QString Q                                      ;
    Q = SC.sql.Update                              (
          PlanTable(Kinships)                      ,
          "where uuid = :UUID"                     ,
          1,"gender"                             ) ;
    SC . Prepare ( Q                      )        ;
    SC . Bind    ( "uuid"          , uuid )        ;
    SC . Bind    ( "gender"        , guid )        ;
    SC . Exec    (                        )        ;
    ItemEditing->setData(1,Qt::UserRole,guid)      ;
    ItemEditing->setText(1,Sexualities[guid])      ;
  LeaveSQL         ( SC , plan->sql          )     ;
  removeOldItem    ( true , 0                )     ;
  Alert            ( Done                    )     ;
}

void N::Kinships::consanguinityFinished(int)
{
  if (IsNull(ItemEditing)) return                 ;
  QComboBox * cc = Casting(QComboBox,ItemWidget)  ;
  if (IsNull(cc)) return                          ;
  SUID    uuid    = nTreeUuid(ItemEditing,0)      ;
  SUID    cuid    = N::GetUuid(cc)         ;
  EnterSQL         ( SC , plan->sql          )    ;
    QString Q                                     ;
    Q = SC.sql.Update                             (
          PlanTable(Kinships)                     ,
          "where uuid = :UUID"                    ,
          1,"consanguinity"                     ) ;
    SC . Prepare ( Q                      )       ;
    SC . Bind    ( "uuid"          , uuid )       ;
    SC . Bind    ( "consanguinity" , cuid )       ;
    SC . Exec    (                        )       ;
    ItemEditing->setData(2,Qt::UserRole,cuid    ) ;
    ItemEditing->setText(2,Consanguinities[cuid]) ;
  LeaveSQL         ( SC , plan->sql          )    ;
  removeOldItem    ( true , 0                )    ;
  Alert            ( Done                    )    ;
}
