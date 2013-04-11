#ifndef IUNKNOWN_EX_HEAD_FILE
#define IUNKNOWN_EX_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//产品版本号
#define PRODUCT_VER				1

//版本定义
#define INTERFACE_VERSION(cbMainVer,cbSubVer)								\
		(DWORD)(															\
		(((BYTE)(PRODUCT_VER))<<24)+										\
		(((BYTE)(cbMainVer))<<16)+											\
		((BYTE)(cbSubVer)<<8))

//获取产品版本
inline BYTE GetProductVer(DWORD dwVersion)
{
	BYTE * cbVersion=(BYTE *)&dwVersion;
	return cbVersion[3];
}

//获取主版本
inline BYTE GetMainVer(DWORD dwVersion)
{
	BYTE * cbVersion=(BYTE *)&dwVersion;
	return cbVersion[2];
}

//获取子版本
inline BYTE GetSubVer(DWORD dwVersion)
{
	BYTE * cbVersion=(BYTE *)&dwVersion;
	return cbVersion[1];
}

//获取编译版本
inline BYTE GetBulidVer(DWORD dwVersion)
{
	BYTE * cbVersion=(BYTE *)&dwVersion;
	return cbVersion[0];
}

//版本对比函数
inline bool InterfaceVersionCheck(DWORD dwQueryVer, DWORD dwInterfaceVer)
{
	if (GetProductVer(dwQueryVer)!=GetProductVer(dwInterfaceVer)) return false;
	if (GetMainVer(dwQueryVer)!=GetMainVer(dwInterfaceVer)) return false;
	if (GetSubVer(dwQueryVer)>GetSubVer(dwInterfaceVer)) return false;
	return true;
};

//////////////////////////////////////////////////////////////////////////

//接口定义
#define STD_METHOD(TYPE,METHOD) virtual TYPE __cdecl METHOD

//接口组件创建函数说明
typedef void * (__cdecl ModuleCreateFunc)(const GUID & Gudi, DWORD dwInterfaceVer);

//////////////////////////////////////////////////////////////////////////

#define VER_IUnknownEx INTERFACE_VERSION(1,1)
static const GUID IID_IUnknownEx={0x92d87e91,0x5a5b,0x4a45,0x8c,0xfd,0x38,0x2f,0xe6,0xb3,0x2b,0xb};

//基础接口
interface IUnknownEx
{
	//是否有效
	virtual bool __cdecl IsValid()=NULL;
	//释放对象
	virtual bool __cdecl Release()=NULL;
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//接口查询宏
#define QUERYINTERFACE(Interface,Guid,dwQueryVer)										\
	if ((Guid==IID_##Interface)&&(InterfaceVersionCheck(dwQueryVer,VER_##Interface)))	\
		return static_cast<Interface *>(this);											

#define QUERYINTERFACE_IUNKNOWNEX(BaseInterface,Guid,dwQueryVer)						\
	if ((Guid==IID_IUnknownEx)&&(InterfaceVersionCheck(dwQueryVer,VER_IUnknownEx)))		\
		return static_cast<IUnknownEx *>(static_cast<BaseInterface *>(this));			

//获取接口宏
#define GET_MYSELF_INTERFACE(Interface)													\
	((Interface *)QueryInterface(IID_##Interface,VER_##Interface))

#define GET_OBJECT_INTERFACE(Object,Interface)											\
	((Interface *)Object.QueryInterface(IID_##Interface,VER_##Interface))

#define GET_OBJECTPTR_INTERFACE(pObject,Interface)										\
	((pObject==NULL)?NULL:((Interface *)pObject->QueryInterface(IID_##Interface,VER_##Interface)))

//////////////////////////////////////////////////////////////////////////

#endif