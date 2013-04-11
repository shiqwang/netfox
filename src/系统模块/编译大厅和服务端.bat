@set path=D:\Program Files\Microsoft Visual Studio .NET 2003\Common7\IDE;%path%

@cls

devenv "共享组件\公共服务\ComService.vcproj" /build release
devenv "共享组件\界面控件\SkinControls.vcproj" /build release
devenv "共享组件\网络组件\SocketModule.vcproj" /build release
       
devenv "客户端组件\客户端共享\ClientShare.vcproj" /build release
devenv "客户端组件\信道模块\ChannelModule.vcproj" /build release
devenv "客户端组件\关系管理\Companion.vcproj" /build release
devenv "客户端组件\头像组件\UserFace.vcproj" /build release
devenv "客户端组件\下载组件\DownLoad.vcproj" /build release
devenv "客户端组件\游戏等级\GameRank.vcproj" /build release
       
devenv "客户端组件\(演示)游戏广场\GamePlaza.vcproj" /build release
devenv "客户端组件\游戏框架\GameFrame.vcproj" /build release
       
       
devenv "服务器组件\登陆服务器\LogonServer.vcproj" /build release
devenv "服务器组件\服务装载器\ServiceLoader.vcproj" /build release
devenv "服务器组件\列表服务\ListService.vcproj" /build release
devenv "服务器组件\内核组件\ServerKernel.vcproj" /build release
devenv "服务器组件\游戏服务\GameService.vcproj" /build release
devenv "服务器组件\中心服务器\CenterServer.vcproj" /build release

pause;

