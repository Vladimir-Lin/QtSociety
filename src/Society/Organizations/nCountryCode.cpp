#include <society.h>

N::CountryCode:: CountryCode ( QWidget * parent , Plan * p )
               : TreeDock    (           parent ,        p )
{
  Configure ( ) ;
}

N::CountryCode::~CountryCode(void)
{
}

void N::CountryCode::Configure(void)
{
  setAccessibleName            ( "N::CountryCode"      ) ;
  setObjectName                ( "N::CountryCode"      ) ;
  ////////////////////////////////////////////////////////
  NewTreeWidgetItem            ( head                  ) ;
  head -> setText              ( 0 , tr("Country"  )   ) ;
  head -> setText              ( 1 , tr("Number"   )   ) ;
  head -> setText              ( 2 , ""                ) ;
  ////////////////////////////////////////////////////////
  setWindowTitle               ( tr("Country codes")   ) ;
  setDragDropMode              ( NoDragDrop            ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setSelectionMode             ( SingleSelection       ) ;
  setColumnCount               ( 3                     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  setAlignment                 ( 1 , Qt::AlignRight      |
                                     Qt::AlignVCenter  ) ;
  assignHeaderItems            ( head                  ) ;
  setColumnWidth               ( 2 , 2                 ) ;
  ////////////////////////////////////////////////////////
  MountClicked                 ( 2                     ) ;
  plan -> setFont              ( this                  ) ;
}

bool N::CountryCode::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true                ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Insert       , Insert          ( ) ) ;
  LinkAction        ( Refresh      , startup         ( ) ) ;
  LinkAction        ( Export       , Export          ( ) ) ;
  LinkAction        ( Delete       , Delete          ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll       ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone      ( ) ) ;
  LinkAction        ( Language     , Language        ( ) ) ;
  LinkAction        ( Font         , setFont         ( ) ) ;
  return true                                              ;
}

void N::CountryCode::run(int T,ThreadData * d)
{
  switch ( T )   {
    case 10001   :
      List ( d ) ;
    break        ;
  }              ;
}

void N::CountryCode::List(ThreadData * d)
{
  nDropOut          ( ! IsContinue ( d )     ) ;
  //////////////////////////////////////////////
  emit clearItems   (                        ) ;
  Countries . clear (                        ) ;
  ccNames   . clear (                        ) ;
  ccMaps    . clear (                        ) ;
  rrMaps    . clear (                        ) ;
  //////////////////////////////////////////////
  EnterSQL         ( SC , plan->sql          ) ;
    QString Q                                  ;
    UUIDs   Uuids                              ;
    SUID    uuid                               ;
    Countries = SC.Uuids(PlanTable(Nations)    ,
                  "uuid","order by id asc"   ) ;
    foreach (uuid,Countries)                   {
      ccNames[uuid] = SC.getName               (
                        PlanTable(Names)       ,
                        "uuid"                 ,
                        plan->LanguageId       ,
                        uuid                 ) ;
      Q = SC.sql.SelectFrom                    (
            "country"                          ,
            PlanTable(Nations)                 ,
            SC.WhereUuid(uuid)               ) ;
      if (SC.Fetch(Q))                         {
        int country = SC.Int(0)                ;
        ccMaps[country] = uuid                 ;
        rrMaps[uuid   ] = country              ;
      }                                        ;
    }                                          ;
    Uuids = SC.Uuids(PlanTable(CountryCode)    ,
              "uuid","order by id asc"       ) ;
    foreach (uuid,Uuids)                       {
      Q = SC.sql.SelectFrom                    (
            "country,phone"                    ,
            PlanTable(CountryCode)             ,
            SC.WhereUuid(uuid)               ) ;
      if (SC.Fetch(Q))                         {
        NewTreeWidgetItem(item)                ;
        int     country = SC . Int    (0)      ;
        QString phone   = SC . String (1)      ;
        SUID    cuid    = ccMaps[country]      ;
        item->setData(0,Qt::UserRole,cuid)     ;
        item->setData(1,Qt::UserRole,uuid)     ;
        item->setText(0,ccNames[cuid]    )     ;
        item->setText(1,phone            )     ;
        setAlignments   ( item )               ;
        addTopLevelItem ( item )               ;
      }                                        ;
    }                                          ;
  LeaveSQL         ( SC , plan->sql          ) ;
  //////////////////////////////////////////////
  emit AutoFit     (                         ) ;
  Alert            ( Done                    ) ;
}

void N::CountryCode::Delete(ThreadData * d)
{
}

bool N::CountryCode::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

void N::CountryCode::Insert(void)
{
  NewTreeWidgetItem ( IT                   ) ;
  IT -> setData     ( 0 , Qt::UserRole , 0 ) ;
  IT -> setData     ( 1 , Qt::UserRole , 0 ) ;
  setAlignments     ( IT                   ) ;
  addTopLevelItem   ( IT                   ) ;
  doubleClicked     ( IT , 0               ) ;
}

void N::CountryCode::Delete(void)
{
  QTreeWidgetItem * item = currentItem()       ;
  if (IsNull(item)) return                     ;
  SUID uuid = nTreeUuid(item,1)                ;
  EnterSQL         ( SC , plan->sql          ) ;
    QString Q                                  ;
    if (uuid>0)                                {
      int index = indexOfTopLevelItem(item)    ;
      Q = SC.sql.DeleteFrom                    (
            PlanTable(CountryCode)             ,
            SC.WhereUuid(uuid)               ) ;
      SC.Query(Q)                              ;
      Q = SC.sql.DeleteFrom                    (
            PlanTable(MajorUuid)               ,
            SC.WhereUuid(uuid)               ) ;
      SC.Query(Q)                              ;
      if (index>=0) takeTopLevelItem(index)    ;
    }                                          ;
  LeaveSQL         ( SC , plan->sql          ) ;
  Alert            ( Done                    ) ;
}

