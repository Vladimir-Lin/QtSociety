#include <society.h>

#pragma message("PhoneLists requires a full implementation")

N::PhoneLists:: PhoneLists ( QWidget * parent , Plan * p )
              : TreeDock   (           parent ,        p )
              , Group      (                             )
{
  Configure ( ) ;
}

N::PhoneLists::~PhoneLists(void)
{
}

void N::PhoneLists::Configure(void)
{
  setAccessibleName            ( "N::PhoneLists"       ) ;
  setObjectName                ( "N::PhoneLists"       ) ;
  ////////////////////////////////////////////////////////
  NewTreeWidgetItem            ( head                  ) ;
  head -> setText              ( 0 , tr("Area code"  ) ) ;
  head -> setText              ( 1 , tr("Number"     ) ) ;
  head -> setText              ( 2 , tr("Full number") ) ;
  head -> setText              ( 3 , ""                ) ;
  ////////////////////////////////////////////////////////
  setWindowTitle               ( tr("Phones" )         ) ;
  setDragDropMode              ( NoDragDrop            ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setSelectionMode             ( SingleSelection       ) ;
  setColumnCount               ( 4                     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  assignHeaderItems            ( head                  ) ;
  setColumnWidth               ( 3 , 2                 ) ;
  ////////////////////////////////////////////////////////
  MountClicked                 ( 2                     ) ;
  plan -> setFont              ( this                  ) ;
}

bool N::PhoneLists::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true                ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Insert       , Insert          ( ) ) ;
  LinkAction        ( Delete       , Delete          ( ) ) ;
  LinkAction        ( Refresh      , startup         ( ) ) ;
  LinkAction        ( Export       , Export          ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll       ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone      ( ) ) ;
  LinkAction        ( Language     , Language        ( ) ) ;
  LinkAction        ( Font         , setFont         ( ) ) ;
  return true                                              ;
}

bool N::PhoneLists::startup(void)
{
  clear            (                          ) ;
  AreaUuids . clear(                          ) ;
  ccCodes   . clear(                          ) ;
  Countries . clear(                          ) ;
  areaNames . clear(                          ) ;
  ccNames   . clear(                          ) ;
  ccPhones  . clear(                          ) ;
  ccMaps    . clear(                          ) ;
  rrMaps    . clear(                          ) ;
  rcMaps    . clear(                          ) ;
  acCodes   . clear(                          ) ;
  acPhones  . clear(                          ) ;
  acCountry . clear(                          ) ;
  acArea    . clear(                          ) ;
  EnterSQL         ( SC , plan->sql           ) ;
    QString Q                                   ;
    UUIDs   Uuids                               ;
    SUID    uuid                                ;
    Countries = SC.Uuids(PlanTable(Nations)     ,
                  "uuid","order by id asc"    ) ;
    AreaUuids = SC.Uuids(PlanTable(Area)        ,
                  "uuid","order by id asc"    ) ;
    ccCodes   = SC.Uuids(PlanTable(CountryCode) ,
                  "uuid","order by id asc"    ) ;
    acCodes   = SC.Uuids(PlanTable(AreaCode)    ,
                  "uuid","order by id asc"    ) ;
    foreach (uuid,Countries)                    {
      ccNames[uuid] = SC.getName                (
                        PlanTable(Names)        ,
                        "uuid"                  ,
                        plan->LanguageId        ,
                        uuid                  ) ;
      Q = SC.sql.SelectFrom                     (
            "country"                           ,
            PlanTable(Nations)                  ,
            SC.WhereUuid(uuid)                ) ;
      if (SC.Fetch(Q))                          {
        int country = SC.Int(0)                 ;
        ccMaps[country] = uuid                  ;
        rrMaps[uuid   ] = country               ;
      }                                         ;
    }                                           ;
    foreach (uuid,AreaUuids)                    {
      areaNames[uuid] = SC.getName              (
                        PlanTable(Names)        ,
                        "uuid"                  ,
                        plan->LanguageId        ,
                        uuid                  ) ;
    }                                           ;
    foreach (uuid,ccCodes)                      {
      Q = SC.sql.SelectFrom                     (
            "country,phone"                     ,
            PlanTable(CountryCode)              ,
            SC.WhereUuid(uuid)                ) ;
      if (SC.Fetch(Q))                          {
        int     country = SC . Int    ( 0 )     ;
        QString phone   = SC . String ( 1 )     ;
        ccPhones[uuid]  = phone                 ;
        rcMaps  [uuid]  = country               ;
      }                                         ;
    }                                           ;
    Uuids . clear ( )                           ;
    foreach (uuid,acCodes)                      {
      Q = SC.sql.SelectFrom                     (
            "countrycode,area,phone"            ,
            PlanTable(AreaCode)                 ,
            SC.WhereUuid(uuid)                ) ;
      if (SC.Fetch(Q))                          {
        SUID    cc       = SC . Uuid   ( 0 )    ;
        SUID    ca       = SC . Uuid   ( 1 )    ;
        QString phone    = SC . String ( 2 )    ;
        if (cc>0 && ca>0)                       {
          acPhones [uuid]  = phone              ;
          acCountry[uuid]  = cc                 ;
          acArea   [uuid]  = ca                 ;
        } else                                  {
          Uuids << uuid                         ;
        }                                       ;
      }                                         ;
    }                                           ;
    foreach (uuid,Uuids)                        {
      int index = acCodes.indexOf(uuid)         ;
      if (index>=0) acCodes.takeAt(index)       ;
    }                                           ;
    Uuids . clear ( )                           ;
    Uuids     = SC.Uuids(PlanTable(Phones)      ,
                  "uuid","order by id desc"   ) ;
    foreach (uuid,Uuids)                        {
      Q = SC.sql.SelectFrom                     (
            "countrycode,areacode,number"       ,
            PlanTable(Phones)                   ,
            SC.WhereUuid(uuid)                ) ;
      if (SC.Fetch(Q))                          {
        NewTreeWidgetItem(item)                 ;
        SUID    cuid  = SC . Uuid   ( 0 )       ;
        SUID    auid  = SC . Uuid   ( 1 )       ;
        QString phone = SC . String ( 2 )       ;
        item->setData(0,Qt::UserRole,auid)      ;
        item->setData(1,Qt::UserRole,uuid)      ;
        item->setText(1,phone            )      ;
        if (auid>0 && cuid>0)                   {
          QString cp = ccPhones  [cuid]         ;
          QString ap = acPhones  [auid]         ;
          QString cs                            ;
          cs  = "+"                             ;
          cs += cp                              ;
          if (ap.length()>0)                    {
            cs += "-"                           ;
            cs += ap                            ;
          }                                     ;
          item->setText(0,cs)                   ;
        }                                       ;
        PresentNumber   ( item )                ;
        addTopLevelItem ( item )                ;
      }                                         ;
    }                                           ;
  LeaveSQL         ( SC , plan->sql          )  ;
  SuitableColumns  (                         )  ;
  Alert            ( Done                    )  ;
  return true                                   ;
}

