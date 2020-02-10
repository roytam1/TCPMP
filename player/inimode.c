/*****************************************************************************
 *
 * This program is free software ; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common/common.h"
#include "inimode.h"

#define HKEY_WRAPPER_KEY            (( HKEY ) (ULONG_PTR)((LONG)0x80000010) )

#define FNC_LOADLIBRARY               0
#define FNC_REGCLOSEKEY               1
#define FNC_REGCONNECTREGISTRY        2
#define FNC_REGCREATEKEYEX            3
#define FNC_REGDELETEKEY              4
#define FNC_REGDELETEVALUE            5
#define FNC_REGENUMKEYEX              6
#define FNC_REGENUMVALUE              7
#define FNC_REGFLUSHKEY               8
#define FNC_REGLOADKEY                9
#define FNC_REGOPENKEYEX             10
#define FNC_REGQUERYINFOKEY          11
#define FNC_REGQUERYMULTIPLEVALUES   12
#define FNC_REGQUERYVALUEEX          13
#define FNC_REGREPLACEKEY            14
#define FNC_REGSAVEKEY               15
#define FNC_REGSETVALUEEX            16
#define FNC_REGUNLOADKEY             17

static HMODULE _stdcall Inimode_LoadLibrary(LPCTSTR lpFileName);
static LONG _stdcall Inimode_RegCloseKey(HKEY hKey);
static LONG _stdcall Inimode_RegConnectRegistry(LPCTSTR lpMachineName,HKEY hKey,PHKEY phkResult);
static LONG _stdcall Inimode_RegCreateKeyEx( HKEY hKey, LPCTSTR lpSubKey, DWORD Reserved, LPTSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition );
static LONG _stdcall Inimode_RegDeleteKey( HKEY hKey, LPCTSTR lpSubKey );
static LONG _stdcall Inimode_RegDeleteValue( HKEY hKey, LPCTSTR lpValueName );
static LONG _stdcall Inimode_RegEnumKeyEx( HKEY hKey, DWORD dwIndex, LPTSTR lpName, LPDWORD lpcName, LPDWORD lpReserved, LPTSTR lpClass, LPDWORD lpcClass, PFILETIME lpftLastWriteTime );
static LONG _stdcall Inimode_RegEnumValue( HKEY hKey, DWORD dwIndex, LPTSTR lpValueName, LPDWORD lpcValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData );
static LONG _stdcall Inimode_RegFlushKey( HKEY hKey );
static LONG _stdcall Inimode_RegLoadKey( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpFile );
static LONG _stdcall Inimode_RegOpenKeyEx( HKEY hKey, LPCTSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult );
static LONG _stdcall Inimode_RegQueryInfoKey( HKEY hKey, LPTSTR lpClass, LPDWORD lpcClass, LPDWORD lpReserved, LPDWORD lpcSubKeys, LPDWORD lpcMaxSubKeyLen, LPDWORD lpcMaxClassLen, LPDWORD lpcValues, LPDWORD lpcMaxValueNameLen, LPDWORD lpcMaxValueLen, LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime );
static LONG _stdcall Inimode_RegQueryMultipleValues( HKEY hKey, PVALENT val_list, DWORD num_vals, LPTSTR lpValueBuf, LPDWORD ldwTotsize );
static LONG _stdcall Inimode_RegQueryValueEx( HKEY hKey, LPCTSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData );
static LONG _stdcall Inimode_RegReplaceKey( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpNewFile, LPCTSTR lpOldFile );
static LONG _stdcall Inimode_RegSaveKey( HKEY hKey, LPCTSTR lpFile, LPSECURITY_ATTRIBUTES lpSecurityAttributes );
static LONG _stdcall Inimode_RegSetValueEx( HKEY hKey, PCTSTR pValueName, DWORD Reserved, DWORD dwType, CONST BYTE *pData, DWORD cbData );
static LONG _stdcall Inimode_RegUnLoadKey( HKEY hKey, LPCTSTR lpSubKey );

static bool_t IsKeyConfig(HKEY hKey);
static bool_t IsKeyCurrentUser(HKEY hKey);
static bool_t IsKeyRkeyWrapper(HKEY hKey);
static bool_t IsKeyCurrentUser(HKEY hKey);
static bool_t IsKeyRkeyWrapper(HKEY hKey);
static bool_t IsKeyConfigDetail(HKEY hKey, char *keyname);

static HKEY AddRkeyWrapper(HKEY hKey, char *keyname);
static void AddAllSectionRkeyWrapper();
static bool_t ExistKeynameRkeyWrapper(char *keyname);
static char* Key2KeynameRkeyWrapper(HKEY hKey);
static HKEY Keyname2KeyRkeyWrapper(char *keyname);
static KeyData* Key2KeyDataRkeyWrapper(HKEY hKey);
static void FreeRkeyWrapperItem(HKEY hKey);
static void FreeRkeyWrapperItemAll();
static void AllocRkeyWrapper();
static void FreeRkeyWrapper();

static char* GetIniSection(HKEY hKey);
static char* GetInifileData(HKEY hKey, char* inkeyname);
static bool_t IsInvalidInifileData(char* pSection, char* pValueName, char* pValue);
static void SetInifileData( HKEY hKey, PCTSTR pValueName, DWORD dwType, CONST BYTE *pData, DWORD cbData );
static void DelInifileData( HKEY hKey, LPCTSTR lpValueName );
static int SetValueRegData(char* value, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData );
static LONG SetEnumKey(HKEY hKey, DWORD dwIndex, LPTSTR lpName, LPDWORD lpcName);
static LONG SetEnumValue(HKEY hKey, DWORD dwIndex, LPTSTR lpValueName, LPDWORD lpcValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData );

static void ReplaceFunctionAll(DWORD dwBase);
static int ReplaceFunction(DWORD dwBase, int no);

static void CleanInifile();
static bool_t ExistInifile();

static RkeyWrapper rkeyWrapper = {0 , 0 , NULL};
static RepControl Cntl[] =
{
    "kernel32.dll","LoadLibrary","LoadLibraryA","LoadLibraryW",(DWORD)Inimode_LoadLibrary, 0x00,
    "Advapi32.dll","RegCloseKey","RegCloseKey","RegCloseKey",(DWORD)Inimode_RegCloseKey, 0x00,
    "Advapi32.dll","RegConnectRegistry","RegConnectRegistryA","RegConnectRegistryW",(DWORD)Inimode_RegConnectRegistry, 0x00,
    "Advapi32.dll","RegCreateKeyEx","RegCreateKeyExA","RegCreateKeyExW",(DWORD)Inimode_RegCreateKeyEx, 0x00,
    "Advapi32.dll","RegDeleteKey","RegDeleteKeyA","RegDeleteKeyW",(DWORD)Inimode_RegDeleteKey, 0x00,
    "Advapi32.dll","RegDeleteValue","RegDeleteValueA","RegDeleteValueW",(DWORD)Inimode_RegDeleteValue, 0x00,
    "Advapi32.dll","RegEnumKeyEx","RegEnumKeyExA","RegEnumKeyExW",(DWORD)Inimode_RegEnumKeyEx, 0x00,
    "Advapi32.dll","RegEnumValue","RegEnumValueA","RegEnumValueW",(DWORD)Inimode_RegEnumValue, 0x00,
    "Advapi32.dll","RegFlushKey","RegFlushKeyA","RegFlushKey",(DWORD)Inimode_RegFlushKey, 0x00,
    "Advapi32.dll","RegLoadKey","RegLoadKeyA","RegLoadKeyW",(DWORD)Inimode_RegLoadKey, 0x00,
    "Advapi32.dll","RegOpenKeyEx","RegOpenKeyExA","RegOpenKeyExW",(DWORD)Inimode_RegOpenKeyEx, 0x00,
    "Advapi32.dll","RegQueryInfoKey","RegQueryInfoKeyA","RegQueryInfoKeyW",(DWORD)Inimode_RegQueryInfoKey, 0x00,
    "Advapi32.dll","RegQueryMultipleValues","RegQueryMultipleValuesA","RegQueryMultipleValuesW",(DWORD)Inimode_RegQueryMultipleValues, 0x00,
    "Advapi32.dll","RegQueryValueEx","RegQueryValueExA","RegQueryValueExW",(DWORD)Inimode_RegQueryValueEx, 0x00,
    "Advapi32.dll","RegReplaceKey","RegReplaceKeyA","RegReplaceKeyW",(DWORD)Inimode_RegReplaceKey, 0x00,
    "Advapi32.dll","RegSaveKey","RegSaveKeyA","RegSaveKeyW",(DWORD)Inimode_RegSaveKey, 0x00,
    "Advapi32.dll","RegSetValueEx","RegSetValueExA","RegSetValueExW",(DWORD)Inimode_RegSetValueEx, 0x00,
    "Advapi32.dll","RegUnLoadKey","RegUnLoadKeyA","RegUnLoadKeyW",(DWORD)Inimode_RegUnLoadKey, 0x00
    };

static bool_t inimode;
static char programname[256];
static char configkeyname[256];

static char inifile[] = "player.ini";
static char inifilepath[MAXPATH];

static HMODULE _stdcall Inimode_LoadLibrary(LPCTSTR lpFileName)
{
    int len;
    int iFnc = FNC_LOADLIBRARY;
    DWORD   dword = 0x00000000;
    HMODULE module = 0x00000000;

    module = ((HMODULE ( _stdcall *)(LPCTSTR))Cntl[iFnc].oldfunc)(lpFileName);
    dword = (DWORD)module;

    if ( dword )
    {
        len = strlen(lpFileName);
        if ( ( len > 4 ) && ( _stricmp(".plg", (lpFileName+len-4) ) == 0 ))
        {
            ReplaceFunctionAll(dword);
        }
        //else
        //{
        //    ReplaceFunction(dword, FNC_LOADLIBRARY);
        //}
    }

    return module;
}

static LONG _stdcall Inimode_RegOpenKeyEx( HKEY hKey, LPCTSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult )
{
    int iFnc = FNC_REGOPENKEYEX;
    LONG rc;

	if (hKey == NULL)
		return 1;
    if( IsKeyConfig(hKey) == FALSE || IsKeyConfigDetail( hKey, (char*)lpSubKey) == FALSE )
    {
        rc = ((LONG ( _stdcall *)(HKEY,LPCTSTR,DWORD,REGSAM, PHKEY ))Cntl[iFnc].oldfunc)(hKey,lpSubKey,ulOptions,samDesired,phkResult);
        return rc;
    }

    *phkResult = AddRkeyWrapper(hKey, (char*)lpSubKey);

    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegCreateKeyEx( HKEY hKey, LPCTSTR lpSubKey, DWORD Reserved, LPTSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition )
{
    int iFnc = FNC_REGCREATEKEYEX;
    LONG rc;

	if (hKey == NULL)
		return 1;

    if( IsKeyConfig(hKey) == FALSE || IsKeyConfigDetail( hKey, (char*)lpSubKey) == FALSE )
    {
        rc = ((LONG ( _stdcall *)(HKEY,LPCTSTR,DWORD,LPTSTR,DWORD,REGSAM,LPSECURITY_ATTRIBUTES,PHKEY, LPDWORD))Cntl[iFnc].oldfunc)(hKey,lpSubKey,Reserved,lpClass,dwOptions,samDesired,lpSecurityAttributes,phkResult, lpdwDisposition);
        return rc;
    }

    *phkResult = AddRkeyWrapper(hKey, (char*)lpSubKey);

    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegCloseKey(HKEY hKey)
{
    int iFnc = FNC_REGCLOSEKEY;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY))Cntl[iFnc].oldfunc)(hKey);
        return rc;
    }

    FreeRkeyWrapperItem(hKey);

    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegSetValueEx( HKEY hKey, PCTSTR pValueName, DWORD Reserved, DWORD dwType, CONST BYTE *pData, DWORD cbData )
{
    int iFnc = FNC_REGSETVALUEEX;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY,PCTSTR,DWORD,DWORD,CONST BYTE*,DWORD))Cntl[iFnc].oldfunc)(hKey,pValueName,Reserved,dwType,pData,cbData);
        return rc;
    }

    SetInifileData( hKey, pValueName, dwType, pData, cbData );

    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegDeleteValue( HKEY hKey, LPCTSTR lpValueName )
{
    int iFnc = FNC_REGDELETEVALUE;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY, LPCTSTR ))Cntl[iFnc].oldfunc)(hKey,lpValueName);
        return rc;
    }

    DelInifileData( hKey, lpValueName);

    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegQueryValueEx( HKEY hKey, LPCTSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData )
{
    int iFnc = FNC_REGQUERYVALUEEX;
    char *value;
    LONG rc;

	if (hKey == NULL)
		return 1;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY,LPCTSTR,LPDWORD,LPDWORD,LPBYTE, LPDWORD ))Cntl[iFnc].oldfunc)(hKey,lpValueName,lpReserved,lpType,lpData,lpcbData);
        return rc;
    }

    value = GetInifileData(hKey, (char*)lpValueName);
    if ( value == NULL )
        return 1;

    rc = SetValueRegData(value, lpType, lpData, lpcbData);

    return rc;
}

static LONG _stdcall Inimode_RegEnumKeyEx( HKEY hKey, DWORD dwIndex, LPTSTR lpName, LPDWORD lpcName, LPDWORD lpReserved, LPTSTR lpClass, LPDWORD lpcClass, PFILETIME lpftLastWriteTime )
{
    int iFnc = FNC_REGENUMKEYEX;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY,DWORD,LPTSTR,LPDWORD,LPDWORD,LPTSTR,LPDWORD, PFILETIME ))Cntl[iFnc].oldfunc)(hKey,dwIndex,lpName,lpcName,lpReserved,lpClass,lpcClass,lpftLastWriteTime);
        return rc;
    }

    rc = SetEnumKey(hKey, dwIndex, lpName, lpcName);

    return rc;
}

static LONG _stdcall Inimode_RegEnumValue( HKEY hKey, DWORD dwIndex, LPTSTR lpValueName, LPDWORD lpcValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData )
{
    int iFnc = FNC_REGENUMVALUE;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY,DWORD,LPTSTR,LPDWORD,LPDWORD,LPDWORD,LPBYTE, LPDWORD ))Cntl[iFnc].oldfunc)(hKey,dwIndex,lpValueName,lpcValueName,lpReserved,lpType,lpData,lpcbData);
        return rc;
    }

    rc = SetEnumValue(hKey, dwIndex, lpValueName, lpcValueName, lpType, lpData, lpcbData );

    return rc;
}

/********************************************************************************************************/
static LONG _stdcall Inimode_RegReplaceKey( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpNewFile, LPCTSTR lpOldFile )
{
    int iFnc = FNC_REGREPLACEKEY;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY,LPCTSTR,LPCTSTR, LPCTSTR ))Cntl[iFnc].oldfunc)(hKey,lpSubKey,lpNewFile,lpOldFile);
        return rc;
    }
    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegConnectRegistry(LPCTSTR lpMachineName,HKEY hKey,PHKEY phkResult)
{
    int iFnc = FNC_REGCONNECTREGISTRY;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(LPCTSTR, HKEY, PHKEY))Cntl[iFnc].oldfunc)(lpMachineName, hKey, phkResult);
        return rc;
    }

    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegFlushKey( HKEY hKey )
{
    int iFnc = FNC_REGFLUSHKEY;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY ))Cntl[iFnc].oldfunc)(hKey);
        return rc;
    }

    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegLoadKey( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpFile )
{
    int iFnc = FNC_REGLOADKEY;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY,LPCTSTR, LPCTSTR ))Cntl[iFnc].oldfunc)(hKey,lpSubKey,lpFile);
        return rc;
    }

    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegSaveKey( HKEY hKey, LPCTSTR lpFile, LPSECURITY_ATTRIBUTES lpSecurityAttributes )
{
    int iFnc = FNC_REGSAVEKEY;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY,LPCTSTR, LPSECURITY_ATTRIBUTES ))Cntl[iFnc].oldfunc)(hKey,lpFile,lpSecurityAttributes);
        return rc;
    }

    return ERROR_SUCCESS;
}

