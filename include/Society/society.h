/****************************************************************************
 *                                                                          *
 * Copyright (C) 2001~2016 Neutrino International Inc.                      *
 *                                                                          *
 * Author : Brian Lin <lin.foxman@gmail.com>, Skype: wolfram_lin            *
 *                                                                          *
 ****************************************************************************/

#ifndef QT_SOCIETY_H
#define QT_SOCIETY_H

#include <QtPhotography>
#include <DataWidgets>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#    if defined(QT_BUILD_SOCIETY_LIB)
#      define Q_SOCIETY_EXPORT Q_DECL_EXPORT
#    else
#      define Q_SOCIETY_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define Q_SOCIETY_EXPORT
#endif

namespace N
{

class Q_SOCIETY_EXPORT NationLists             ;
class Q_SOCIETY_EXPORT NationEditor            ;
class Q_SOCIETY_EXPORT AdministrativeDivisions ;
class Q_SOCIETY_EXPORT CountryCode             ;
class Q_SOCIETY_EXPORT AreaCode                ;
class Q_SOCIETY_EXPORT PhoneLists              ;
class Q_SOCIETY_EXPORT ComityEdit              ;
class Q_SOCIETY_EXPORT ProfessionLists         ;
class Q_SOCIETY_EXPORT OrganizationEdit        ;
class Q_SOCIETY_EXPORT Kinships                ;
class Q_SOCIETY_EXPORT IpaEditor               ;
class Q_SOCIETY_EXPORT CultureLists            ;
class Q_SOCIETY_EXPORT HistoryTypes            ;

// To be obsoleted, used in PeopleEditor or AlbumEditor
class Q_SOCIETY_EXPORT OrganizationProcessor   ;

/*****************************************************************************
 *                                                                           *
 *                              Geographic widgets                           *
 *                                                                           *
 *****************************************************************************/

class Q_SOCIETY_EXPORT NationLists : public TreeDock
{
  Q_OBJECT
  public:

    explicit NationLists           (StandardConstructor) ;
    virtual ~NationLists           (void) ;

  protected:

    QPoint dragPoint ;

    virtual bool FocusIn           (void) ;
    virtual void Configure         (void) ;

    virtual void setCheckable      (QTreeWidgetItem * item) ;

    virtual bool        hasItem    (void);
    virtual bool        startDrag  (QMouseEvent * event);
    virtual bool        fetchDrag  (QMouseEvent * event);
    virtual QMimeData * dragMime   (void);
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag (QMouseEvent * event);

