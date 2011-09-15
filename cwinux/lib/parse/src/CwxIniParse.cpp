#include "CwxIniParse.h"

CWINUX_BEGIN_NAMESPACE
/**
*@brief  加载配置文件.
*@param [in] strFile 配置文件名。
*@return false：失败；true：成功.
*/ 
bool CwxIniParse::load(string const& strFile){
	//清空属性
	{
		map<string, list<pair<string, string> >*>::iterator iter = m_attrs.begin();
		while(iter != m_attrs.end()){
			delete iter->second;
			iter++;
		}
		m_attrs.clear();
	}
	string line;
	FILE*  fd = fopen(strFile.c_str(), "rb");
	if (!fd){
		CwxCommon::snprintf(m_szErrMsg, 511, "Failure to open conf file:%s, errno=%d", strFile.c_str(), errno);
		return false;
	}
	bool ret = true;
	string strSection;
	string strKey;
	string strValue;
	list<pair<string, string> >* pAttr=NULL;
	while((ret=CwxFile::readTxtLine(fd, line))==true){
		if (line.empty()) break;
		CwxCommon::trim(line);
		if (!line.length()) continue;
		if ('#' == line[0]) continue;
		if (('['==line[0])&&(']'==line[line.length()-1])){
			strSection = line.substr(1, line.length()-2);
			CwxCommon::trim(strSection);
			if (m_attrs.find(strSection) != m_attrs.end()){
				fclose(fd);
				CwxCommon::snprintf(m_szErrMsg, 511, "Section[%s] is duplicate.", strSection.c_str());
				return false;
			}
			pAttr = new list<pair<string, string> >;
			m_attrs[strSection] = pAttr;
			continue;
		}
		if (!pAttr){
			fclose(fd);
			CwxCommon::snprintf(m_szErrMsg, 511, "Property[%s] is in secion.", line.c_str());
			return false;
		}
		if (line.find('=')==string::npos){
			fclose(fd);
			CwxCommon::snprintf(m_szErrMsg, 511, "Property[%s] isn't key=value format.", line.c_str());
			return false;
		}
		if ('=' == line[0]){
			fclose(fd);
			CwxCommon::snprintf(m_szErrMsg, 511, "Property[%s]'s key is empty.", line.c_str());
			return false;
		}
		strKey = line.substr(0, string::npos);
		strValue = line.substr(string::npos+1);
		CwxCommon::trim(strKey);
		CwxCommon::trim(strValue);
		if (!strKey.length()){
			fclose(fd);
			CwxCommon::snprintf(m_szErrMsg, 511, "Property[%s]'s key is empty.", line.c_str());
			return false;
		}
		pAttr->push_back(pair<string, string>(strKey, strValue));
	}
	fclose(fd);
	if (!ret){
		CwxCommon::snprintf(m_szErrMsg, 511, "Failure to open conf file:%s, errno=%d", strFile.c_str(), errno);
		return false;
	}
	return true;
}

/**
*@brief  获取配置中所有的Section。
*@param [out] sections 配置中的section。
*@return void.
*/ 
void CwxIniParse::getSections(set<string>& sections) const
{
	sections.clear();
	map<string, list<pair<string, string> >*>::const_iterator iter = m_attrs.begin();
	while(iter != m_attrs.end()){
		sections.insert(iter->first);
		iter++;
	}
}

/**
*@brief  获取一个section下的所有属性
*@param [in] strSection section的名字。
*@param [out] attr下的所有属性及其value。
*@return false：section不存在；true：section存在.
*/ 
bool CwxIniParse::getAttr(string const& strSection,
				 list<pair<string, string> >& attr) const
{
	map<string, list<pair<string, string> >*>::const_iterator iter = m_attrs.find(strSection);
	if (iter == m_attrs.end()) return false;
	attr.clear();
	attr = *iter->second;
	return true;
}
/**
*@brief  获取一个字符串型属性值.
*@param [in] strSection section的名字。
*@param [in] strAttr 属性名。
*@param [out] strValue 属性值。
*@return false：不存在；true：存在.
*/ 
bool CwxIniParse::getAttr(string const& strSection,
				 string const& strAttr,
				 string& strValue) const
{
	map<string, list<pair<string, string> >*>::const_iterator iter = m_attrs.find(strSection);
	if (iter == m_attrs.end()) return false;
	list<pair<string, string> >::const_iterator attr_iter = iter->second->begin();
	while(attr_iter != iter->second->end())
	{
		if (attr_iter->first == strAttr)
		{
			strValue = attr_iter->second;
			return true;
		}
		attr_iter++;
	}
	return false;
}


CWINUX_END_NAMESPACE