/********************************************************************************************************/
static LONG _stdcall Inimode_RegQueryInfoKey( HKEY hKey, LPTSTR lpClass, LPDWORD lpcClass, LPDWORD lpReserved, LPDWORD lpcSubKeys, LPDWORD lpcMaxSubKeyLen, LPDWORD lpcMaxClassLen, LPDWORD lpcValues, LPDWORD lpcMaxValueNameLen, LPDWORD lpcMaxValueLen, LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime )
{
    int iFnc = FNC_REGQUERYINFOKEY;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY,LPTSTR,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD, PFILETIME ))Cntl[iFnc].oldfunc)(hKey,lpClass,lpcClass,lpReserved,lpcSubKeys,lpcMaxSubKeyLen,lpcMaxClassLen,lpcValues,lpcMaxValueNameLen,lpcMaxValueLen,lpcbSecurityDescriptor,lpftLastWriteTime);
        return rc;
    }

    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegQueryMultipleValues( HKEY hKey, PVALENT val_list, DWORD num_vals, LPTSTR lpValueBuf, LPDWORD ldwTotsize )
{
    int iFnc = FNC_REGQUERYMULTIPLEVALUES;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyCurrentUser(hKey) == TRUE )
    {
        rc = ((LONG ( _stdcall *)(HKEY,PVALENT,DWORD,LPTSTR, LPDWORD ))Cntl[iFnc].oldfunc)(hKey,val_list,num_vals,lpValueBuf,ldwTotsize);
        return rc;
    }
    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegUnLoadKey( HKEY hKey, LPCTSTR lpSubKey )
{
    int iFnc = FNC_REGUNLOADKEY;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyConfigDetail( hKey, (char*)lpSubKey) == FALSE )
    {
        rc = ((LONG ( _stdcall *)(HKEY, LPCTSTR ))Cntl[iFnc].oldfunc)(hKey,lpSubKey);
        return rc;
    }

    return ERROR_SUCCESS;
}

