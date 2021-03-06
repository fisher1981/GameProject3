﻿#include "stdafx.h"
#include "DBInterface/CppMysql.h"
#include "Guild.h"
#include "GameService.h"
#include "DataPool.h"


CGuild::CGuild()
{
	m_pGuildData = NULL;
	m_u64LeaderID = 0;
}

CGuild::~CGuild()
{
	if(m_pGuildData != NULL)
	{
		m_pGuildData->release();
	}
}

BOOL CGuild::Init()
{
	return TRUE;
}

BOOL CGuild::LoadGuildMember(CppMySQLQuery& QueryResult)
{
	UINT64 uRoleID = QueryResult.getInt64Field("roleid");

	MemberDataObject* pMemberObject = g_pMemberDataObjectPool->NewObject(FALSE);
	pMemberObject->m_uRoleID = uRoleID;
	pMemberObject->m_uGuildID = m_pGuildData->m_uGuid;
	pMemberObject->m_Pos = QueryResult.getIntField("pose");
	pMemberObject->m_dwJoinTime = QueryResult.getInt64Field("join_time");

	m_mapMemberData.insert(std::make_pair(uRoleID, pMemberObject));

	if (pMemberObject->m_Pos == EGP_LEADER)
	{
		m_u64LeaderID = uRoleID;
	}

	return TRUE;
}

CHAR* CGuild::GetGuildName()
{
	return m_pGuildData->m_szName;
}

UINT64 CGuild::GetGuildID()
{
	return m_pGuildData->m_uGuid;
}

MemberDataObject* CGuild::GetGuildMember(UINT64 uID)
{
	auto itor = m_mapMemberData.find(uID);
	if(itor == m_mapMemberData.end())
	{
		return NULL;
	}

	return itor->second;
}

MemberDataObject* CGuild::GetLeader()
{
	if (m_u64LeaderID == 0)
	{
		return NULL;
	}

	auto itor = m_mapMemberData.find(m_u64LeaderID);
	if (itor == m_mapMemberData.end())
	{
		return NULL;
	}

	return itor->second;
}

MemberDataObject* CGuild::AddGuildMember(UINT64 uRoleID)
{
	MemberDataObject* pMemberObject = g_pMemberDataObjectPool->NewObject(TRUE);
	pMemberObject->lock();
	pMemberObject->m_uRoleID = uRoleID;
	pMemberObject->m_uGuildID = m_pGuildData->m_uGuid;
	pMemberObject->m_Pos = EGP_MEMBER;
	pMemberObject->m_dwJoinTime = CommonFunc::GetCurrTime();
	pMemberObject->unlock();

	m_mapMemberData.insert(std::make_pair(uRoleID, pMemberObject));

	return pMemberObject;
}
