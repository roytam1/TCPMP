Option Explicit

Dim objWsh
Dim objFs
Dim objRegexp

Dim objText1
Dim objText2
Dim strLine
Dim strBuf
Dim rc

Const inifile = "player.ini"
Const regfile1 = "tcpmpmod.reg"
Const regfile2 = "tcpmpmod2.reg"

' 初期設定
Set objWsh = CreateObject("WScript.Shell")
Set objFs = CreateObject("Scripting.FileSystemObject")
Set objRegexp = New RegExp

' player.iniの存在を確認
If objFs.FileExists(inifile) = True Then
    rc = MsgBox("既に" & inifile & "がありますが、上書きしますか?", VbYesNo, "config-org2mod.vbs")
    If Not rc <> vbOK Then
        MsgBox "キャンセルしました"
        WScript.Quit(1)
    End If
End If

'レジストリファイル書き出し
objWsh.Run "regedit /s /e " & regfile1 & " ""HKEY_CURRENT_USER\Software\TCPMP mod""", 0, True
If objFs.FileExists(regfile1) = False Then
    rc = MsgBox("TCPMP modのレジストリが無いため、終了します。")
    WScript.Quit(1)
End If

' レジストリとiniファイルオープン
Set objText1 = objFS.OpenTextFile(regfile1, 1, False, -1)
Set objText2 = objFS.OpenTextFile(inifile, 2, True, -1)

' 置換処理
strBuf = ""
Do Until objText1.AtEndOfStream
    strLine = objText1.ReadLine
    strLine = Replace(strLine, _
                            "[HKEY_CURRENT_USER\Software\TCPMP mod\", _
                            "[")
    strLine = Replace(strLine, _
                            "Windows Registry Editor Version 5.00", _
                            "")
    strLine = Replace(strLine, _
                            "[HKEY_CURRENT_USER\Software\TCPMP mod]", _
                            "")
    objRegexp.Pattern = """(.*)""="
    strLine = objRegexp.Replace(strLine,"$1=")
    objRegexp.Pattern = "=(dword:.*)"
    strLine = objRegexp.Replace(strLine,"=""$1""")
    objRegexp.Pattern = "=(hex:.*)"
    strLine = objRegexp.Replace(strLine,"=""$1""")
    
    strBuf = strBuf & strLine & vbCrLf
Loop
strBuf = Replace(strBuf, _
                 vbCrLf & vbCrLf & vbCrLf & vbCrLf, _
                 vbCrLf)

objText2.Write(strBuf)

objText1.Close
objText2.Close
Set objText1 = Nothing
Set objText2 = Nothing

' mod用の設定があれば削除するか聞く
objWsh.Run "regedit /s /e " & regfile1 & " ""HKEY_CURRENT_USER\Software\TCPMP mod""", 0, True
If objFs.FileExists(regfile1) = True Then
    rc = MsgBox("TCPMPmod用のレジストリを削除しますか?", VbYesNo, "config-org2mod.vbs")
    If rc = vbOK Then
        ' 置換処理
        Set objText1 = objFS.OpenTextFile(regfile1, 1, False, -1)
        Set objText2 = objFS.OpenTextFile(regfile2, 2, True, -1)
        Do Until objText1.AtEndOfStream
            strLine = objText1.ReadLine
            strLine = Replace(strLine, _
              "[HKEY_CURRENT_USER\Software\TCPMP mod", _
              "[-HKEY_CURRENT_USER\Software\TCPMP mod")
            objText2.WriteLine(strLine)
        Loop
        
        objText1.Close
        objText2.Close
        Set objText1 = Nothing
        Set objText2 = Nothing
        
        objWsh.Run "regedit /s /i " & regfile2, 0, True
        
        Call objFs.DeleteFile(regfile1,True)
        Call objFs.DeleteFile(regfile2,True)
    End If

End If

If objFs.FileExists(regfile1) = True Then
    Call objFs.DeleteFile(regfile1, True)
End If

If objFs.FileExists(regfile2) = True Then
    Call objFs.DeleteFile(regfile2, True)
End If

' 終了処理
Set objWsh = Nothing
Set objFs = Nothing
Set objRegexp = Nothing

MsgBox "完了"