static LONG _stdcall Inimode_RegDeleteKey( HKEY hKey, LPCTSTR lpSubKey )
{
    int iFnc = FNC_REGDELETEKEY;
    LONG rc;

    if( IsKeyConfig(hKey) == FALSE || IsKeyConfigDetail( hKey, (char*)lpSubKey) == FALSE )
    {
        rc = ((LONG ( _stdcall *)(HKEY, LPCTSTR ))Cntl[iFnc].oldfunc)(hKey,lpSubKey);
        return rc;
    }

    return ERROR_SUCCESS;
}
/********************************************************************************************************/

static bool_t IsKeyConfig(HKEY hKey)
{
    if ( ( hKey == HKEY_CLASSES_ROOT) ||
         /*       ( hKey == HKEY_CURRENT_USER) || */
         ( hKey == HKEY_LOCAL_MACHINE) ||
         ( hKey == HKEY_USERS) ||
         ( hKey == HKEY_PERFORMANCE_DATA) ||
         ( hKey == HKEY_CURRENT_CONFIG) ||
         ( hKey == HKEY_DYN_DATA) )
    {
        return FALSE;
    }

    if ( hKey < HKEY_CLASSES_ROOT )
    {
        return FALSE;
    }

    return TRUE;
}

static bool_t IsKeyCurrentUser(HKEY hKey)
{
    if ( hKey == HKEY_CURRENT_USER )
        return TRUE;

    return FALSE;
}

