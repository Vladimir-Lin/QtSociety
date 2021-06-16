#include <society.h>

N::AreaCode:: AreaCode ( QWidget * parent , Plan * p )
            : TreeDock (           parent ,        p )
{
  Configure ( ) ;
}

N::AreaCode::~AreaCode(void)
{
}

void N::AreaCode::Configure(void)
{
  NewTreeWidgetItem            ( head                   ) ;
  head -> setText              ( 0 , tr("Area"        ) ) ;
  head -> setText              ( 1 , tr("Country code") ) ;
  head -> setText              ( 2 , tr("Code"        ) ) ;
  head -> setText              ( 3 , tr("Number"      ) ) ;
  head -> setText              ( 4 , ""                 ) ;
  setWindowTitle               ( tr("Area codes")       ) ;
  setDragDropMode              ( NoDragDrop             ) ;
  setRootIsDecorated           ( false                  ) ;
  setAlternatingRowColors      ( true                   ) ;
  setSelectionMode             ( SingleSelection        ) ;
  setColumnCount               ( 5                      ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded  ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded  ) ;
  assignHeaderItems            ( head                   ) ;
  MountClicked                 ( 2                      ) ;
  plan -> setFont              ( this                   ) ;
}

bool N::AreaCode::FocusIn(void)
{
  AssignAction ( Label   , windowTitle     ( ) ) ;
  LinkAction   ( Refresh , startup         ( ) ) ;
  LinkAction   ( Insert  , Insert          ( ) ) ;
  LinkAction   ( Delete  , Delete          ( ) ) ;
  LinkAction   ( Copy    , CopyToClipboard ( ) ) ;
  LinkAction   ( Export  , Export          ( ) ) ;
  return true                                    ;
}