    virtual bool acceptDrop        (QWidget * source,const QMimeData * mime);
    virtual bool dropNew           (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropPeople        (nDeclDrops,const UUIDs & Uuids ) ;
    virtual bool dropPictures      (nDeclDrops,const UUIDs & Uuids ) ;

    virtual bool JoinPeople        (SUID uuid,const UUIDs & Uuids) ;
    virtual bool JoinPictures      (SUID uuid,const UUIDs & Uuids) ;

    virtual void run               (int Type,ThreadData * data) ;

    virtual void JoinPeople        (ThreadData * data) ;
    virtual void JoinPictures      (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup           (void) ;
    virtual bool List              (void) ;
    virtual void Export            (void) ;
    virtual void Copy              (void) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;

  private slots:

  signals:

    void Gallery                   (QString name,SUID uuid) ;
    void ListPeople                (QString name,SUID uuid,int type) ;
    void ListPeople                (QString name,UUIDs & Uuids) ;
    void Translations              (QString name,UUIDs & Uuids) ;
    void FullNames                 (SUID uuid) ;

} ;

class Q_SOCIETY_EXPORT NationEditor : public TreeWidget
{
  Q_OBJECT
  public:

    enum  NationEditorMsgIDs           {
          NationMsgItems    = 12124001 ,
          NationMsgPeople   = 12124002 ,
          NationMsgPictures = 12124003 ,
    } ;

    explicit     NationEditor      (StandardConstructor) ;
    virtual     ~NationEditor      (void) ;

  protected:

    QPoint dragPoint ;

    virtual void Configure         (void) ;
    virtual bool FocusIn           (void) ;

    virtual bool        hasItem    (void);
    virtual bool        startDrag  (QMouseEvent * event);
    virtual bool        fetchDrag  (QMouseEvent * event);
    virtual QMimeData * dragMime   (void);
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag (QMouseEvent * event);

    virtual bool acceptDrop        (QWidget * source,const QMimeData * mime);
    virtual bool dropNew           (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropPeople        (nDeclDrops,const UUIDs & Uuids ) ;
    virtual bool dropPictures      (nDeclDrops,const UUIDs & Uuids ) ;

    virtual void run               (int Type,ThreadData * data) ;

    virtual void ListItem          (SqlConnection   & SC     ,
                                    QTreeWidgetItem * item   ,
                                    SUID              uuid ) ;
    virtual void List              (ThreadData * data) ;
    virtual void DropIn            (ThreadData * data) ;
    virtual void Iso3166           (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup           (void) ;

    virtual void Insert            (void) ;

  protected slots:

    virtual void singleClicked     (QTreeWidgetItem *item,int column) ;
    virtual void doubleClicked     (QTreeWidgetItem *item,int column) ;

    virtual bool Menu              (QPoint pos) ;

    virtual void nameFinished      (void) ;
    virtual void twoFinished       (void) ;
    virtual void threeFinished     (void) ;
    virtual void numericFinished   (void) ;
    virtual void codeFinished      (void) ;
    virtual void positionFinished  (void) ;
    virtual void shortFinished     (void) ;
    virtual void englishFinished   (void) ;
    virtual void nativeFinished    (void) ;

  private slots:

  signals:

    void Gallery                   (QString name,SUID uuid) ;
    void ListPeople                (QString name,SUID uuid,int type) ;
    void Translations              (QString name,UUIDs & Uuids) ;
    void FullNames                 (SUID uuid) ;

} ;

class Q_SOCIETY_EXPORT AdministrativeDivisions : public ItemEditor
{
  Q_OBJECT
  public:

    explicit AdministrativeDivisions  (StandardConstructor) ;
    virtual ~AdministrativeDivisions  (void);

  protected:

    virtual void Configure            (void) ;

  private:

  public slots:

  protected slots:

  private slots:

  signals:

} ;

class Q_SOCIETY_EXPORT CountryCode : public TreeDock
{
  Q_OBJECT
  public:

    explicit     CountryCode     (StandardConstructor) ;
    virtual     ~CountryCode     (void);

  protected:

    UUIDs Countries ;
    NAMEs ccNames   ;
    CMAPs ccMaps    ;
    RMAPs rrMaps    ;

    virtual void Configure       (void) ;
    virtual bool FocusIn         (void) ;

    virtual void ListCountries   (QComboBox * combo) ;
    virtual bool isPhone         (QString phone) ;

    virtual void run             (int Type, ThreadData * data) ;
    virtual void List            (ThreadData * data) ;
    virtual void Delete          (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup         (void) ;
    virtual void Export          (void) ;
    virtual void Insert          (void) ;
    virtual void Delete          (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void countryFinished (int index) ;
    virtual void nameFinished    (void) ;

  private slots:

  signals:

    void         Translations    (QString title,UUIDs & Uuids);
    void         FullNames       (SUID uuid) ;

} ;

class Q_SOCIETY_EXPORT AreaCode : public TreeDock
{
  Q_OBJECT
  public:

    explicit AreaCode            (StandardConstructor) ;
    virtual ~AreaCode            (void);

  protected:

    UUIDs AreaUuids ;
    UUIDs ccCodes   ;
    UUIDs Countries ;
    NAMEs areaNames ;
    NAMEs ccNames   ;
    NAMEs ccPhones  ;
    CMAPs ccMaps    ;
    RMAPs rrMaps    ;
    RMAPs rcMaps    ;

    virtual void Configure       (void) ;
    virtual bool FocusIn         (void) ;

    void ListCC                  (QComboBox * combo) ;
    void ListAreas               (QComboBox * combo) ;
    bool isPhone                 (QString phone) ;
    void PresentNumber           (QTreeWidgetItem * item) ;

  private:

  public slots:

    virtual bool startup         (void) ;
    virtual void Export          (void) ;
    virtual void Insert          (void) ;
    virtual void Delete          (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void areaFinished    (int index) ;
    virtual void countryFinished (int index) ;
    virtual void nameFinished    (void) ;

  private slots:

  signals:

    void Translations          (QString title,UUIDs & Uuids);
    void FullNames             (SUID uuid) ;

} ;

class Q_SOCIETY_EXPORT PhoneLists : public TreeDock
                                  , public Group
{
  Q_OBJECT
  public:

    explicit PhoneLists        (StandardConstructor);
    virtual ~PhoneLists        (void);

  protected:

    UUIDs AreaUuids ;
    UUIDs ccCodes   ;
    UUIDs acCodes   ;
    UUIDs Countries ;
    NAMEs areaNames ;
    NAMEs ccNames   ;
    NAMEs ccPhones  ;
    NAMEs acPhones  ;
    CMAPs ccMaps    ;
    RMAPs rrMaps    ;
    RMAPs rcMaps    ;
    SMAPs acCountry ;
    SMAPs acArea    ;

    virtual void Configure     (void) ;
    virtual bool FocusIn       (void) ;

    void ListAreas             (QComboBox * combo) ;
    bool isPhone               (QString phone) ;
    void PresentNumber         (QTreeWidgetItem * item) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void Export        (void) ;
    virtual void Insert        (void) ;
    virtual void Delete        (void) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;
    virtual void areaFinished  (int index) ;
    virtual void nameFinished  (void) ;

  private slots:

  signals:

} ;

/*****************************************************************************
 *                                                                           *
 *                                Society widgets                            *
 *                                                                           *
 *****************************************************************************/

class Q_SOCIETY_EXPORT ComityEdit : public ItemEditor
{
  Q_OBJECT
  public:

    explicit ComityEdit            (StandardConstructor) ;
    virtual ~ComityEdit            (void);

  protected:

    virtual void Configure         (void) ;

    virtual bool dropOrganizations (nDeclWidget,QPointF pos,const UUIDs & Uuids) ;

  private:

  public slots:

  protected slots:

  private slots:

  signals:

} ;

class Q_SOCIETY_EXPORT ProfessionLists : public ItemEditor
{
  Q_OBJECT
  public:

    explicit ProfessionLists (StandardConstructor) ;
    virtual ~ProfessionLists (void) ;

  protected:

    virtual void Configure   (void) ;

  private:

  public slots:

  protected slots:

  private slots:

  signals:

} ;

class Q_SOCIETY_EXPORT OrganizationEdit : public TreeDock
                                        , public Group
{
  Q_OBJECT
  public:

    explicit            OrganizationEdit  (StandardConstructor) ;
    virtual            ~OrganizationEdit  (void);

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual void        run               (int Type,ThreadData * data) ;

    virtual bool        FocusIn           (void) ;
    virtual void        Configure         (void) ;

    virtual bool        hasItem           (void) ;
    virtual bool        startDrag         (QMouseEvent * event) ;
    virtual bool        fetchDrag         (QMouseEvent * event) ;
    virtual QMimeData * dragMime          (void) ;
    virtual void        dragDone          (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag        (QMouseEvent * event) ;

    virtual bool        acceptDrop        (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew           (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool        dropText          (nDeclDrops,const QString & text  ) ;
    virtual bool        dropPeople        (nDeclDrops,const UUIDs   & Uuids ) ;
    virtual bool        dropDocuments     (nDeclDrops,const UUIDs   & Uuids ) ;
    virtual bool        dropCommodities   (nDeclDrops,const UUIDs   & Uuids ) ;
    virtual bool        dropOrganizations (nDeclDrops,const UUIDs   & Uuids ) ;

    virtual UUIDs       LoadUuids         (SqlConnection & Connection) ;
    virtual SUID        AppendUuid        (SqlConnection & Connection);
    virtual void        LoadNames         (SqlConnection & Connection,UUIDs & Uuids) ;
    virtual bool        AppendName        (SqlConnection & Connection,SUID uuid,QString name);

    virtual void        ListOrganizations (ThreadData * data) ;
    virtual void        Delete            (ThreadData * data) ;
    virtual void        DropIn            (ThreadData * data) ;

  private:

  public slots:

    virtual bool        List              (void) ;
    virtual void        New               (void) ;
    virtual void        Delete            (void) ;
    virtual void        Paste             (void) ;
    virtual void        Paste             (QString text);
    virtual void        Export            (void) ;
    virtual void        Copy              (void) ;
    virtual void        CopyIt            (void) ;

  protected slots:

    virtual bool        Menu              (QPoint pos) ;
    virtual void        doubleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void        editingFinished   (void) ;

  private slots:

    virtual void        ArrangeItems      (void) ;

  signals:

    void                arrangeItems      (void) ;
    void                Trading           (QString name,SUID uuid) ;
    void                Cargo             (QString name,SUID uuid) ;
    void                Documents         (QString name,SUID uuid) ;
    void                Commodities       (QString name,SUID uuid) ;
    void                People            (QString name,SUID uuid) ;
    void                EditNames         (QString name,SUID uuid) ;
    void                Bookmarks         (QString name,SUID uuid) ;
    void                Edit              (QString name,SUID uuid) ;
    void                Subsidiaries      (QString name,SUID uuid,int type) ;
    void                Translations      (QString title,UUIDs & Uuids);

} ;

// To be obsoleted
class Q_SOCIETY_EXPORT OrganizationProcessor : public InteractiveEditor
                                             , public Object
{
  Q_OBJECT
  public:

    explicit OrganizationProcessor (StandardConstructor) ;
    virtual ~OrganizationProcessor (void) ;

  protected:

    virtual void Configure         (void) ;

    virtual bool Interpret         (QString command) ;
    virtual void run               (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual void Enter             (void) ;

    virtual void Help              (void) ;

  protected slots:

  private slots:

  signals:

} ;

class Q_SOCIETY_EXPORT Kinships : public TreeDock
{
  Q_OBJECT
  public:

    explicit Kinships (StandardConstructor) ;
    virtual ~Kinships (void);

  protected:

    UUIDs Genders         ;
    UUIDs Relations       ;
    NAMEs Sexualities     ;
    NAMEs Consanguinities ;

    virtual void Configure             (void) ;
    virtual bool FocusIn               (void) ;

    void ListGender                    (QComboBox * combo) ;
    void ListConsanguinities           (QComboBox * combo) ;

  private:

  public slots:

    virtual bool startup               (void) ;
    virtual void Export                (void) ;
    virtual void Insert                (void) ;

  protected slots:

    virtual bool Menu                  (QPoint pos) ;
    virtual void doubleClicked         (QTreeWidgetItem * item,int column) ;
    virtual void genderFinished        (int index) ;
    virtual void consanguinityFinished (int index) ;
    virtual void nameFinished          (void) ;

  private slots:

  signals:

    void Translations          (QString title,UUIDs & Uuids);
    void FullNames             (SUID uuid) ;

} ;

// International Phonetic Alphabet
class Q_SOCIETY_EXPORT IpaEditor : public TreeWidget
{
  Q_OBJECT
  public:

    explicit IpaEditor           (StandardConstructor) ;
    virtual ~IpaEditor           (void) ;

    virtual QSize SuitableSize   (void) ;

  protected:

    virtual bool FocusIn         (void) ;
    virtual void Configure       (void) ;

    QString    CharUnicode       (ushort  schar ) ;
    QString    SymbolUnicode     (QString symbol) ;

  private:

  public slots:

    virtual void List            (void) ;
    virtual void New             (void) ;
    virtual void Rename          (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;

    virtual void symbolChanged   (void) ;
    virtual void nameChanged     (void) ;
    virtual void removeOldItem   (void) ;

    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

} ;

class Q_SOCIETY_EXPORT CultureLists : public TreeWidget
{
  Q_OBJECT
  public:

    explicit CultureLists        (StandardConstructor) ;
    virtual ~CultureLists        (void) ;

    virtual QSize sizeHint       (void) const ;

  protected:

    enum Qt::SortOrder sorting ;

    virtual void Configure       (void) ;
    virtual bool FocusIn         (void) ;

    virtual void run             (int type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup         (void) ;
    virtual void List            (void) ;
    virtual void New             (void) ;
    virtual void Rename          (void) ;
    virtual void Paste           (void) ;
    virtual void Search          (void) ;
    virtual void Translation     (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void singleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;

    virtual void removeOldItem   (void) ;
    virtual void editingFinished (void) ;

  private slots:

  signals:

    void Translations          (QString title,UUIDs & Uuids);
    void FullNames             (SUID uuid) ;

} ;

class Q_SOCIETY_EXPORT HistoryTypes : public TreeWidget
                                    , public HistoryManager
{
  Q_OBJECT
  public:

    explicit HistoryTypes        (StandardConstructor) ;
    virtual ~HistoryTypes        (void) ;

  protected:

    enum Qt::SortOrder sorting ;

    virtual void Configure       (void) ;
    virtual bool FocusIn         (void) ;

    virtual void run             (int type,ThreadData * data) ;

    virtual void List            (ThreadData * data) ;
    virtual void Insert          (ThreadData * data) ;

    virtual bool assureItem      (QString           name     ,
                                  QTreeWidgetItem * item     ,
                                  int               column ) ;

  private:

  public slots:

    virtual bool startup         (void) ;
    virtual void List            (void) ;
    virtual void New             (void) ;
    virtual void Rename          (void) ;
    virtual void Paste           (void) ;
    virtual void Search          (void) ;
    virtual void Translation     (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;

    virtual void removeOldItem   (void) ;
    virtual void singleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void editingFinished (void) ;
    virtual void EditNames       (QTreeWidgetItem * item) ;

  private slots:

  signals:

    void         Translations    (QString title,UUIDs & Uuids) ;
    void         FullNames       (SUID uuid) ;

} ;

}

QT_END_NAMESPACE

#endif