static bool_t IsKeyRkeyWrapper(HKEY hKey)
{
    if ( hKey >= HKEY_WRAPPER_KEY )
        return TRUE;

    return TRUE;
}

static bool_t IsKeyConfigDetail(HKEY hKey, char *keyname)
{
    bool_t flg;
    int  point, lenconfig, lenfull;
    char *keynamehead;
    char fullkeyname[MAXPATH];

    if ( ExistKeynameRkeyWrapper(keyname) == TRUE )
        return TRUE;

    if( IsKeyCurrentUser(hKey) == TRUE )
    {
        strcpy_s( fullkeyname,
                  sizeof(fullkeyname),
                  keyname);
    }
    else
    {
        keynamehead = Key2KeynameRkeyWrapper(hKey);
        if (keynamehead == NULL)
            sprintf_s(fullkeyname,
                      sizeof(fullkeyname),
                      "%s\\%s",
                      keynamehead,
                      keyname);
    }

    flg = TRUE;
    lenconfig = strlen(configkeyname);
    lenfull = strlen(fullkeyname);
    for ( point = 0 ; point < lenconfig ; point++ )
    {
        if( point > lenfull )
        {
            flg = FALSE;
            break;
        }

        if ( fullkeyname[point] != configkeyname[point] )
        {
            flg = FALSE;
            break;
        }

    }

    if ( flg == TRUE && ( fullkeyname[point] != '\\' && fullkeyname[point] != '\0' ) )
        flg = FALSE;

    return flg;
}

static void AddAllSectionRkeyWrapper()
{
    unsigned int rc;
    unsigned int i, size;
    char *allsection = NULL;
    char section[MAXPATH];

    for( i = 0 ; i < 100 ; i++)
    {
        size = (i+1)*1024;
        allsection = realloc ( allsection, size);
        memset(allsection, '\0', size);
        rc = GetPrivateProfileSectionNames( allsection,
                                            size,
                                            inifilepath);
        if ( rc == (size - 2) )
		{
			if(allsection != NULL)
			{
	            free(allsection);
				allsection = NULL;
			}
            continue;
		}
        else
            break;
    }
    if ( rc == (size - 2) )
    {
        if(allsection != NULL)
            free(allsection);
        return;
    }

    i = 0;
    while( strlen(allsection+i) != 0 )
    {
        sprintf_s(section,
                  sizeof(section),
                  "%s\\%s",
                  configkeyname, allsection+i);
        (void*)AddRkeyWrapper(HKEY_CURRENT_USER, section);
        i = i + strlen(allsection+i) + 1;
    }

    if(allsection != NULL)
        free(allsection);
    return;
}