bool N::AreaCode::startup(void)
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
    Uuids     = SC.Uuids(PlanTable(AreaCode)    ,
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
    foreach (uuid,Uuids)                        {
      Q = SC.sql.SelectFrom                     (
            "countrycode,area,phone"            ,
            PlanTable(AreaCode)                 ,
            SC.WhereUuid(uuid)                ) ;
      if (SC.Fetch(Q))                          {
        NewTreeWidgetItem(item)                 ;
        SUID    cuid  = SC . Uuid   ( 0 )       ;
        SUID    auid  = SC . Uuid   ( 1 )       ;
        QString phone = SC . String ( 2 )       ;
        item->setData(0,Qt::UserRole,auid)      ;
        item->setData(1,Qt::UserRole,cuid)      ;
        item->setData(2,Qt::UserRole,uuid)      ;
        item->setText(2,phone            )      ;
        item->setTextAlignment                  (
          2,Qt::AlignRight | Qt::AlignVCenter ) ;
        if (cuid>0)                             {
          QString iphone = ccPhones[cuid]       ;
          iphone = QString("+%1").arg(iphone)   ;
          item->setText(1,iphone)               ;
        }                                       ;
        if (auid>0)                             {
          item->setText(0,areaNames[auid])      ;
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

void N::AreaCode::Insert(void)
{
  NewTreeWidgetItem(item)                     ;
  item->setData(0,Qt::UserRole,0)             ;
  item->setData(1,Qt::UserRole,0)             ;
  item->setData(2,Qt::UserRole,0)             ;
  item->setData(3,Qt::UserRole,0)             ;
  item->setTextAlignment(2,Qt::AlignRight     |
                           Qt::AlignVCenter ) ;
  addTopLevelItem(item)                       ;
  doubleClicked(item,2)                       ;
}

void N::AreaCode::Delete(void)
{
  QTreeWidgetItem * item = currentItem()       ;
  if (IsNull(item)) return                     ;
  SUID uuid = nTreeUuid(item,2)                ;
  EnterSQL         ( SC , plan->sql          ) ;
    QString Q                                  ;
    if (uuid>0)                                {
      int index = indexOfTopLevelItem(item)    ;
      Q = SC.sql.DeleteFrom                    (
            PlanTable(AreaCode)                ,
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

void N::AreaCode::Export(void)
{
  QString filename = QFileDialog::getSaveFileName  (
                       this                        ,
                       tr("Save phone area codes") ,
                       plan->Temporary("")         ,
                       tr("Text file (*.txt)")   ) ;
  if (filename.length()<=0) return                 ;
  QFile F(filename)                                ;
  if (!F.open(QIODevice::WriteOnly)) return        ;
  for (int i=0;i<topLevelItemCount();i++)          {
    QTreeWidgetItem * item  = topLevelItem(i)      ;
    SUID auid  = nTreeUuid(item,0)                 ;
    SUID cuid  = nTreeUuid(item,1)                 ;
    SUID uuid  = nTreeUuid(item,2)                 ;
    if (auid>0 && cuid>0 && uuid>0)                {
      QString full  = item->text(3)                ;
      QString area  = item->text(0)                ;
      QString space = " "                          ;
      QString report                               ;
      space = space.repeated(10-full.length())     ;
      report = tr("%1%2 : %3\n"                    )
              .arg(full                            )
              .arg(space                           )
              .arg(area                          ) ;
      F.write(report.toUtf8())                     ;
    }                                              ;
  }                                                ;
  F.close()                                        ;
  Alert ( Done )                                   ;
}

void N::AreaCode::ListCC(QComboBox * combo)
{
  SUID uuid                              ;
  combo -> clear ( )                     ;
  foreach (uuid,ccCodes)                 {
    int     country = rcMaps   [uuid   ] ;
    SUID    cuid    = ccMaps   [country] ;
    QString cname   = ccNames  [cuid   ] ;
    QString phone   = ccPhones [uuid   ] ;
    QString name                         ;
    name = tr("+%1 %2"                   )
           .arg(phone).arg(cname       ) ;
    combo->addItem(name,uuid)            ;
  }                                      ;
}

void N::AreaCode::ListAreas(QComboBox * combo)
{
  SUID uuid                              ;
  combo -> clear ( )                     ;
  foreach (uuid,AreaUuids)               {
    combo->addItem(areaNames[uuid],uuid) ;
  }                                      ;
}

void N::AreaCode::PresentNumber(QTreeWidgetItem * item)
{
  SUID    auid  = nTreeUuid(item,0) ;
  SUID    cuid  = nTreeUuid(item,1) ;
  SUID    uuid  = nTreeUuid(item,2) ;
  QString phone = item->text(2)     ;
  if (uuid<=0) return               ;
  if (auid<=0) return               ;
  if (cuid<=0) return               ;
  QString full                      ;
  full = item->text(1)              ;
  if (full.length()<=0) return      ;
  if (phone.length()>0)             {
    full += "-"                     ;
    full += phone                   ;
  }                                 ;
  item->setText(3,full)             ;
}

bool N::AreaCode::isPhone(QString phone)
{
  for (int i=0;i<phone.length();i++) {
    QChar c = phone.at(0)            ;
    if (!c.isDigit()) return false   ;
  }                                  ;
  return true                        ;
}

void N::AreaCode::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                         ;
  QComboBox * cc                           ;
  SUID        auid = nTreeUuid(item,0)     ;
  SUID        cuid = nTreeUuid(item,1)     ;
  SUID        uuid = nTreeUuid(item,2)     ;
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
      cc   = setComboBox                   (
               item,column                 ,
               SIGNAL(activated(int))      ,
               SLOT(countryFinished(int))) ;
      ListCC(cc)                           ;
      if (cuid>0) (*cc) <= cuid            ;
      cc->setEditable(true)                ;
      cc->showPopup()                      ;
    break                                  ;
    case 2                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (nameFinished   ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
  }                                        ;
}

void N::AreaCode::countryFinished(int)
{
  if (IsNull(ItemEditing)) return                  ;
  QComboBox * cc = Casting(QComboBox,ItemWidget)   ;
  if (IsNull(cc)) return                           ;
  SUID    uuid    = nTreeUuid(ItemEditing,2)       ;
  SUID    cuid    = N::GetUuid(cc)                 ;
  EnterSQL         ( SC , plan->sql              ) ;
    QString Q                                      ;
    if (uuid>0 && cuid>0)                          {
      QString phone                                ;
      phone = QString("+%1").arg(ccPhones[cuid])   ;
      Q = SC.sql.Update(PlanTable(AreaCode)        ,
            "where uuid = :UUID",1,"countrycode" ) ;
      SC . Prepare ( Q                    )        ;
      SC . Bind    ( "uuid"        , uuid )        ;
      SC . Bind    ( "countrycode" , cuid )        ;
      SC . Exec    (                      )        ;
      ItemEditing->setData(1,Qt::UserRole,cuid)    ;
      ItemEditing->setText(1,phone            )    ;
    }                                              ;
  LeaveSQL         ( SC , plan->sql          )     ;
  if (uuid>0) PresentNumber(ItemEditing)           ;
  removeOldItem    ( true , 2                )     ;
  Alert            ( Done                    )     ;
}

void N::AreaCode::areaFinished(int)
{
  if (IsNull(ItemEditing)) return                ;
  QComboBox * cc = Casting(QComboBox,ItemWidget) ;
  if (IsNull(cc)) return                         ;
  SUID    uuid    = nTreeUuid(ItemEditing,2)     ;
  SUID    auid    = N::GetUuid(cc)               ;
  EnterSQL         ( SC , plan->sql            ) ;
    QString Q                                    ;
    if (uuid>0 && auid>0)                        {
      Q = SC.sql.Update(PlanTable(AreaCode)      ,
            "where uuid = :UUID",1,"area"      ) ;
      SC . Prepare ( Q             )             ;
      SC . Bind    ( "uuid" , uuid )             ;
      SC . Bind    ( "area" , auid )             ;
      SC . Exec    (               )             ;
      ItemEditing->setData(0,Qt::UserRole,auid)  ;
      ItemEditing->setText(0,cc->currentText())  ;
    }                                            ;
  LeaveSQL         ( SC , plan->sql          )   ;
  if (uuid>0) PresentNumber(ItemEditing)         ;
  removeOldItem    ( true , 2                )   ;
  Alert            ( Done                    )   ;
}

void N::AreaCode::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                     ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget)    ;
  if (IsNull(line)) return                            ;
  SUID    uuid  = nTreeUuid(ItemEditing,2)            ;
  QString phone = line->text()                        ;
  EnterSQL         ( SC , plan->sql          )        ;
    QString Q                                         ;
    if (uuid>0 && isPhone(phone))                     {
      Q = SC.sql.Update                               (
            PlanTable(AreaCode)                       ,
            "where uuid = :UUID"                      ,
            1,"phone"                               ) ;
      SC.Prepare ( Q                        )         ;
      SC.Bind    ( "uuid"  , uuid           )         ;
      SC.Bind    ( "phone" , phone.toUtf8() )         ;
      SC.Exec    (                          )         ;
      ItemEditing->setText(2,phone)                   ;
      Alert      ( Done                     )         ;
    } else                                            {
      if (isPhone(phone))                             {
        uuid = SC.Unique(PlanTable(MajorUuid)         ,
                         "uuid",9127                ) ;
        SC.assureUuid(PlanTable(MajorUuid)            ,
                      uuid,Types::AreaCode) ;
        Q = SC.sql.InsertInto(PlanTable(AreaCode)     ,
                              2,"uuid","phone"      ) ;
        SC.Prepare ( Q                        )       ;
        SC.Bind    ( "uuid"  , uuid           )       ;
        SC.Bind    ( "phone" , phone.toUtf8() )       ;
        SC.Exec    (                          )       ;
        ItemEditing->setData(2,Qt::UserRole,uuid)     ;
        ItemEditing->setText(2,phone)                 ;
        Alert ( Done  )                               ;
      } else                                          {
        Alert ( Error )                               ;
      }                                               ;
    }                                                 ;
  LeaveSQL         ( SC , plan->sql          )        ;
  if (uuid>0) PresentNumber(ItemEditing)              ;
  removeOldItem    ( true , 2                )        ;
}

bool N::AreaCode::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                     ;
  QMdiSubWindow * mdi    = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget   * dock   = Casting ( QDockWidget   , parent() ) ;
  QAction    * aa                                               ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))  ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))  ;
  mm.setFont ( plan )                                           ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
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
