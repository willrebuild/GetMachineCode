// GetMachineCode.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <atlbase.h>

using namespace std;

string GetMachaineId() {
	//DWORD   serial;
	DWORD   n;
	char    guid[MAX_PATH];
	HKEY    hSubKey;

	n=sizeof(guid);

	RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Microsoft\\Cryptography", 0, 
		KEY_READ | KEY_WOW64_64KEY, &hSubKey);

	RegQueryValueExA(hSubKey, "MachineGuid", NULL,
		NULL, (LPBYTE)guid, &n);

	RegCloseKey(hSubKey);        
	//printf ("MachineGuid: %s\n", guid);
	return guid;
}


std::string ExeCmd(std::wstring pszCmd)
{
	// 创建匿名管道
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
	HANDLE hRead, hWrite;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		return " ";
	}

	// 设置命令行进程启动信息(以隐藏方式启动命令并定位其输出到hWrite
	STARTUPINFO si = {sizeof(STARTUPINFO)};
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;

	// 启动命令行
	PROCESS_INFORMATION pi;
	if (!CreateProcess(NULL, (LPWSTR)pszCmd.c_str(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		return "Cannot create process";
	}

	// 立即关闭hWrite
	CloseHandle(hWrite);

	// 读取命令行返回值
	std::string strRetTmp;
	char buff[1024] = {0};
	DWORD dwRead = 0;
	strRetTmp = buff;
	while (ReadFile(hRead, buff, 1024, &dwRead, NULL))
	{
		strRetTmp += buff;
	}
	CloseHandle(hRead);

	return strRetTmp;

}

string GetBiosUuid() {
	string retVal = ExeCmd(L"wmic csproduct get UUID");

	int pos =  retVal.find("\n");
	if(pos <= 0)
		return retVal;
	retVal = retVal.substr(pos+1);
	pos = retVal.find(" ");
	if(pos <= 0)
		return retVal;

	retVal = retVal.substr(0, pos);
	return retVal;
}

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "MachineGUID: " << GetMachaineId().c_str() << endl;

	cout << "主板smBIOS UUID: " << GetBiosUuid().c_str() << endl;

	cin.get();
	return 0;
}

