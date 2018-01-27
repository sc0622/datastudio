/*==============================================================*/
/* DBMS name:      Microsoft SQL Server 2008                    */
/* Created on:     2017/8/2 10:40:27                            */
/*==============================================================*/


if exists (select 1
   from sys.sysreferences r join sys.sysobjects o on (o.id = r.constid and o.type = 'F')
   where r.fkeyid = object_id('ICDSystem') and o.name = 'FK_ICDSYSTE_REFERENCE_ICDPLANE')
alter table ICDSystem
   drop constraint FK_ICDSYSTE_REFERENCE_ICDPLANE
go

if exists (select 1
   from sys.sysreferences r join sys.sysobjects o on (o.id = r.constid and o.type = 'F')
   where r.fkeyid = object_id('UserPower') and o.name = 'FK_USERPOWE_REFERENCE_ICDLEAF')
alter table UserPower
   drop constraint FK_USERPOWE_REFERENCE_ICDLEAF
go

if exists (select 1
            from  sysobjects
           where  id = object_id('ICDLeaf')
            and   type = 'U')
   drop table ICDLeaf
go

if exists (select 1
            from  sysobjects
           where  id = object_id('ICDPlane')
            and   type = 'U')
   drop table ICDPlane
go

if exists (select 1
            from  sysobjects
           where  id = object_id('ICDSystem')
            and   type = 'U')
   drop table ICDSystem
go

if exists (select 1
            from  sysobjects
           where  id = object_id('UserPower')
            and   type = 'U')
   drop table UserPower
go

/*==============================================================*/
/* Table: ICDLeaf                                               */
/*==============================================================*/
create table ICDLeaf (
   tableName            varchar(42)          not null,
   ptName               varchar(42)          null,
   id                   varchar(32)          null,
   code                 varchar(30)          null,
   length               int                  null,
   bGroup               varchar(40)          null,
   describe             varchar(120)         null,
   remark               varchar(200)         null,
   constraint PK_ICDLEAF primary key (tableName)
)
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '用于组建分系统与ICD表的树形关系图，于具体的ICD表则以“飞机型号/所属组”的形式进行区分',
   'user', @CurrentUser, 'table', 'ICDLeaf'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '表名',
   'user', @CurrentUser, 'table', 'ICDLeaf', 'column', 'tableName'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '父表名',
   'user', @CurrentUser, 'table', 'ICDLeaf', 'column', 'ptName'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '编号',
   'user', @CurrentUser, 'table', 'ICDLeaf', 'column', 'id'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '编码',
   'user', @CurrentUser, 'table', 'ICDLeaf', 'column', 'code'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '长度',
   'user', @CurrentUser, 'table', 'ICDLeaf', 'column', 'length'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '界面顺序',
   'user', @CurrentUser, 'table', 'ICDLeaf', 'column', 'uiLevel'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '所属组',
   'user', @CurrentUser, 'table', 'ICDLeaf', 'column', 'bGroup'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '描述',
   'user', @CurrentUser, 'table', 'ICDLeaf', 'column', 'describe'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '备注',
   'user', @CurrentUser, 'table', 'ICDLeaf', 'column', 'remark'
go

/*==============================================================*/
/* Table: ICDPlane                                              */
/*==============================================================*/
create table ICDPlane (
   code                 int                  not null,
   describe             varchar(30)          null,
   sign                 varchar(30)          null,
   remark               varchar(200)         null,
   constraint PK_ICDPLANE primary key (code)
)
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '机型',
   'user', @CurrentUser, 'table', 'ICDPlane'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '机型码',
   'user', @CurrentUser, 'table', 'ICDPlane', 'column', 'code'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '描述',
   'user', @CurrentUser, 'table', 'ICDPlane', 'column', 'describe'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '标志',
   'user', @CurrentUser, 'table', 'ICDPlane', 'column', 'sign'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '备注',
   'user', @CurrentUser, 'table', 'ICDPlane', 'column', 'remark'
go

/*==============================================================*/
/* Table: ICDSystem                                             */
/*==============================================================*/
create table ICDSystem (
   typeCode             int                  not null,
   systemCode           int                  not null,
   describe             varchar(30)          null,
   sign                 varchar(30)          null,
   remark               varchar(200)         null,
   constraint PK_ICDSYSTEM primary key (typeCode, systemCode)
)
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '系统表',
   'user', @CurrentUser, 'table', 'ICDSystem'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '机型码',
   'user', @CurrentUser, 'table', 'ICDSystem', 'column', 'typeCode'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '系统码',
   'user', @CurrentUser, 'table', 'ICDSystem', 'column', 'systemCode'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '描述',
   'user', @CurrentUser, 'table', 'ICDSystem', 'column', 'describe'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '标志',
   'user', @CurrentUser, 'table', 'ICDSystem', 'column', 'sign'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '备注',
   'user', @CurrentUser, 'table', 'ICDSystem', 'column', 'remark'
go

/*==============================================================*/
/* Table: UserPower                                             */
/*==============================================================*/
create table UserPower (
   userName             varchar(40)          not null,
   tableName            varchar(42)          not null,
   powerType            int                  null,
   constraint PK_USERPOWER primary key (userName)
)
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '用户对分系统所有权限',
   'user', @CurrentUser, 'table', 'UserPower'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '用户名',
   'user', @CurrentUser, 'table', 'UserPower', 'column', 'userName'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '表名',
   'user', @CurrentUser, 'table', 'UserPower', 'column', 'tableName'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '权限类型',
   'user', @CurrentUser, 'table', 'UserPower', 'column', 'powerType'
go

alter table ICDSystem
   add constraint FK_ICDSYSTE_REFERENCE_ICDPLANE foreign key (typeCode)
      references ICDPlane (code)
go

alter table UserPower
   add constraint FK_USERPOWE_REFERENCE_ICDLEAF foreign key (tableName)
      references ICDLeaf (tableName)
go