void N::CountryCode::ListCountries(QComboBox * combo)
{
  SUID uuid                            ;
  combo -> clear ( )                   ;
  foreach (uuid,Countries)             {
    combo->addItem(ccNames[uuid],uuid) ;
  }                                    ;
}

bool N::CountryCode::isPhone(QString phone)
{
  for (int i=0;i<phone.length();i++) {
    QChar c = phone.at(0)            ;
    if (!c.isDigit()) return false   ;
  }                                  ;
  return true                        ;
}

void N::CountryCode::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                         ;
  QComboBox * cc                           ;
  SUID        cuid = nTreeUuid(item,0)     ;
  switch (column)                          {
    case 0                                 :
      cc   = setComboBox                   (
               item,column                 ,
               SIGNAL(activated(int))      ,
               SLOT(countryFinished(int))) ;
      ListCountries(cc)                    ;
      (*cc) <= cuid                        ;
      cc->setEditable(true)                ;
      cc->showPopup()                      ;
    break                                  ;
    case 1                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (nameFinished   ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
  }                                        ;
}

void N::CountryCode::countryFinished(int)
{
  if (IsNull(ItemEditing)) return                  ;
  QComboBox * cc = Casting(QComboBox,ItemWidget)   ;
  if (IsNull(cc)) return                           ;
  SUID    uuid    = nTreeUuid(ItemEditing,1)       ;
  SUID    cuid    = N::GetUuid(cc)                 ;
  int     country = rrMaps[cuid]                   ;
  EnterSQL         ( SC , plan->sql              ) ;
    QString Q                                      ;
    if (uuid<=0)                                   {
      uuid = SC.Unique                             (
               PlanTable(MajorUuid)                ,
               "uuid"                              ,
               9289                              ) ;
      SC.assureUuid                                (
        PlanTable(MajorUuid)                       ,
        uuid                                       ,
        Types::CountryCode             ) ;
      Q = SC.sql.InsertInto                        (
            PlanTable(CountryCode)                 ,
            2,"uuid","country"                   ) ;
      SC . Prepare ( Q                   )         ;
      SC . Bind    ( "uuid"    , uuid    )         ;
      SC . Bind    ( "country" , country )         ;
      SC . Exec    (                     )         ;
    } else                                         {
      Q = SC.sql.Update(PlanTable(CountryCode)     ,
            "where uuid = :UUID",1,"country"     ) ;
      SC . Prepare ( Q                   )         ;
      SC . Bind    ( "uuid"    , uuid    )         ;
      SC . Bind    ( "country" , country )         ;
      SC . Exec    (                     )         ;
    }                                              ;
    ItemEditing->setData(0,Qt::UserRole,cuid)      ;
    ItemEditing->setData(1,Qt::UserRole,uuid)      ;
    ItemEditing->setText(0,cc->currentText())      ;
  LeaveSQL         ( SC , plan->sql          )     ;
  removeOldItem    ( true , 1                )     ;
  Alert            ( Done                    )     ;
}

void N::CountryCode::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                   ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget)  ;
  if (IsNull(line)) return                          ;
  SUID    uuid  = nTreeUuid(ItemEditing,1)          ;
  QString phone = line->text()                      ;
  EnterSQL         ( SC , plan->sql          )      ;
    QString Q                                       ;
    if (uuid>0 && isPhone(phone))                   {
      Q = SC.sql.Update                             (
            PlanTable(CountryCode)                  ,
            "where uuid = :UUID"                    ,
            1,"phone"                             ) ;
      SC.Prepare ( Q                        )       ;
      SC.Bind    ( "uuid"  , uuid           )       ;
      SC.Bind    ( "phone" , phone.toUtf8() )       ;
      SC.Exec    (                          )       ;
      ItemEditing->setText(1,phone)                 ;
      Alert ( Done  )                               ;
    } else                                          {
      Alert ( Error )                               ;
    }                                               ;
  LeaveSQL         ( SC , plan->sql          )      ;
  removeOldItem    ( true , 1                )      ;
}

void N::CountryCode::Export(void)
{
  QString filename = QFileDialog::getSaveFileName                  (
                       this                                        ,
                       tr("Save international phone country code") ,
                       plan->Temporary("")                         ,
                       tr("Text file (*.txt)")                   ) ;
  if (filename.length()<=0) return                                 ;
  QFile F(filename)                                                ;
  if (!F.open(QIODevice::WriteOnly)) return                        ;
  for (int i=0;i<topLevelItemCount();i++)                          {
    QTreeWidgetItem * item  = topLevelItem(i)                      ;
    QString           phone = item->text(1)                        ;
    QString           name  = item->text(0)                        ;
    QString           space = " "                                  ;
    space = space.repeated(4-phone.length())                       ;
    phone = space + phone                                          ;
    space = QString("%1 - %2\r\n").arg(phone).arg(name)            ;
    F.write(space.toUtf8())                                        ;
  }                                                                ;
  F.close()                                                        ;
  Alert ( Done )                                                   ;
}

bool N::CountryCode::Menu(QPoint pos)
{
  nScopedMenu    ( mm , this                     ) ;
  QAction    * aa                                  ;
  DockingMenu    ( mm                            ) ;
  mm . setFont   ( plan                          ) ;
  aa = mm . exec (                               ) ;
  nKickOut       ( IsNull     (      aa ) , true ) ;
  nKickOut       ( RunDocking ( mm , aa ) , true ) ;
  return true                                      ;
}
