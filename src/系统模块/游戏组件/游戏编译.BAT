
REM /////////////////////////////////////////////////////////////
REM
REM 游戏组件工程编译处理
REM
REM /////////////////////////////////////////////////////////////

DEL 编译日志.txt
@set path=d:\Program Files\Microsoft Visual Studio 8\Common7\IDE;%path%

devenv "拱猪\游戏服务器\GameServer.vcproj" /build release
devenv "拱猪\游戏客户端\GameClient.vcproj" /build release

devenv "梭哈\游戏服务器\GameServer.vcproj" /build release
devenv "梭哈\游戏客户端\GameClient.vcproj" /build release

devenv "中国象棋\游戏服务器\GameServer.vcproj" /build release
devenv "中国象棋\游戏客户端\GameClient.vcproj" /build release

devenv "围棋\游戏服务器\GameServer.vcproj" /build release
devenv "围棋\游戏客户端\GameClient.vcproj" /build release

devenv "斗地主\游戏服务器\GameServer.vcproj" /build release
devenv "斗地主\游戏客户端\GameClient.vcproj" /build release

devenv "锄大地\游戏服务器\GameServer.vcproj" /build release
devenv "锄大地\游戏客户端\GameClient.vcproj" /build release

devenv "两副升级\游戏服务器\GameServer.vcproj" /build release
devenv "两副升级\游戏客户端\GameClient.vcproj" /build release

devenv "四国军棋\游戏服务器\GameServer.vcproj" /build release
devenv "四国军棋\游戏客户端\GameClient.vcproj" /build release

devenv "诈金花\游戏服务器\GameServer.vcproj" /build release
devenv "诈金花\游戏客户端\GameClient.vcproj" /build release


devenv "黑杰克\游戏服务器\BJServer.vcproj" /build release
devenv "黑杰克\游戏客户端\BJ.vcproj" /build release

devenv "麻将\游戏服务器\GameServer.vcproj" /build release
devenv "麻将\游戏客户端\GameClient.vcproj" /build release

pause

