/*==============================================================*/
/* DBMS name:      Microsoft SQL Server 2008                    */
/* Created on:     2016/11/23 15:06:29                          */
/*==============================================================*/


if exists (select 1
            from  sysobjects
           where  id = object_id('Dic_CheckType')
            and   type = 'U')
   drop table Dic_CheckType
go

if exists (select 1
            from  sysobjects
           where  id = object_id('Dic_DataType')
            and   type = 'U')
   drop table Dic_DataType
go

if exists (select 1
            from  sysobjects
           where  id = object_id('Dic_PowerType')
            and   type = 'U')
   drop table Dic_PowerType
go

/*==============================================================*/
/* Table: Dic_CheckType                                         */
/*==============================================================*/
create table Dic_CheckType (
   code                 int                  not null,
   describe             varchar(30)          null,
   remark               varchar(120)         null,
   constraint PK_DIC_CHECKTYPE primary key (code)
)
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '����У������',
   'user', @CurrentUser, 'table', 'Dic_CheckType'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   'У����',
   'user', @CurrentUser, 'table', 'Dic_CheckType', 'column', 'code'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '����',
   'user', @CurrentUser, 'table', 'Dic_CheckType', 'column', 'describe'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '��ע',
   'user', @CurrentUser, 'table', 'Dic_CheckType', 'column', 'remark'
go

/*==============================================================*/
/* Table: Dic_DataType                                          */
/*==============================================================*/
create table Dic_DataType (
   code                 int                  not null,
   describe             varchar(30)          null,
   remark               varchar(120)         null,
   constraint PK_DIC_DATATYPE primary key (code)
)
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '�������ְ�ͷ�����ݺ�У��',
   'user', @CurrentUser, 'table', 'Dic_DataType'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '������',
   'user', @CurrentUser, 'table', 'Dic_DataType', 'column', 'code'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '����',
   'user', @CurrentUser, 'table', 'Dic_DataType', 'column', 'describe'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '��ע',
   'user', @CurrentUser, 'table', 'Dic_DataType', 'column', 'remark'
go

/*==============================================================*/
/* Table: Dic_PowerType                                         */
/*==============================================================*/
create table Dic_PowerType (
   code                 int                  not null,
   describe             varchar(30)          null,
   remark               varchar(120)         null,
   constraint PK_DIC_POWERTYPE primary key (code)
)
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '��Ϊֻ������д����Ȩ������',
   'user', @CurrentUser, 'table', 'Dic_PowerType'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '������',
   'user', @CurrentUser, 'table', 'Dic_PowerType', 'column', 'code'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '����',
   'user', @CurrentUser, 'table', 'Dic_PowerType', 'column', 'describe'
go

declare @CurrentUser sysname
select @CurrentUser = user_name()
execute sp_addextendedproperty 'MS_Description', 
   '��ע',
   'user', @CurrentUser, 'table', 'Dic_PowerType', 'column', 'remark'
go