void N::PhoneLists::Insert(void)
{
  NewTreeWidgetItem  ( IT                    ) ;
  IT -> setData      ( 0  , Qt::UserRole , 0 ) ;
  IT -> setData      ( 1  , Qt::UserRole , 0 ) ;
  insertTopLevelItem ( 0  , IT               ) ;
  doubleClicked      ( IT , 0                ) ;
}

void N::PhoneLists::Delete(void)
{
  QTreeWidgetItem * item = currentItem()       ;
  if (IsNull(item)) return                     ;
  SUID uuid = nTreeUuid(item,1)                ;
  EnterSQL         ( SC , plan->sql          ) ;
    QString Q                                  ;
    if (uuid>0)                                {
      int index = indexOfTopLevelItem(item)    ;
      Q = SC.sql.DeleteFrom                    (
            PlanTable(Phones)                  ,
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

void N::PhoneLists::Export(void)
{
  QString filename = QFileDialog::getSaveFileName (
                       this                       ,
                       tr("Save phone numbers")   ,
                       plan->Temporary("")        ,
                       tr("Text file (*.txt)")  ) ;
  if (filename.length()<=0) return                ;
  QFile F(filename)                               ;
  if (!F.open(QIODevice::WriteOnly)) return       ;
  for (int i=0;i<topLevelItemCount();i++)         {
    QTreeWidgetItem * item  = topLevelItem(i)     ;
    QString full  = item->text(2)                 ;
    if (full.length()>0)                          {
      full += "\r\n"                              ;
      F.write(full.toUtf8())                      ;
    }                                             ;
  }                                               ;
  F.close()                                       ;
  Alert ( Done )                                  ;
}

void N::PhoneLists::ListAreas(QComboBox * combo)
{
  SUID uuid                       ;
  combo -> clear ( )              ;
  foreach (uuid,acCodes)          {
    QString name                  ;
    SUID    ac = acArea    [uuid] ;
    SUID    cc = acCountry [uuid] ;
    QString cp = ccPhones  [cc  ] ;
    QString ap = acPhones  [uuid] ;
    QString nn = areaNames [ac  ] ;
    QString cs                    ;
    cs  = "+"                     ;
    cs += cp                      ;
    if (ap.length()>0)            {
      cs += "-"                   ;
      cs += ap                    ;
    }                             ;
    name = tr("%1 : %2"           )
           .arg(cs                )
           .arg(nn              ) ;
    combo->addItem(name,uuid)     ;
  }                               ;
}

void N::PhoneLists::PresentNumber(QTreeWidgetItem * item)
{
  QString cc = item->text(0) ;
  QString nu = item->text(1) ;
  if (cc.length()<=0) return ;
  if (nu.length()<=0) return ;
  QString pn = cc + "-" + nu ;
  item->setText(2,pn)        ;
}

bool N::PhoneLists::isPhone(QString phone)
{
  for (int i=0;i<phone.length();i++) {
    QChar c = phone.at(0)            ;
    if (!c.isDigit() && c!='-')      {
      return false                   ;
    }                                ;
  }                                  ;
  return true                        ;
}

void N::PhoneLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                         ;
  QComboBox * cc                           ;
  SUID        auid = nTreeUuid(item,0)     ;
  switch (column)                          {
    case 0                                 :
      cc   = setComboBox                   (
               item,column                 ,
               SIGNAL(activated(int))      ,
               SLOT(areaFinished(int))   ) ;
      ListAreas(cc)                        ;
      if (auid>0) (*cc) <= auid            ;
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

void N::PhoneLists::areaFinished(int)
{
  if (IsNull(ItemEditing)) return                ;
  QComboBox * cc = Casting(QComboBox,ItemWidget) ;
  if (IsNull(cc)) return                         ;
  SUID    uuid    = nTreeUuid(ItemEditing,1)     ;
  SUID    auid    = N::GetUuid(cc)               ;
  SUID    ccid    = acCountry[auid]              ;
  EnterSQL         ( SC , plan->sql            ) ;
    QString Q                                    ;
    if (uuid<=0)                                 {
      uuid = SC.Unique                           (
               PlanTable(MajorUuid),"uuid",9278) ;
      SC.assureUuid(PlanTable(MajorUuid)         ,
                    uuid,Types::Phone) ;
      Q = SC.sql.InsertInto(PlanTable(Phones)    ,
            3,"uuid","countrycode","areacode"  ) ;
    } else                                       {
      Q = SC.sql.Update(PlanTable(Phones)        ,
            "where uuid = :UUID"                 ,
            2,"countrycode","areacode"         ) ;
    }                                            ;
    SC.Prepare ( Q                             ) ;
    SC.Bind    ( "uuid"        , uuid          ) ;
    SC.Bind    ( "countrycode" , ccid          ) ;
    SC.Bind    ( "areacode"    , auid          ) ;
    SC.Exec    (                               ) ;
    ItemEditing->setData(0,Qt::UserRole,auid)    ;
    ItemEditing->setData(1,Qt::UserRole,uuid)    ;
    if (auid>0)                                  {
      QString cp = ccPhones  [ccid]              ;
      QString ap = acPhones  [auid]              ;
      QString cs                                 ;
      cs  = "+"                                  ;
      cs += cp                                   ;
      if (ap.length()>0)                         {
        cs += "-"                                ;
        cs += ap                                 ;
      }                                          ;
      ItemEditing->setText(0,cs)                 ;
    }                                            ;
  LeaveSQL         ( SC , plan->sql          )   ;
  if (uuid>0) PresentNumber(ItemEditing)         ;
  removeOldItem    ( true , 1                )   ;
  Alert            ( Done                    )   ;
}

void N::PhoneLists::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                  ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(line)) return                         ;
  SUID    uuid  = nTreeUuid(ItemEditing,1)         ;
  QString phone = line->text()                     ;
  EnterSQL         ( SC , plan->sql          )     ;
    QString Q                                      ;
    if (isPhone(phone))                            {
      QString ac = ItemEditing->text(0)            ;
      QString full                                 ;
      full = ac + "-" + phone                      ;
      Q = SC.sql.Update                            (
            PlanTable(Phones)                      ,
            "where uuid = :UUID"                   ,
            2,"number","full"                    ) ;
      SC.Prepare ( Q                             ) ;
      SC.Bind    ( "uuid"   , uuid               ) ;
      SC.Bind    ( "number" , phone.toUtf8()     ) ;
      SC.Bind    ( "full"   , full .toUtf8()     ) ;
      SC.Exec    (                               ) ;
      ItemEditing->setText(1,phone)                ;
    }                                              ;
  LeaveSQL         ( SC , plan->sql              ) ;
  Alert            ( Done                        ) ;
  if (uuid>0) PresentNumber(ItemEditing)           ;
  removeOldItem    ( true , 1                    ) ;
}

bool N::PhoneLists::Menu(QPoint pos)
{
  nScopedMenu    ( mm , this )               ;
  QAction * aa                               ;
  DockingMenu    ( mm        )               ;
  mm . setFont   ( plan      )               ;
  aa = mm . exec (           )               ;
  nKickOut ( IsNull     (      aa ) , true ) ;
  nKickOut ( RunDocking ( mm , aa ) , true ) ;
  return true                                ;
}
