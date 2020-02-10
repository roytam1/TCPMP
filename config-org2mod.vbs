Option Explicit

Dim objWsh
Dim objFs

Dim objText1
Dim objText2
Dim StrLine
Dim rc

Const file1 = "tcpmp1.reg"
Const file2 = "tcpmp2.reg"

' 初期設定
Set objWsh = CreateObject("WScript.Shell")
Set objFs = CreateObject("Scripting.FileSystemObject")

' 実行可否
rc = MsgBox("TCPMPの設定をmod用に変換します。", VbOKCancel, "config-org2mod.vbs")
If Not rc = vbOK Then
    WScript.Quit(1)
End If

'レジストリファイル書き出し
objWsh.Run "regedit /s /e " & file1 & " HKEY_CURRENT_USER\Software\TCPMP", 0, True

' レジストリファイルオープン
Set objText1 = objFS.OpenTextFile(file1, 1, False, -1)
Set objText2 = objFS.OpenTextFile(file2, 2, True, -1)

' 置換処理
Do Until objText1.AtEndOfStream
    strLine = objText1.ReadLine
    strLine = Replace(strLine, _
                            "HKEY_CURRENT_USER\Software\TCPMP", _
                            "HKEY_CURRENT_USER\Software\TCPMP mod")
    objText2.WriteLine(strLine)
Loop

' レジストリファイル閉じる
objText1.Close
objText2.Close
Set objText1 = Nothing
Set objText2 = Nothing

'レジストリファイル登録
objWsh.Run "regedit /s /i " & file2, 0, True

'ファイル削除
Call objFs.DeleteFile(file1,True)
Call objFs.DeleteFile(file2,True)

' 終了処理
Set objWsh = Nothing
Set objFs = Nothing


MsgBox "設定完了"