static HKEY AddRkeyWrapper(HKEY hKey, char *keyname)
{
    char *keynamehead;
    char fullkeyname[MAXPATH];
    HKEY wkKey;

    if ( ExistKeynameRkeyWrapper(keyname) == TRUE )
    {
        wkKey = Keyname2KeyRkeyWrapper(keyname);
        return wkKey;
    }

    rkeyWrapper.num++;
    if ( rkeyWrapper.num > rkeyWrapper.max)
    {
        AllocRkeyWrapper();
    }

    wkKey = ( HKEY ) (ULONG_PTR)((LONG)HKEY_WRAPPER_KEY + rkeyWrapper.num - 1);
    rkeyWrapper.data[rkeyWrapper.num - 1].hKey = wkKey;
    rkeyWrapper.data[rkeyWrapper.num - 1].item = NULL;
    if( IsKeyCurrentUser(hKey) == TRUE )
    {
        sprintf_s(fullkeyname,
                  sizeof(fullkeyname),
                  "%s",
                  keyname);
    }
    else
    {
        keynamehead = Key2KeynameRkeyWrapper(hKey);
        sprintf_s(fullkeyname,
                  sizeof(fullkeyname),
                  "%s\\%s",
                  keynamehead,
                  keyname);
    }

    strcpy_s(rkeyWrapper.data[rkeyWrapper.num - 1].keyname,
             sizeof(rkeyWrapper.data[rkeyWrapper.num - 1].keyname),
             fullkeyname);

    return wkKey;
}

static char* GetIniSection(HKEY hKey)
{
    char *keyname;

    keyname = Key2KeynameRkeyWrapper(hKey);

    if ( keyname == NULL)
        return NULL;

    if ( ( memcmp( configkeyname, keyname, strlen(configkeyname) ) == 0 ) &&
         ( strlen(keyname) != strlen(configkeyname)  ))
    {
        return keyname + strlen(configkeyname) + 1;
    }

    return NULL;
}

static char* GetInifileData(HKEY hKey, char* inikeyname)
{
    bool_t flg;
    int rc;
    int i, num;
    char *section;
    KeyData *keydata;
    char wkBuff[MAXPATH];

    section = GetIniSection(hKey);
    keydata = Key2KeyDataRkeyWrapper(hKey);

    if ( section == NULL || keydata == NULL )
        return NULL;

    flg = FALSE;
    for ( i = 0 ; i < keydata->itemnum ; i++ )
    {
        if ( ( _stricmp( keydata->item[i].name, inikeyname ) == 0 ) )
        {
            flg = TRUE;
            break;
        }
    }

    if ( flg ==TRUE )
    {
        num = i;
    }
    else
    {
        num = keydata->itemnum;
        keydata->itemnum++;
        keydata->item = realloc( keydata->item, keydata->itemnum * sizeof(KeyItem));
        memset( &(keydata->item[num]), '\0', sizeof(KeyItem) );

        strcpy_s( keydata->item[num].name,
                  sizeof(keydata->item[num].name),
                  inikeyname );

    }

    rc = GetPrivateProfileString( section,
                                  inikeyname,
                                  "",
                                  wkBuff,
                                  sizeof(wkBuff),
                                  inifilepath);
    strcpy_s( keydata->item[num].value,
              sizeof(keydata->item[num].value),
              wkBuff);
       
    return keydata->item[num].value;

}

static void SetInifileData( HKEY hKey, PCTSTR pValueName, DWORD dwType, CONST BYTE *pData, DWORD cbData )
{
    int i;
    int BuffSize;
    char *section;
    char *wkBuff;

    char type_dword[] = "dword:";
    char type_binary[] = "hex:";

    section = GetIniSection(hKey);

    if ( section == NULL )
        return;

    switch (dwType)
    {
    case REG_SZ:
        BuffSize = cbData+10;
        wkBuff = malloc(BuffSize);
        if (wkBuff == NULL)
            return;
        memset( wkBuff, '\0', BuffSize );
        wkBuff[strlen(wkBuff)] = '"';
        memcpy(wkBuff+strlen(wkBuff),
               pData,
               cbData);
        wkBuff[strlen(wkBuff)] = '"';
        break;
    case REG_DWORD:
        BuffSize = MAXPATH;

        wkBuff = malloc(BuffSize);
        if (wkBuff == NULL)
            return;

        memset( wkBuff, '\0', BuffSize );
        wkBuff[strlen(wkBuff)] = '"';
        sprintf_s(wkBuff + strlen(wkBuff),
                  BuffSize,
                  type_dword);
        sprintf_s(wkBuff + strlen(wkBuff),
                  (BuffSize-strlen(wkBuff)),
                  "%.*x",
                  cbData*2 ,*(unsigned int*)pData);
        wkBuff[strlen(wkBuff)] = '"';

        break;
    case REG_BINARY:
        BuffSize = MAXPATH;
        wkBuff = malloc(BuffSize);
        if (wkBuff == NULL)
            return;

        memset( wkBuff, '\0', BuffSize );
        wkBuff[strlen(wkBuff)] = '"';
        sprintf_s(wkBuff + strlen(wkBuff),
                  BuffSize,
                  type_binary);
        for ( i = 0 ; i < (int)cbData ; i++ )
        {
            sprintf_s(wkBuff + strlen(wkBuff),
                      (BuffSize-strlen(wkBuff)-i),
                      "%.2x,",
                      (unsigned char)pData[i]);
            // (unsigned char)pData[(int)cbData-i-1]);
        }
        wkBuff[strlen(wkBuff)-1] = '"';

        break;
    default:
        break;
    }

        WritePrivateProfileString (section,
                                   pValueName,
                                   wkBuff,
                                   inifilepath);

    if (wkBuff != NULL)
        free(wkBuff);
    return;

}

static void DelInifileData( HKEY hKey, LPCTSTR lpValueName )
{
    char *section;

    section = GetIniSection(hKey);

    if ( section == NULL )
        return;

    WritePrivateProfileString (section,
                               lpValueName,
                               NULL,
                               inifilepath);
    return;

}

static void GetInifileSectionData(HKEY hKey)
{
    int i;
    int rc;
    int size;
    char *p;
    char *section = NULL;
    char *keyname = NULL;
    char *allvalue = NULL;

    section = GetIniSection(hKey);
    keyname = Key2KeynameRkeyWrapper(hKey);

    if ( section == NULL || keyname == NULL )
        return;

    for( i = 0 ; i < 100 ; i++)
    {
        size = (i+1)*1024;
        allvalue = realloc( allvalue, size );
        memset(allvalue, '\0', size);
        rc = GetPrivateProfileSection( section,
                                       allvalue,
                                       size,
                                       inifilepath);
        if ( rc == (size - 2) )
		{
			if(allvalue != NULL)
			{
	            free(allvalue);
				allvalue = NULL;
			}
            continue;
		}
        else
            break;
    }
    if ( rc == (size - 2) )
    {
        if(allvalue != NULL)
            free(allvalue);
        return;
    }

    i = 0;
    while( strlen(allvalue+i) != 0 )
    {
        p =  memchr(allvalue + i, '=', strlen(allvalue+i) );
        if (p == NULL)
            continue;
        *p='\0';
        GetInifileData(hKey, allvalue+i);
        i = i + strlen(allvalue+i) + 1;
        i = i + strlen(allvalue+i) + 1; // replace = to \0.
    }

    if(allvalue != NULL)
        free(allvalue);
}

static int SetValueRegData(char* value, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData )
{
    int rc;
    int i;
    int  len;
    int  type;
    char *val;
    char inc[3];

    char type_dword[] = "dword:";
    char type_binary[] = "hex:";

    rc = ERROR_SUCCESS;

    val = value;
    type = REG_SZ;

    if ( ( strlen(value) >= strlen(type_dword)) &&
         ( memcmp(value, type_dword, strlen(type_dword)) == 0 ) )
    {
        val = value + strlen(type_dword);
        type = REG_DWORD;
    }
    else if ( ( strlen(value) >= strlen(type_binary)) &&
              ( memcmp(value, type_binary, strlen(type_binary)) == 0 ) )
    {
        val = value + strlen(type_binary);
        type = REG_BINARY;
    }

    memset( lpData, '\0', *lpcbData);
    switch (type)
    {
    case REG_SZ:
        *lpType = REG_SZ;
        strcpy_s( (char*)lpData,
                  *lpcbData-1,
                  val);
        break;
    case REG_DWORD:
        *lpType = REG_DWORD;
        *(DWORD*)lpData = (DWORD)strtoul(val, NULL, 16);
        break;
    case REG_BINARY:
        *lpType = REG_BINARY;
        len = strlen(val);
        for ( i = 0 ; i < (int)*lpcbData ; i++ )
        {
            memcpy(inc, val+i*3, 3);
            inc[2] = '\0';
            lpData[i] = (BYTE)strtoul(inc, NULL, 16);
            // (unsigned char)lpData[sizeof(int64_t)-i-1] = (unsigned char)strtoul(inc, NULL, 16);
        }
        break;
    }

    return rc;
}

static LONG SetEnumKey(HKEY hKey, DWORD dwIndex, LPTSTR lpName, LPDWORD lpcName)
{
    int i,cnt;

    char *keyname = NULL;

	keyname = Key2KeynameRkeyWrapper(hKey);

    if ( keyname == NULL )
        return ERROR_NO_MORE_ITEMS;

    if ( _stricmp(keyname, configkeyname) != 0 )
        return ERROR_NO_MORE_ITEMS;

    if ( (int)dwIndex + 1 > rkeyWrapper.max )
        return ERROR_NO_MORE_ITEMS;

    cnt = 0;
    for ( i = 0 ; i < rkeyWrapper.max ; i++ )
    {
        if( strlen(rkeyWrapper.data[i].keyname) == 0 )
            return ERROR_NO_MORE_ITEMS;
        if( _stricmp(rkeyWrapper.data[i].keyname, configkeyname) == 0 )
            continue;
        if( memcmp(rkeyWrapper.data[i].keyname, configkeyname, strlen(configkeyname)) != 0 )
            continue;
        if ( cnt == (int)dwIndex )
            break;
        cnt++;
    }

    if ( cnt != (int)dwIndex )
        return ERROR_NO_MORE_ITEMS;
    cnt = i;

    strcpy_s( lpName,
              *lpcName,
              rkeyWrapper.data[cnt].keyname );

    return ERROR_SUCCESS;
}

static LONG SetEnumValue(HKEY hKey, DWORD dwIndex, LPTSTR lpValueName, LPDWORD lpcValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData )
{
    int rc;
    int i,cnt;
    KeyData *keydata = NULL;

    rc = ERROR_SUCCESS;

    keydata = Key2KeyDataRkeyWrapper(hKey);

    if ( keydata == NULL )
        return ERROR_NO_MORE_ITEMS;

    if ( _stricmp(keydata->keyname, configkeyname) == 0 )
        return ERROR_NO_MORE_ITEMS;

    if ( (int)dwIndex + 1 > keydata->itemnum )
        return ERROR_NO_MORE_ITEMS;

    if ( (int)dwIndex == 0 )
        GetInifileSectionData(hKey); // inidata in memory.

    cnt = 0;
    for ( i = 0 ; i < keydata->itemnum ; i++ )
    {
        if( strlen(keydata->item[i].name) == 0 )
            return ERROR_NO_MORE_ITEMS;
        if( strlen(keydata->item[i].value) == 0 )
            continue;
        if ( cnt == (int)dwIndex)
            break;
        cnt++;
    }

    if ( cnt != (int)dwIndex )
        return ERROR_NO_MORE_ITEMS;
    cnt = i;

    strcpy_s( lpValueName,
              *lpcValueName,
              keydata->item[cnt].name );

    if ( lpType != NULL && lpData != NULL && lpcbData != NULL )
        rc = SetValueRegData(keydata->item[cnt].value, lpType, lpData, lpcbData);

    return rc;
}

static bool_t IsInvalidInifileData(char* pSection, char* pValueName, char* pValue)
{
    
    if ( pSection != NULL)
    {
        if( _stricmp(pSection,"DLLPath") == 0 )
            return TRUE;
        
        if( _stricmp(pSection,"DLLStamp") == 0 )
            return TRUE;
    }

    if ( pValueName != NULL && pValue != NULL)
    {
        if( (_stricmp(pValueName,"4") == 0) &&
            ( (_stricmp(pValue+strlen(pValue)-3, ".dll") == 0) ||
              (_stricmp(pValue+strlen(pValue)-3, ".plg") == 0)))
            return TRUE;
    }

    return TRUE;
}

static bool_t ExistKeynameRkeyWrapper(char *keyname)
{
    int i;

    for ( i = 0 ; i < rkeyWrapper.num ; i++ )
    {
        if ( _stricmp( keyname, rkeyWrapper.data[i].keyname ) == 0 )
            return TRUE;
    }

    return FALSE;
}

static KeyData* Key2KeyDataRkeyWrapper(HKEY hKey)
{
    int i;

    for ( i = 0 ; i < rkeyWrapper.num ; i++ )
    {
        if ( hKey == rkeyWrapper.data[i].hKey )
        {
            return &(rkeyWrapper.data[i]);
        }
    }

    return NULL;
}

static char* Key2KeynameRkeyWrapper(HKEY hKey)
{
    int i;

    for ( i = 0 ; i < rkeyWrapper.num ; i++ )
    {
        if ( hKey == rkeyWrapper.data[i].hKey )
        {
            return rkeyWrapper.data[i].keyname;
        }
    }

    return NULL;
}

static HKEY Keyname2KeyRkeyWrapper(char *keyname)
{
    int i;

    for ( i = 0 ; i < rkeyWrapper.num ; i++ )
    {
        if ( _stricmp( keyname, rkeyWrapper.data[i].keyname ) == 0 )
            return rkeyWrapper.data[i].hKey;
    }

    return NULL;
}

static void FreeRkeyWrapperItem(HKEY hKey)
{
    KeyData *keydata;

    keydata = Key2KeyDataRkeyWrapper(hKey);

    if ( ( keydata != NULL ) &&
         ( keydata->item != NULL) )
    {
        free(keydata->item);
        keydata->itemnum = 0;
        keydata->item = NULL;
    }

    return;
}

static void FreeRkeyWrapperItemAll()
{
    int i;

    for ( i = 0 ; i < rkeyWrapper.num ; i++ )
    {
        if ( rkeyWrapper.data[i].item != NULL )
        {
            free(rkeyWrapper.data[i].item);
            rkeyWrapper.data[i].item = NULL;
        }
    }

    return;
}

static void AllocRkeyWrapper()
{
    int allocsize = 10;

    rkeyWrapper.max += allocsize;
    rkeyWrapper.data = realloc(rkeyWrapper.data, rkeyWrapper.max * sizeof(KeyData));

    memset( &(rkeyWrapper.data[rkeyWrapper.max-allocsize]) ,
            '\0',
            allocsize * sizeof(KeyData));

    return;
}

static void FreeRkeyWrapper()
{
    FreeRkeyWrapperItemAll();

    rkeyWrapper.max = 0;
    rkeyWrapper.num = 0;
    if ( rkeyWrapper.data != NULL )
    {
        free( rkeyWrapper.data );
        rkeyWrapper.data = NULL;
    }

    return;
}

static bool_t GetInifileName()
{
    bool_t b;
    b = TRUE;
    return b;
}

static void CleanInifile()
{
    unsigned int rc;
    unsigned int i, size;
    char *allsection = NULL;

//	return;

    WritePrivateProfileString ("DLLPath",
	                           NULL,
		                       NULL,
			                   inifilepath);

    WritePrivateProfileString ("DLLStamp",
	                           NULL,
		                       NULL,
			                   inifilepath);


    for( i = 0 ; i < 100 ; i++)
    {
        size = (i+1)*1024;
        allsection = realloc ( allsection, size);
        memset(allsection, '\0', size);
        rc = GetPrivateProfileSectionNames( allsection,
                                            size,
                                            inifilepath);
        if ( rc == (size - 2) )
		{
			if(allsection != NULL)
			{
	            free(allsection);
				allsection = NULL;
			}
            continue;
		}
        else
            break;
    }
    if ( rc == (size - 2) )
    {
        if(allsection != NULL)
            free(allsection);
        return;
    }

    i = 0;
    while( strlen(allsection+i) != 0 )
    {
	    WritePrivateProfileString (allsection+i,
		                           "4",
			                       NULL,
				                   inifilepath);

        i = i + strlen(allsection+i) + 1;
    }
	
    if(allsection != NULL)
        free(allsection);

	return;
}

static bool_t ExistInifile()
{
    bool_t b;
	FILE *fp;

	fopen_s(&fp,inifilepath,"r");
	if( fp != NULL)
	{
	    b = TRUE;
		fclose(fp);
	}
	else
	{
	    b = FALSE;
	}

    return b;
}

static void ReplaceFunctionAll(DWORD dwBase)
{
    ReplaceFunction((DWORD)dwBase, FNC_LOADLIBRARY);
    ReplaceFunction((DWORD)dwBase, FNC_REGCLOSEKEY);
    ReplaceFunction((DWORD)dwBase, FNC_REGCONNECTREGISTRY);
    ReplaceFunction((DWORD)dwBase, FNC_REGCREATEKEYEX);
    ReplaceFunction((DWORD)dwBase, FNC_REGDELETEKEY);
    ReplaceFunction((DWORD)dwBase, FNC_REGDELETEVALUE);
    ReplaceFunction((DWORD)dwBase, FNC_REGENUMKEYEX);
    ReplaceFunction((DWORD)dwBase, FNC_REGENUMVALUE);
    ReplaceFunction((DWORD)dwBase, FNC_REGFLUSHKEY);
    ReplaceFunction((DWORD)dwBase, FNC_REGLOADKEY);
    ReplaceFunction((DWORD)dwBase, FNC_REGOPENKEYEX);
    ReplaceFunction((DWORD)dwBase, FNC_REGQUERYINFOKEY);
    ReplaceFunction((DWORD)dwBase, FNC_REGQUERYMULTIPLEVALUES);
    ReplaceFunction((DWORD)dwBase, FNC_REGQUERYVALUEEX);
    ReplaceFunction((DWORD)dwBase, FNC_REGREPLACEKEY);
    ReplaceFunction((DWORD)dwBase, FNC_REGSAVEKEY);
    ReplaceFunction((DWORD)dwBase, FNC_REGSETVALUEEX);
    ReplaceFunction((DWORD)dwBase, FNC_REGUNLOADKEY);
}

static int ReplaceFunction(DWORD dwBase, int no)
{
    PIMAGE_IMPORT_DESCRIPTOR pImgDesc;
    PIMAGE_THUNK_DATA        IMG_IAT,IMG_INT;
    PIMAGE_IMPORT_BY_NAME    pImportName;
    DWORD dwOldProtect;
    DWORD dwSize;
    char *pModule;

    /* Get .Data */
    pImgDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
        (HMODULE)dwBase,
        TRUE,
        IMAGE_DIRECTORY_ENTRY_IMPORT,
        &dwSize);

    /* Search Dll */
    while(pImgDesc->Name)
    {
        pModule = (char*)(dwBase+pImgDesc->Name);

        if( _stricmp(pModule, Cntl[no].TgtModule) == 0 )
            break; /* Found */
        pImgDesc++;

    }
    if (pImgDesc->Name == 0)
        return 1;

    /* Search Function */
    IMG_IAT = (PIMAGE_THUNK_DATA)(dwBase+pImgDesc->FirstThunk);
    IMG_INT = (PIMAGE_THUNK_DATA)(dwBase+pImgDesc->OriginalFirstThunk);
    while(IMG_IAT->u1.Function)
    {
        if(IMAGE_SNAP_BY_ORDINAL(IMG_INT->u1.Ordinal))continue;
        pImportName = (PIMAGE_IMPORT_BY_NAME)
            (dwBase+(DWORD)IMG_INT->u1.AddressOfData);

        //if ( ( _stricmp((char*)pImportName->Name, Cntl[no].TgtFunc1) == 0) ||
        //     ( _stricmp((char*)pImportName->Name, Cntl[no].TgtFunc2) == 0) ||
        //     ( _stricmp((char*)pImportName->Name, Cntl[no].TgtFunc3) == 0) )

        //if ( ( _stricmp((char*)pImportName->Name, Cntl[no].TgtFunc1) == 0) ||
        //     ( _stricmp((char*)pImportName->Name, Cntl[no].TgtFunc2) == 0) )
        if ( _stricmp((char*)pImportName->Name, Cntl[no].TgtFunc2) == 0)
        {
            /* Replase Function */
            if(Cntl[no].oldfunc == 0x00)
                Cntl[no].oldfunc = IMG_IAT->u1.Function;
            VirtualProtect(&IMG_IAT->u1.Function, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
            IMG_IAT->u1.Function = Cntl[no].newfunc;
            VirtualProtect(&IMG_IAT->u1.Function,sizeof(DWORD),dwOldProtect,&dwOldProtect);
            break; /* Found */
        }

        IMG_IAT++;
        IMG_INT++;
    }
    if (Cntl[no].oldfunc == 0x00)
        return 1;

    return 0;
}

void Inimode_Init(char* ProgramName)
{
    HMODULE dwBase;
	char *p;
	bool_t b;

    strcpy_s(programname, sizeof(programname), ProgramName);

	GetModuleFileName(NULL,inifilepath,MAXPATH);
	p = tcsrchr(inifilepath,'\\');
	if (p==NULL) return;
	sprintf_s( p,
		       sizeof(inifilepath),
			   "\\%s",
			   inifile);
    sprintf_s( configkeyname,
               sizeof(configkeyname),
               "SOFTWARE\\%s",
               programname);

    b = ExistInifile();
	inimode = b;
    if ( b == FALSE ) 
		return;

	CleanInifile();

    AllocRkeyWrapper();
    AddAllSectionRkeyWrapper();

    dwBase = GetModuleHandle(NULL);
    if(dwBase != NULL) {
        ReplaceFunctionAll((DWORD)dwBase);
      }

    dwBase = GetModuleHandle("common.dll");
    if(dwBase != NULL) {
        ReplaceFunctionAll((DWORD)dwBase);
      }

    return;
}

void Inimode_Done()
{
	if (inimode == FALSE)
		return;

	CleanInifile();
	FreeRkeyWrapper();
    return;
}
